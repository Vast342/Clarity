#pragma once

#include "globals.h"

void resetEngine();
Move think(Board board, int timeLeft);
void resizeTT(int newSize);
int benchSearch(Board board, int depthToSearch);