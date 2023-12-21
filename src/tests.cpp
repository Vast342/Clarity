#include "testessentials.h"
#include "testsuites.h"

// runs a single perft test
int perft(Board board, int depth) {
    // tests to make sure incremental updates were working
    //std::cout << "incremental says: "<< std::to_string(board.getEvaluation()) << ", full regen says " << std::to_string(board.fullEvalRegen()) << " at position " << board.getFenString() << '\n';
    //assert(board.getEvaluation() == board.fullEvalRegen());
    //uint64_t regenerated = board.fullZobristRegen();
    //std::cout << "incremental says: "<< std::to_string(board.zobristHash) << ", full regen says " << std::to_string(regenerated) << " at position " << board.getFenString() << '\n';
    //assert(board.zobristHash == regenerated);
    if(depth == 0) return 1;
    std::array<Move, 256> moves;
    int numMoves = board.getMoves(moves);
    int result = 0;
    for(int i = 0; i < numMoves; i++) {
        if(board.makeMove(moves[i])) {
            result += perft(board, depth-1);
            board.undoMove();
        }
    }
    return result;
}

// runs an entire suite of perft tests
void runPerftSuite(int number) {
    if(number == 0) {
        int i = 0;
        int passed = 0;
        int failed = 0;
        int skipped = 0;
        double total = 0;
        clock_t start = clock();
        for(PerftTest test : etherealSuite) {
            i++;
            if(test.expectedOutput <= 5000000) {
                int result = perft(Board(test.fen), test.depth);
                total += result;
                if(result == test.expectedOutput) {
                    std::cout << "Test " << std::to_string(i) << " Passed\n";
                    passed++;
                } else {
                    std::cout << "Test " << std::to_string(i) << " Failed, outputted " << std::to_string(result) << " With fen string "  << test.fen << " and depth " << std::to_string(test.depth) << '\n';
                    failed++;
                }
            } else {
                std::cout << "Test " << std::to_string(i) << " Skipped\n";
                skipped++;
            }
        }
        clock_t end = clock();
        std::cout << "Passed " << std::to_string(passed) << ", Failed " << std::to_string(failed) << ", Skipped " << std::to_string(skipped) << '\n';
        std::cout << "Tests took: " << std::to_string((end-start)/static_cast<double>(1000)) << '\n';
        std::cout << "Total nodes: " << std::to_string(static_cast<int>(total)) << '\n';
        std::cout << "NPS: " << std::to_string(total / ((end-start)/static_cast<double>(1000))) << '\n';
    }
}

// runs perft split by what the first move that is done is
void splitPerft(Board board, int depth) {
    std::array<Move, 256> moves;
    int numMoves = board.getMoves(moves);
    int total = 0;
    clock_t start = clock();
    for(int i = 0; i < numMoves; i++) {
        if(board.makeMove(moves[i])) {
            int result = perft(board, depth - 1);
            board.undoMove();
            total += result;
            std::cout << toLongAlgebraic(moves[i]) << ": " << std::to_string(result) << '\n';
        }
    }
    clock_t end = clock();
    std::cout << "Total: " << std::to_string(total) << '\n';
    std::cout << "Time: " << std::to_string((end-start)/static_cast<double>(1000)) << '\n';
    std::cout << "NPS: " << std::to_string(total / ((end-start)/static_cast<double>(1000))) << '\n';
}

// runs an individual perft test, and outputs the results
void individualPerft(Board board, int depth) {
    clock_t start = clock();
    int result = perft(board, depth);
    clock_t end = clock();
    std::cout << "Result: " << std::to_string(result) << '\n';
    std::cout << "Time: " << std::to_string((end-start)/static_cast<double>(1000)) << '\n';
    std::cout << "NPS: " << std::to_string(result / ((end-start)/static_cast<double>(1000))) << '\n';
}
