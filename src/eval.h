#pragma once

#include "globals.h"

constexpr int inputSize = 768;
constexpr int layer1Size = 128;

// organizing this somewhat similarly to code I've seen, mostly from clarity_sp_nnue, made by Ciekce.

struct Network {
    alignas(32) std::array<std::int16_t, inputSize * layer1Size> featureWeights;
    alignas(32) std::array<std::int16_t, layer1Size> featureBiases;
    alignas(32) std::array<std::int16_t, layer1Size * 2> outputWeights;
    std::int16_t outputBias;
};

struct Accumulator {
    alignas(32) std::array<std::int16_t, layer1Size> black;
    alignas(32) std::array<std::int16_t, layer1Size> white;
    void initialize(std::span<const std::int16_t, layer1Size> bias);
};

class NetworkState {
    public:
        NetworkState();
        void push();
        void pop();
        void reset();
        void activateFeature(int square, int type);
        void disableFeature(int square, int type);
        int evaluate(int colorToMove);
    private:
        std::vector<Accumulator> accumulatorStates;
        Accumulator *currentAccumulator;
        static std::pair<uint32_t, uint32_t> getFeatureIndices(int square, int type);
        int forward(const std::span<std::int16_t, layer1Size> us, const std::span<std::int16_t, layer1Size> them, const std::array<std::int16_t, layer1Size * 2> weights);
};