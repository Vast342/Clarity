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
constexpr int p_outputCount = 3760;

struct PolicyNetwork {
    alignas(64) std::array<float, p_l1Size * p_inputSize> featureWeights;
    alignas(64) std::array<float, p_l1Size> featureBiases;
    alignas(64) std::array<float, p_outputCount * p_l1Size> outputWeights;
    alignas(64) std::array<float, p_outputCount> outputBiases;
};

struct PolicyAccumulator {
    alignas(64) std::array<float, p_l1Size> black;
    alignas(64) std::array<float, p_l1Size> white;
    void initialize(std::span<const float, p_l1Size> bias);
    void initHalf(std::span<const float, p_l1Size> bias, int color);
};

struct PolicyRefreshTableEntry {
    PolicyAccumulator accumulator;
    std::array<BoardState, 2> boards{};

    BoardState &colorBoards(int c) {
        return boards[c];
    }
};

struct PolicyRefreshTable {
    std::vector<PolicyRefreshTableEntry> table;

    void init();
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
        void performUpdates(NetworkUpdates updates, int blackKing, int whiteKing, const BoardState &state);
        void performUpdatesAndPush(NetworkUpdates updates, int blackKing, int whiteKing, const BoardState &state);
        inline void pop() {
            current--;
        }
        void reset();
        void activateFeature(int square, int type, int blackKing, int whiteKing);
        void activateFeatureSingle(int square, int type, int color, int king);
        void activateFeatureAndPush(int square, int type, int blackKing, int whiteKing);
        void disableFeature(int square, int type, int blackKing, int whiteKing);
        void disableFeatureSingle(int square, int type, int color, int king);
        void refreshAccumulator(int color, const BoardState &state, int king);
        float evaluateMove(Move move, const Board &board, const std::span<float, p_l1Size / 2> us, const std::span<float, p_l1Size / 2> them) const;
        void fullRefresh(const BoardState &state, int blackKing, int whiteKing);
        void halfRefresh(int color, const BoardState &state, int king);
        std::array<float, 256> labelMoves(const std::array<Move, 256> &moves, int moveCount, int ctm, const Board &board) const;
    private:
        PolicyRefreshTable refreshTable;
        int current;
        std::vector<PolicyAccumulator> stack;
        static std::pair<uint32_t, uint32_t> getFeatureIndices(int square, int type, int blackKing, int whiteKing);
        static int getFeatureIndex(int square, int type, int color, int king);
        float forward(const int move_idx, const std::span<float, p_l1Size / 2> us, const std::span<float, p_l1Size / 2> them, const std::span<const float, p_l1Size * p_outputCount> weights) const;
};

constexpr bool policyRefreshRequired(int oldKingSquare, int newKingSquare) {
    return (oldKingSquare % 8 > 3) != (newKingSquare % 8 > 3);
}
