#include "bmi2.h"

// the nested arrays used to lookup the attacks
std::array<std::array<uint64_t, 512>, 64> bishopAttackLookup;
std::array<std::array<uint64_t, 4096>, 64> rookAttackLookup;

int getBishopBlockerCombinations(std::array<uint64_t, 512> &blockers, const uint64_t mask) {
    // calculates the total amount of possible blocker configuration (2^n)
    int totalPatterns = 1 << __builtin_popcountll(mask);
    assert(totalPatterns <= 512);

    // generates them, using pdep
    for(int i = 0; i < totalPatterns; i++) {
        blockers[i] = _pdep_u64(i, mask);
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
        blockers[i] = _pdep_u64(i, mask);
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
            //std::cout << std::to_string(rookAttackLookup[i][calculateRookIndex(rookBlockers[j], i)]) << '\n';
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

// uses pext to calculate the index for each square and blockers
uint64_t calculateRookIndex(const uint64_t occupiedBitboard, const int square) {
    return _pext_u64(occupiedBitboard, rookMasks[square]);
}

uint64_t calculateBishopIndex(const uint64_t occupiedBitboard, const int square) {
    return _pext_u64(occupiedBitboard, bishopMasks[square]);
}

// gets the attacks from the tables so the values don't have to be public
uint64_t getRookAttacksFromTable(const uint64_t occupiedBitboard, const int square) {
    return rookAttackLookup[square][calculateRookIndex(occupiedBitboard, square)];
}

uint64_t getBishopAttacksFromTable(const uint64_t occupiedBitboard, const int square) {
    return bishopAttackLookup[square][calculateBishopIndex(occupiedBitboard, square)];
}