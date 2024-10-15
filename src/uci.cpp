/*
    Clarity
    Copyright (C) 2024 Joseph Pasfield

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
#include "tt.h"
#include "bench.h"
#include "external/fathom/tbprobe.h"
#include "uci.h"

bool useSyzygy = false;

/*
    The entirety of my implementation of UCI, read the standard for that if you want more information
    There are things not supported here though, such as go infinite, and quite a few options
*/

int defaultMovesToGo = 20;

Board board("8/8/8/8/8/8/8/8 w - - 0 1");
TranspositionTable TT;
std::vector<Engine> engines;
std::vector<std::jthread> threads;
int threadCount = 1;

int rootColorToMove;

// resets everything
void newGame() {
    engines.clear();
    engines.reserve(threadCount);
    for(int i = 0; i < threadCount; i++) {
        engines.emplace_back(&TT);
        engines[i].resetEngine();
    }
    TT.clearTable();
    board = Board("8/8/8/8/8/8/8/8 w - - 0 1");
}

// runs a fixed depth search on a fixed set of positions, to see if a test changes how the engine behaves
void runBench(int depth) {
    engines[0].resetEngine();
    uint64_t total = 0;
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    for(std::string fen : benchFens) {
        Board benchBoard(fen);
        int j = engines[0].benchSearch(benchBoard, depth);
        total += j;
    }
    const auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - begin).count();
    std::cout << total << " nodes " << std::to_string(int(total / (double(elapsedTime) / 1000))) << " nps" << '\n';
}

// sets options, though currently just the hash size
void setOption(const std::vector<std::string>& bits) {
    std::string name = bits[2];
    if(name == "Hash") {
        uint64_t newSizeMB = std::stoi(bits[4]);
        TT.resize(newSizeMB);
    } else if(name == "Threads") {
        //clock_t start = clock();
        threadCount = std::stoi(bits[4]);
        newGame();
        //clock_t end = clock();
        //std::cout << "operation took " << std::to_string((end-start)/static_cast<double>(1000)) << std::endl;
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
    rootColorToMove = board.getColorToMove();
}

// has the engine identify itself when the GUI says uci
void identify() {
    std::cout << "id name Clarity V7.2.0" << std::endl;
    std::cout << "id author Vast" << std::endl;
    std::cout << "option name Hash type spin default 64 min 1 max 524288" << std::endl;
    std::cout << "option name Threads type spin default 1 min 1 max 16384" << std::endl;
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
    int time = 0;
    int depth = 0;
    int inc = 0;
    int movestogo = defaultMovesToGo;
    int nodes = 0;
    bool infinite = false;
    for(int i = 1; i < std::ssize(bits); i+=2) {
        if(bits[i] == "wtime" && board.getColorToMove() == 1) {
            time = std::stoi(bits[i+1]);
        }
        if(bits[i] == "btime" && board.getColorToMove() == 0) {
            time = std::stoi(bits[i+1]);
        }
        if(bits[i] == "depth") {
            depth = std::stoi(bits[i+1]);
        }
        if(bits[i] == "movestogo") {
            movestogo = std::stoi(bits[i+1]);
        }
        if(bits[i] == "winc" && board.getColorToMove() == 1) {
            inc = std::stoi(bits[i+1]);
        }
        if(bits[i] == "binc" && board.getColorToMove() == 0) {
            inc = std::stoi(bits[i+1]);
        }
        if(bits[i] == "nodes") {
            nodes = std::stoi(bits[i+1]);
        }
        if(bits[i] == "infinite") {
            infinite = true;
            i--;
        }
    }
    // go depth x
    if(depth != 0) {
        for(int i = 0; i < threadCount; i++) {
            threads.emplace_back([depth, i]{
                engines[i].fixedDepthSearch(board, depth, i == 0);
            });
        }
        //bestMove = engines.fixedDepthSearch(board, depth, true);
    } else if(nodes != 0) {
        for(int i = 0; i < threadCount; i++) {
            threads.emplace_back([nodes, i]{
                engines[i].fixedNodesSearch(board, nodes, i == 0);
            });
        }
    } else if(infinite) {
        for(int i = 0; i < threadCount; i++) {
            threads.emplace_back([i]{
                engines[i].fixedDepthSearch(board, 100, i == 0);
            });
        }
    } else {
        // go wtime x btime x
        // the formulas here are former formulas from Stormphrax
        const int softBound = tmsMultiplier.value * (time / movestogo + inc * tmsNumerator.value / tmsDenominator.value);
        const int hardBound = time / tmhDivisor.value;
        for(int i = 0; i < threadCount; i++) {
            threads.emplace_back([i, softBound, hardBound]{
                engines[i].think(board, softBound, hardBound, i == 0);
            });
        }
        //bestMove = engine.think(board, softBound, hardBound, true);
    }
}

uint64_t getTotalNodes() {
    uint64_t sum = 0;
    for(const auto &engine : engines) {
        sum += engine.nodes;
    }
    return sum;
}

void stopThePresses() {
    timesUp.store(true);
    for(int i = 0; i < threadCount; i++) {
        if(threads[i].joinable()) threads[i].join();
    }
    threads.clear();
}

void stopOtherThreads() {
    for(uint32_t i = 1; i < threads.size(); i++) {
        if(threads[i].joinable()) threads[i].join();
    }
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
        if(bits[1] == "ethereal") {
            runPerftSuite(0);
        }
    } else if(bits[0] == "incheck") {
        std::cout << std::to_string(board.isInCheck()) << '\n';
    } else if(bits[0] == "getfen") {
        std::cout << board.getFenString() << '\n';
    } else if(bits[0] == "perft") {
        individualPerft(board, std::stoi(bits[1]));
    } else if(bits[0] == "splitperft") {
        splitPerft(board, std::stoi(bits[1]));
    } else if(bits[0] == "evaluate") {
        std::cout << "evaluation " << board.getEvaluation() << '\n';
    } else if(bits[0] == "bench") {
        if(bits.size() == 1) {
            runBench(14);
        } else {
            runBench(std::stoi(bits[1]));
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
    } else if(bits[0] == "stop") {
        stopThePresses();
    } else if(bits[0] == "showthreats") {
        std::cout << board.getThreats() << std::endl; 
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
        runBench(14);
        return 0;
    }
    std::string command;
    while(true) {
        std::getline(std::cin, command, '\n');
        if(mainThreadDone && threads.size() != 0) {
            threads.clear();
        }
        if(command == "quit") {
            if(threads.size() != 0) {
                stopThePresses();
            }
            return 0;
        }
        interpretCommand(command);
    }
    return 0;
};
