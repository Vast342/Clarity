/*
    Clarity
    Copyright (C) 2023 Joseph Pasfield

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

// these functions are used for movegen regardless of pext or magics, and then they are defined in a different file based on which is being used

uint64_t calculateRookIndex(uint64_t occupiedBitboard, int square);
uint64_t calculateBishopIndex(uint64_t occupiedBitboard, int square);

uint64_t getRookAttacksFromTable(uint64_t occupiedBitboard, int square);
uint64_t getBishopAttacksFromTable(uint64_t occupiedBitboard, int square);

void generateLookups();
