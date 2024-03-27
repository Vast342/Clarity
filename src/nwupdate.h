#include "globals.h"

enum UpdateType {
    NoUpdate, Add, Subtract
};

struct NetworkUpdate {
    NetworkUpdate(int t, int s, int p) {
        type = t;
        square = s;
        piece = p;
    }
    NetworkUpdate() {
        type = NoUpdate;
        square = 0;
        piece = 0;
    }
    int type;
    int square;
    int piece;  
};

struct NetworkUpdates {
    NetworkUpdates() {
        numAdds = 0;
        numSubs = 0;
    }
    void addAdd(int square, int piece) {
        adds[numAdds] = NetworkUpdate(Add, square, piece);
        numAdds++;
    }
    void addSub(int square, int piece) {
        subs[numSubs] = NetworkUpdate(Subtract, square, piece);
        numSubs++;
    }
    std::array<NetworkUpdate, 3> adds;
    int numAdds;
    std::array<NetworkUpdate, 3> subs;
    int numSubs;
};