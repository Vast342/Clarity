#pragma once

#include "globals.h"
#include "masks.h"

// these functions are used for movegen regardless of pext or magics, and then they are defined in a different file based on which is being used

uint64_t calculateRookIndex(uint64_t occupiedBitboard, int square);
uint64_t calculateBishopIndex(uint64_t occupiedBitboard, int square);

uint64_t getRookAttacksFromTable(uint64_t occupiedBitboard, int square);
uint64_t getBishopAttacksFromTable(uint64_t occupiedBitboard, int square);

void generateLookups();
