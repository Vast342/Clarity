/*
    Clarity
    Copyright (C) 2025 Joseph Pasfield

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See thfe
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#include "globals.h"
#include "testessentials.h"
#include "search.h"
#include "bench.h"
#include "external/fathom/tbprobe.h"
#include "tunables.h"
#include "limits.h"
#include "tt.h"

bool useSyzygy = false;

/*
    The entirety of my implementation of UCI, read the standard for that if you want more information
    There are things not supported here though, such as go infinite, and quite a few options
*/

int defaultMovesToGo = 20;

Board board("8/8/8/8/8/8/8/8 w - - 0 1");
int64_t moveOverhead = 10;
int benchDepth = 7;
TranspositionTable TT;
std::vector<Searcher> searchers;
std::vector<std::thread> threads;
int threadCount = 1;

// resets everything
void newGame() {
    searchers.clear();
    for(int i = 0; i < threadCount; i++) {
        searchers.emplace_back(&TT);
        searchers[i].newGame();
    }
    board = Board("8/8/8/8/8/8/8/8 w - - 0 1");
    TT.clearTable(threadCount);
}

uint64_t getTotalNodes() {
    uint64_t sum = 0;
    for(int i = 0; i < threadCount; i++) {
        sum += searchers[i].getNodes();
    }
    return sum;
}

// runs a fixed depth search on a fixed set of positions, to see if a test changes how the engine behaves
void runBench(int depth = benchDepth) {
    searchers[0].newGame();
    uint64_t total = 0;
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    auto limiters = Limiters();
    limiters.load_values(0, 0, 0, depth, 0, 0);
    for(std::string fen : benchFens) {
        Board benchBoard(fen);
        searchers[0].think(benchBoard, limiters, false);
        total += searchers[0].getNodes();
    }
    const auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - begin).count();
    std::cout << total << " nodes " << std::to_string(int(total / (double(elapsedTime) / 1000))) << " nps" << '\n';
}

// sets options, need to add `Hash` and `Threads` back when i do those things
void setOption(const std::vector<std::string>& bits) {
    const std::string name = bits[2];
    if(name == "Hash") {
        const uint64_t newSizeMB = std::stoull(bits[4]);
        TT.resize(newSizeMB, threadCount);
    } else if(name == "Threads") {
        threadCount = std::stoi(bits[4]);
        searchers.clear();
        for(int i = 0; i < threadCount; i++) {
            searchers.emplace_back(&TT);
        }
        newGame();
    } else if(name == "MoveOverhead") {
        moveOverhead = std::stoull(bits[4]);
    } else if(name == "SyzygyPath") {
        bool initSuccess = tb_init(bits[4].c_str());
        useSyzygy = initSuccess;
    } else {
        adjustTunable(name, std::stod(bits[4]));
    }
}

// loads a position, either startpos, kiwipete(not part of uci but IO just felt like I should), or from a fen string
void loadPosition(const std::vector<std::string>& bits) {
    if(bits[1] == "startpos") {
        board = Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        for(int i = 3; i < static_cast<int>(bits.size()); i++) {
            board.makeMove<false>(Move(bits[i], board));
        }
    } else if(bits[1] == "fen") {
        board = Board(bits[2] + " " + bits[3] + " " + bits[4] + " " + bits[5] + " " + bits[6] + " " + bits[7]);
        for(int i = 9; i < static_cast<int>(bits.size()); i++) {
            board.makeMove<false>(Move(bits[i], board));
        }
    } else if(bits[1] == "kiwipete") {
        board = Board("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
        for(int i = 3; i < static_cast<int>(bits.size()); i++) {
            board.makeMove<false>(Move(bits[i], board));
        }
    } else {
        std::cout << "invalid position command\n";
    }
}

// has the engine identify itself when the GUI says uci
void identify() {
    std::cout << "id name Clarity V7.2.0" << std::endl;
    std::cout << "id author Vast" << std::endl;
    std::cout << "option name Hash type spin default 64 min 1 max 524288" << std::endl;
    std::cout << "option name Threads type spin default 1 min 1 max 16384" << std::endl;
    std::cout << "option name MoveOverhead type spin default 10 min 1 max 100000" << std::endl;
    std::cout << "option name SyzygyPath type string default <empty>" << std::endl;
    //outputTunables();
    std::cout << "uciok" << std::endl;
}

// tells the engine to search, with support for a few different types
void go(std::vector<std::string> bits) {
    if(useSyzygy) {
        if(__builtin_popcountll(board.getOccupiedBitboard()) <= 7) {
            // probe endgame tt at root
            unsigned probeResult = tb_probe_root(board.getColoredBitboard(1), 
                                                board.getColoredBitboard(0),
                                                board.getPieceBitboard(King),
                                                board.getPieceBitboard(Queen),
                                                board.getPieceBitboard(Rook),
                                                board.getPieceBitboard(Bishop),
                                                board.getPieceBitboard(Knight),
                                                board.getPieceBitboard(Pawn),
                                                board.getFiftyMoveCount() * 2,
                                                0,
                                                board.getEnPassantIndex() == 64 ? 0 : board.getEnPassantIndex(),
                                                board.getColorToMove(),
                                                NULL);
            if(probeResult != TB_RESULT_FAILED) {
                int start = TB_GET_FROM(probeResult);
                int end = TB_GET_TO(probeResult);
                int promotion = TB_GET_PROMOTES(probeResult);
                int ep = TB_GET_EP(probeResult);
                Move tbBest = Move(start, end, promotion, ep, board);
                std::cout << "bestmove " << toLongAlgebraic(tbBest) << std::endl;
                return;
            }
        }
    }
    uint64_t time = 0;
    int depth = 0;
    uint64_t inc = 0;
    int movestogo = defaultMovesToGo;
    uint64_t nodes = 0;
    uint64_t movetime = 0;
    for(int i = 1; i < std::ssize(bits); i+=2) {
        if(bits[i] == "wtime" && board.getColorToMove() == 1) {
            time = std::max(std::stoll(bits[i+1]), 0ll);
        }
        if(bits[i] == "btime" && board.getColorToMove() == 0) {
            time = std::max(std::stoll(bits[i+1]), 0ll);
        }
        if(bits[i] == "depth") {
            depth = std::stoll(bits[i+1]);
        }
        if(bits[i] == "movestogo") {
            movestogo = std::stoll(bits[i+1]);
        }
        if(bits[i] == "winc" && board.getColorToMove() == 1) {
            inc = std::max(std::stoll(bits[i+1]), 0ll);
        }
        if(bits[i] == "binc" && board.getColorToMove() == 0) {
            inc = std::max(std::stoll(bits[i+1]), 0ll);
        }
        if(bits[i] == "nodes") {
            nodes = std::stoll(bits[i+1]);
        }
        if(bits[i] == "movetime") {
            movetime = std::stoll(bits[i+1]);
        }
    }
    time -= moveOverhead;
    auto limiters = Limiters();
    limiters.load_values(time, inc, nodes, depth, movetime, movestogo);
    for(int i = 0; i < threadCount; i++) {
        threads.emplace_back([i, limiters]() {
            searchers[i].think(board, limiters, i == 0);
        });
    }
    //bestMove = engine.think(board, softBound, hardBound, true);
}

void stopThePresses() {
    endSearch.store(true);
    for (auto& t : threads) {
        if (t.joinable()) t.join();
    }
    threads.clear();
}

// interprets the command
void interpretCommand(std::string command) {
    std::vector<std::string> bits = split(command, ' ');

    if(bits.empty()) {
        return;
    } else if(bits[0] == "printstate") {
        board.toString();
    } else if(bits[0] == "position") {
        loadPosition(bits);
    } else if(bits[0] == "isready") {
        std::cout << "readyok\n";
    } else if(bits[0] == "uci") {
        identify();
    } else if(bits[0] == "go") {
        go(bits);
    } else if(bits[0] == "ucinewgame") {
        newGame();
    } else if(bits[0] == "setoption") {
        setOption(bits);
    } else if(bits[0] == "perftsuite") {
        // not technically necessary since ethereal is the only perft suite I have but I think it's best to have it in case I get any more.
        // its been 2 years i don't have any more lmao
        if(bits[1] == "ethereal") {
            runPerftSuite(0);
        }
    } else if(bits[0] == "incheck") {
        std::cout << std::to_string(board.isInCheck()) << '\n';
    } else if(bits[0] == "getfen") {
        std::cout << board.getFenString() << '\n';
    } else if(bits[0] == "perft") {
        individualPerft(board, std::stoull(bits[1]));
    } else if(bits[0] == "splitperft") {
        splitPerft(board, std::stoull(bits[1]));
    } else if(bits[0] == "evaluate") {
        std::cout << "evaluation " << board.getEvaluation() << '\n';
    } else if(bits[0] == "bench") {
        if(bits.size() == 1) {
            runBench();
        } else {
            runBench(std::stoull(bits[1]));
        }
    } else if(bits[0] == "makemove") {
        board.makeMove<true>(Move(bits[1], board));
    } else if(bits[0] == "undomove") {
        board.undoMove<true>();
    } else if(bits[0] == "nullmove") {
        board.changeColor();
    } else if(bits[0] == "undonullmove") {
        board.undoChangeColor();
    } else if(bits[0] == "isrepeated") {
        std::cout << board.isRepeatedPosition() << '\n';
    } else if(bits[0] == "tunablejson") {
        outputTunableJSON();
    } else if(bits[0] == "tunableob") {
        outputTunableOB(); 
    } else if(bits[0] == "showthreats") {
        std::cout << board.getThreats() << std::endl; 
    } else if (bits[0] == "stop") {
        stopThePresses();
    } else if(bits[0] == "calcthreats") {
        std::cout << board.calculateThreats() << std::endl;
    } else {
        std::cout << "invalid or unsupported command\n";
    }
}

int main(int argc, char* argv[]) {
    initialize();
    newGame();
    std::cout << std::boolalpha;
    if(argc > 1 && std::string(argv[1]) == "bench") {
        runBench();
        return 0;
    }
    std::string command;
    while(true) {
        std::getline(std::cin, command, '\n');
        if(command == "quit") {
            stopThePresses();
            return 0;
        }
        interpretCommand(command);
    }
    return 0;
};
