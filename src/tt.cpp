#include "tt.h"

int TranspositionTable::getScore(uint64_t zkey) {
    return table[zkey & mask].score;
}

Move TranspositionTable::getBestMove(uint64_t zkey) {
    return table[zkey & mask].bestMove;
}

bool TranspositionTable::matchZobrist(uint64_t zkey) {
    return table[zkey & mask].zobristKey == zkey;
}

uint8_t TranspositionTable::getFlag(uint64_t zkey) {
    return table[zkey & mask].flag;
}

Transposition TranspositionTable::getEntry(uint64_t zkey) {
    //if(table[zkey & mask].zobristKey != 0) std::cout << std::to_string(table[zkey & mask].score) << ", " << std::to_string(zkey) << '\n';
    return table[zkey & mask];
}

int TranspositionTable::getDepth(uint64_t zkey) {
    return table[zkey & mask].depth;
}

void TranspositionTable::setEntry(uint64_t zkey, Transposition entry) {
    //std::cout << "recieved, writing an entry at " << std::to_string(zkey) << " with score " << std::to_string(entry.score) << '\n';
    table[zkey & mask] = entry;
    //std::cout << "reading entry just written, score is " << std::to_string(table[zkey & mask].score) << '\n';
}

void TranspositionTable::setScore(uint64_t zkey, int score) {
    table[zkey & mask].score = score;
}

void TranspositionTable::setBestMove(uint64_t zkey, Move bestMove) {
    table[zkey & mask].bestMove = bestMove;
}

void TranspositionTable::setZobrist(uint64_t zkey) {
    table[zkey & mask].zobristKey = zkey;
}

void TranspositionTable::setFlag(uint64_t zkey, uint8_t flag) {
    table[zkey & mask].flag = flag;
}

void TranspositionTable::setDepth(uint64_t zkey, uint16_t depth) {
    table[zkey & mask].depth = depth;
}

void TranspositionTable::clearTable() {
    std::fill(table.begin(), table.end(), Transposition());
}
