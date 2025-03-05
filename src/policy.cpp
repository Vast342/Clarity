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
    refreshTable.init();

    stack[current].initialize(p_network->featureBiases);
}

void PolicyNetworkState::performUpdates(NetworkUpdates updates, int blackKing, int whiteKing, const BoardState &state) {
    assert(updates.numAdds <= 2);
    assert(updates.numSubs <= 2);
    if(updates.policyBucketChange) {
        for(int i = 0; i < updates.numAdds; i++) {
            activateFeatureSingle(updates.adds[i].square, updates.adds[i].piece, 1 - updates.policyBucketUpdate.piece, updates.policyBucketUpdate.piece == 1 ? blackKing : whiteKing);
        }
        for(int i = 0; i < updates.numSubs; i++) {
            disableFeatureSingle(updates.subs[i].square, updates.subs[i].piece, 1 - updates.policyBucketUpdate.piece, updates.policyBucketUpdate.piece == 1 ? blackKing : whiteKing);
        }
        refreshAccumulator(updates.policyBucketUpdate.piece /*being used here to store color*/, state, updates.policyBucketUpdate.piece == 0 ? blackKing : whiteKing);
    } else {
        for(int i = 0; i < updates.numAdds; i++) {
            activateFeature(updates.adds[i].square, updates.adds[i].piece, blackKing, whiteKing);
        }
        for(int i = 0; i < updates.numSubs; i++) {
            disableFeature(updates.subs[i].square, updates.subs[i].piece, blackKing, whiteKing);                
        }
    }
}
void PolicyNetworkState::performUpdatesAndPush(NetworkUpdates updates, int blackKing, int whiteKing, const BoardState &state) {
    assert(updates.numAdds <= 2);
    assert(updates.numSubs <= 2);
    if(updates.policyBucketChange) {
        if(updates.policyBucketUpdate.piece == 0) {
            stack[current + 1].white = stack[current].white; 
        } else {
            stack[current + 1].black = stack[current].black;
        }
        current++;
        for(int i = 0; i < updates.numAdds; i++) {
            activateFeatureSingle(updates.adds[i].square, updates.adds[i].piece, 1 - updates.policyBucketUpdate.piece, updates.policyBucketUpdate.piece == 1 ? blackKing : whiteKing);
        }
        for(int i = 0; i < updates.numSubs; i++) {
            disableFeatureSingle(updates.subs[i].square, updates.subs[i].piece, 1 - updates.policyBucketUpdate.piece, updates.policyBucketUpdate.piece == 1 ? blackKing : whiteKing);
        }
        refreshAccumulator(updates.policyBucketUpdate.piece /*being used here to store color*/, state, updates.policyBucketUpdate.piece == 0 ? blackKing : whiteKing);
    } else {
        activateFeatureAndPush(updates.adds[0].square, updates.adds[0].piece, blackKing, whiteKing);
        for(int i = 1; i < updates.numAdds; i++) {
            activateFeature(updates.adds[i].square, updates.adds[i].piece, blackKing, whiteKing);
        }
        for(int i = 0; i < updates.numSubs; i++) {
            disableFeature(updates.subs[i].square, updates.subs[i].piece, blackKing, whiteKing);
        }
    }
}

void PolicyAccumulator::initialize(std::span<const float, p_l1Size> bias) {
    std::copy(bias.begin(), bias.end(), black.begin());
    std::copy(bias.begin(), bias.end(), white.begin());
}
void PolicyAccumulator::initHalf(std::span<const float, p_l1Size> bias, int color) {
    std::copy(bias.begin(), bias.end(), color == 0 ? black.begin() : white.begin());
}

int p_getIBucket(int king) {
    return king % 8 > 3;
}

void PolicyNetworkState::refreshAccumulator(int color, const BoardState &state, int king) {
    const int bucket = p_getIBucket(king);

    PolicyRefreshTableEntry &entry = refreshTable.table[bucket];
    BoardState &prevBoards = entry.colorBoards(color);

    for(int piece = 0; piece < None; ++piece) {
        for(int c = 0; c < 2; c++) {
            const uint64_t prev = prevBoards.pieceBitboards[piece] & prevBoards.coloredBitboards[c];
            const uint64_t curr = state.pieceBitboards[piece] & state.coloredBitboards[c];

            uint64_t added = curr & ~prev;
            uint64_t removed = prev & ~curr;
            //std::cout << "added:  " << added << std::endl;
            //std::cout << "removed: " << removed << std::endl;

            while(added) {
                const int sq = popLSB(added);
                const int index = getFeatureIndex(sq, c * 8 + piece, color, king);
                //std::cout << "sq " << sq << " p " << p << std::endl;
                // change values for all of them
                if(color == 0) {
                    for(int i = 0; i < p_l1Size; ++i) {
                        entry.accumulator.black[i] += p_network->featureWeights[index * p_l1Size + i];
                    }
                } else {
                    for(int i = 0; i < p_l1Size; ++i) {
                        entry.accumulator.white[i] += p_network->featureWeights[index * p_l1Size + i];
                    }
                }
            }

            while(removed) {
                const int sq = popLSB(removed);
                const int index = getFeatureIndex(sq, c * 8 + piece, color, king);
                //std::cout << "sq " << sq << " p " << p << std::endl;
                // change values for all of them
                if(color == 0) {
                    for(int i = 0; i < p_l1Size; ++i) {
                        entry.accumulator.black[i] -= p_network->featureWeights[index * p_l1Size + i];
                    }
                } else {
                    for(int i = 0; i < p_l1Size; ++i) {
                        entry.accumulator.white[i] -= p_network->featureWeights[index * p_l1Size + i];
                    }
                }
            }
        }
    }
    if(color == 0) {
        std::memcpy(&stack[current].black, &entry.accumulator.black, sizeof(std::array<std::int16_t, p_l1Size>));
    } else {
        std::memcpy(&stack[current].white, &entry.accumulator.white, sizeof(std::array<std::int16_t, p_l1Size>));
    }
    std::memcpy(&prevBoards, &state, sizeof(BoardState));
}

void PolicyNetworkState::fullRefresh(const BoardState &state, int blackKing, int whiteKing) {
    halfRefresh(0, state, blackKing);
    halfRefresh(1, state, whiteKing);
}

void PolicyNetworkState::halfRefresh(int color, const BoardState &state, int king) {
    stack[current].initHalf(p_network->featureBiases, color);

    for(int c = 0; c < 2; c++) {
        for(int piece = 0; piece < 6; piece++) {
            uint64_t bitboard = state.pieceBitboards[piece] & state.coloredBitboards[c];

            while(bitboard != 0) {
                int index = popLSB(bitboard);
                int totalPiece = 8 * c + piece;
                activateFeatureSingle(index, totalPiece, color, king);
            }
        }
    }
}

void PolicyRefreshTable::init() {
    table.clear();
    table.resize(2);
    for(int i = 0; i < 2; i++) {
        table[i].accumulator.initialize(p_network->featureBiases);
        std::memset(table[i].boards.data(), 0, sizeof(BoardState) * 2);
    }
}

constexpr uint32_t ColorStride = 64 * 6;
constexpr uint32_t PieceStride = 64;


std::pair<uint32_t, uint32_t> PolicyNetworkState::getFeatureIndices(int square, int piece, int blackKing, int whiteKing) {
    return {getFeatureIndex(square, piece, 0, blackKing), getFeatureIndex(square, piece, 1, whiteKing)};
}

int PolicyNetworkState::getFeatureIndex(int square, int piece, int color, int king) {
    int c = getColor(piece) == color ? 0 : 1;
    if(color == 0) {
        square ^= 56;
    }
    if(king % 8 > 3) {
        square ^= 7;
    }
    return c * ColorStride + getType(piece) * PieceStride + square;
}

void PolicyNetworkState::activateFeature(int square, int piece, int blackKing, int whiteKing){ 
    activateFeatureSingle(square, piece, 0, blackKing);
    activateFeatureSingle(square, piece, 1, whiteKing);
}

void PolicyNetworkState::activateFeatureSingle(int square, int piece, int color, int king){ 
    const int index = getFeatureIndex(square, piece, color, king);

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

void PolicyNetworkState::activateFeatureAndPush(int square, int piece, int blackKing, int whiteKing){ 
    const auto [blackIdx, whiteIdx] = getFeatureIndices(square, piece, blackKing, whiteKing);

    // change values for all of them
    for(int i = 0; i < p_l1Size; ++i) {
        stack[current + 1].black[i] = stack[current].black[i] + p_network->featureWeights[blackIdx * p_l1Size + i];
        stack[current + 1].white[i] = stack[current].white[i] + p_network->featureWeights[whiteIdx * p_l1Size + i];
    }
    current++;
}

void PolicyNetworkState::disableFeature(int square, int piece, int blackKing, int whiteKing) {
    disableFeatureSingle(square, piece, 0, blackKing);
    disableFeatureSingle(square, piece, 1, whiteKing);
}
void PolicyNetworkState::disableFeatureSingle(int square, int piece, int color, int king) {
    const int index = getFeatureIndex(square, piece, color, king);

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

std::array<float, p_l1Size / 2> pairwise(const std::array<float, p_l1Size>& input) {
    std::array<float, p_l1Size / 2> result;
    constexpr size_t half = p_l1Size / 2;

    for (size_t i = 0; i < half; ++i) {
        result[i] = input[i] * input[i + half];
    }

    return result;
}

// woooo softmax yayyyy
std::array<float, 256> PolicyNetworkState::labelMoves(const std::array<Move, 256> &moves, int moveCount, int ctm, const Board &board) const {
    std::array<float, 256> result = {};
    // pairwise multiply both perspective HLs
    const auto black = pairwise(stack[current].black);
    const auto white = pairwise(stack[current].white);
    std::array<float, p_l1Size / 2> us = (ctm == 0) ? black : white;
    std::array<float, p_l1Size / 2> them = (ctm == 0) ? white : black;

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

float PolicyNetworkState::forward(const int move_idx, const std::span<float, p_l1Size / 2> us, const std::span<float, p_l1Size / 2> them, const std::span<const float, p_l1Size * p_outputCount> weights) const {
    float sum = 0;

    for(int i = 0; i < p_l1Size / 2; ++i)
    {
        float activated = std::clamp(us[i], 0.0f, 1.0f);
        sum += activated * weights[i * p_l1Size + move_idx];
    }

    for(int i = 0; i < p_l1Size / 2; ++i)
    {
        float activated = std::clamp(them[i], 0.0f, 1.0f);
        sum += activated * weights[i * p_l1Size + p_l1Size / 2 + move_idx];
    }

    return sum;
}

float PolicyNetworkState::evaluateMove(Move move, const Board &board, const std::span<float, p_l1Size / 2> us, const std::span<float, p_l1Size / 2> them) const {
    int move_idx = map_move_to_index(board, move);
    const auto output = forward(move_idx, us, them, p_network->outputWeights);
    return output + p_network->outputBiases[move_idx];
}


