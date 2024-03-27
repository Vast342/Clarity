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
#pragma once

#include "globals.h"

// Current Net: clarity_net010
// Arch: (768->768)x2->1x8 
constexpr int inputSize = 768;
constexpr int layer1Size = 768;
constexpr int outputBucketCount = 8;

// organizing this somewhat similarly to code I've seen, mostly from clarity_sp_nnue, made by Ciekce.

struct Network {
    alignas(32) std::array<std::int16_t, inputSize * layer1Size> featureWeights;
    alignas(32) std::array<std::int16_t, layer1Size> featureBiases;
    alignas(32) std::array<std::int16_t, layer1Size * 2 * outputBucketCount> outputWeights;
    alignas(32) std::array<std::int16_t, outputBucketCount> outputBiases;
};

struct Accumulator {
    alignas(32) std::array<std::int16_t, layer1Size> black;
    alignas(32) std::array<std::int16_t, layer1Size> white;
    void initialize(std::span<const std::int16_t, layer1Size> bias);
};

class NetworkState {
    public:
        NetworkState() {
            stack.reserve(256);
        }
        inline void push() {
            stack.push_back(*current);
            current = &stack.back();
        }
        inline void pop() {
            stack.pop_back();
            current = &stack.back();
        }
        void reset();
        void activateFeature(int square, int type);
        void disableFeature(int square, int type);
        int evaluate(int colorToMove, int materialCount);
    private:
        Accumulator *current;
        std::vector<Accumulator> stack;
        static std::pair<uint32_t, uint32_t> getFeatureIndices(int square, int type);
        int forward(const int bucket, const std::span<std::int16_t, layer1Size> us, const std::span<std::int16_t, layer1Size> them, const std::span<const std::int16_t, layer1Size * 2 * outputBucketCount> weights);
};
