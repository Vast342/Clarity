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

inline std::array<std::array<uint64_t, 64>, 64> betweenRays;

inline std::array<std::array<uint64_t, 64>, 64> calcBetweenRays() {
    std::array<std::array<uint64_t, 64>, 64> result = {};

    for(int from = 0; from < 64; from++) {
        const uint64_t fromMask = 1ULL << from;
        const uint64_t rookAttacks = getRookAttacks(from, 0);
        const uint64_t bishopAttacks = getBishopAttacks(from, 0);

        for(int to = 0; to < 64; to++) {
            if(from == to) continue;

            const uint64_t toMask = 1ULL << to;

            if((rookAttacks & toMask) != 0) {
                result[from][to] = getRookAttacks(from, toMask) & getRookAttacks(to, fromMask);
            } else if((bishopAttacks & toMask) != 0) {
                result[from][to] = getBishopAttacks(from, toMask) & getBishopAttacks(to, fromMask);
            }
        }
    }

    return result;
}

inline void initRays() {
    betweenRays = calcBetweenRays();
}

inline uint64_t getBetweenRays(int from, int to) {
    return betweenRays[from][to];
}