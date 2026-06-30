/*
    Clarity
    Copyright (C) 2026 Joseph Pasfield

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
Current Net: mlt_01
Arch: (768x8hm->64)x2-pw>(16->32->1)x16
Special Details: 
 - 128 hl test net just to see if naive inference works
 -FEN: rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
EVAL: 78.419914
FEN: r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1
EVAL: -136.21596
FEN: r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1
EVAL: 340.42267
FEN: rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8
EVAL: 45.798145
FEN: 8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1
EVAL: 476.53342
*/ 
constexpr int inputSize = 768;
constexpr int inputBucketCount = 8;
constexpr int ftSize = 64;
constexpr int l1Size = 16;
constexpr int l2Size = 32;
constexpr int outputBucketCount = 16;

constexpr int16_t Q0 = 255;
constexpr int Q1 = 128;
constexpr int Q = 64;
constexpr int QTo4 = Q * Q * Q * Q;

constexpr std::array<int, 64> inputBuckets = []{
    constexpr std::array<int, 32> rawInputBuckets = {
        0, 1, 2, 3,
        4, 4, 5, 5,
        6, 6, 6, 6,
        6, 6, 6, 6,
        7, 7, 7, 7, 
        7, 7, 7, 7, 
        7, 7, 7, 7, 
        7, 7, 7, 7
    };

    std::array<int, 64> result = {};

    for(int rank = 0; rank < 8; rank++) {
        for(int file = 0; file < 4; file++) {
            const int src = rank * 4 + file;
            const int dst = rank * 8 + file;

            result[dst] = rawInputBuckets[src];
            result[dst ^ 7] = rawInputBuckets[src] + inputBucketCount;
        }
    }

    return result;
}();

// organizing this somewhat similarly to code I've seen, mostly from clarity_sp_nnue, made by Ciekce.
#if defined(__AVX512F__) && defined(__AVX512BW__)
constexpr int alignmentAmount = 64;
#else 
constexpr int alignmentAmount = 32;
#endif

// it would be nice to have a MultiArray here
struct alignas(alignmentAmount) Network {
    // inputs -> ft
    std::array<int16_t, inputSize * inputBucketCount * ftSize> featureWeights;
    std::array<int16_t, ftSize> featureBiases;
    // pairwised ft -> l1
    std::array<int8_t, ftSize * l1Size * outputBucketCount> l1Weights;
    std::array<std::array<int32_t, l1Size>, outputBucketCount> l1Biases;
    // l1 -> l2
    std::array<int32_t, l1Size * l2Size * outputBucketCount> l2Weights;
    std::array<std::array<int32_t, l2Size>, outputBucketCount> l2Biases;
    // l2 -> output
    std::array<int32_t, l2Size * outputBucketCount> outputWeights;
    std::array<int32_t, outputBucketCount> outputBiases;
};

struct Accumulator {
    alignas(alignmentAmount) std::array<int16_t, ftSize> black;
    alignas(alignmentAmount) std::array<int16_t, ftSize> white;
    void initialize(std::span<const int16_t, ftSize> bias);
    void initHalf(std::span<const int16_t, ftSize> bias, int color);
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
        int64_t forward(const int bucket, const std::span<int16_t, ftSize> us, const std::span<int16_t, ftSize> them);
};

constexpr bool refreshRequired(int color, int oldKingSquare, int newKingSquare) {
    if(color == 0) {
        oldKingSquare ^= 56;
        newKingSquare ^= 56;
    }

    return inputBuckets[oldKingSquare] != inputBuckets[newKingSquare];
}