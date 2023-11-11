#pragma once

void generateData(int numGames);
void threadFunction(int numGames, int threadID);
void dumpToArray(double result, std::vector<std::string>& fenVector);
double runGame(std::vector<std::string>& fenVector);