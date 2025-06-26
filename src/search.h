/*
    Clarity
    Copyright (C) 2025 Joseph Pasfield

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
#include "limits.h"
#include "tt.h"
#include "history.h"

constexpr int16_t mateScore = 32000;
constexpr int plyLimit = 256;

struct StackEntry {
    std::array<Move, plyLimit> pvTable;
    int pvLength;
    bool isNull;
};

struct Searcher {
    public:
        void think(Board board, const Limiters &limiters, const bool info);
        void newGame();
        uint64_t getNodes() const {
            return nodes;
        }
        explicit Searcher(TranspositionTable* TT) : rootBestMove(Move()), nodes(0), seldepth(0), endSearch(false), stack({}), history({}), TT(TT) {}
    private:
        Move rootBestMove;
        uint64_t nodes;
        int seldepth;
        bool endSearch;
        std::array<StackEntry, plyLimit> stack;

        HistoryTables history;

        TranspositionTable* TT;

        std::chrono::steady_clock::time_point startTime;
        void outputInfo(const Board& board, const int score, const int depth, const int elapsedTime) const;
        template <bool isPV = false> int16_t search(Board &board, const int depth, int16_t alpha, const int16_t beta, const int ply, const Limiters &limiters);
        int16_t qsearch(Board &board, int16_t alpha, const int16_t beta, const int ply, const Limiters &limiters);
        std::string getPV() const;
        int getTimeElapsed() const {
            return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startTime).count();
        }
};