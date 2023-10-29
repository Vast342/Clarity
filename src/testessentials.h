// something something header guards
#pragma once
#include "globals.h"

// perft tests
struct PerftTest {
    public:
        std::string fen;
        int depth;
        int expectedOutput;
        PerftTest(std::string s, int d, int e) {
            fen = s;
            depth = d;
            expectedOutput = e;
        }
};

void runPerftSuite(int number);
int perft(Board board, int depth);
void splitPerft(Board board, int depth);
void individualPerft(Board board, int depth);