/*
    Clarity
    Copyright (C) 2026 Joseph Pasfield

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
#include "immintrin.h"
#include "simd.h"

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
    INCBIN(network, NetworkFile);
    const Network *network = reinterpret_cast<const Network *>(g_networkData);
}

void NetworkState::reset() {
    std::fill(stack.begin(), stack.end(), Accumulator{});
    current = 0;
    refreshTable.init();

    stack[current].initialize(network->l1Biases);
}

void NetworkState::performUpdates(NetworkUpdates updates, int blackKing, int whiteKing, const BoardState &state) {
    assert(updates.numAdds <= 2);
    assert(updates.numSubs <= 2);
    if(updates.bucketChange) {
        for(int i = 0; i < updates.numAdds; i++) {
            activateFeatureSingle(updates.adds[i].square, updates.adds[i].piece, 1 - updates.bucketUpdate.piece, updates.bucketUpdate.piece == 1 ? blackKing : whiteKing);
        }
        for(int i = 0; i < updates.numSubs; i++) {
            disableFeatureSingle(updates.subs[i].square, updates.subs[i].piece, 1 - updates.bucketUpdate.piece, updates.bucketUpdate.piece == 1 ? blackKing : whiteKing);
        }
        refreshAccumulator(updates.bucketUpdate.piece /*being used here to store color*/, state, updates.bucketUpdate.piece == 0 ? blackKing : whiteKing);
    } else {
        for(int i = 0; i < updates.numAdds; i++) {
            activateFeature(updates.adds[i].square, updates.adds[i].piece, blackKing, whiteKing);
        }
        for(int i = 0; i < updates.numSubs; i++) {
            disableFeature(updates.subs[i].square, updates.subs[i].piece, blackKing, whiteKing);                
        }
    }
}
void NetworkState::performUpdatesAndPush(NetworkUpdates updates, int blackKing, int whiteKing, const BoardState &state) {
    assert(updates.numAdds <= 2);
    assert(updates.numSubs <= 2);
    if(updates.bucketChange) {
        if(updates.bucketUpdate.piece == 0) {
            stack[current + 1].white = stack[current].white; 
        } else {
            stack[current + 1].black = stack[current].black;
        }
        current++;
        for(int i = 0; i < updates.numAdds; i++) {
            activateFeatureSingle(updates.adds[i].square, updates.adds[i].piece, 1 - updates.bucketUpdate.piece, updates.bucketUpdate.piece == 1 ? blackKing : whiteKing);
        }
        for(int i = 0; i < updates.numSubs; i++) {
            disableFeatureSingle(updates.subs[i].square, updates.subs[i].piece, 1 - updates.bucketUpdate.piece, updates.bucketUpdate.piece == 1 ? blackKing : whiteKing);
        }
        refreshAccumulator(updates.bucketUpdate.piece /*being used here to store color*/, state, updates.bucketUpdate.piece == 0 ? blackKing : whiteKing);
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

void Accumulator::initialize(std::span<const int16_t, l1Size> bias) {
    std::copy(bias.begin(), bias.end(), black.begin());
    std::copy(bias.begin(), bias.end(), white.begin());
}
void Accumulator::initHalf(std::span<const int16_t, l1Size> bias, int color) {
    std::copy(bias.begin(), bias.end(), color == 0 ? black.begin() : white.begin());
}

int getIBucket(int color, int king) {
    if(color == 0) {
        king ^= 56;
    }
    return inputBuckets[king];
}

void NetworkState::refreshAccumulator(int color, const BoardState &state, int king) {
    const int bucket = getIBucket(color, king);

    RefreshTableEntry &entry = refreshTable.table[bucket];
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
                
                // copying this from the search rewrite, reportedly this was to fix my speed when compiling on windows?
                // I don't know but i should probably bring it back anyway.
                int16_t *__restrict__ acc =
                    (color == 0 ? entry.accumulator.black.data()
                                : entry.accumulator.white.data());

                for(int i = 0; i < l1Size; ++i) {
                    acc[i] += network->l1Weights[index][i];
                }
            }

            while(removed) {
                const int sq = popLSB(removed);
                const int index = getFeatureIndex(sq, c * 8 + piece, color, king);

                int16_t *__restrict__ acc =
                    (color == 0 ? entry.accumulator.black.data()
                                : entry.accumulator.white.data());

                for(int i = 0; i < l1Size; ++i) {
                    acc[i] -= network->l1Weights[index][i];
                }
            }
        }
    }
    if(color == 0) {
        std::memcpy(&stack[current].black, &entry.accumulator.black, sizeof(std::array<int16_t, l1Size>));
    } else {
        std::memcpy(&stack[current].white, &entry.accumulator.white, sizeof(std::array<int16_t, l1Size>));
    }
    std::memcpy(&prevBoards, &state, sizeof(BoardState));
}

void NetworkState::fullRefresh(const BoardState &state, int blackKing, int whiteKing) {
    halfRefresh(0, state, blackKing);
    halfRefresh(1, state, whiteKing);
}

void NetworkState::halfRefresh(int color, const BoardState &state, int king) {
    stack[current].initHalf(network->l1Biases, color);

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

void RefreshTable::init() {
    table.clear();
    table.resize(inputBucketCount * 2);
    for(int i = 0; i < inputBucketCount * 2; i++) {
        table[i].accumulator.initialize(network->l1Biases);
        std::memset(table[i].boards.data(), 0, sizeof(BoardState) * 2);
    }
}

constexpr uint32_t ColorStride = 64 * 6;
constexpr uint32_t PieceStride = 64;
constexpr int Scale = 400;


std::pair<uint32_t, uint32_t> NetworkState::getFeatureIndices(int square, int piece, int blackKing, int whiteKing) {
    return {getFeatureIndex(square, piece, 0, blackKing), getFeatureIndex(square, piece, 1, whiteKing)};
}

int NetworkState::getFeatureIndex(int square, int piece, int color, int king) {
    int c = getColor(piece) == color ? 0 : 1;
    if(color == 0) {
        square ^= 56;
        king ^= 56;
    }
    if(king % 8 > 3) {
        square ^= 7;
        king ^= 7;
    }
    return inputBuckets[king] * inputSize + c * ColorStride + getType(piece) * PieceStride + square;
}

int getBucket(int pieceCount) {
    const int divisor = (32 + outputBucketCount - 1) / outputBucketCount;
    return (pieceCount - 2) / divisor;
}

int64_t NetworkState::forward(const int bucket, const std::span<int16_t, l1Size> us, const std::span<int16_t, l1Size> them) {
    // pairwise crelu!
    // vectors of u8s
    std::array<Vector, l1Size / bytesPerVector> l1AccV;
    const Vector q0Vec = simd_set1_epi16(Q0);
    const Vector zeroVec = simd_zero();
    const auto castUs = std::span<const Vector, l1Size / weightsPerVector>(
        reinterpret_cast<const Vector*>(us.data()),
        l1Size / weightsPerVector);
    const auto castThem = std::span<const Vector, l1Size / weightsPerVector>(
        reinterpret_cast<const Vector*>(them.data()),
        l1Size / weightsPerVector);
    
    #pragma unroll
    for(int i = 0; i < (l1Size / 2) / bytesPerVector; i++) {
        // us
        Vector a = simd_mulhi_epi16(
            simd_min_epi16(
                simd_load(
                    &castUs[2 * i]
                ),
                q0Vec
            ),
            simd_slli_epi16(
                simd_min_epi16(
                    simd_max_epi16(
                        simd_load(
                            &castUs[2 * i + l1Size / weightsPerVector / 2]
                        ), 
                        zeroVec
                    ),
                    q0Vec
                ),
                7
            )
        );
        Vector b = simd_mulhi_epi16(
            simd_min_epi16(
                simd_load(
                    &castUs[2 * i + 1]
                ),
                q0Vec
            ),
            simd_slli_epi16(
                simd_min_epi16(
                    simd_max_epi16(
                        simd_load(
                            &castUs[2 * i + 1 + l1Size / weightsPerVector / 2]
                        ), 
                        zeroVec
                    ),
                    q0Vec
                ),
                7
            )
        );
        l1AccV[i] = simd_packus_unpermuted_epi16(a, b);

        // them
        a = simd_mulhi_epi16(
            simd_min_epi16(
                simd_load(
                    &castThem[2 * i]
                ),
                q0Vec
            ),
            simd_slli_epi16(
                simd_min_epi16(
                    simd_max_epi16(
                        simd_load(
                            &castThem[2 * i + l1Size / weightsPerVector / 2]
                        ), 
                        zeroVec
                    ),
                    q0Vec
                ),
                7
            )
        );
        b = simd_mulhi_epi16(
            simd_min_epi16(
                simd_load(
                    &castThem[2 * i + 1]
                ),
                q0Vec
            ),
            simd_slli_epi16(
                simd_min_epi16(
                    simd_max_epi16(
                        simd_load(
                            &castThem[2 * i + 1 + l1Size / weightsPerVector / 2]
                        ), 
                        zeroVec
                    ),
                    q0Vec
                ),
                7
            )
        );
        l1AccV[i + ((l1Size / 2) / bytesPerVector)] = simd_packus_unpermuted_epi16(a, b);
    }

    // l1 -> l2
    constexpr int i32PerVector = bytesPerVector / 4;

    std::array<Vector, l2Size / i32PerVector> l2AccV{};
    const uint8_t* l1AccBytes = reinterpret_cast<const uint8_t*>(l1AccV.data());

    for(int b = 0; b < l1Size / 4; b++) {
        int32_t actChunk;
        std::memcpy(&actChunk, &l1AccBytes[4 * b], sizeof(int32_t));
        const Vector actVec = simd_set1_epi32(actChunk);
        const auto* weightBlock = reinterpret_cast<const Vector*>(network->l2Weights[bucket][b].data());
        for(int c = 0; c < l2Size / i32PerVector; c++) {
            l2AccV[c] = simd_dpbusd_epi32(l2AccV[c], actVec, simd_load(&weightBlock[c]));
        }
    }

    std::array<Vector, l2Size / i32PerVector> l2AccAct;
    const Vector zeroVecI32 = simd_zero();
    const Vector qVecI32 = simd_set1_epi32(Q);

    for(int c = 0; c < l2Size / i32PerVector; c++) {
        Vector v = simd_srai_epi32(l2AccV[c], 8);
        v = simd_add_epi32(v, simd_load(reinterpret_cast<const Vector*>(&network->l2Biases[bucket][c * i32PerVector])));
        v = simd_max_epi32(v, zeroVecI32);
        v = simd_min_epi32(v, qVecI32);
        l2AccAct[c] = simd_mullo_epi32(v, v);
    }

    std::array<int32_t, l2Size> l2Acc;
    for(int c = 0; c < l2Size / i32PerVector; c++) {
        simd_store(reinterpret_cast<Vector*>(&l2Acc[c * i32PerVector]), l2AccAct[c]);
    }

    // l2 -> l3
    constexpr int i32PerVecL3 = bytesPerVector / 4;
    constexpr int l3Chunks = l3Size / i32PerVecL3;

    std::array<Vector, l3Chunks> l3AccV;
    for(int c = 0; c < l3Chunks; c++) {
        l3AccV[c] = simd_load(reinterpret_cast<const Vector*>(&network->l3Biases[bucket][c * i32PerVecL3]));
    }

    for(int l2Node = 0; l2Node < l2Size; l2Node++) {
        const Vector actVec = simd_set1_epi32(l2Acc[l2Node]);
        const auto* w = reinterpret_cast<const Vector*>(network->l3Weights[bucket][l2Node].data());
        for(int c = 0; c < l3Chunks; c++) {
            l3AccV[c] = simd_add_epi32(l3AccV[c], simd_mullo_epi32(actVec, simd_load(&w[c])));
        }
    }

    // l3 -> output
    Vector accVec = simd_zero();
    const Vector qqqVec = simd_set1_epi32(Q*Q*Q);
    for(int c = 0; c < l3Chunks; c++) {
        Vector v = simd_max_epi32(l3AccV[c], simd_zero());
        v = simd_min_epi32(v, qqqVec);
        const Vector w = simd_load(reinterpret_cast<const Vector*>(&network->outputWeights[bucket][c * i32PerVecL3]));
        accVec = simd_add_epi32(accVec, simd_mullo_epi32(v, w));
    }
    int64_t output = simd_reduce_add_epi32(accVec) + network->outputBiases[bucket];

    return output;
}

void NetworkState::activateFeature(int square, int piece, int blackKing, int whiteKing){ 
    activateFeatureSingle(square, piece, 0, blackKing);
    activateFeatureSingle(square, piece, 1, whiteKing);
}

void NetworkState::activateFeatureSingle(int square, int piece, int color, int king){ 
    const int index = getFeatureIndex(square, piece, color, king);

    int16_t *__restrict__ acc =
        (color == 0 ? stack[current].black.data()
                    : stack[current].white.data());

    for(int i = 0; i < l1Size; ++i) {
        acc[i] += network->l1Weights[index][i];
    }
}

void NetworkState::activateFeatureAndPush(int square, int piece, int blackKing, int whiteKing){ 
    const auto [blackIdx, whiteIdx] = getFeatureIndices(square, piece, blackKing, whiteKing);

    const int16_t *__restrict__ blackPrev = stack[current].black.data();
    const int16_t *__restrict__ whitePrev = stack[current].white.data();

    int16_t *__restrict__ blackNext = stack[current + 1].black.data();
    int16_t *__restrict__ whiteNext = stack[current + 1].white.data();

    for(int i = 0; i < l1Size; ++i) {
        blackNext[i] = blackPrev[i] + network->l1Weights[blackIdx][i];
        whiteNext[i] = whitePrev[i] + network->l1Weights[whiteIdx][i];
    }

    current++;
}

void NetworkState::disableFeature(int square, int piece, int blackKing, int whiteKing) {
    disableFeatureSingle(square, piece, 0, blackKing);
    disableFeatureSingle(square, piece, 1, whiteKing);
}

void NetworkState::disableFeatureSingle(int square, int piece, int color, int king) {
    const int index = getFeatureIndex(square, piece, color, king);

    int16_t *__restrict__ acc =
        (color == 0 ? stack[current].black.data()
                    : stack[current].white.data());

    for(int i = 0; i < l1Size; ++i) {
        acc[i] -= network->l1Weights[index][i];
    }
}

// todo: lazy updates (oh no)
int NetworkState::evaluate(int colorToMove, int materialCount) {
    const int bucket = getBucket(materialCount);
    const auto output = colorToMove == 0 ? forward(bucket, stack[current].black, stack[current].white) : forward(bucket, stack[current].white, stack[current].black);
    return output * int64_t(Scale) / QTo4;
}