#pragma once

void generateData(int numGames);
void threadFunction(int numGamesD);
void dumpToArray(double result, std::vector<std::string>& fenVector);
double runGame(std::vector<std::string>& fenVector, Board board);