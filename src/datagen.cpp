#include "globals.h"
#include "datagen.h"
#include "search.h"


int main() {
    std::cout << "Generate data? Y/N\n";
    std::string response = "";
    std::cin >> response;
    if(response == "y" || response == "Y") {
        generateData();
    }
    return 0;
}

// manages the threads
void generateData() {
    for(int i = 0; i < 10; i++) {
        threadFunction();
        std::cout << "finished game " << i << '\n';
    }
}

// run on each thread
void threadFunction() {
    std::vector<std::string> fenVector;
    double result = runGame(fenVector);
    if(result == 2) std::cout << "Error! invalid game result\n";
    dumpToArray(result, fenVector);
}

// idk what to use here lol
constexpr uint8_t threadCount = 5;

// manages the games
double runGame(std::vector<std::string>& fenVector) {
    int score = 0;
    Board board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    for(int i = 0; i <= 100; i++) {
        if(i < 8) {
            // make a random move
            std::random_device rd;
            std::mt19937_64 gen(rd());

            // get moves
            std::array<Move, 256> moves;
            const int totalMoves = board.getMoves(moves);

            // distribution
            std::uniform_int_distribution distribution{0, totalMoves - 1};

            // legality check
            int numTestedMoves = 0;
            while(true) {
                const int index = distribution(gen);
                if(board.makeMove(moves[index])) {
                    break;
                }
                numTestedMoves++;
                // position has no legal moves, checkmate? stalemate? restart.
                if(numTestedMoves > 100) {
                    board = Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
                    i = 0;
                }
            }
            // move has been made now, cool
            std::cout << board.getFenString() << std::endl;
        } else {
            // get move from engine normally
            std::cout << "sending board with position " << board.getFenString() << std::endl;
            std::pair<Move, int> move = dataGenSearch(board, 8);
            score = move.second;
            std::cout << "score is now " << score << std::endl;
            if(board.makeMove(move.first)) {
                if(abs(move.second) < abs(mateScore + 256)) {
                    // non-mate, add fen string to vector
                    fenVector.push_back(board.getFenString() + " " + std::to_string(move.second));
                } else {
                    // checkmate found, no more use for this
                    break;
                }
            } else {
                std::cout << "Engine made an illegal move\n";
            }
            std::cout << board.getFenString() << std::endl;
        }
    }

    // return 1 if white won, 0 if black won, and 0.5 if draw, this will be useful later
    if(score > 1) {
        return 1;
    } else if(score < -1) {
        return 0;
    } else {
        return 0.5;
    }
    // error
    return 2;
}

void dumpToArray(double result, std::vector<std::string>& fenVector) {
    for(std::string fen : fenVector) {
        std::cout << "adding " << fen << " " << result << " to file\n";
        // add to file

        // add [result]\n

    }
}