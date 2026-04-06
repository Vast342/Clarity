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
// oh boy movepicker
#pragma once

#include "globals.h"
#include "info.h"
#include "see.h"

constexpr int historyCap = 16384;
constexpr int killerScore = 81922;
constexpr int counterScore = 81921;
constexpr int goodCaptureBonus= 500000;

enum class MovegenStage : int {
    TTMove = 0,
    GenerateNoisy,
    GoodNoisy,
    Killer,
    Counter,
    GenerateQuiet,
    Quiets,
    BadNoisy,
    QSGenAll,
    QSAll,
};

inline MovegenStage& operator++(MovegenStage& v) {
    v = static_cast<MovegenStage>(static_cast<int>(v) + 1);
    return v;
}

class MovePicker {
public:
     std::pair<Move, int> next() {
        switch(stage) {
            case MovegenStage::TTMove: {
                ++stage;
                if(ttMove && board.isPseudolegal(ttMove)) {
                    return {ttMove, 1000000000};
                }
                [[fallthrough]];
            }
            case MovegenStage::GenerateNoisy: {
                totalMoves = board.getNoisies(moves, 0);
                idx = 0;
                scoreMoves();

                ++stage;
                [[fallthrough]];
            }
            case MovegenStage::GoodNoisy: {
                while(idx < totalMoves) {
                    const auto [move, score] = getNextInternal();

                    if(move == ttMove) continue;

                    // to test later: see threshold here (failed before i think)
                    if(!see(board, move, 0)) {
                        moves[badNoisyEnd] = move;
                        moveScores[badNoisyEnd] = score;
                        badNoisyEnd++;
                    } else {
                        return {move, score};
                    }
                }

                ++stage;
                [[fallthrough]];
            }
            case MovegenStage::Killer: {
                ++stage;
                const auto killer = info.stack[ply].killer;
                if(killer && board.isPseudolegal(killer) && killer != ttMove) {
                    return {killer, killerScore};
                }
                [[fallthrough]];
            }
            case MovegenStage::Counter: {
                ++stage;
                if(ply > 0) {
                    const auto counter = info.counterMoves[info.stack[ply - 1].move.getStartSquare()][info.stack[ply - 1].move.getEndSquare()];
                    if(counter && board.isPseudolegal(counter) && counter != ttMove && counter != info.stack[ply].killer) {
                        return {counter, counterScore};
                    }
                }
                [[fallthrough]];
            }
            case MovegenStage::GenerateQuiet: {
                totalMoves = board.getQuiets(moves, totalMoves);
                scoreMoves();

                ++stage;
                [[fallthrough]];
            }
            case MovegenStage::Quiets: {
                const auto counter = ply > 0 ? info.counterMoves[info.stack[ply - 1].move.getStartSquare()][info.stack[ply - 1].move.getEndSquare()] : Move();
                const auto killer = info.stack[ply].killer;
                while(idx < totalMoves) {
                    const auto [move, score] = getNextInternal();
                    if(move == ttMove || move == killer || move == counter) continue;
                    return {move, score};
                }

                idx = 0;
                totalMoves = badNoisyEnd;
                ++stage;
                [[fallthrough]];
            }
            case MovegenStage::BadNoisy: {
                return getNextInternal();
            }
            case MovegenStage::QSGenAll: {
                totalMoves = board.getMovesQSearch(moves);
                idx = 0;
                scoreMovesQS();

                ++stage;
                [[fallthrough]];
            }
            case MovegenStage::QSAll: {
                return getNextInternal();
            }
            default:
                return {Move(), 0};
        }
    }
    static MovePicker search(const Board &board, const Move ttMove, SearchInfo &tables, const int ply) {
        return MovePicker(board, ttMove, tables, MovegenStage::TTMove, ply);
    }
    static MovePicker qsearch(const Board &board, const Move ttMove, SearchInfo &tables) {
        return MovePicker(board, ttMove, tables, MovegenStage::QSGenAll, 0);
    }
    int getTotalMoves() const {
        return totalMoves;
    }
private:
    explicit MovePicker(const Board &board, const Move ttMove, SearchInfo &tables, const MovegenStage stage, const int ply) : stage(stage),
    board{board}, ttMove(ttMove), idx{0}, totalMoves{0}, info{tables}, ply(ply), badNoisyEnd(0) {}
    void scoreMoves() {
        const uint64_t occupied = board.getOccupiedBitboard();
        const int colorToMove = board.getColorToMove();
        for(int i = idx; i < totalMoves; i++) {
            Move move = moves[i];
            const int end = move.getEndSquare();
            const int start = move.getStartSquare();
            const int piece = getType(board.pieceAtIndex(start));
            if(move == ttMove) {
                moveScores[i] = 1000000000;
            } else if((occupied & (1ULL << end)) != 0) {
                // Captures!
                const int victim = getType(board.pieceAtIndex(end));
                // Capthist!
                moveScores[i] = MVV_values[victim]->value + info.noisyHistoryTable[colorToMove][piece][end][victim][board.squareIsUnderAttack(end)];
            } else {
                // if not in qsearch, killers
                if(move == info.stack[ply].killer) {
                    moveScores[i] = killerScore;
                } else if(ply > 0 && move == info.counterMoves[info.stack[ply - 1].move.getStartSquare()][info.stack[ply - 1].move.getEndSquare()]) {
                    moveScores[i] = counterScore;   
                } else {
                    int hash = board.getPawnHashIndex();
                    // read from history
                    moveScores[i] = info.historyTable[colorToMove][start][board.squareIsUnderAttack(start)][end][board.squareIsUnderAttack(end)]
                        + (ply > 0 ? (*info.stack[ply - 1].ch_entry)[colorToMove][piece][end] : 0)
                        + (ply > 1 ? (*info.stack[ply - 2].ch_entry)[colorToMove][piece][end] : 0)
                        + (ply > 3 ? (*info.stack[ply - 4].ch_entry)[colorToMove][piece][end] : 0)
                        + info.pawnHistoryTable[hash][colorToMove][piece][end];
                }
            }
        }
    }

    void scoreMovesQS() {
        const int colorToMove = board.getColorToMove();
        for(int i = 0; i < totalMoves; i++) {
            Move move = moves[i];
            const int end = move.getEndSquare();
            const int start = move.getStartSquare();
            const int piece = getType(board.pieceAtIndex(start));
            if(move == ttMove) {
                moveScores[i] = 1000000000;
            } else {
                // Captures!
                const int victim = getType(board.pieceAtIndex(end));
                // Capthist!
                moveScores[i] = MVV_values[victim]->value + info.qsHistoryTable[colorToMove][piece][end][victim];
            }
        }
    }
    std::pair<Move, int> getNextInternal() {
        if (idx >= totalMoves) return {Move(), 0};

        int bestIdx = idx;
        for(int j = idx + 1; j < totalMoves; j++) {
            if(moveScores[j] > moveScores[bestIdx]) {
                bestIdx = j;
            }
        }

        if(bestIdx != idx) {
            std::swap(moveScores[bestIdx], moveScores[idx]);
            std::swap(moves[bestIdx], moves[idx]);
        }

        return {moves[idx], moveScores[idx++]};
    }
    MovegenStage stage;
    const Board &board;
    Move ttMove;
    int idx;
    int totalMoves;
    SearchInfo &info;
    int ply;
    std::array<Move, 256> moves;
    std::array<int, 256> moveScores;
    int badNoisyEnd{};
};