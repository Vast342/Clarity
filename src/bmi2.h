// you know what, don't blame toanth, he probably gets things right more often than I do
#pragma once

#include "globals.h"
#include "masks.h"
#include "immintrin.h"

uint64_t calculateRookIndex(uint64_t occupiedBitboard, int square);
uint64_t calculateBishopIndex(uint64_t occupiedBitboard, int square);

uint64_t getRookAttacksFromTable(uint64_t occupiedBitboard, int square);
uint64_t getBishopAttacksFromTable(uint64_t occupiedBitboard, int square);

void generateLookups();