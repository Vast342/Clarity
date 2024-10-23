
#include "globals.h"

struct Corrhist {
    static constexpr int size = 16384;
    static constexpr int scale = 256;
    static constexpr int mask = size - 1;
    static constexpr int min = -64;
    static constexpr int max = 64;
    std::array<std::array<int32_t, 2>, size> pawnTable;
    inline int correct(int ctm, int pawnHash, int staticEval) {
        int correction = pawnTable[pawnHash][ctm];
        constexpr int corrhistScale = 256;
        return (staticEval + correction / corrhistScale);
    }
    inline void clear() {
        std::memset(pawnTable.data(), 0, sizeof(pawnTable));
    }
    inline void push(int pawnHash, int ctm, int bestScore, int staticEval, int depth) {
        // don't worry about it gents, i'll make this tunable soon:tm:
        auto &score = pawnTable[pawnHash][ctm];
        int error = bestScore - staticEval;
        int scaled_bonus = error * scale;
        const int weight = std::min(depth - 1, 16);
        score = (score * (scale - weight) + scaled_bonus * weight) / scale;
        score = std::clamp(score, min * scale, max * scale);
    }
};

