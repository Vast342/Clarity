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
    history.clear();
    stack = {};
}

template <bool isPV>
int16_t Searcher::search(Board &board, const int depth, int16_t alpha, const int16_t beta, const int ply, const Limiters &limiters) {
    stack[ply].pvLength = 0;
    // repetition check
    if(ply > 0 && (board.getFiftyMoveCount() >= 50 || board.isRepeatedPosition())) return 0;
    // time manager
    if((nodes % 4096 == 0 || limiters.useNodes) && !limiters.keep_searching_hard(getTimeElapsed(), nodes)) {
        endSearch = true;
        return 0;
    }
    if(depth <= 0) return qsearch(board, alpha, beta, ply, limiters);
    if(ply >= plyLimit) return board.getEvaluation();
    // update seldepth
    if(ply > seldepth) seldepth = ply + 1;

    const auto zobristHash = board.getZobristHash();
    const auto entry = TT->getEntry(zobristHash);

    // Prunings!
    if constexpr(!isPV) {
        const auto staticEval = board.getEvaluation();
        const auto inCheck = board.isInCheck();
        // Reverse Futility Pruning (RFP)
        if(!inCheck && staticEval - rfpMultiplier.value * depth >= beta && depth < rfpDepthCondition.value) return staticEval;

        // Null Move Pruning (NMP)
        // "I could probably detect zugzwang here but ehhhhh" -Me, 2 years ago
        if(ply > 0 && !stack[ply - 1].isNull && depth >= nmpDepthCondition.value && !inCheck && staticEval >= beta) {
            board.changeColor();
            stack[ply].isNull = true;
            constexpr auto r = 3;
            const int score = -search(board, depth - r, 0-beta, 1-beta, ply + 1, limiters);
            board.undoChangeColor();
            stack[ply].isNull = false;
            if(score >= beta) {
                return score;
            }
        }

        // tt cutoffs
        if(ply > 0 && entry->zobristKey == shrink(zobristHash) && entry->depth >= depth && (
                    entry->flag == Exact // exact score
                || (entry->flag == BetaCutoff && entry->score >= beta) // lower bound, fail high
                || (entry->flag == FailLow && entry->score <= alpha) // upper bound, fail low
        )) {
            return entry->score;
        }
    }

    // move loop
    auto picker = MovePicker::search(board, entry->bestMove, history);
    int16_t bestScore = -mateScore;
    auto flag = FailLow;
    auto bestMove = Move();
    uint8_t legalMoves = 0;
    std::array<Move, 256> testedMoves;
    while(const auto move = picker.next()) {
        // make the move
        if(!board.isLegal(move)) {
            continue;
        }
        board.makeMove<true>(move);
        testedMoves[legalMoves++] = move;
        nodes++;

        // Recursion:tm:
        const int newDepth = depth - 1;
        int16_t score = 0;
        // first move
        if(!isPV || legalMoves > 1) {
            score = -search(board, newDepth, -alpha-1, -alpha, ply + 1, limiters);
        }
        if(isPV && (legalMoves == 1 || score > alpha)) {
            score = -search<true>(board, newDepth, -beta, -alpha, ply + 1, limiters);
        }
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
                flag = Exact;
                if constexpr(isPV) {
                    stack[ply].pvTable[0] = move;
                    stack[ply].pvLength = stack[ply + 1].pvLength + 1;
                    for (int i = 0; i < stack[ply + 1].pvLength; i++)
                        stack[ply].pvTable[i + 1] = stack[ply + 1].pvTable[i];
                }
            }
            // beta cutoff
            if(score >= beta) {
                history.betaCutoff(board, board.getColorToMove(), move, testedMoves, legalMoves, depth);
                flag = BetaCutoff;
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
    TT->setEntry(zobristHash, Transposition(zobristHash, bestMove, flag, bestScore, depth));

    return bestScore;
}

int16_t Searcher::qsearch(Board &board, int16_t alpha, const int16_t beta, const int ply, const Limiters &limiters) {
    stack[ply].pvLength = 0;
    // time manager
    if((nodes % 4096 == 0 || limiters.useNodes) && !limiters.keep_searching_hard(getTimeElapsed(), nodes)) {
        endSearch = true;
        return 0;
    }
    // update seldepth
    if(ply > seldepth) seldepth = ply + 1;
    if(ply >= plyLimit) return board.getEvaluation();

    const auto zobristHash = board.getZobristHash();
    const auto entry = TT->getEntry(zobristHash);

    // tt cutoffs
    if(ply > 0 && entry->zobristKey == shrink(zobristHash) && (
                entry->flag == Exact // exact score
            || (entry->flag == BetaCutoff && entry->score >= beta) // lower bound, fail high
            || (entry->flag == FailLow && entry->score <= alpha) // upper bound, fail low
    )) {
        return entry->score;
    }

    // stand-pat
    const auto staticEval = board.getEvaluation();
    int16_t bestScore = staticEval;
    if(bestScore >= beta) return bestScore;
    if(alpha < bestScore) alpha = bestScore;

    // move loop
    auto picker = MovePicker::qsearch(board, entry->bestMove, history);
    auto flag = FailLow;
    Move bestMove = Move();
    while(const auto move = picker.next()) {
        // make the move
        if(!board.isLegal(move)) {
            continue;
        }
        board.makeMove<true>(move);
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
                stack[ply].pvTable[0] = move;
                stack[ply].pvLength = stack[ply + 1].pvLength + 1;
                for (int i = 0; i < stack[ply + 1].pvLength; i++)
                    stack[ply].pvTable[i + 1] = stack[ply + 1].pvTable[i];
            }
            if(score >= beta) {
                flag = BetaCutoff;
                break;
            }
        }
    }

    // push to TT
    TT->setEntry(zobristHash, Transposition(zobristHash, bestMove, flag, bestScore, 0));

    return bestScore;
}

std::string Searcher::getPV() const {
    std::string pv;
    for(int i = 0; i < stack[0].pvLength; i++) {
        pv += toLongAlgebraic(stack[0].pvTable[i]) + " ";
    }
    return pv;
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
    std::cout << "info depth " << std::to_string(depth)
              << " seldepth " << std::to_string(seldepth)
              << " nodes " << std::to_string(nodes)
              << " time " << std::to_string(elapsedTime)
              << " nps " << std::to_string(int(double(nodes) / (elapsedTime == 0 ? 1 : elapsedTime) * 1000))
              << scoreString
              << " pv " << getPV() << std::endl;
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
    int16_t score = 0;
    while(limiters.keep_searching_soft(getTimeElapsed(), nodes, depth)) {
        const Move previousBest = rootBestMove;
        if(depth > aspDepthCondition.value) {
            int delta = aspBaseDelta.value;
            int alpha = std::max(int(-mateScore), score - delta);
            int beta = std::min(int(mateScore), score + delta);
            while(true) {
                score = search<true>(board, depth, alpha, beta, 0, limiters);
                if(endSearch) break;

                if(score >= beta) {
                    beta = std::min(beta + delta, int(mateScore));
                } else if(score <= alpha) {
                    beta = (alpha + beta) / 2;
                    alpha = std::max(alpha - delta, int(-mateScore));
                } else break;

                delta *= aspDeltaMultiplier.value;
            }
        } else {
            score = search<true>(board, depth, -mateScore, mateScore, 0, limiters);
        }
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
            if(!board.isLegal(move)) {
                continue;
            }

            rootBestMove = move;
            break;
        }
    }

    if(info) std::cout << "bestmove " << toLongAlgebraic(rootBestMove) << std::endl;
}