/*
    Clarity
    Copyright (C) 2024 Joseph Pasfield

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
#include "globals.h"

enum UpdateType {
    NoUpdate, Add, Subtract
};

struct NetworkUpdate {
    NetworkUpdate(int s, int p) {
        square = s;
        piece = p;
    }
    NetworkUpdate() {
        square = 0;
        piece = 0;
    }
    int square;
    int piece;  
};

struct NetworkUpdates {
    NetworkUpdates() {
        numAdds = 0;
        numSubs = 0;
        valueBucketChange = false;
        policyBucketChange = false;
    }
    void pushAdd(int square, int piece) {
        adds[numAdds] = NetworkUpdate(square, piece);
        numAdds++;
    }
    void pushSub(int square, int piece) {
        subs[numSubs] = NetworkUpdate(square, piece);
        numSubs++;
    }
    void pushValueBucket(int square, int color) {
        valueBucketChange = true;
        valueBucketUpdate = NetworkUpdate(square, color);
    }
    void pushPolicyBucket(int square, int color) {
        policyBucketChange = true;
        policyBucketUpdate = NetworkUpdate(square, color);
    }
    std::array<NetworkUpdate, 2> adds;
    int numAdds;
    std::array<NetworkUpdate, 2> subs;
    int numSubs;
    bool valueBucketChange;
    NetworkUpdate valueBucketUpdate;
    bool policyBucketChange;
    NetworkUpdate policyBucketUpdate;
};
