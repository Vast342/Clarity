/*
    Clarity
    Copyright (C) 2026 Joseph Pasfield

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/  
#pragma once

#include "globals.h"
#include "tunables.h"

struct SearchLimiters {
public:
    // etime = elapsed time
    inline bool softLimitCheck(uint64_t etime, 
                               Move rootBestMove, 
                               uint64_t nodes, 
                               std::array<std::array<int, 64>, 64> &nodeTMTable, 
                               int depth,
                               int stability) {
        if(useTimeLimit) {
            double frac = nodes
            ? nodeTMTable[rootBestMove.getStartSquare()][rootBestMove.getEndSquare()]
                / static_cast<double>(nodes)
            : 0.0;
            if(etime > softTimeLimit() 
                          * (depth > ntmDepthCondition.value ? (ntmSubtractor.value - frac) * ntmMultiplier.value : ntmDefault.value) 
                          * bmStabilityNumbers[std::min(stability, 6)]->value) {
                return false;
            }
        }
        if(useDepthLimit && depth >= depthLimit) {
            return false;
        }
        if(useHardNodeLimit) {
            if(nodes >= hardNodeLimit) {
                return false;
            }
        }
        if(useSoftNodeLimit && nodes >= softNodeLimit) {
            return false;
        }
        return true;
    }

    // returns whether the search should continue
    inline bool hardLimitCheck(uint64_t nodes,
                               std::chrono::steady_clock::time_point begin) {
        if(useTimeLimit && nodes % 4096 == 0) {
            uint64_t etime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - begin).count();
            if(etime > hardTimeLimit()) {
                return false;
            }
        }
        if(useHardNodeLimit) {
            if(nodes >= hardNodeLimit) {
                return false;
            }
        }
        return true;
    }

    inline void writeValues(uint64_t time, uint64_t inc, uint64_t mtg, uint64_t depth, uint64_t hardNodes, uint64_t softNodes) {
        if(time != 0 || inc != 0 || mtg != 20) {
            useTimeLimit = true;
            stime = time;
            sinc = inc;
            movestogo = mtg;
        }
        if(depth != 0) {
            useDepthLimit = true;
            depthLimit = depth;
        }
        if(hardNodes != 0) {
            useHardNodeLimit = true;
            hardNodeLimit = hardNodes;
        }
        if(softNodes != 0) {
            useSoftNodeLimit = true;
            softNodeLimit = softNodes;
        }
    }

    inline int getDepthLimit() {
        return useDepthLimit ? depthLimit : 100;
    }
private:
    bool useTimeLimit = false;
    // start time and start increment
    uint64_t stime = 0;
    uint64_t sinc = 0;
    uint64_t movestogo = 0;
    bool useDepthLimit = false;
    int depthLimit = 0;
    bool useHardNodeLimit = false;
    uint64_t hardNodeLimit = 0;
    bool useSoftNodeLimit = false;
    uint64_t softNodeLimit = 0;

    inline uint64_t softTimeLimit() {
        return tmsMultiplier.value * (stime / movestogo + sinc * tmsNumerator.value / tmsDenominator.value);
    }

    inline uint64_t hardTimeLimit() {
        return stime / tmhDivisor.value;
    }
};