// oh boy movepicker
#pragma once

#include "globals.h"

enum class MovegenStage : int {
    GenOther = 0,
    Other,
};

inline MovegenStage& operator++(MovegenStage& v) {
    v = static_cast<MovegenStage>(static_cast<int>(v) + 1);
    return v;
}

class MovePicker {
public:
    inline Move next() {
        switch(stage) {
            case MovegenStage::GenOther: {
                totalMoves = board.getMoves(moves);
                idx = 0;
                scoreMoves();

                ++stage;
                [[fallthrough]];
            }
            case MovegenStage::Other: {
                // incremental sort
                for(int j = idx + 1; j < totalMoves; j++) {
                    if(moveScores[j] > moveScores[idx]) {
                        std::swap(moveScores[j], moveScores[idx]);
                        std::swap(moves[j], moves[idx]);
                    }
                }

                return moves[idx++];
            }
            default:
                return Move();
        }
    }
    explicit MovePicker(const Board &board) : stage(MovegenStage::GenOther),
    board{board}, idx{0}, totalMoves{0}, moveScores{{}} {}
private:
    inline void scoreMoves() {
        for(int i = 0; i < totalMoves; ++i) {
            const auto move = moves[i];
            const auto victim = getType(board.pieceAtIndex(move.getEndSquare()));
            // captures, mvv-lva
            if(victim != None) {
                const auto piece = getType(board.pieceAtIndex(move.getStartSquare()));
                moveScores[i] = MVV_values[victim]->value * 10 - MVV_values[piece]->value;
            }
        }
    }
    MovegenStage stage;
    const Board &board;
    int idx;
    int totalMoves;
    std::array<Move, 256> moves;
    std::array<int, 256> moveScores;
};
