#include "globals.h"
#include "see.h"

constexpr std::array<size_t, 65> OFFSETS = {
    0, 
    23, 
    47, 
    72, 
    97, 
    122, 
    147, 
    171, 
    194, 
    218, 
    245, 
    274, 
    303, 
    332,
    361,
    388,
    412,
    437,
    466,
    499,
    532,
    565,
    598,
    627,
    652,
    677,
    706,
    739,
    774,
    809,
    842,
    871,
    896,
    921,
    950,
    983,
    1018,
    1053,
    1086,
    1115,
    1140,
    1165,
    1194,
    1227,
    1260,
    1293,
    1326,
    1355,
    1380,
    1404,
    1431,
    1460,
    1489,
    1518,
    1547,
    1574,
    1598,
    1621,
    1645,
    1670,
    1695,
    1720,
    1745,
    1769,
    1792,
};

constexpr std::array<uint64_t, 64> ALL_DESTINATIONS = {
    9313761861428512766ULL, 
    180779649147539453ULL, 
    289501704257216507ULL, 
    578721933554499319ULL, 
    1157442771892337903ULL,
    2314886639001336031ULL,
    4630054752962539711ULL,
    9332167099946164351ULL,
    4693051017167109639ULL,
    9386102034350996751ULL,
    325459995009219359ULL,
    578862400275412798ULL,
    1157444425085677436ULL,
    2315169225636372472ULL,
    4702396040998862832ULL,
    9404792077685915616ULL,
    2382695603659212551ULL,
    4765391211613458191ULL,
    9530782427521949471ULL,
    614821880829263422ULL,
    1157867642580597884ULL,
    2387511404205701368ULL,
    4775021704588030192ULL,
    9550042305352687840ULL,
    1227520104343209737ULL,
    2455041308214890258ULL,
    4910083715958251300ULL,
    9820448902615023177ULL,
    1266211321280232594ULL,
    2460365044244346916ULL,
    4920447509705322568ULL,
    9840612440627273872ULL,
    650497458799315217ULL,
    1301276396887151138ULL,
    2602834273062822980ULL,
    5277725044946913672ULL,
    10555448994677166609ULL,
    2664152820428055586ULL,
    5255965472313067588ULL,
    10439590776083091592ULL,
    506652789238731041ULL,
    1085364276338762306ULL,
    2242787250538824836ULL,
    4485294125612632072ULL,
    8970307875760115984ULL,
    17940335376038371873ULL,
    17361587605057586242ULL,
    16204092063096014980ULL,
    575905529148285249ULL,
    1152093637079876226ULL,
    2304469852943057924ULL,
    4537163586841610248ULL,
    9002551054605291536ULL,
    17933325985854398752ULL,
    17347849204449690177ULL,
    16176895641640273026ULL,
    18304606945994162561ULL,
    18234809986805039618ULL,
    18095216068426728452ULL,
    17815745661460023304ULL,
    17256804838970232848ULL,
    16138922098757279776ULL,
    13830818648828297536ULL,
    9214611748970332801ULL,
};

//constexpr size_t MAX_MOVES = 96;
constexpr size_t PROMOS = 4 * 22;
//constexpr size_t NUM_MOVES = 2 * (OFFSETS[64] + PROMOS);

inline size_t map_move_to_index(const Board& pos, const Move& mov) {
    int ctm = pos.getColorToMove();
    int king = pos.getBoardState().kingSquares[ctm];
    int flag = mov.getFlag();
    int hm = (king % 8 > 3) ? 7 : 0;
    size_t good_see = (OFFSETS[64] + PROMOS) * see(pos, mov, -108);

    size_t idx;
    // promotion
    if(flag > DoublePawnPush) {
        int ffile = (mov.getStartSquare() ^ hm) % 8;
        int tfile = (mov.getEndSquare() ^ hm) % 8;
        int promo_id = 2 * ffile + tfile;

        idx = OFFSETS[64] + 22 * (flag - promotions[0]) + promo_id;
    } else {
        int flip = (ctm == 0) ? 56 : 0;
        size_t from = mov.getStartSquare() ^ flip ^ hm;
        size_t dest = mov.getEndSquare() ^ flip ^ hm;
        uint64_t below = ALL_DESTINATIONS[from] & ((1ULL << dest) - 1);
        idx = OFFSETS[from] + __builtin_popcountll(below);
    }

    return good_see + idx;
}