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
#include "search.h"

struct HistoryTables {
    static constexpr int quietHistCount = 2;
    static constexpr int historyCap = 16384;
    void clear() {
        std::memset(quietHist.data(), 0, sizeof(quietHist));
        std::memset(contHist.data(), 0, sizeof(contHist));
    }
    // handles 1: bonus for move, 2: penalties for other moves
    void betaCutoff(const Board &board, const int colorToMove, const Move cutoffMove, const std::array<Move, 256> &testedMoves, const int legalMoves, const int depth, const std::array<StackEntry, plyLimit> &stack, const int ply) {
        auto bonus = quietBonus(depth);
        // cutoff move bonus
        if(board.pieceAtIndex(cutoffMove.getEndSquare()) == None) {
            const auto start = cutoffMove.getStartSquare();
            const auto end = cutoffMove.getEndSquare();
            const auto startAttack = board.squareIsUnderAttack(start);
            const auto endAttack = board.squareIsUnderAttack(end);
            const auto piece = getType(board.pieceAtIndex(start));
            updateHistory(colorToMove, start, end, startAttack, endAttack, piece, bonus, stack, ply);
        }
        bonus = -bonus;
        // all other move penalties
        for(int idx = 0; idx < legalMoves - 1; idx += 1) {
            const auto move = testedMoves[idx];
            if(board.pieceAtIndex(move.getEndSquare()) == None) {
                const auto start = move.getStartSquare();
                const auto end = move.getEndSquare();
                const auto startAttack = board.squareIsUnderAttack(start);
                const auto endAttack = board.squareIsUnderAttack(end);
                const auto piece = getType(board.pieceAtIndex(start));
                updateHistory(colorToMove, start, end, startAttack, endAttack, piece, bonus, stack, ply);
            }
        }
    }
    void updateHistory(const int colorToMove, const int start, const int end, const bool startAttack, const bool endAttack, const int piece, const int bonus, const std::array<StackEntry, plyLimit> &stack, const int ply) {
        update(&quietHist[colorToMove][start][startAttack][end][endAttack], bonus);
        if(ply > 0) {
            update(&(*stack[ply - 1].chEntry)[colorToMove][piece][end], bonus);
        }
    }
    int getQuietHistory(const int colorToMove, const int start, const int end, const bool startAttack, const bool endAttack, const int piece, const std::array<StackEntry, plyLimit> &stack, const int ply) const {
        return quietHist[colorToMove][start][startAttack][end][endAttack]
             + (ply > 0 ? (*stack[ply - 1].chEntry)[colorToMove][piece][end] : 0);
    }
    CHTable contHist = {};
private:
    std::array<std::array<std::array<std::array<std::array<int16_t, 2>, 64>, 2>, 64>, 2> quietHist = {};
    static void update(int16_t *value, const int bonus) {
        const auto thingToAdd = bonus - *value * std::abs(bonus) / historyCap;
        *value += thingToAdd;
    }
    static int quietBonus(const int depth) {
        return std::min(hstMaxBonus.value, hstAdder.value * depth - hstSubtractor.value);
    }
};