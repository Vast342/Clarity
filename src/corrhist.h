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
#include "tunables.h"

struct Corrhist {
    static constexpr int size = 16384;
    static constexpr int mask = size - 1;
    std::array<std::array<int32_t, 2>, size> pawnTable;
    std::array<std::array<std::array<int32_t, 2>, 2>, size> nonPawnTables;
    std::array<std::array<int32_t, 2>, size> majorTable;
    inline int correct(int ctm, int pawnHash, int staticEval, std::array<int, 2> nonPawnHashes, int majorHash) {
        int pawn_correction = (pawnTable[pawnHash][ctm] * pawnChWeight.value) / 512;
        int non_pawn_correction = (nonPawnTables[nonPawnHashes[ctm]][ctm][ctm] + nonPawnTables[nonPawnHashes[1 - ctm]][ctm][1 - ctm] * nonpawnChWeight.value) / 512;
        int major_correction = (majorTable[majorHash][ctm] * majorChWeight.value) / 512;
        int correction = pawn_correction + non_pawn_correction + major_correction;
        return (staticEval + correction / chScale.value);
    }
    inline void clear() {
        std::memset(pawnTable.data(), 0, sizeof(pawnTable));
        std::memset(nonPawnTables.data(), 0, sizeof(nonPawnTables));
    }
    inline void push(int pawnHash, int ctm, int bestScore, int staticEval, int depth, std::array<int, 2> nonPawnHashes, int majorHash) {
        int error = bestScore - staticEval;
        int scaled_bonus = error * chScale.value;
        const int weight = std::min(depth - 1, 16);

        auto &pawnScore = pawnTable[pawnHash][ctm];
        pawnScore = (pawnScore * (chScale.value - weight) + scaled_bonus * weight) / chScale.value;
        pawnScore = std::clamp(pawnScore, int(chMin.value * chScale.value), int(chMax.value * chScale.value));
        
        for(int color = 0; color < 2; color++) {
            auto &nonPawnScore = nonPawnTables[nonPawnHashes[color]][ctm][color];
            nonPawnScore = (nonPawnScore * (chScale.value - weight) + scaled_bonus * weight) / chScale.value;
            nonPawnScore = std::clamp(nonPawnScore, int(chMin.value * chScale.value), int(chMax.value * chScale.value));
        }

        auto &majorScore = majorTable[majorHash][ctm];
        majorScore = (majorScore * (chScale.value - weight) + scaled_bonus * weight) / chScale.value;
        majorScore = std::clamp(majorScore, int(chMin.value * chScale.value), int(chMax.value * chScale.value));
    }
};

