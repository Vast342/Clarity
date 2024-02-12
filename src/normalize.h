#pragma once

#include "globals.h"

// initial values borrowed from stockfish
std::array<double, 8> params = {-1.719, 32.248, -71.755, 292.083, 75.399, 2.505, -129.953, 14.509};

// function to get horizontal shift parameter
inline double get_logistic_a(int move_count) {
    return ((params[0] * move_count / 32 + params[1]) * move_count / 32 + params[2]) * move_count / 32 + params[3];
}

// function to get curve aggressiveness parameter
inline double get_logistic_b(int move_count) {
    return ((params[4] * move_count / 32 + params[5]) * move_count / 32 + params[6]) * move_count / 32 + params[7];
}

// calculates the win percentage based on the above parameters (we love logistdic curves in this household)
inline double calculate_winpercent(int score, int move_count) {
    return 1 / (1 + exp(-(score - get_logistic_a(move_count))) / get_logistic_b(move_count));
}

// calculates the ratio of wins and draws and losses
inline std::array<double, 3> calculate_wdl(int score, int move_count) {
    const double win_ratio = calculate_winpercent(score, move_count);
    const double loss_ratio = calculate_winpercent(-score, move_count);
    const double draw_ratio = 1 - win_ratio - loss_ratio;
    return {win_ratio, draw_ratio, loss_ratio};
}

inline int normalize(int score, int plyCount) {
    return std::round(100.0 * double(score) / get_logistic_a(plyCount));
}