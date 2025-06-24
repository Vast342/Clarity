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
#include "masks.h"

// classical approach move generation
constexpr uint64_t getRookAttacksOld(int square, uint64_t occupiedBitboard) {
    uint64_t attacks = 0;
    for(int direction = 0; direction < 4; direction++) {
        uint64_t currentAttack = slideyPieceRays[direction][square];
        if((direction & 1) == 0) {
            currentAttack ^= slideyPieceRays[direction][std::clamp(std::countr_zero(currentAttack & occupiedBitboard), 0, 63)];
        } else {
            currentAttack ^= slideyPieceRays[direction][std::clamp(63 - std::countl_zero(currentAttack & occupiedBitboard), 0, 63)];
        }
        attacks |= currentAttack;
    }
    return attacks;
}
constexpr uint64_t getBishopAttacksOld(int square, uint64_t occupiedBitboard) {
    uint64_t attacks = 0;
    for(int direction = 4; direction < 8; direction++) {
        uint64_t currentAttack = slideyPieceRays[direction][square];
        if((direction & 1) == 0) {
            currentAttack ^= slideyPieceRays[direction][std::clamp(std::countr_zero(currentAttack & occupiedBitboard), 0, 63)];
        } else {
            currentAttack ^= slideyPieceRays[direction][std::clamp(63 - std::countl_zero(currentAttack & occupiedBitboard), 0, 63)];
        }
        attacks |= currentAttack;
    }
    return attacks;
}

// these functions are used for movegen regardless of pext or magics, and then they are defined in a different file based on which is being used

uint64_t calculateRookIndex(uint64_t occupiedBitboard, int square);
uint64_t calculateBishopIndex(uint64_t occupiedBitboard, int square);

uint64_t getRookAttacksFromTable(uint64_t occupiedBitboard, int square);
uint64_t getBishopAttacksFromTable(uint64_t occupiedBitboard, int square);

void generateLookups();
