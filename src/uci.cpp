#include "globals.h"
#include "testessentials.h"
#include "search.h"
#include "tt.h"
#include "bench.h"

/*
    The entirety of my implementation of UCI, read the standard for that if you want more information
    There are things not supported here though, such as go nodes, and quite a few options
*/

int hardBoundDivisor = 2;
int softBoundFractionNumerator = 3;
int softBoundFractionDenominator = 4;
double softBoundMultiplier = 0.6;
int defaultMovesToGo = 20;

Board board("8/8/8/8/8/8/8/8 w - - 0 1");
Engine engine;

int rootColorToMove;

// runs a fixed depth search on a fixed set of positions, to see if a test changes how the engine behaves
void runBench(int depth) {
    engine.resetEngine();
    uint64_t total = 0;
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    for(std::string fen : benchFens) {
        Board benchBoard(fen);
        int j = engine.benchSearch(benchBoard, depth);
        total += j;
    }
    const auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - begin).count();
    std::cout << "nodes " << total << " time " << elapsedTime << '\n';
}

// sets options, though currently just the hash size
void setOption(const std::vector<std::string>& bits) {
    std::string name = bits[2];
    if(name == "Hash") {
        int newSizeMB = std::stoi(bits[4]);
        int newSizeB = newSizeMB * 1024 * 1024;
        // this should be 16 bytes
        int entrySizeB = sizeof(Transposition);
        assert(entrySizeB == 16); 
        int newSizeEntries = newSizeB / entrySizeB;
        //std::cout << log2(newSizeEntries);
        engine.resizeTT(newSizeEntries);
    } else if(name == "lmrBase") {
        lmrBase = std::stod(bits[4]) / 100;
        calculateReductions();
    } else if(name == "lmrMultiplier") {
        lmrMultiplier = std::stod(bits[4]) / 100;
        calculateReductions();
    } else if(name == "hardBoundDivisor") {
        hardBoundDivisor = std::stoi(bits[4]);
    } else if(name == "softBoundFractionNumerator") {
        softBoundFractionNumerator = std::stoi(bits[4]);
    } else if(name == "softBoundFractionDenominator") {
        softBoundFractionDenominator = std::stoi(bits[4]);
    } else if(name == "softBoundMultiplier") {
        softBoundMultiplier = std::stod(bits[4]) / 100;
    } else if(name == "defaultMovesToGo") {
        defaultMovesToGo = std::stoi(bits[4]);
    } else if(name == "ASP_BaseDelta") {
        ASP_BaseDelta = std::stoi(bits[4]);
    } else if(name == "ASP_DeltaMultiplier") {
        ASP_DeltaMultiplier = std::stod(bits[4]) / 10;
    } else if(name == "ASP_DepthCondition") {
        ASP_DepthCondition = std::stoi(bits[4]);
    } else if(name == "MVV_VictimScoreMultiplier") {
        MVV_VictimScoreMultiplier = std::stoi(bits[4]);
    } else if(name == "FirstKillerScore") {
        FirstKillerScore = std::stoi(bits[4]) * 1000;
    } else if(name == "SecondKillerScore") {
        SecondKillerScore = std::stoi(bits[4]) * 1000;
    } else if(name == "RFP_DepthCondition") {
        RFP_DepthCondition = std::stoi(bits[4]);
    } else if(name == "RFP_Multiplier") {
        RFP_Multiplier = std::stoi(bits[4]);
    } else if(name == "IIR_DepthCondition") {
        IIR_DepthCondition = std::stoi(bits[4]);
    } else if(name == "FP_DepthCondition") {
        FP_DepthCondition = std::stoi(bits[4]);
    } else if(name == "FP_Base") {
        FP_Base = std::stoi(bits[4]);
    } else if(name == "FP_Multiplier") {
        FP_Multiplier = std::stoi(bits[4]);
    } else if(name == "LMP_DepthCondition") {
        LMP_DepthCondition = std::stoi(bits[4]);
    } else if(name == "LMP_Base") {
        LMP_Base = std::stoi(bits[4]);
    } else if(name == "SPR_DepthCondition") {
        SPR_DepthCondition = std::stoi(bits[4]);
    } else if(name == "SPR_CaptureThreshold") {
        SPR_CaptureThreshold = -std::stoi(bits[4]);
    } else if(name == "SPR_QuietThreshold") {
        SPR_QuietThreshold = -std::stoi(bits[4]);
    } else if(name == "NMP_Adder") {
        NMP_Adder = std::stoi(bits[4]);
    } else if(name == "NMP_Divisor") {
        NMP_Divisor = std::stoi(bits[4]);
    } else if(name == "NMP_Subtractor") {
        NMP_Subtractor = std::stoi(bits[4]);
    } else if(name == "NMP_DepthCondition") {
        NMP_DepthCondition = std::stoi(bits[4]);
    }
}

// loads a position, either startpos, kiwipete(not part of uci but IO just felt like I should), or from a fen string
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

// has the engine identify itself when the GUI says uci
void identify() {
    std::cout << "id name Clarity V3.0.0\n";
    std::cout << "id author Vast\n";
    std::cout << "option name Hash type spin default 64 min 1 max 2048\n";
    std::cout << "option name lmrBase type spin default 77 min 0 max 200\n";
    std::cout << "option name lmrMultiplier type spin default 42 min 0 max 70\n";
    std::cout << "option name hardBoundDivisor type spin default 2 min 0 max 10\n";
    std::cout << "option name softBoundNumerator type spin default 3 min 0 max 10\n";
    std::cout << "option name softBoundDenominator type spin default 4 min 0 max 10\n";
    std::cout << "option name softBoundMultiplier type spin default 60 min 0 max 100\n";
    std::cout << "option name defaultMovesToGo type spin default 20 min 0 max 40\n";
    std::cout << "option name ASP_BaseDelta type spin default 25 min 0 max 100\n";
    std::cout << "option name ASP_DeltaMultiplier type spin default 15 min 0 max 40\n";
    std::cout << "option name ASP_DepthCondition type spin default 3 min 0 max 10\n";
    std::cout << "option name MVV_VictimScoreMultiplier type spin default 500 min 0 max 1000\n";
    std::cout << "option name FirstKillerScore type spin default 54 min 0 max 100\n";
    std::cout << "option name SecondKillerScore type spin default 53 min 0 max 100\n";
    std::cout << "option name RFP_DepthCondition type spin default 9 min 0 max 20\n";
    std::cout << "option name RFP_Multiplier type spin default 80 min 0 max 120\n";
    std::cout << "option name IIR_DepthCondition type spin default 3 min 0 max 20\n";
    std::cout << "option name FP_DepthCondition type spin default 8 min 0 max 20\n";
    std::cout << "option name FP_Base type spin default 250 min 0 max 400\n";
    std::cout << "option name FP_Multiplier type spin default 60 min 0 max 100\n";
    std::cout << "option name LMP_DepthCondition type spin default 7 min 0 max 20\n";
    std::cout << "option name LMP_Base type spin default 5 min 0 max 20\n";
    std::cout << "option name SPR_DepthCondition type spin default 8 min 0 max 20\n";
    std::cout << "option name SPR_CaptureThreshold type spin default 90 min 0 max 200\n";
    std::cout << "option name SPR_QuietThreshold type spin default 50 min 0 max 200\n";
    std::cout << "option name NMP_Adder type spin default 1 min 0 max 15\n";
    std::cout << "option name NMP_Divisor type spin default 3 min 0 max 10\n";
    std::cout << "option name NMP_Subtractor type spin default 2 min 0 max 10\n";
    std::cout << "option name NMP_DepthCondition type spin default 2 min 0 max 20\n";
    std::cout << "uciok\n";
}

// tells the engine to search, with support for a few different types
void go(std::vector<std::string> bits) {
    int time = 0;
    int depth = 0;
    int inc = 0;
    int movestogo = defaultMovesToGo;
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
    }
    Move bestMove;
    // go depth x
    if(depth != 0) {
        bestMove = engine.fixedDepthSearch(board, depth, true);
    } else {
        // go wtime x btime x
        // the formulas here are former formulas from Stormphrax
        const int softBound = softBoundMultiplier * (time / movestogo + inc * softBoundFractionNumerator / softBoundFractionDenominator);
        const int hardBound = time / hardBoundDivisor;
        bestMove = engine.think(board, softBound, hardBound, true);
    }
    std::cout << "bestmove " << toLongAlgebraic(bestMove) << '\n';
    board.makeMove(bestMove);
}

// resets everything
void newGame() {
    engine.resetEngine();
    board = Board("8/8/8/8/8/8/8/8 w - - 0 1");
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
        runBench(std::stoi(bits[1]));
    } else if(bits[0] == "makemove") {
        board.makeMove(Move(bits[1], board));
    } else if(bits[0] == "undomove") {
        board.undoMove();
    } else if(bits[0] == "nullmove") {
        board.changeColor();
    } else if(bits[0] == "undonullmove") {
        board.undoChangeColor();
    } else if(bits[0] == "isrepeated") {
        std::cout << board.isRepeatedPosition() << '\n';
    } else {
        std::cout << "invalid or unsupported command\n";
    }
}

int main() {
    initialize();
    std::cout << std::boolalpha;
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
