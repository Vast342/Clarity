#include "globals.h"
#include "external/incbin.h"
#include "eval.h"

// this is my first time doing anything with neural networks, or anything with pointers, so code here might be a bit questionable to look at

namespace {
    INCBIN(networkData, "F:/ChessEngine/src/clarity_spnet_01.nnue");
    const Network *network = reinterpret_cast<const Network *>(gnetworkDataData);
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

int NetworkState::forward(const std::span<int16_t, layer1Size> us, const std::span<int16_t, layer1Size> them, const std::array<int16_t, layer1Size * 2> weights) {
    int sum = 0;

    for (int i = 0; i < layer1Size; ++i)
    {
        int activated = std::clamp(static_cast<int>(us[i]), 0, 255);
        sum += activated * weights[i];
    }

    for (int i = 0; i < layer1Size; ++i)
    {
        int activated = std::clamp(static_cast<int>(them[i]), 0, 255);
        sum += activated * weights[layer1Size + i];
    }

    return sum;
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
    for (int i = 0; i < layer1Size; ++i) {
        currentAccumulator->white[i] += network->featureWeights[blackIdx * layer1Size + i];
        currentAccumulator->white[i] -= network->featureWeights[whiteIdx * layer1Size + i];
    }
}

void NetworkState::disableFeature(int square, int type) {
    const auto [blackIdx, whiteIdx] = getFeatureIndices(square, type);

    // change values for all of them
    for (int i = 0; i < layer1Size; ++i) {
        currentAccumulator->white[i] -= network->featureWeights[blackIdx * layer1Size + i];
        currentAccumulator->white[i] -= network->featureWeights[whiteIdx * layer1Size + i];
    }
}

int NetworkState::evaluate(int colorToMove) {
    const auto output = colorToMove == 0 ? forward(currentAccumulator->black, currentAccumulator->white, network->outputWeights) : forward(currentAccumulator->white, currentAccumulator->black, network->outputWeights);
    return (output + network->outputBias) * Scale / Q;
}