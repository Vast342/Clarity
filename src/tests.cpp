/*
    Clarity
    Copyright (C) 2026 Joseph Pasfield

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
#include "testessentials.h"
#include "testsuites.h"

// runs a single perft test
int perft(Board &board, int depth) {
    if(depth == 0) return 1;
    std::array<Move, 256> moves;
    int numMoves = board.getMoves(moves);
    int result = 0;
    for(int i = 0; i < numMoves; i++) {
        if(board.isLegal(moves[i])) {
            board.makeMove<false>(moves[i]);
            result += perft(board, depth-1);
            board.undoMove<false>();
        }
    }
    return result;
}

// runs an entire suite of perft tests
void runPerftSuite(const int cap) {
    int i = 0;
    int passed = 0;
    int failed = 0;
    double total = 0;
    uint64_t expectedTotalNodes = 0;
    uint64_t completedExpectedNodes = 0;
    for(const auto& test : etherealSuite) {
        if(test.expectedOutput < cap || cap == 0) {
            expectedTotalNodes += test.expectedOutput;
        }
    }
    auto start = std::chrono::steady_clock::now();
    for(const auto& test : etherealSuite) {
        i++;
        if(test.expectedOutput < cap || cap == 0) {
            Board board(test.fen);
            int result = perft(board, test.depth);
            total += result;
            completedExpectedNodes += test.expectedOutput;
            auto now = std::chrono::steady_clock::now();
            double elapsed = std::chrono::duration<double>(now - start).count();
            double progress = 100.0 * completedExpectedNodes / expectedTotalNodes;
            uint64_t avgNps = elapsed > 0.0 ? static_cast<uint64_t>(std::llround(total / elapsed)) : 0;
            double eta = progress > 0.0 ? elapsed * (100.0 - progress) / progress : 0.0;
            if(result == test.expectedOutput) {
                printf("Test %i Passed | Progress: %.2f%% | Avg NPS: %llu | ETA: %.1fs\n",
                i,
                progress,
                (unsigned long long)avgNps,
                eta);
                passed++;
            } else {
                std::cout
                    << "Test " << i
                    << " Failed, outputted " << result
                    << " With fen string " << test.fen
                    << " and depth " << test.depth << '\n';

                failed++;
            }
        }
    }

    auto end = std::chrono::steady_clock::now();
    double elapsed = std::chrono::duration<double>(end - start).count();
    std::cout << "Passed " << passed << ", Failed " << failed << '\n';
    std::cout << "Tests took: " << elapsed << " seconds\n";
    std::cout << "Total nodes: " << static_cast<uint64_t>(total) << '\n';
    uint64_t nps = static_cast<uint64_t>(std::llround(total / elapsed));
    std::cout << "NPS: " << nps << '\n';
}

// runs perft split by what the first move that is done is
void splitPerft(Board board, int depth) {
    std::array<Move, 256> moves;
    int numMoves = board.getMoves(moves);
    int total = 0;
    clock_t start = clock();
    for(int i = 0; i < numMoves; i++) {
        if(board.isLegal(moves[i])) {
            board.makeMove<false>(moves[i]);
            int result = perft(board, depth - 1);
            board.undoMove<false>();
            total += result;
            std::cout << toLongAlgebraic(moves[i]) << ": " << std::to_string(result) << '\n';
        }
    }
    clock_t end = clock();
    std::cout << "Total: " << std::to_string(total) << '\n';
    std::cout << "Time: " << std::to_string((end-start)/static_cast<double>(1000)) << '\n';
    std::cout << "NPS: " << std::to_string(total / ((end-start)/static_cast<double>(1000))) << '\n';
}

// runs an individual perft test, and outputs the results
void individualPerft(Board board, int depth) {
    clock_t start = clock();
    int result = perft(board, depth);
    clock_t end = clock();
    std::cout << "Result: " << std::to_string(result) << '\n';
    std::cout << "Time: " << std::to_string((end-start)/static_cast<double>(1000)) << '\n';
    std::cout << "NPS: " << std::to_string(result / ((end-start)/static_cast<double>(1000))) << '\n';
}