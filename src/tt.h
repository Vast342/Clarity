#pragma once

#include "globals.h"

/*
    Transposition Table:
    Stores the result of past searches in a large table, so that you don't have to make redundant searches
*/

// at 16 bytes per entry, this gives me a 256mb hash table
constexpr uint64_t defaultSize = 16777216;

struct Transposition {
    uint64_t zobristKey;
    int score;
    Move bestMove;
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
        zobristKey = _zobristKey;
        bestMove = _bestMove;
        flag = _flag;
        score = _score;
        depth = _depth;
    }
};

struct TranspositionTable {
    public:
        int getScore(uint64_t zkey);
        Move getBestMove(uint64_t zkey);
        bool matchZobrist(uint64_t zkey);
        uint8_t getFlag(uint64_t zkey);
        Transposition getEntry(uint64_t zkey);
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
