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
#include "globals.h"
#include "datagen.h"
#include "search.h"

// todo: book support
std::string getRandomPos(std::mt19937_64& gen) {
    Board board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    // currently just does 8 random moves
    for(int i = 0; i < 8; i++) {
        // get moves
        std::array<Move, 256> PLmoves;
        const int totalMoves = board.getMoves(PLmoves);

        std::array<Move, 256> moves;
        int legalMoves = 0;
        // legality check
        for(int j = 0; j < totalMoves; j++) {
            if(board.makeMove<true>(PLmoves[j])) {
                moves[legalMoves] = PLmoves[j];
                legalMoves++;
                board.undoMove<true>();
            }
        }
        // checkmate or stalemate? doesn't matter, restart
        if(legalMoves == 0) {
            return "Invalid";
        }
        // distribution
        std::uniform_int_distribution distribution{0, legalMoves - 1};

        const int index = distribution(gen);
        board.makeMove<false>(moves[index]);
    }
    return board.getFenString();
}

void genFens(int numPos, uint64_t seed) {
    std::mt19937_64 gen(seed);
    for(int pos = 0; pos < numPos; pos++) {
        std::string fen = "Invalid";
        while(fen == "Invalid") {
            fen = getRandomPos(gen);
        }
        std::cout << "info string genfens " << fen << std::endl;
    }
}