#pragma once

void generateData();
void threadFunction();
void dumpToArray(double result, std::vector<std::string>& fenVector);
int checkForMates(Board& board);
double runGame(std::vector<std::string>& fenVector);