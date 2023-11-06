#pragma once

void generateData(int numGames);
void threadFunction();
void dumpToArray(double result, std::vector<std::string>& fenVector);
double runGame(std::vector<std::string>& fenVector);