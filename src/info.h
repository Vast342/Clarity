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
#include "corrhist.h"

constexpr int depthLimit = 120;

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

struct SearchInfo {
    std::array<StackEntry, depthLimit> stack;
    std::array<std::array<std::array<std::array<std::array<int16_t, 2>, 64>, 2>, 64>, 2> historyTable;
    std::array<std::array<std::array<std::array<std::array<int16_t, 2>, 7>, 64>, 6>, 2> noisyHistoryTable;
    std::array<std::array<std::array<std::array<int16_t, 7>, 64>, 6>, 2> qsHistoryTable;
    std::array<std::array<std::array<std::array<int16_t, 64>, 6>, 2>, 32768> pawnHistoryTable;
    Corrhist corrhist;
    std::unique_ptr<CHTable> conthistTable;
    std::array<std::array<Move, 64>, 64> counterMoves;
    SearchInfo() {
        conthistTable = std::make_unique<CHTable>();
    }
};