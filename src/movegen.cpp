#include "masks.h"
#include "slidey.h"

// classical approach move generation
uint64_t getRookAttacksOld(int square, uint64_t occupiedBitboard) {
	uint64_t attacks = 0;
	for(int direction = 0; direction < 4; direction++) {
		uint64_t currentAttack = slideyPieceRays[direction][square];
		if((direction & 1) == 0) {
			currentAttack ^= slideyPieceRays[direction][std::countr_zero(currentAttack & occupiedBitboard)];
		} else {
			currentAttack ^= slideyPieceRays[direction][63 - std::countl_zero(currentAttack & occupiedBitboard)];
		}
		attacks |= currentAttack;
    }
	return attacks;
}
uint64_t getBishopAttacksOld(int square, uint64_t occupiedBitboard) {
	uint64_t attacks = 0;
	for(int direction = 4; direction < 8; direction++) {
		uint64_t currentAttack = slideyPieceRays[direction][square];
		if((direction & 1) == 0) {
			currentAttack ^= slideyPieceRays[direction][std::countr_zero(currentAttack & occupiedBitboard)];
		} else {
			currentAttack ^= slideyPieceRays[direction][63 - std::countl_zero(currentAttack & occupiedBitboard)];
		}
		attacks |= currentAttack;
    }
	return attacks;
}
uint64_t getRookAttacks(int square, uint64_t occupiedBitboard) {
    assert(square < 64);
    return getRookAttacksFromTable(occupiedBitboard, square);
}
uint64_t getBishopAttacks(int square, uint64_t occupiedBitboard) {
    assert(square < 64);
    return getBishopAttacksFromTable(occupiedBitboard, square);
}
uint64_t getPawnPushes(uint64_t pawnBitboard, uint64_t emptyBitboard, int colorToMove) {
	return (colorToMove == 0 ? pawnBitboard >> 8 : pawnBitboard << 8) & emptyBitboard;
}
uint64_t getDoublePawnPushes(uint64_t pawnAttacks, uint64_t emptyBitboard, int colorToMove) {
	pawnAttacks &= getRankMask(colorToMove == 1 ? 2 : 5);
	return (colorToMove == 0 ? pawnAttacks >> 8 : pawnAttacks << 8) & emptyBitboard;
}
// using the lookups from Homura
uint64_t getPawnAttacks(int square, int colorToMove) {
    return pawnAttacks[colorToMove][square];
}
uint64_t getKnightAttacks(int square) {
    return knightAttacks[square];
}
uint64_t getKingAttacks(int square) {
    return kingAttacks[square];
}
// using my own lookups, cool
uint64_t getPassedPawnMask(int square, int colorToMove) {
    return passedPawnMasks[colorToMove][square];
}
