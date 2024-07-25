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
#include "immintrin.h"

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
    std::memset(stack.data(), 0, sizeof(Accumulator) * stack.size());
    std::memset(&refreshTable, 0, sizeof(RefreshTable));
    current = 0;
    refreshTable.init();

    stack[current].initialize(network->featureBiases);
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

void Accumulator::initialize(std::span<const int16_t, layer1Size> bias) {
    std::copy(bias.begin(), bias.end(), black.begin());
    std::copy(bias.begin(), bias.end(), white.begin());
}
void Accumulator::initHalf(std::span<const int16_t, layer1Size> bias, int color) {
    std::copy(bias.begin(), bias.end(), color == 0 ? black.begin() : white.begin());
}

int getIBucket(int color, int king) {
    if(color == 0) {
        king ^= 56;
    }
    if(king % 8 > 3) {
        king ^= 7;
    }
    return inputBuckets[king];
}

void NetworkState::refreshAccumulator(int color, const BoardState &state, int king) {
    const int bucket = inputBuckets[king];

    RefreshTableEntry &entry = refreshTable.table[bucket];
    BoardState &prevBoards = entry.colorBoards(color);
    //entry.accumulator.initHalf(network->featureBiases, color);

    for(int piece = 0; piece < None; ++piece) {
        const uint64_t prev = prevBoards.pieceBitboards[piece];
        const uint64_t curr = state.pieceBitboards[piece];

        uint64_t added = curr & ~prev;
        uint64_t removed = prev & ~curr;
        //std::cout << "added:  " << added << std::endl;
        //std::cout << "removed: " << removed << std::endl;

        while(added) {
            const int sq = popLSB(added);
            const int p = state.mailbox[sq];
            const int index = getFeatureIndex(sq, p, color, king);
            // change values for all of them
            if(color == 0) {
                for(int i = 0; i < layer1Size; ++i) {
                    entry.accumulator.black[i] += network->featureWeights[index * layer1Size + i];
                }
            } else {
                for(int i = 0; i < layer1Size; ++i) {
                    entry.accumulator.white[i] += network->featureWeights[index * layer1Size + i];
                }
            }
        }

        while(removed) {
            const int sq = popLSB(removed);
            const int p = prevBoards.mailbox[sq];
            const int index = getFeatureIndex(sq, p, color, king);
            // change values for all of them
            if(color == 0) {
                for(int i = 0; i < layer1Size; ++i) {
                    entry.accumulator.black[i] -= network->featureWeights[index * layer1Size + i];
                }
            } else {
                for(int i = 0; i < layer1Size; ++i) {
                    entry.accumulator.white[i] -= network->featureWeights[index * layer1Size + i];
                }
            }
        }
    }
    if(color == 0) {
        std::memcpy(&stack[current].black, &entry.accumulator.black, sizeof(std::array<std::int16_t, layer1Size>));
    } else {
        std::memcpy(&stack[current].white, &entry.accumulator.white, sizeof(std::array<std::int16_t, layer1Size>));
    }
    prevBoards = state;
}

void RefreshTable::init() {
    for(int i = 0; i < inputBucketCount * 2; i++) {
        table[i].accumulator.initialize(network->featureBiases);
        std::memset(table[i].boards.data(), 0, sizeof(BoardState) * 2);
    }
}

constexpr uint32_t ColorStride = 64 * 6;
constexpr uint32_t PieceStride = 64;
constexpr int Scale = 400;
constexpr int Qa = 256;
constexpr int Qb = 64;
constexpr int Qab = Qa * Qb;


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

/*
    A technique that I am using here was invented yesterday (as of writing this) by SomeLizard, developer of the engine Lizard
    I am using the SCReLU activation function, which is CReLU(x)^2 * W
    the technique is to use CReLU(x) * w * CReLU(x) which allows you (assuming weight is in (-127, 127))
    to fit the resulting number in a 16 bit integer, allowing you to perform the remaining functions
    on twice as many numbers at once, leading to a pretty sizeable speedup
*/

#if defined(__AVX512F__) && defined(__AVX512BW__)
using Vector = __m512i;
constexpr int weightsPerVector = sizeof(Vector) / sizeof(int16_t);
// SCReLU!
int NetworkState::forward(const int bucket, const std::span<int16_t, layer1Size> us, const std::span<int16_t, layer1Size> them, const std::span<const int16_t, layer1Size * 2 * outputBucketCount> weights) {
    Vector sum = _mm512_setzero_si512();
    Vector vector0, vector1;
    const int bucketIncrement = 2 * layer1Size * bucket;

    for(int i = 0; i < layer1Size / weightsPerVector; ++i)
    {
        // us
        vector0 = _mm512_max_epi16(_mm512_min_epi16(_mm512_load_si512(reinterpret_cast<const Vector *>(&us[i * weightsPerVector])), _mm512_set1_epi16(Qa)), _mm512_setzero_si512());
        vector1 = _mm512_mullo_epi16(vector0, _mm512_load_si512(reinterpret_cast<const Vector *>(&weights[i * weightsPerVector + bucketIncrement])));
        vector1 = _mm512_madd_epi16(vector0, vector1);
        sum = _mm512_add_epi32(sum, vector1);
        
        // them
        vector0 = _mm512_max_epi16(_mm512_min_epi16(_mm512_load_si512(reinterpret_cast<const Vector *>(&them[i * weightsPerVector])), _mm512_set1_epi16(Qa)), _mm512_setzero_si512());
        vector1 = _mm512_mullo_epi16(vector0, _mm512_load_si512(reinterpret_cast<const Vector *>(&weights[layer1Size + i * weightsPerVector + bucketIncrement])));
        vector1 = _mm512_madd_epi16(vector0, vector1);
        sum = _mm512_add_epi32(sum, vector1);
    }

    return _mm512_reduce_add_epi32(sum);
}

#elif defined(__AVX2__)
using Vector = __m256i;
constexpr int weightsPerVector = sizeof(Vector) / sizeof(int16_t);
// SCReLU!
int NetworkState::forward(const int bucket, const std::span<int16_t, layer1Size> us, const std::span<int16_t, layer1Size> them, const std::span<const int16_t, layer1Size * 2 * outputBucketCount> weights) {
    Vector sum = _mm256_setzero_si256();
    Vector vector0, vector1;
    const int bucketIncrement = 2 * layer1Size * bucket;

    for(int i = 0; i < layer1Size / weightsPerVector; ++i)
    {
        // us
        vector0 = _mm256_max_epi16(_mm256_min_epi16(_mm256_load_si256(reinterpret_cast<const Vector *>(&us[i * weightsPerVector])), _mm256_set1_epi16(Qa)), _mm256_setzero_si256());
        vector1 = _mm256_mullo_epi16(vector0, _mm256_load_si256(reinterpret_cast<const Vector *>(&weights[i * weightsPerVector + bucketIncrement])));
        vector1 = _mm256_madd_epi16(vector0, vector1);
        sum = _mm256_add_epi32(sum, vector1);
        
        // them
        vector0 = _mm256_max_epi16(_mm256_min_epi16(_mm256_load_si256(reinterpret_cast<const Vector *>(&them[i * weightsPerVector])), _mm256_set1_epi16(Qa)), _mm256_setzero_si256());
        vector1 = _mm256_mullo_epi16(vector0, _mm256_load_si256(reinterpret_cast<const Vector *>(&weights[layer1Size + i * weightsPerVector + bucketIncrement])));
        vector1 = _mm256_madd_epi16(vector0, vector1);
        sum = _mm256_add_epi32(sum, vector1);
    }

    __m128i sum0;
    __m128i sum1;
    // divide into halves
    sum0 = _mm256_castsi256_si128(sum);
    sum1 = _mm256_extracti128_si256(sum, 1);
    // add the halves
    sum0 = _mm_add_epi32(sum0, sum1);
    // get half of the result
    sum1 = _mm_unpackhi_epi64(sum0, sum0);
    // add the halves:
    sum0 = _mm_add_epi32(sum0, sum1);
    // reorder so it's right
    sum1 = _mm_shuffle_epi32(sum0, _MM_SHUFFLE(2, 3, 0, 1));
    // final add
    sum0 = _mm_add_epi32(sum0, sum1);
    // cast back to int
    return _mm_cvtsi128_si32(sum0);
}
#else
using Vector = __m128i;
constexpr int weightsPerVector = sizeof(Vector) / sizeof(int16_t);
// SCReLU!
int NetworkState::forward(const int bucket, const std::span<int16_t, layer1Size> us, const std::span<int16_t, layer1Size> them, const std::span<const int16_t, layer1Size * 2 * outputBucketCount> weights) {
    Vector sum = _mm_setzero_si128();
    Vector vector0, vector1;
    const int bucketIncrement = 2 * layer1Size * bucket;

    for(int i = 0; i < layer1Size / weightsPerVector; ++i)
    {
        // us
        vector0 = _mm_max_epi16(_mm_min_epi16(_mm_load_si128(reinterpret_cast<const Vector *>(&us[i * weightsPerVector])), _mm_set1_epi16(Qa)), _mm_setzero_si128());
        vector1 = _mm_mullo_epi16(vector0, _mm_load_si128(reinterpret_cast<const Vector *>(&weights[i * weightsPerVector + bucketIncrement])));
        vector1 = _mm_madd_epi16(vector0, vector1);
        sum = _mm_add_epi32(sum, vector1);
        
        // them
        vector0 = _mm_max_epi16(_mm_min_epi16(_mm_load_si128(reinterpret_cast<const Vector *>(&them[i * weightsPerVector])), _mm_set1_epi16(Qa)), _mm_setzero_si128());
        vector1 = _mm_mullo_epi16(vector0, _mm_load_si128(reinterpret_cast<const Vector *>(&weights[layer1Size + i * weightsPerVector + bucketIncrement])));
        vector1 = _mm_madd_epi16(vector0, vector1);
        sum = _mm_add_epi32(sum, vector1);
    }

    const auto high64 = _mm_unpackhi_epi64(sum, sum);
    const auto sum64 = _mm_add_epi32(sum, high64);

    const auto high32 = _mm_shuffle_epi32(sum64, _MM_SHUFFLE(2, 3, 0, 1));
    const auto sum32 = _mm_add_epi32(sum64, high32);

    return _mm_cvtsi128_si32(sum32);
}

#endif
void NetworkState::activateFeature(int square, int piece, int blackKing, int whiteKing){ 
    activateFeatureSingle(square, piece, 0, blackKing);
    activateFeatureSingle(square, piece, 1, whiteKing);
}

void NetworkState::activateFeatureSingle(int square, int piece, int color, int king){ 
    const int index = getFeatureIndex(square, piece, color, king);

    // change values for all of them
    if(color == 0) {
        for(int i = 0; i < layer1Size; ++i) {
            stack[current].black[i] += network->featureWeights[index * layer1Size + i];
        }
    } else {
        for(int i = 0; i < layer1Size; ++i) {
            stack[current].white[i] += network->featureWeights[index * layer1Size + i];
        }
    }
}

void NetworkState::activateFeatureAndPush(int square, int piece, int blackKing, int whiteKing){ 
    const auto [blackIdx, whiteIdx] = getFeatureIndices(square, piece, blackKing, whiteKing);

    // change values for all of them
    for(int i = 0; i < layer1Size; ++i) {
        stack[current + 1].black[i] = stack[current].black[i] + network->featureWeights[blackIdx * layer1Size + i];
        stack[current + 1].white[i] = stack[current].white[i] + network->featureWeights[whiteIdx * layer1Size + i];
    }
    current++;
}

void NetworkState::disableFeature(int square, int piece, int blackKing, int whiteKing) {
    disableFeatureSingle(square, piece, 0, blackKing);
    disableFeatureSingle(square, piece, 1, whiteKing);
}
void NetworkState::disableFeatureSingle(int square, int piece, int color, int king) {
    const int index = getFeatureIndex(square, piece, color, king);

    // change values for all of them
    if(color == 0) {
        for(int i = 0; i < layer1Size; ++i) {
            stack[current].black[i] -= network->featureWeights[index * layer1Size + i];
        }
    } else {
        for(int i = 0; i < layer1Size; ++i) {
            stack[current].white[i] -= network->featureWeights[index * layer1Size + i];
        }
    }
}

int NetworkState::evaluate(int colorToMove, int materialCount) {
    const int bucket = getBucket(materialCount);
    const auto output = colorToMove == 0 ? forward(bucket, stack[current].black, stack[current].white, network->outputWeights) : forward(bucket, stack[current].white, stack[current].black, network->outputWeights);
    return (output / Qa + network->outputBiases[bucket]) * Scale / Qab;
}
