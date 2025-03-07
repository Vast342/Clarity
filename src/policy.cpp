/*
    Clarity
    Copyright (C) 2024 Joseph Pasfield

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "globals.h"
#include "movemap.h"
#include <filesystem>

#ifdef _MSC_VER
#define SP_MSVC
#pragma push_macro("_MSC_VER")
#undef _MSC_VER
#endif

#define INCBIN_PREFIX g_
#include "external/incbin.h"

#ifdef SP_MSVC
#pragma pop_macro("_MSC_VER")
#undef SP_MSVC
#endif

namespace {
    INCBIN(networkTwo, "src/policy/cpn_001.pn");
    const PolicyNetwork *p_network = reinterpret_cast<const PolicyNetwork *>(g_networkTwoData);
}

void PolicyNetworkState::reset() {
    std::memset(stack.data(), 0, sizeof(stack));
    current = 0;

    stack[current].initialize(p_network->featureBiases);
}

void PolicyNetworkState::performUpdates(NetworkUpdates updates) {
    assert(updates.numAdds <= 2);
    assert(updates.numSubs <= 2);
    for(int i = 0; i < updates.numAdds; i++) {
        activateFeature(updates.adds[i].square, updates.adds[i].piece);
    }
    for(int i = 0; i < updates.numSubs; i++) {
        disableFeature(updates.subs[i].square, updates.subs[i].piece);                
    }
}
void PolicyNetworkState::performUpdatesAndPush(NetworkUpdates updates) {
    assert(updates.numAdds <= 2);
    assert(updates.numSubs <= 2);
    activateFeatureAndPush(updates.adds[0].square, updates.adds[0].piece);
    for(int i = 1; i < updates.numAdds; i++) {
        activateFeature(updates.adds[i].square, updates.adds[i].piece);
    }
    for(int i = 0; i < updates.numSubs; i++) {
        disableFeature(updates.subs[i].square, updates.subs[i].piece);
    }
}

void PolicyAccumulator::initialize(std::span<const float, p_l1Size> bias) {
    std::copy(bias.begin(), bias.end(), black.begin());
    std::copy(bias.begin(), bias.end(), white.begin());
}
void PolicyAccumulator::initHalf(std::span<const float, p_l1Size> bias, int color) {
    std::copy(bias.begin(), bias.end(), color == 0 ? black.begin() : white.begin());
}

void PolicyNetworkState::fullRefresh(const BoardState &state) {
    halfRefresh(0, state);
    halfRefresh(1, state);
}

void PolicyNetworkState::halfRefresh(int color, const BoardState &state) {
    stack[current].initHalf(p_network->featureBiases, color);

    for(int c = 0; c < 2; c++) {
        for(int piece = 0; piece < 6; piece++) {
            uint64_t bitboard = state.pieceBitboards[piece] & state.coloredBitboards[c];

            while(bitboard != 0) {
                int index = popLSB(bitboard);
                int totalPiece = 8 * c + piece;
                activateFeatureSingle(index, totalPiece, color);
            }
        }
    }
}

constexpr uint32_t ColorStride = 64 * 6;
constexpr uint32_t PieceStride = 64;


std::pair<uint32_t, uint32_t> PolicyNetworkState::getFeatureIndices(int square, int piece) {
    return {getFeatureIndex(square, piece, 0), getFeatureIndex(square, piece, 1)};
}

int PolicyNetworkState::getFeatureIndex(int square, int piece, int color) {
    int c = getColor(piece) == color ? 0 : 1;
    if(color == 0) {
        square ^= 56;
    }
    return c * ColorStride + getType(piece) * PieceStride + square;
}

void PolicyNetworkState::activateFeature(int square, int piece){ 
    activateFeatureSingle(square, piece, 0);
    activateFeatureSingle(square, piece, 1);
}

void PolicyNetworkState::activateFeatureSingle(int square, int piece, int color){ 
    const int index = getFeatureIndex(square, piece, color);

    // change values for all of them
    if(color == 0) {
        for(int i = 0; i < p_l1Size; ++i) {
            stack[current].black[i] += p_network->featureWeights[index * p_l1Size + i];
        }
    } else {
        for(int i = 0; i < p_l1Size; ++i) {
            stack[current].white[i] += p_network->featureWeights[index * p_l1Size + i];
        }
    }
}

void PolicyNetworkState::activateFeatureAndPush(int square, int piece){ 
    const auto [blackIdx, whiteIdx] = getFeatureIndices(square, piece);

    // change values for all of them
    for(int i = 0; i < p_l1Size; ++i) {
        stack[current + 1].black[i] = stack[current].black[i] + p_network->featureWeights[blackIdx * p_l1Size + i];
        stack[current + 1].white[i] = stack[current].white[i] + p_network->featureWeights[whiteIdx * p_l1Size + i];
    }
    current++;
}

void PolicyNetworkState::disableFeature(int square, int piece) {
    disableFeatureSingle(square, piece, 0);
    disableFeatureSingle(square, piece, 1);
}
void PolicyNetworkState::disableFeatureSingle(int square, int piece, int color) {
    const int index = getFeatureIndex(square, piece, color);

    // change values for all of them
    if(color == 0) {
        for(int i = 0; i < p_l1Size; ++i) {
            stack[current].black[i] -= p_network->featureWeights[index * p_l1Size + i];
        }
    } else {
        for(int i = 0; i < p_l1Size; ++i) {
            stack[current].white[i] -= p_network->featureWeights[index * p_l1Size + i];
        }
    }
}

// woooo softmax yayyyy
std::array<float, 256> PolicyNetworkState::labelMoves(const std::array<Move, 256> &moves, int moveCount, int ctm, const Board &board) const {
    std::array<float, 256> result = {};
    // pairwise multiply both perspective HLs
    std::array<float, p_l1Size> us = (ctm == 0) ? stack[current].black : stack[current].white;
    std::array<float, p_l1Size> them = (ctm == 0) ? stack[current].white : stack[current].black;

    // get each move scores
    float sum = 0;
    for(int i = 0; i < moveCount; i++) {
        float thing = evaluateMove(moves[i], board, std::span(us), std::span(them));
        std::cout << toLongAlgebraic(moves[i]) << " : " << thing << std::endl;
        result[i] = exp(evaluateMove(moves[i], board, std::span(us), std::span(them)));
        sum += result[i];
    }
    // softmax
    for(int i = 0; i < moveCount; i++) {
        result[i] /= sum;
    }
    return result;
}

float PolicyNetworkState::forward(const int move_idx, const std::span<float, p_l1Size> us, const std::span<float, p_l1Size> them, const std::span<const float, p_l1Size * p_outputCount * 2> weights) const {
    float sum = 0;
    int move_offset = p_l1Size * move_idx;

    for(int i = 0; i < p_l1Size; ++i)
    {
        float activated = std::clamp(us[i], 0.0f, 1.0f);
        sum += activated * weights[move_offset + i];
    }

    for(int i = 0; i < p_l1Size; ++i)
    {
        float activated = std::clamp(them[i], 0.0f, 1.0f);
        sum += activated * weights[move_offset + p_l1Size + i];
    }

    return sum;
}

float PolicyNetworkState::evaluateMove(Move move, const Board &board, const std::span<float, p_l1Size> us, const std::span<float, p_l1Size> them) const {
    int move_idx = map_move_to_index(board, move);
    const auto output = forward(move_idx, us, them, p_network->outputWeights);
    return output + p_network->outputBiases[move_idx];
}


