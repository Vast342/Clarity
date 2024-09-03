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

/*
Current Net: cn_028
Arch: (768x6->1024)x2->1x8
Activation: SCReLU
Special Details: 
 - Horizontal Mirroring
 - Few more buckets, let's see if we can get some improvement from finny tables now
*/ 
constexpr int inputSize = 768;
constexpr int inputBucketCount = 6;
constexpr int layer1Size = 1024;
constexpr int outputBucketCount = 8;

constexpr std::array<int, 64> inputBuckets = {
    0, 0, 1, 1, 7, 7, 6, 6,
    2, 2, 3, 3, 9, 9, 8, 8, 
    4, 4, 4, 4,10,10,10,10,
    4, 4, 4, 4,10,10,10,10,
    5, 5, 5, 5,11,11,11,11,
    5, 5, 5, 5,11,11,11,11,
    5, 5, 5, 5,11,11,11,11,
    5, 5, 5, 5,11,11,11,11,
};


// organizing this somewhat similarly to code I've seen, mostly from clarity_sp_nnue, made by Ciekce.

struct Network {
    alignas(32) std::array<std::int16_t, inputSize * inputBucketCount * layer1Size> featureWeights;
    alignas(32) std::array<std::int16_t, layer1Size> featureBiases;
    alignas(32) std::array<std::int16_t, layer1Size * 2 * outputBucketCount> outputWeights;
    alignas(32) std::array<std::int16_t, outputBucketCount> outputBiases;
};

struct Accumulator {
    alignas(32) std::array<std::int16_t, layer1Size> black;
    alignas(32) std::array<std::int16_t, layer1Size> white;
    void initialize(std::span<const std::int16_t, layer1Size> bias);
    void initHalf(std::span<const std::int16_t, layer1Size> bias, int color);
};

struct RefreshTableEntry {
    Accumulator accumulator;
    std::array<BoardState, 2> boards{};

    BoardState &colorBoards(int c) {
        return boards[c];
    }
};

struct RefreshTable {
    std::vector<RefreshTableEntry> table;

    void init();
};

class NetworkState {
    public:
        NetworkState() {
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
        int evaluate(int colorToMove, int materialCount);
        void fullRefresh(const BoardState &state, int blackKing, int whiteKing);
        void halfRefresh(int color, const BoardState &state, int king);
    private:
        RefreshTable refreshTable;
        int current;
        std::vector<Accumulator> stack;
        static std::pair<uint32_t, uint32_t> getFeatureIndices(int square, int type, int blackKing, int whiteKing);
        static int getFeatureIndex(int square, int type, int color, int king);
        int forward(const int bucket, const std::span<std::int16_t, layer1Size> us, const std::span<std::int16_t, layer1Size> them, const std::span<const std::int16_t, layer1Size * 2 * outputBucketCount> weights);
};

constexpr bool refreshRequired(int color, int oldKingSquare, int newKingSquare) {
    if(color == 0) {
        oldKingSquare ^= 56;
        newKingSquare ^= 56;
    }

    return inputBuckets[oldKingSquare] != inputBuckets[newKingSquare];
}
