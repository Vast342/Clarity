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
#pragma once

#include "globals.h"


constexpr int p_inputSize = 768;
constexpr int p_l1Size = 128;
constexpr int p_outputCount = 1880;

struct PolicyNetwork {
    alignas(32) std::array<float, p_l1Size * p_inputSize> featureWeights;
    alignas(32) std::array<float, p_l1Size> featureBiases;
    alignas(32) std::array<float, p_outputCount * p_l1Size> outputWeights;
    alignas(32) std::array<float, p_outputCount> outputBiases;
};

struct PolicyAccumulator {
    alignas(32) std::array<float, p_l1Size> black;
    alignas(32) std::array<float, p_l1Size> white;
    void initialize(std::span<const float, p_l1Size> bias);
};

class PolicyNetworkState {
    public:
        PolicyNetworkState() {
            stack.resize(128);
            reset();
        }
        inline void push() {
            stack[current + 1] = stack[current];
            current++;
        }
        void performUpdates(NetworkUpdates updates);
        void performUpdatesAndPush(NetworkUpdates updates);
        inline void pop() {
            current--;
        }
        void reset();
        void activateFeature(int square, int type);
        void activateFeatureAndPush(int square, int type);
        void disableFeature(int square, int type);
        float evaluateMove(Move move, const Board &board, const std::span<float, p_l1Size / 2> us, const std::span<float, p_l1Size / 2> them) const;
        void fullRefresh(const BoardState &state);
        std::array<float, 256> labelMoves(const std::array<Move, 256> &moves, int moveCount, int ctm, const Board &board) const;
    private:
        int current;
        std::vector<PolicyAccumulator> stack;
        static std::pair<uint32_t, uint32_t> getFeatureIndices(int square, int type);
        static int getFeatureIndex(int square, int type, int color);
        float forward(const int move_idx, const std::span<float, p_l1Size / 2> us, const std::span<float, p_l1Size / 2> them, const std::span<const float, p_l1Size * p_outputCount> weights) const;
};

constexpr std::array<float, p_l1Size / 2> pairwiseMultiplication(const std::array<float, p_l1Size>& input) {
    std::array<float, p_l1Size / 2> result;
    constexpr size_t half = p_l1Size / 2;

    for (size_t i = 0; i < half; ++i) {
        result[i] = input[i] * input[i + half];
    }

    return result;
}