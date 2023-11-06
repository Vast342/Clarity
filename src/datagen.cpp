#include "globals.h"
#include "datagen.h"
#include "search.h"

std::ofstream output;

int main() {
    std::cout << "Where to save the result?\n";
    std::string directory = "";
    std::cin >> directory;
    output.open(directory);
    std::cout << "How many games?\n";
    int numGames = 0;
    std::cin >> numGames;
    std::cout << "Generate data? Y/N\n";
    std::string response = "";
    std::cin >> response;
    if(response == "y" || response == "Y") {
        generateData(numGames);
    }
    output.close();
    return 0;
}

// manages the threads
void generateData(int numGames) {
    for(int i = 0; i < numGames; i++) {
        threadFunction();
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
            const int legalMoves = board.getMoves(moves);

            /*std::array<Move, 256> moves;
            int legalMoves = 0;
            // legality check
            for(Move move : PLmoves) {
                if(board.makeMove(move)) {
                    moves[legalMoves] = move;
                    legalMoves++;
                    board.undoMove();
                }
            }
            // checkmate or stalemate? doesn't matter, restart
            if(legalMoves == 0) {
                board = Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
                i = 0;
                continue;
            }*/
            // distribution
            std::uniform_int_distribution distribution{0, legalMoves - 1};

            int numTestedMoves = 0;
            while(true) {
                const int index = distribution(gen);
                if(board.makeMove(moves[index])) {
                    break;
                }
                numTestedMoves++;
            }
            // move has been made now, cool
            //std::cout << board.getFenString() << std::endl;
        } else {
            // get move from engine normally
            //std::cout << "sending board with position " << board.getFenString() << std::endl;
            const auto move = dataGenSearch(board,5000);
            const uint64_t capturable = board.getOccupiedBitboard();
            score = move.second;
            if(((1ULL << move.first.getEndSquare()) & capturable) != 0 || move.first.getFlag() == EnPassant) {
                if(abs(move.second) < abs(mateScore + 256)) {
                    // non-mate, add fen string to vector
                    fenVector.push_back(board.getFenString() + " | " + std::to_string(board.getColorToMove() == 1 ? move.second : -move.second));
                } else {
                    // checkmate found, no more use for this
                    break;
                }
            }
            //std::cout << "score is now " << score << std::endl;
            if(!board.makeMove(move.first)) {
                std::cout << "Engine made an illegal move\n";
            }
            //std::cout << board.getFenString() << std::endl;
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

int games = 0;
int outputFrequency = 100;
void dumpToArray(double result, std::vector<std::string>& fenVector) {
    games++;
    std::cout << "Dumping result of game " << games << std::endl;
    for(const std::string &fen : fenVector) {
        // add to file and append result \n
        output << fen << " | " << result << '\n';
        // i'll figure it out lol
    }
}