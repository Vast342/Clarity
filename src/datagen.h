#pragma once

#include "search.h"

void generateData(int numGames, int numThreads);
void threadFunction(int numGames, int threadID);
void dumpToArray(std::ofstream &output, double result, std::vector<std::string>& fenVector);
double runGame(Engine &engine, std::vector<std::string>& fenVector, Board board);
