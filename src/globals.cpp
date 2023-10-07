#include "globals.h"
#include "bmi2.h"
#include "psqt.h"

int getType(int value) {
    return value & 7;
}
int getColor(int value) {
    return value >> 3;
}
uint64_t fileMask = 0b100000001000000010000000100000001000000010000000100000001;
uint64_t rankMask = 0b11111111;
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

std::array<std::array<uint8_t, 218>, 50> reductions;

void calculateReductions() {
    for(int i = 0; i < 50; i++) {
        for(int j = 0; j < 218; j++) {
            reductions[i][j] = uint8_t(sqrt(double(i-1)) + sqrt(double(j-1)));
        }
    }
}

void initialize() {
    generateLookups();
    computePSQTs();
    initializeZobrist();
    calculateReductions();
}

std::vector<std::string> split(const std::string string, const char seperator) {
    std::stringstream stream(string);
    std::string segment;
    std::vector<std::string> list;

    while (std::getline(stream, segment, seperator)) {
        list.push_back(segment);
    }
    
    return list;
}

void sortMoves(std::array<int, 256> &values, std::array<Move, 256> &moves, int numMoves) {
    int lowestIndex;
 
    // for each value
    for(int i = 0; i < numMoves - 1; i++) {
 
        // find the lowest number that hasn't been sorted yet
        lowestIndex = i;
        for(int j = i + 1; j < numMoves; j++) {
            if (values[j] < values[lowestIndex])
                lowestIndex = j;
        }
 
        // swap the elements
        if(lowestIndex != i) {
            std::swap(values[lowestIndex], values[i]);
            std::swap(moves[lowestIndex], moves[i]);
        }
    }
}

int flipIndex(int index) {
    return index ^ 56;
}

// thanks z5
void incrementalSort(std::array<int, 256> &values, std::array<Move, 256> &moves, int numMoves, int i) {
    for (int j = i + 1; j < numMoves; j++) {
        if (values[j] > values[i]) {
            std::swap(values[j], values[i]);
            std::swap(moves[j], moves[i]);
        }
    }
}