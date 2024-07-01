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

uint64_t index(uint64_t key, int size) {
    // this emits a single mul on both x64 and arm64
    return static_cast<uint64_t>((static_cast<unsigned __int128>(key) * static_cast<unsigned __int128>(size)) >> 64);
}

int TranspositionTable::getScore(uint64_t zkey) {
    return table[index(zkey, size)].score;
}

Move TranspositionTable::getBestMove(uint64_t zkey) {
    return table[index(zkey, size)].bestMove;
}

bool TranspositionTable::matchZobrist(uint64_t zkey) {
    return table[index(zkey, size)].zobristKey == shrink(zkey);
}

uint8_t TranspositionTable::getFlag(uint64_t zkey) {
    return table[index(zkey, size)].flag;
}

Transposition* TranspositionTable::getEntry(uint64_t zkey) {
    //if(table[index(zkey, size)].zobristKey != 0) std::cout << std::to_string(table[index(zkey, size)].score) << ", " << std::to_string(zkey) << '\n';
    return &table[index(zkey, size)];
}

int TranspositionTable::getDepth(uint64_t zkey) {
    return table[index(zkey, size)].depth;
}

void TranspositionTable::setEntry(uint64_t zkey, Transposition entry) {
    //std::cout << "recieved, writing an entry at " << std::to_string(zkey) << " with score " << std::to_string(entry.score) << '\n';
    table[index(zkey, size)] = entry;
    //std::cout << "reading entry just written, score is " << std::to_string(table[index(zkey, size)].score) << '\n';
}

void TranspositionTable::setScore(uint64_t zkey, int score) {
    table[index(zkey, size)].score = score;
}

void TranspositionTable::setBestMove(uint64_t zkey, Move bestMove) {
    table[index(zkey, size)].bestMove = bestMove;
}

void TranspositionTable::setZobrist(uint64_t zkey) {
    table[index(zkey, size)].zobristKey = shrink(zkey);
}

void TranspositionTable::setFlag(uint64_t zkey, uint8_t flag) {
    table[index(zkey, size)].flag = flag;
}

void TranspositionTable::setDepth(uint64_t zkey, uint16_t depth) {
    table[index(zkey, size)].depth = depth;
}

void TranspositionTable::clearTable() {
    std::fill(table.begin(), table.end(), Transposition());
}

void TranspositionTable::resize(int newSize) {
    size = newSize;
    table.resize(newSize, Transposition());
    clearTable();
}
