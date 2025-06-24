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

#include "movepick.h"
#include "normalize.h"

void Searcher::newGame() {
    // nothing really to reset here yet, so placeholder until I get a TT or histories
}

int16_t Searcher::search(Board &board, const int depth, int16_t alpha, int16_t beta, const int ply, const Limiters &limiters) {
    // repetition check
    if(ply > 0 && (board.getFiftyMoveCount() >= 50 || board.isRepeatedPosition())) return 0;
    // time manager
    if((nodes % 4096 == 0 || limiters.useNodes) && !limiters.keep_searching_hard(getTimeElapsed(), nodes)) {
        endSearch = true;
        return 0;
    }
    if(depth <= 0) return qsearch(board, alpha, beta, ply, limiters);
    if(ply > 256) return board.getEvaluation();
    // update seldepth
    if(ply > seldepth) seldepth = ply + 1;

    auto zobristHash = board.getZobristHash();
    auto entry = TT->getEntry(zobristHash);

    // move loop
    auto picker = MovePicker::search(board, entry->bestMove);
    int16_t bestScore = -mateScore;
    Move bestMove = Move();
    uint8_t legalMoves = 0;
    while(const auto move = picker.next()) {
        // make the move
        if(!board.makeMove<true>(move)) {
            continue;
        }
        legalMoves++;
        nodes++;

        // Recursion:tm:
        const int newDepth = depth - 1;
        const int16_t score = -search(board, newDepth, -beta, -alpha, ply + 1, limiters);
        board.undoMove<true>();

        // time check
        if(endSearch) return 0;

        if(score > bestScore) {
            bestScore = score;
            // alpha raise, new best move detected
            if(score > alpha) {
                alpha = score;
                bestMove = move;
                if(ply == 0) rootBestMove = move;
            }
            // beta cutoff
            if(score >= beta) {
                break;
            }
        }
    }
    // mate check
    if(legalMoves == 0) {
        if(board.isInCheck()) {
            return -mateScore + ply;
        }
        return 0;
    }

    // push to TT
    TT->setEntry(zobristHash, Transposition(bestMove));

    return bestScore;
}

int16_t Searcher::qsearch(Board &board, int16_t alpha, int16_t beta, const int ply, const Limiters &limiters) {
    // time manager
    if((nodes % 4096 == 0 || limiters.useNodes) && !limiters.keep_searching_hard(getTimeElapsed(), nodes)) {
        endSearch = true;
        return 0;
    }
    // update seldepth
    if(ply > seldepth) seldepth = ply + 1;

    // stand-pat
    const auto staticEval = board.getEvaluation();
    int16_t bestScore = staticEval;
    if(bestScore >= beta) return bestScore;
    if(alpha < bestScore) alpha = bestScore;

    auto zobristHash = board.getZobristHash();
    auto entry = TT->getEntry(zobristHash);

    // move loop
    auto picker = MovePicker::qsearch(board, entry->bestMove);
    Move bestMove = Move();
    while(const auto move = picker.next()) {
        // make the move
        if(!board.makeMove<true>(move)) {
            continue;
        }
        nodes++;

        // Recursion:tm:
        const int16_t score = -qsearch(board,  -beta, -alpha, ply + 1, limiters);
        board.undoMove<true>();

        // time check
        if(endSearch) return 0;

        if(score > bestScore) {
            bestScore = score;
            if(score > alpha) {
                alpha = score;
                bestMove = move;
            }
            if(score >= beta) {
                break;
            }
        }
    }

    // push to TT
    TT->setEntry(zobristHash, Transposition(bestMove));

    return bestScore;
}

void Searcher::outputInfo(const Board& board, const int score, const int depth, const int elapsedTime) const {
    std::string scoreString = " score ";
    if(abs(score) < abs(mateScore + 256)) {
        scoreString += "cp ";
        scoreString += std::to_string(normalize(score, board.getPlyCount()));
    } else {
        // score is checkmate in score - matedScore ply
        // position fen rn1q2rk/pp3p1p/2p4Q/3p4/7P/2NP2R1/PPP3P1/4RK2 w - - 0 1
        // ^^ mate in 3 test position
        const int colorMultiplier = score > 0 ? 1 : -1;
        scoreString += "mate ";
        scoreString += std::to_string((abs(abs(score) - mateScore) / 2 + board.getColorToMove()) * colorMultiplier);
    }
    std::cout << "info depth " << std::to_string(depth) << " seldepth " << std::to_string(seldepth) << " nodes " << std::to_string(nodes) << " time " << std::to_string(elapsedTime) << " nps " << std::to_string(int(double(nodes) / (elapsedTime == 0 ? 1 : elapsedTime) * 1000)) << scoreString << std::endl;
}

void Searcher::think(Board board, const Limiters &limiters, const bool info) {
    // reset things
    rootBestMove = Move();
    endSearch = false;
    nodes = 0;
    seldepth = 0;
    startTime = std::chrono::steady_clock::now();

    // Iterative Deepening
    int depth = 1;
    while(limiters.keep_searching_soft(getTimeElapsed(), nodes, depth)) {
        const Move previousBest = rootBestMove;
        const int16_t score = search(board, depth, -mateScore, mateScore, 0, limiters);
        if(endSearch) {
            rootBestMove = previousBest;
        }
        if(info) outputInfo(board, score, depth, getTimeElapsed());
        depth++;
    }

    if(rootBestMove == Move()) {
        std::array<Move, 256> moves;
        const int totalMoves = board.getMoves(moves);
        for(uint8_t moveIndex = 0; moveIndex < totalMoves; moveIndex++) {
            // information gathering
            const Move move = moves[moveIndex];

            // make the move
            if(!board.makeMove<true>(move)) {
                continue;
            }

            board.undoMove<false>();

            rootBestMove = move;
            break;
        }
    }

    if(info) std::cout << "bestmove " << toLongAlgebraic(rootBestMove) << std::endl;
}