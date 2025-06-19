/*
    Clarity
    Copyright (C) 2025 Joseph Pasfield

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
#include "search.h"
#include "normalize.h"

void Searcher::newGame() {
    // nothing really to reset here yet, so placeholder until i get a TT or histories
}

int16_t Searcher::negamax(Board &board, int depth, int ply, Limiters limiters) {
    if(depth <= 0 || ply > 256) return board.getEvaluation();
    if((nodes % 4096 == 0 || limiters.use_nodes) && !limiters.keep_searching_hard(getTimeElapsed(), nodes)) return 0;
    if(ply > seldepth) seldepth = ply;
    // get moves (don't worry)
    std::array<Move, 256> moves;
    const int totalMoves = board.getMoves(moves);

    // move loop
    int16_t bestScore = -mateScore;
    uint8_t legalMoves = 0;
    for(uint8_t moveIndex = 0; moveIndex < totalMoves; moveIndex++) {
        // information gathering
        const Move move = moves[moveIndex];

        // make the move
        if(!board.makeMove<true>(move)) {
            continue;
        }
        legalMoves++;
        nodes++;
        // Recursion:tm:
        const int newDepth = depth - 1;
        const int score = -negamax(board, newDepth, ply + 1, limiters);
        board.undoMove<true>();

        if(score > bestScore) {
            bestScore = score;
            if(ply == 0) rootBestMove = move;
        }

    }
    // mate check
    if(legalMoves == 0) {
        if(board.isInCheck()) {
            return -mateScore + ply;
        }
        return 0;
    }

    return bestScore;
}

void Searcher::outputInfo(const Board& board, int score, int depth, int elapsedTime) {
    //std::cout << "root pv length: " << stack[0].pvLength << std::endl;
    std::string scoreString = " score ";
    if(abs(score) < abs(mateScore + 256)) {
        scoreString += "cp ";
        scoreString += std::to_string(normalize(score, board.getPlyCount()));
    } else {
        // score is checkmate in score - matedScore ply
        // position fen rn1q2rk/pp3p1p/2p4Q/3p4/7P/2NP2R1/PPP3P1/4RK2 w - - 0 1
        // ^^ mate in 3 test position
        int colorMultiplier = score > 0 ? 1 : -1;
        scoreString += "mate ";
        scoreString += std::to_string((abs(abs(score) - mateScore) / 2 + board.getColorToMove()) * colorMultiplier);
    }
    std::cout << "info depth " << std::to_string(depth) << " seldepth " << std::to_string(seldepth) << " nodes " << std::to_string(nodes) << " time " << std::to_string(elapsedTime) << " nps " << std::to_string(int(double(nodes) / (elapsedTime == 0 ? 1 : elapsedTime) * 1000)) << scoreString << std::endl;
}

void Searcher::think(Board board, Limiters limiters, bool info) {
    // reset things
    rootBestMove = Move();
    nodes = 0;
    seldepth = 0;
    startTime = std::chrono::steady_clock::now();

    // Iterative Deepening
    int depth = 1;
    while(limiters.keep_searching_soft(getTimeElapsed(), nodes, depth)) {
        const Move previousBest = rootBestMove;
        int16_t score = negamax(board, depth, 0, limiters);
        if(!limiters.keep_searching_hard(getTimeElapsed(), nodes)) {
            rootBestMove = previousBest;
        }
        if(info) outputInfo(board, score, depth, getTimeElapsed());
        depth++;
    }

    if(info) std::cout << "bestmove " << toLongAlgebraic(rootBestMove) << std::endl;
}