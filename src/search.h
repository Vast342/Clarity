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
#include "tt.h"

extern bool timesUp;

constexpr int depthLimit = 120;

constexpr int matedScore = -10000000;

extern int badCaptureScore;

struct StackEntry {
    // conthist!
    CHEntry *ch_entry;
    // killer move
    Move killer;
    // static eval used for improving
    int staticEval;
    bool inCheck;
    // excluded move
    Move excluded;
    int doubleExtensions;
};

struct Engine {
    public: 
        void resetEngine();
        Move think(Board board, int softBound, int hardBound, bool info);
        Move getBestMove();
        int benchSearch(Board board, int depthToSearch);
        Move fixedDepthSearch(Board board, int depthToSearch, bool info);
        std::pair<Move, int> dataGenSearch(Board board, int nodeCap);
        Move fixedNodesSearch(Board board, int nodes, bool info);
        Engine(TranspositionTable *tt) {
            conthistTable = std::make_unique<CHTable>();
            TT = tt;
        }
    private:
        bool useNodeCap = false;

        Move rootBestMove = Move();

        int hardLimit = 0;

        int seldepth = 0;

        std::array<StackEntry, depthLimit> stack;

        TranspositionTable* TT;

        std::array<std::array<std::array<int, 64>, 64>, 2> historyTable;
        std::array<std::array<std::array<std::array<int, 7>, 64>, 7>, 2> noisyHistoryTable;
        std::array<std::array<std::array<std::array<int, 7>, 64>, 7>, 2> qsHistoryTable;
        std::unique_ptr<CHTable> conthistTable;

        std::array<std::array<int, 64>, 64> nodeTMTable;

        std::chrono::steady_clock::time_point begin;
        void clearHistory();
        int estimateMoveValue(const Board& board, const int end, const int flag);
        bool see(const Board& board, Move move, int threshold);
        void scoreMoves(const Board& board, std::array<Move, 256> &moves, std::array<int, 256> &values, int numMoves, Move ttMove, int ply);
        void scoreMovesQS(const Board& board, std::array<Move, 256> &moves, std::array<int, 256> &values, int numMoves, Move ttMove);
        int qSearch(Board &board, int alpha, int beta, int ply);
        void updateHistory(const int colorToMove, const int start, const int end, const int piece, const int bonus, const int ply);
        void updateNoisyHistory(const int colorToMove, const int piece, const int end, const int victim, const int bonus);
        void updateQSHistory(const int colorToMove, const int piece, const int end, const int victim, const int bonus);
        int negamax(Board &board, int depth, int alpha, int beta, int ply, bool nmpAllowed);
        std::string getPV(Board board, std::vector<uint64_t> &hashVector, int numEntries);
        void outputInfo(const Board& board, int score, int depth, int elapsedTime);
};