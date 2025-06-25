// oh boy movepicker
#pragma once

#include "globals.h"
#include "history.h"

enum class MovegenStage : int {
    TTMove = 0,
    GenAll,
    All,
    QSGenAll,
    QSAll,
};

inline MovegenStage& operator++(MovegenStage& v) {
    v = static_cast<MovegenStage>(static_cast<int>(v) + 1);
    return v;
}

class MovePicker {
public:
    inline Move next() {
        switch(stage) {
            case MovegenStage::TTMove: {
                ++stage;
                if(ttMove && board.isPseudolegal(ttMove)) {
                    return ttMove;
                }
                [[fallthrough]];
            }
            case MovegenStage::GenAll: {
                totalMoves = board.getMoves(moves);
                idx = 0;
                scoreMoves();

                ++stage;
                [[fallthrough]];
            }
            case MovegenStage::All: {
                auto move = getNextInternal();
                // skips tt move because its staged
                if(move == ttMove) move = getNextInternal();

                return move;
            }
            case MovegenStage::QSGenAll: {
                totalMoves = board.getMovesQSearch(moves);
                idx = 0;
                scoreMoves();

                ++stage;
                [[fallthrough]];
            }
            case MovegenStage::QSAll: {
                return getNextInternal();
            }
            default:
                return Move();
        }
    }
    static inline MovePicker search(const Board &board, const Move ttMove, HistoryTables &tables) {
        return MovePicker(board, ttMove, tables, MovegenStage::TTMove);
    }
    static inline MovePicker qsearch(const Board &board, const Move ttMove, HistoryTables &tables) {
        return MovePicker(board, ttMove, tables, MovegenStage::QSGenAll);
    }
    inline int getTotalMoves() const {
        return totalMoves;
    }
private:
    explicit MovePicker(const Board &board, const Move ttMove, HistoryTables &tables, const MovegenStage stage) : stage(stage),
    board{board}, ttMove(ttMove), idx{0}, totalMoves{0}, tables{tables} {}
    inline void scoreMoves() {
        for(int i = 0; i < totalMoves; ++i) {
            const auto move = moves[i];
            if(move == ttMove) {
                moveScores[i] = 100000000;
            } else {
                const auto victim = getType(board.pieceAtIndex(move.getEndSquare()));
                // captures, mvv-lva
                if(victim != None) {
                    const auto piece = getType(board.pieceAtIndex(move.getStartSquare()));
                    moveScores[i] = MVV_values[victim]->value * 10 - MVV_values[piece]->value + 16384;
                } else {
                    const auto start = move.getStartSquare();
                    const auto end = move.getEndSquare();
                    const auto startAttack = board.squareIsUnderAttack(start);
                    const auto endAttack = board.squareIsUnderAttack(end);
                    moveScores[i] = tables.historyTable[board.getColorToMove()][start][startAttack][end][endAttack];
                }
            }
        }
    }
    inline Move getNextInternal() {
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

        return moves[idx++];
    }
    MovegenStage stage;
    const Board &board;
    Move ttMove;
    int idx;
    int totalMoves;
    HistoryTables &tables;
    std::array<Move, 256> moves;
    std::array<int, 256> moveScores;
};
