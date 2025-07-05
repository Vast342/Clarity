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
#include "globals.h"

#include "search.h"
#include "slidey.h"
#include "tunables.h"

// takes a piece number and gets the type of it
int getType(int value) {
    return value & 7;
}
// takes a piece number and gets the color of it
int getColor(int value) {
    return value >> 3;
}
// a mask for a single file on the board
uint64_t fileMask = 0b100000001000000010000000100000001000000010000000100000001;
// a mask for a single rank on the board
uint64_t rankMask = 0b11111111;

// gets a mask for either a rank or file.
uint64_t getFileMask(int file) {
    return fileMask << file;
}
uint64_t getRankMask(int rank) {
    return rankMask << (8 * rank);
}

// finds the least significant bit in the uint64_t, gets rid of it, and returns its index
int popLSB(uint64_t &bitboard) {
    int lsb = std::countr_zero(bitboard);
    bitboard &= bitboard - 1;
    return lsb;
}

// converts a move to long algebraic form
std::string toLongAlgebraic(Move move) {
    std::string longAlgebraic = "";
    longAlgebraic += squareNames[move.getStartSquare()];
    longAlgebraic += squareNames[move.getEndSquare()];
    switch(move.getFlag()) {
        case promotions[0]:
            longAlgebraic += 'n';
            break;
        case promotions[1]:
            longAlgebraic += 'b';
            break;
        case promotions[2]:
            longAlgebraic += 'r';
            break;
        case promotions[3]:
            longAlgebraic += 'q';
            break;
        default:
            break;
    }
    return longAlgebraic;
}

std::array<uint64_t, 64> squareToBitboard;

void generateSquareToBitboard() {
    for(int i = 0; i < 64; i++) {
        squareToBitboard[i] = (1ULL << i);
    }
}

std::array<std::array<uint8_t, 219>, plyLimit> reductions;

void calculateReductions() {
    for(int i = 0; i < plyLimit; i++) {
        for(int j = 0; j < 219; j++) {
            reductions[i][j] = uint8_t((lmrBase.value + log(i) * log(j)) / lmrDivisor.value);
        }
    }
}

/*ran on startup, does 4 things:
1: generates the lookups for sliding pieces
2: generates the numbers used for zobrist hashing
3: calculates the numbers for LMR
4: calculates numbers for square to bitboard lookups
 */
void initialize() {
    generateSquareToBitboard();
    generateLookups();
    initializeZobrist();
    calculateReductions();
}

// splits a string into segments based on the seperator
std::vector<std::string> split(const std::string &string, const char seperator) {
    std::stringstream stream(string);
    std::string segment;
    std::vector<std::string> list;

    // every time that it can get a segment
    while(std::getline(stream, segment, seperator)) {
        // add it to the vector
        list.push_back(segment);
    }
    
    return list;
}

// flips the index vertically, used for indexing the psqts
int flipIndex(int index) {
    return index ^ 56;
}
