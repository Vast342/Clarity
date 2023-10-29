#include "psqt.h"

std::array<std::array<int, 64>, 6> mgTables;
std::array<std::array<int, 64>, 6> egTables;

// adds material values to the psqts on startup, to save a bit of computation later
void computePSQTs() {
    for(int i = 0; i < 6; i++) {
        for(int j = 0; j < 64; j++) {
            mgTables[i][j] = mgBonusTables[i][j] + mg_value[i];
            egTables[i][j] = egBonusTables[i][j] + eg_value[i];
        }
    }
}