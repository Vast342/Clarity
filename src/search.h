#pragma once

#include "globals.h"

void resetEngine();
Move think(Board board, int softBound, int hardBound);
void resizeTT(int newSize);
int benchSearch(Board board, int depthToSearch);
Move fixedDepthSearch(Board board, int depthToSearch);
bool see(const Board& board, Move move);