/*
    Clarity
    Copyright (C) 2025 Joseph Pasfield

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
// handles search limiters, such as nodes, depth, etc

#include "tunables.h"
#include <cstdint>
#include <algorithm>

class Limiters {
    private:
        bool useTime;
        int64_t time;
        int64_t increment;
        uint64_t nodeLimit;
        bool use_depth;
        int32_t depthLimit;
        bool useMoveTime;
        int64_t moveTime;
        int64_t movesToGo;
        int64_t softCap;
        int64_t hardCap;

        uint64_t time_allocated_soft() const {
            return tmsMultiplier.value * (time / movesToGo + increment * tmsNumerator.value / tmsDenominator.value);
        }

        uint64_t time_allocated_hard() const {
            return time / tmhDivisor.value;
        }

    public:
        bool use_nodes;
        Limiters() : useTime(false), time(0), increment(0), 
                    nodeLimit(0), use_depth(false), depthLimit(0), 
                    useMoveTime(false), moveTime(0), movesToGo(20), use_nodes(false) {}

        void load_values(int64_t tim, int64_t inc, uint64_t nodes, int32_t depth, int64_t movetime, int64_t mtg) {
            useTime = (tim != 0);
            use_nodes = (nodes != 0);
            use_depth = (depth != 0);
            useMoveTime = (movetime != 0);
            
            time = tim;
            increment = inc;
            nodeLimit = nodes;
            depthLimit = depth;
            moveTime = movetime;
            movesToGo = mtg;
            
            softCap = time_allocated_soft();
            hardCap = time_allocated_hard();
        }

        bool keep_searching_soft(int64_t tim, uint64_t nodes, int32_t depth) const {
            if (useTime && tim >= softCap) {
                return false;
            }
            if (use_nodes && nodes >= nodeLimit) {
                return false;
            }
            if (use_depth && depth > depthLimit) {
                return false;
            }
            if (useMoveTime && tim >= moveTime) {
                return false;
            }
            return true;
        }
        bool keep_searching_hard(int64_t tim, uint64_t nodes) const {
            if (useTime && tim >= hardCap) {
                return false;
            }
            if (use_nodes && nodes >= nodeLimit) {
                return false;
            }
            if (useMoveTime && tim >= moveTime) {
                return false;
            }
            return true;
        }
};