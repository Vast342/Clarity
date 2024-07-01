/*
    Clarity
    Copyright (C) 2024 Joseph Pasfield

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
#include "tt.h"

// all the functions for the transposition table

int TranspositionTable::getScore(uint64_t zkey) {
    return table[zkey & mask].score;
}

Move TranspositionTable::getBestMove(uint64_t zkey) {
    return table[zkey & mask].bestMove;
}

bool TranspositionTable::matchZobrist(uint64_t zkey) {
    return table[zkey & mask].zobristKey == (zkey & 0xFFFF);
}

uint8_t TranspositionTable::getFlag(uint64_t zkey) {
    return table[zkey & mask].flag;
}

Transposition* TranspositionTable::getEntry(uint64_t zkey) {
    //if(table[zkey & mask].zobristKey != 0) std::cout << std::to_string(table[zkey & mask].score) << ", " << std::to_string(zkey) << '\n';
    return &table[zkey & mask];
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
    table[zkey & mask].zobristKey = (zkey & 0xFFFF);
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

void TranspositionTable::resize(int newSize) {
    mask = newSize - 1;
    table.resize(newSize, Transposition());
    clearTable();
}
