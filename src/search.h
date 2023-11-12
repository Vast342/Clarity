#pragma once

#include "globals.h"

constexpr int mateScore = -10000000;

extern bool timesUp;
void resetEngine();
Move think(Board board, int softBound, int hardBound, bool info);
void resizeTT(int newSize);
int benchSearch(Board board, int depthToSearch);
Move fixedDepthSearch(Board board, int depthToSearch, bool info);
std::pair<Move, int> dataGenSearch(Board board, int nodeCap);
bool see(const Board& board, Move move, int threshold);