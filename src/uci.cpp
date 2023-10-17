#include "globals.h"
#include "testessentials.h"
#include "search.h"
#include "tuner.h"
#include "tt.h"
#include "bench.h"

Board board("8/8/8/8/8/8/8/8 w - - 0 1");

int rootColorToMove;

void runBench(int depth) {
    uint64_t total = 0;
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    for(std::string fen : fens) {
        Board benchBoard(fen);
        int j = benchSearch(benchBoard, depth);
        total += j;
    }
    const auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - begin).count();
    std::cout << "searched 50 positions to a depth of " << depth << ", total nodes " << total << '\n';
    std::cout << "bench took " << elapsedTime << "ms\n";
}

void setOption(const std::vector<std::string>& bits) {
    if(bits[2] == "Hash") {
        int newSizeMB = std::stoi(bits[4]);
        int newSizeB = newSizeMB * 1024 * 1024;
        // this should be 16 bytes
        int entrySizeB = sizeof(Transposition);
        int newSizeEntries = newSizeB / entrySizeB;
        //std::cout << log2(newSizeEntries);
        resizeTT(newSizeEntries);
    }
}

void loadPosition(const std::vector<std::string>& bits) {
    if(bits[1] == "startpos") {
        board = Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        for(int i = 3; i < static_cast<int>(bits.size()); i++) {
            board.makeMove(Move(bits[i], board));
        }
    } else if(bits[1] == "fen") {
        board = Board(bits[2] + " " + bits[3] + " " + bits[4] + " " + bits[5] + " " + bits[6] + " " + bits[7]);
        for(int i = 9; i < static_cast<int>(bits.size()); i++) {
            board.makeMove(Move(bits[i], board));
        }
    } else if(bits[1] == "kiwipete") {
        board = Board("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
        for(int i = 3; i < static_cast<int>(bits.size()); i++) {
            board.makeMove(Move(bits[i], board));
        }
    } else {
        std::cout << "invalid position command\n";
    }
    rootColorToMove = board.getColorToMove();
}

/* leftover from debugging tuning
void sigmoidTest() {
    for(int i = -200; i <= 200; i++) {
        std::cout << std::to_string(i) << ", " << std::to_string(sigmoid(i)) << '\n';
    }
}*/

void identify() {
    std::cout << "id name Clarity V1.0.0\n";
    std::cout << "id author Vast\n";
    std::cout << "option name Hash type spin default 256 min 1 max 2048\n";
}

void go(std::vector<std::string> bits) {
    int time = 0;
    int depth = 0;
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
    }
    Move bestMove;
    if(depth != 0) {
        bestMove = fixedDepthSearch(board, depth);
    } else {
        bestMove = think(board, time);
    }
    std::cout << "bestmove " << toLongAlgebraic(bestMove) << '\n';
    board.makeMove(bestMove);
}

void newGame() {
    resetEngine();
    board = Board("8/8/8/8/8/8/8/8 w - - 0 1");
}

void interpretCommand(std::string command) {
    std::vector<std::string> bits = split(command, ' ');

    if(bits.empty()) {
        return;
    } else if(bits[0] == "printstate") {
        board.toString();
    } else if(bits[0] == "perftsuite") {
        // not technically necessary since ethereal is the only perft suite I have but I think it's best to have it in case I get any more.
        if(bits[1] == "ethereal") {
            runPerftSuite(0);
        }
    } else if(bits[0] == "incheck") {
        std::cout << std::to_string(board.isInCheck()) << '\n';
    } else if(bits[0] == "getfen") {
        std::cout << board.getFenString() << '\n';
    } else if(bits[0] == "position") {
        loadPosition(bits);
    } else if(bits[0] == "isready") {
        std::cout << "readyok\n";
    } else if(bits[0] == "uci") {
        identify();
        std::cout << "uciok\n";
    } else if(bits[0] == "go") {
        go(bits);
    } else if(bits[0] == "ucinewgame") {
        newGame();    
    } else if(bits[0] == "perft") {
        individualPerft(board, std::stoi(bits[1]));
    } else if(bits[0] == "splitperft") {
        splitPerft(board, std::stoi(bits[1]));
    } else if(bits[0] == "evaluate") {
        std::cout << "evaluation " << board.getEvaluation() << '\n';
    } else if(bits[0] == "setoption") {
        setOption(bits);
    } else if(bits[0] == "bench") {
        runBench(std::stoi(bits[1]));
    } else {
        std::cout << "invalid command\n";
    }
}

int main() {
    initialize();
    std::string command;
    while(true) {
        std::getline(std::cin, command, '\n');
        if(command == "quit") {
            return 0;
        }
        interpretCommand(command);
    }
    return 0;
};
