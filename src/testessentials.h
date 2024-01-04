/*
    Clarity
    Copyright (C) 2023 Joseph Pasfield

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
