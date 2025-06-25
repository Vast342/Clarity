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
#include "slidey.h"

constexpr std::array<std::array<uint64_t, 64>, 64> generateBetweens() {
    std::array<std::array<uint64_t, 64>, 64> result = {};

    for(int from = 0; from < 64; from++) {
        const auto fromMask = (1ULL << from);

        const auto rookAttacks = getRookAttacksOld(from, 0);
        const auto bishopAttacks = getBishopAttacksOld(from, 0);

        for(int to = 0; to < 64; to++) {
            if(from == to) {
                continue;
            }
            const auto toMask = (1ULL << to);

            if(rookAttacks & toMask) {
                result[from][to] = getRookAttacksOld(from, toMask) & getRookAttacksOld(to, fromMask);
            } else if(bishopAttacks & toMask) {
                result[from][to] = getBishopAttacksOld(from, toMask) & getBishopAttacksOld(to, fromMask);
            }
        }
    }

    return result;
}

constexpr std::array<std::array<uint64_t, 64>, 64> generateIntersections() {
    std::array<std::array<uint64_t, 64>, 64> result = {};

    for(int from = 0; from < 64; from++) {
        const auto fromMask = (1ULL << from);

        const auto rookAttacks = getRookAttacksOld(from, 0);
        const auto bishopAttacks = getBishopAttacksOld(from, 0);

        for(int to = 0; to < 64; to++) {
            if(from == to) {
                continue;
            }
            const auto toMask = (1ULL << to);

            if(rookAttacks & toMask) {
                result[from][to] = (fromMask | getRookAttacksOld(from, 0)) & (toMask | getRookAttacksOld(to, 0));
            } else if(bishopAttacks & toMask) {
                result[from][to] = (fromMask | getBishopAttacksOld(from, 0)) & (toMask | getBishopAttacksOld(to, 0));
            }
        }
    }

    return result;
}

constexpr std::array<std::array<uint64_t, 64>, 64> betweenRays = generateBetweens();
constexpr std::array<std::array<uint64_t, 64>, 64> intersectingRays = generateIntersections();