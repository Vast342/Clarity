// oh boy movepicker
#pragma once

#include "globals.h"

enum class MovegenStage : int {
    GenAll = 0,
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
            case MovegenStage::GenAll: {
                totalMoves = board.getMoves(moves);
                idx = 0;
                scoreMoves();

                ++stage;
                [[fallthrough]];
            }
            case MovegenStage::All: {
                incrementalSort();
                return moves[idx++];
            }
            case MovegenStage::QSGenAll: {
                totalMoves = board.getMovesQSearch(moves);
                idx = 0;
                scoreMoves();

                ++stage;
                [[fallthrough]];
            }
            case MovegenStage::QSAll: {
                incrementalSort();
                return moves[idx++];
            }
            default:
                return Move();
        }
    }
    static inline MovePicker search(const Board &board, const Move ttMove) {
        return MovePicker(board, ttMove, MovegenStage::GenAll);
    }
    static inline MovePicker qsearch(const Board &board, const Move ttMove) {
        return MovePicker(board, ttMove, MovegenStage::QSGenAll);
    }
private:
    explicit MovePicker(const Board &board, const Move ttMove, const MovegenStage stage) : stage(stage),
    board{board}, ttMove(ttMove), idx{0}, totalMoves{0}, moveScores{{}} {}
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
                    moveScores[i] = MVV_values[victim]->value * 10 - MVV_values[piece]->value;
                }
            }
        }
    }
    inline void incrementalSort() {
        for(int j = idx + 1; j < totalMoves; j++) {
            if(moveScores[j] > moveScores[idx]) {
                std::swap(moveScores[j], moveScores[idx]);
                std::swap(moves[j], moves[idx]);
            }
        }
    }
    MovegenStage stage;
    const Board &board;
    Move ttMove;
    int idx;
    int totalMoves;
    std::array<Move, 256> moves;
    std::array<int, 256> moveScores;
};
