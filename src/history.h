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
#include "tunables.h"

struct HistoryTables {
public:
    inline void clear() {
        std::memset(historyTable.data(), 0, sizeof(historyTable));
    }
    // handles 1: bonus for move, 2: penalties for other moves
    inline void betaCutoff(const Board &board, const int colorToMove, const Move cutoffMove, const std::array<Move, 256> &testedMoves, const int legalMoves, const int depth) {
        auto bonus = quietBonus(depth);
        // cutoff move bonus
        if(board.pieceAtIndex(cutoffMove.getEndSquare()) == None) {
            auto start = cutoffMove.getStartSquare();
            auto end = cutoffMove.getEndSquare();
            auto startAttack = board.squareIsUnderAttack(start);
            auto endAttack = board.squareIsUnderAttack(end);
            updateHistory(colorToMove, start, end, startAttack, endAttack, bonus);
        }
        bonus = -bonus;
        // all other move penalties
        for(int idx = 0; idx < legalMoves - 1; idx += 1) {
            const auto move = testedMoves[idx];
            if(board.pieceAtIndex(move.getEndSquare()) == None) {
                auto start = cutoffMove.getStartSquare();
                auto end = cutoffMove.getEndSquare();
                auto startAttack = board.squareIsUnderAttack(start);
                auto endAttack = board.squareIsUnderAttack(end);
                updateHistory(colorToMove, start, end, startAttack, endAttack, bonus);
            }
        }
    }
    inline void updateHistory(const int colorToMove, const int start, const int end, const bool startAttack, const bool endAttack, const bool bonus) {
        update(&historyTable[colorToMove][start][startAttack][end][endAttack], bonus);
    }
    std::array<std::array<std::array<std::array<std::array<int16_t, 2>, 64>, 2>, 64>, 2> historyTable;
private:
    static constexpr int historyCap = 16384;
    static void update(int16_t *value, const int bonus) {
        const auto thingToAdd = bonus - *value * std::abs(bonus) / historyCap;
        *value += thingToAdd;
    }
    static int quietBonus(const int depth) {
        return std::min(hstMaxBonus.value, hstAdder.value * depth - hstSubtractor.value);
    }
};