/*
    Clarity
    Copyright (C) 2023 Joseph Pasfield

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
    currentAccumulator.initialize(network->featureBiases);
}

void Accumulator::initialize(std::span<const int16_t, layer1Size> bias) {
    std::copy(bias.begin(), bias.end(), black.begin());
    std::copy(bias.begin(), bias.end(), white.begin());
}

constexpr uint32_t ColorStride = 64 * 6;
constexpr uint32_t PieceStride = 64;
constexpr int Scale = 400;
constexpr int Qa = 255;
constexpr int Qb = 64;
constexpr int Qab = Qa * Qb;
constexpr int weightsPerVector = sizeof(Vector) / sizeof(int16_t);


std::pair<uint32_t, uint32_t> NetworkState::getFeatureIndices(int square, int type) {
    assert(type != None);
    assert(square != 64);

    const uint32_t pieceType = static_cast<uint32_t>(getType(type));
    const uint32_t color = getColor(type) == 1 ? 0 : 1;

    const uint32_t blackIdx = !color * ColorStride + pieceType * PieceStride + (static_cast<uint32_t>(flipIndex(square)));
    const uint32_t whiteIdx =  color * ColorStride + pieceType * PieceStride +  static_cast<uint32_t>(square)        ;

    return {blackIdx, whiteIdx};
}

// SCReLU!
int NetworkState::forward(const std::span<int16_t, layer1Size> us, const std::span<int16_t, layer1Size> them, const std::array<int16_t, layer1Size * 2> weights) {
    Vector sum = zeroVector();
    Vector vector0, vector1;

    for(int i = 0; i < layer1Size / weightsPerVector; ++i)
    {
        // us
        vector0 = _mm256_max_epi16(_mm256_min_epi16(_mm256_load_si256(reinterpret_cast<const Vector *>(&us[i * weightsPerVector])), _mm256_set1_epi16(Qa)), _mm256_setzero_si256());
        vector1 = _mm256_mullo_epi16(vector0, _mm256_load_si256(reinterpret_cast<const Vector *>(&weights[i * weightsPerVector])));
        vector1 = _mm256_madd_epi16(vector0, vector1);
        sum = _mm256_add_epi32(sum, vector1);
        
        // them
        vector0 = _mm256_max_epi16(_mm256_min_epi16(_mm256_load_si256(reinterpret_cast<const Vector *>(&them[i * weightsPerVector])), _mm256_set1_epi16(Qa)), _mm256_setzero_si256());
        vector1 = _mm256_mullo_epi16(vector0, _mm256_load_si256(reinterpret_cast<const Vector *>(&weights[layer1Size + i * weightsPerVector])));
        vector1 = _mm256_madd_epi16(vector0, vector1);
        sum = _mm256_add_epi32(sum, vector1);
    }

    return vectorSum(sum);
}
void NetworkState::activateFeature(int square, int type){ 
    const auto [blackIdx, whiteIdx] = getFeatureIndices(square, type);

    // change values for all of them
    for(int i = 0; i < layer1Size; ++i) {
        currentAccumulator.black[i] += network->featureWeights[blackIdx * layer1Size + i];
        currentAccumulator.white[i] += network->featureWeights[whiteIdx * layer1Size + i];
    }
}

void NetworkState::disableFeature(int square, int type) {
    const auto [blackIdx, whiteIdx] = getFeatureIndices(square, type);

    // change values for all of them
    for(int i = 0; i < layer1Size; ++i) {
        currentAccumulator.black[i] -= network->featureWeights[blackIdx * layer1Size + i];
        currentAccumulator.white[i] -= network->featureWeights[whiteIdx * layer1Size + i];
    }
}

int NetworkState::evaluate(int colorToMove) {
    const auto output = colorToMove == 0 ? forward(currentAccumulator.black, currentAccumulator.white, network->outputWeights) : forward(currentAccumulator.white, currentAccumulator.black, network->outputWeights);
    return (output / Qa + network->outputBias) * Scale / Qab;
}
