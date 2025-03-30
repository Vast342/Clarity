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

constexpr float QA = 128;
constexpr float QB = 128;
constexpr float QAB = QA * QB;

struct PolicyNetwork {
    alignas(32) std::array<int8_t, p_l1Size * p_inputSize> featureWeights;
    alignas(32) std::array<int16_t, p_l1Size> featureBiases;
    alignas(32) std::array<int16_t, p_outputCount * p_l1Size> outputWeights;
    alignas(32) std::array<int, p_outputCount> outputBiases;
};

struct PolicyAccumulator {
    alignas(32) std::array<int16_t, p_l1Size> black;
    alignas(32) std::array<int16_t, p_l1Size> white;
    void initialize(std::span<const int16_t, p_l1Size> bias);
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
        float evaluateMove(Move move, const Board &board, const std::span<int, p_l1Size / 2> us, const std::span<int, p_l1Size / 2> them) const;
        void fullRefresh(const BoardState &state);
        std::array<float, 256> labelMoves(const std::array<Move, 256> &moves, int moveCount, int ctm, const Board &board) const;
    private:
        int current;
        std::vector<PolicyAccumulator> stack;
        static std::pair<uint32_t, uint32_t> getFeatureIndices(int square, int type);
        static int getFeatureIndex(int square, int type, int color);
        int forward(const int move_idx, const std::span<int, p_l1Size / 2> us, const std::span<int, p_l1Size / 2> them, const std::span<const int16_t, p_l1Size * p_outputCount> weights) const;
};

constexpr int16_t activate(const int16_t x) {
    return std::clamp(x, int16_t(0), int16_t(QA));
}

inline std::array<int, p_l1Size / 2> pairwise_and_activate(const std::array<int16_t, p_l1Size>& input) {
    std::array<int, p_l1Size / 2> result = {};
    constexpr size_t half = p_l1Size / 2;

    for (size_t i = 0; i < half; ++i) {
        result[i] = activate(input[i]) * activate(input[i + half]);
    }

    return result;
}