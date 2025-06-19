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
#include "slidey.h"
#include "magic.h"

// the nested arrays used to lookup the attacks
std::array<std::array<uint64_t, 512>, 64> bishopAttackLookup;
std::array<std::array<uint64_t, 4096>, 64> rookAttackLookup;

// yoooo thanks ciekce, pdep made in code because some cpus don't support it but it's still quite useful
[[nodiscard]] constexpr auto pdep(uint64_t v, uint64_t mask)
{
    uint64_t dst{};

    for(uint64_t bit = 1; mask != 0; bit <<= 1)
    {
        if((v & bit) != 0)
            dst |= mask & -mask;
        mask &= mask - 1;
    }

    return dst;
}

int getBishopBlockerCombinations(std::array<uint64_t, 512> &blockers, const uint64_t mask) {
    // calculates the total amount of possible blocker configuration (2^n)
    int totalPatterns = 1 << __builtin_popcountll(mask);
    assert(totalPatterns <= 512);

    // generates them, using pdep
    for(int i = 0; i < totalPatterns; i++) {
        blockers[i] = pdep(i, mask);
    }

    return totalPatterns;
}

// seperated so that I can have the assertions be right
int getRookBlockerCombinations(std::array<uint64_t, 4096> &blockers, const uint64_t mask) {
    // calculates the total amount of possible blocker configuration (2^n)
    int totalPatterns = 1 << __builtin_popcountll(mask);
    assert(totalPatterns <= 4096);

    // generates them, using pdep
    for(int i = 0; i < totalPatterns; i++) {
        blockers[i] = pdep(i, mask);
    }

    return totalPatterns;
}

// generates the lookup tables for each square, ran on startup
void generateLookups() {
    // loops through each square
    for(int i = 0; i < 64; i++) {
        // generate the different blocker patterns
        std::array<uint64_t, 4096> rookBlockers;
        int rookPatterns = getRookBlockerCombinations(rookBlockers, rookMasks[i]);

        // calculate the moves for each one
        for(int j = 0; j < rookPatterns; j++) {
            rookAttackLookup[i][calculateRookIndex(rookBlockers[j], i)] = getRookAttacksOld(i, rookBlockers[j]);
        }

        // bishop blocker patterns
        std::array<uint64_t, 512> bishopBlockers;
        int bishopPatterns = getBishopBlockerCombinations(bishopBlockers, bishopMasks[i]);

        // calculate the moves for each one
        for(int j = 0; j < bishopPatterns; j++) {
            bishopAttackLookup[i][calculateBishopIndex(bishopBlockers[j], i)] = getBishopAttacksOld(i, bishopBlockers[j]);
        }
    }
}

// uses magic to calculate the index for each square and blockers
uint64_t calculateRookIndex(const uint64_t occupiedBitboard, const int square) {
    return ((occupiedBitboard & rookMasks[square]) * rookMagics[square]) >> (64 - __builtin_popcountll(rookMasks[square]));
}

uint64_t calculateBishopIndex(const uint64_t occupiedBitboard, const int square) {
    return ((occupiedBitboard & bishopMasks[square]) * bishopMagics[square]) >> (64 - __builtin_popcountll(bishopMasks[square]));
}

// gets the attacks from the tables so the values don't have to be public
uint64_t getRookAttacksFromTable(const uint64_t occupiedBitboard, const int square) {
    return rookAttackLookup[square][calculateRookIndex(occupiedBitboard, square)];
}

uint64_t getBishopAttacksFromTable(const uint64_t occupiedBitboard, const int square) {
    return bishopAttackLookup[square][calculateBishopIndex(occupiedBitboard, square)];
}
