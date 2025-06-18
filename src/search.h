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
#include "tt.h"
#include "corrhist.h"

extern std::atomic<bool> timesUp;

constexpr int depthLimit = 120;

constexpr int16_t matedScore = -32000;

struct StackEntry {
    std::array<Move, depthLimit> pvTable;
    int pvLength;
    // conthist!
    CHEntry *ch_entry;
    Move move;
    // killer move
    Move killer;
    // static eval used for improving
    int staticEval;
    bool inCheck;
    // excluded move
    Move excluded;
    int doubleExtensions;
    int reduction;
};

struct Engine {
    public: 
        void resetEngine();
        Move think(Board board, int softBound, int hardBound, bool info);
        Move getBestMove();
        int benchSearch(Board board, int depthToSearch);
        Move fixedDepthSearch(Board board, int depthToSearch, bool info);
        std::pair<Move, int> dataGenSearch(Board board, uint64_t nodeCap);
        Move fixedNodesSearch(Board board, int nodeCount, bool info);
        Engine(TranspositionTable *tt) {
            conthistTable = std::make_unique<CHTable>();
            TT = tt;
        }

        uint64_t nodes = 0;
    private:
        bool useNodeCap = false;

        Move rootBestMove = Move();

        int hardLimit = 0;

        int seldepth = 0;

        std::array<StackEntry, depthLimit> stack;

        TranspositionTable* TT;

        std::array<std::array<std::array<std::array<std::array<int16_t, 2>, 64>, 2>, 64>, 2> historyTable;
        std::array<std::array<std::array<std::array<std::array<int16_t, 2>, 7>, 64>, 6>, 2> noisyHistoryTable;
        std::array<std::array<std::array<std::array<int16_t, 7>, 64>, 6>, 2> qsHistoryTable;
        std::array<std::array<std::array<std::array<int16_t, 64>, 6>, 2>, 32768> pawnHistoryTable;
        Corrhist corrhist;
        std::unique_ptr<CHTable> conthistTable;
        std::array<std::array<Move, 64>, 64> counterMoves;

        std::array<std::array<int, 64>, 64> nodeTMTable;

        std::chrono::steady_clock::time_point begin;
        void clearHistory();
        int estimateMoveValue(const Board& board, const int end, const int flag);
        bool see(const Board& board, Move move, int threshold);
        void scoreMoves(const Board& board, std::array<Move, 256> &moves, std::array<int, 256> &values, int numMoves, Move ttMove, int16_t ply);
        void scoreMovesQS(const Board& board, std::array<Move, 256> &moves, std::array<int, 256> &values, int numMoves, Move ttMove);
        int16_t qSearch(Board &board, int alpha, int beta, int16_t ply);
        void updateHistory(const int colorToMove, const int start, const int end, const int piece, const int bonus, const int16_t ply, const int hash, const bool startAttack, const bool endAttack);
        void updateNoisyHistory(const int colorToMove, const int piece, const int end, const int victim, const int bonus, const bool endAttack);
        void updateQSHistory(const int colorToMove, const int piece, const int end, const int victim, const int bonus);
        int16_t negamax(Board &board, int depth, int alpha, int beta, int16_t ply, bool nmpAllowed, bool isCutNode);
        std::string getPV();
        void outputInfo(const Board& board, int score, int depth, int elapsedTime);
};
