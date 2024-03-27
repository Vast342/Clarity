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
    }
    void pushAdd(int square, int piece) {
        adds[numAdds] = NetworkUpdate(square, piece);
        numAdds++;
    }
    void pushSub(int square, int piece) {
        subs[numSubs] = NetworkUpdate(square, piece);
        numSubs++;
    }
    std::array<NetworkUpdate, 2> adds;
    int numAdds;
    std::array<NetworkUpdate, 2> subs;
    int numSubs;
};