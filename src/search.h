#pragma once

#include "globals.h"


int see(Board board, int square);
void resetEngine();
Move think(Board board, int softBound, int hardBound);
void resizeTT(int newSize);
int benchSearch(Board board, int depthToSearch);
Move fixedDepthSearch(Board board, int depthToSearch);
void softTimeCheck();
void hardTimeCheck();