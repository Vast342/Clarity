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
#include "globals.h"
#include "eval.h"

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

// this is my first time doing anything with neural networks, or anything with pointers, so code here might be a bit questionable to look at

namespace {
    INCBIN(network, NetworkFile);
    const Network *network = reinterpret_cast<const Network *>(g_networkData);
}

void Accumulator::initialize(std::span<const int16_t, layer1Size> bias) {
    std::copy(bias.begin(), bias.end(), black.begin());
    std::copy(bias.begin(), bias.end(), white.begin());
}

constexpr uint32_t ColorStride = 64 * 6;
constexpr uint32_t PieceStride = 64;
constexpr int Scale = 400;
constexpr int Q = 255 * 64;


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
    int sum = 0;

    for(int i = 0; i < layer1Size; ++i)
    {
        int activated = std::clamp(static_cast<int>(us[i]), 0, 255);
        activated *= activated;
        sum += activated * weights[i];
    }

    for(int i = 0; i < layer1Size; ++i)
    {
        int activated = std::clamp(static_cast<int>(them[i]), 0, 255);
        activated *= activated;
        sum += activated * weights[layer1Size + i];
    }

    return sum / 255;
}

NetworkState::NetworkState() {
    accumulatorStates.reserve(256);
}

void NetworkState::push() {
    accumulatorStates.push_back(*currentAccumulator);
    currentAccumulator = &accumulatorStates.back();
}

void NetworkState::pop() {
    accumulatorStates.pop_back();
    currentAccumulator = &accumulatorStates.back();
}

void NetworkState::reset() {
    accumulatorStates.clear();
    currentAccumulator = &accumulatorStates.emplace_back();
    currentAccumulator->initialize(network->featureBiases);
}

void NetworkState::activateFeature(int square, int type){ 
    const auto [blackIdx, whiteIdx] = getFeatureIndices(square, type);

    // change values for all of them
    for(int i = 0; i < layer1Size; ++i) {
        currentAccumulator->black[i] += network->featureWeights[blackIdx * layer1Size + i];
        currentAccumulator->white[i] += network->featureWeights[whiteIdx * layer1Size + i];
    }
}

void NetworkState::disableFeature(int square, int type) {
    const auto [blackIdx, whiteIdx] = getFeatureIndices(square, type);

    // change values for all of them
    for(int i = 0; i < layer1Size; ++i) {
        currentAccumulator->black[i] -= network->featureWeights[blackIdx * layer1Size + i];
        currentAccumulator->white[i] -= network->featureWeights[whiteIdx * layer1Size + i];
    }
}

int NetworkState::evaluate(int colorToMove) {
    const auto output = colorToMove == 0 ? forward(currentAccumulator->black, currentAccumulator->white, network->outputWeights) : forward(currentAccumulator->white, currentAccumulator->black, network->outputWeights);
    return (output + network->outputBias) * Scale / Q;
}
