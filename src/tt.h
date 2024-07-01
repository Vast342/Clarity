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
#pragma once

#include "globals.h"

/*
    Transposition Table:
    Stores the result of past searches in a large table, so that you don't have to make redundant searches
*/


constexpr uint16_t shrink(uint64_t hash) {
    return (hash & 0xFFFF);
}

// this gives me a 64mb hash table
constexpr uint64_t defaultSize = 4194304 * 2;;

#pragma pack(push, 1)
struct Transposition {
    int16_t score;
    Move bestMove;
    uint16_t zobristKey;
    uint8_t flag;
    uint8_t depth;
    Transposition() {
        zobristKey = 0;
        bestMove = Move();
        flag = 0;
        score = 0;
        depth = 0;
    }
    Transposition(uint64_t _zobristKey, Move _bestMove, uint8_t _flag, int _score, uint8_t _depth) {
        zobristKey = shrink(_zobristKey);
        bestMove = _bestMove;
        flag = _flag;
        score = _score;
        depth = _depth;
    }
};
#pragma pack(pop)

struct TranspositionTable {
    public:
        int getScore(uint64_t zkey);
        Move getBestMove(uint64_t zkey);
        bool matchZobrist(uint64_t zkey);
        uint8_t getFlag(uint64_t zkey);
        Transposition* getEntry(uint64_t zkey);
        int getDepth(uint64_t zkey);
        void setEntry(uint64_t zkey, Transposition entry);
        void setScore(uint64_t zkey, int score);
        void setBestMove(uint64_t zkey, Move bestMove);
        void setZobrist(uint64_t zkey);
        void setFlag(uint64_t zkey, uint8_t flag);
        void setDepth(uint64_t zkey, uint16_t depth);
        void clearTable();
        void resize(int newSize);
        TranspositionTable() {
            resize(defaultSize);
            clearTable();
        }
        TranspositionTable(int newSize) {
            resize(newSize);
            clearTable();
        }
        uint64_t mask;
    private:
        std::vector<Transposition> table;
};