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
                // would score moves here too Later:tm:

                ++stage;
                [[fallthrough]];
            }
            case MovegenStage::Other: {
                return moves[idx++];
            }
            default:
                return Move();
        }
    }
    explicit MovePicker(const Board &board) : stage(MovegenStage::GenOther),
    board{board}, idx{0}, totalMoves{0} {}
private:
    MovegenStage stage;
    const Board &board;
    int idx;
    int totalMoves;
    std::array<Move, 256> moves;
};
