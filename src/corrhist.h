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

#include "globals.h"

struct Corrhist {
    static constexpr int size = 16384;
    static constexpr int scale = 256;
    static constexpr int mask = size - 1;
    static constexpr int min = -64;
    static constexpr int max = 64;
    std::array<std::array<int32_t, 2>, size> pawnTable;
    std::array<std::array<std::array<int32_t, 2>, 2>, size> nonPawnTables;
    inline int correct(int ctm, int pawnHash, int staticEval, std::array<int, 2> nonPawnHashes) {
        int pawn_correction = pawnTable[pawnHash][ctm];
        int non_pawn_correction = (nonPawnTables[nonPawnHashes[ctm]][ctm][ctm] + nonPawnTables[nonPawnHashes[1 - ctm]][ctm][1 - ctm]) / 2;
        int correction = pawn_correction + non_pawn_correction;
        return (staticEval + correction / scale);
    }
    inline void clear() {
        std::memset(pawnTable.data(), 0, sizeof(pawnTable));
        std::memset(nonPawnTables.data(), 0, sizeof(nonPawnTables));
    }
    inline void push(int pawnHash, int ctm, int bestScore, int staticEval, int depth, std::array<int, 2> nonPawnHashes) {
        // don't worry about it gents, i'll make this tunable soon:tm:
        int error = bestScore - staticEval;
        int scaled_bonus = error * scale;
        const int weight = std::min(depth - 1, 16);

        auto &score = pawnTable[pawnHash][ctm];
        score = (score * (scale - weight) + scaled_bonus * weight) / scale;
        score = std::clamp(score, min * scale, max * scale);
        
        for(int color = 0; color < 2; color++) {
            auto &score = nonPawnTables[nonPawnHashes[color]][ctm][color];
            score = (score * (scale - weight) + scaled_bonus * weight) / scale;
            score = std::clamp(score, min * scale, max * scale);
        }
    }
};

