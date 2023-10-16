#include "search.h"
#include "psqt.h"
#include "tt.h"

constexpr int startDepth = 3;

constexpr int nmpMin = 2;

Move rootBestMove = Move();

int nodes = 0;

int timeToSearch = 0;

TranspositionTable TT;

std::array<std::array<std::array<int, 64>, 64>, 2> historyTable;

std::chrono::steady_clock::time_point begin;

constexpr int historyCap = 16384;

bool timesUp = false;

void clearHistory() {
    for(int h = 0; h < 2; h++) {
        for(int i = 0; i < 64; i++) {
            for(int j = 0; j < 64; j++) {
                historyTable[h][i][j] = 0;
            }
        }
    }
}

void resizeTT(int newSize) {
    TT.resize(newSize);
}

void resetEngine() {
    TT.clearTable();
    clearHistory();
}

void orderMoves(const Board& board, std::array<Move, 256> &moves, int numMoves, int ttMoveValue) {
    std::array<int, 256> values;
    const uint64_t occupied = board.getOccupiedBitboard();
    for(int i = 0; i < numMoves; i++) {
        if(moves[i].getValue() == ttMoveValue) {
            values[i] = 1000000000;
        } else if((occupied & (1ULL << moves[i].getEndSquare())) != 0) {
            // mvv lva (ciekce was here)
            const auto attacker = getType(board.pieceAtIndex(moves[i].getStartSquare()));
            const auto victim = getType(board.pieceAtIndex(moves[i].getEndSquare()));
            values[i] = 100 * eg_value[victim] - eg_value[attacker];
        } else {
            values[i] = historyTable[board.getColorToMove()][moves[i].getStartSquare()][moves[i].getEndSquare()];
        }
        values[i] = -values[i];
        // incremental sort was broken, I need to come back to it at some point
        //incrementalSort(values, moves, numMoves, i);
    }
    sortMoves(values, moves, numMoves);
}

int qSearch(Board &board, int alpha, int beta) {
    if(board.isRepeated) return 0;
    // time check every 4096 nodes
    if(nodes % 4096 == 0) {
        if(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - begin).count() > timeToSearch) {
            timesUp = true;
            return 0;
        }
    }

    // TT check
    Transposition entry = TT.getEntry(board.zobristHash);

    if(entry.zobristKey == board.zobristHash && (
        entry.flag == Exact // exact score
            || (entry.flag == BetaCutoff && entry.score >= beta) // lower bound, fail high
            || (entry.flag == FailLow && entry.score <= alpha) // upper bound, fail low
    )) {
        //std::cout << "did a TT cutoff with hash " << std::to_string(board.zobristHash) << " and score " << std::to_string(entry.score) << '\n';
        return entry.score;
    }

    // stand pat shenanigans
    int bestScore = board.getEvaluation();
    if(bestScore >= beta) return bestScore;
    if(alpha < bestScore) alpha = bestScore;
  
    // get the legal moves and sort them
    std::array<Move, 256> moves;
    const int totalMoves = board.getMovesQSearch(moves);
    orderMoves(board, moves, totalMoves, entry.bestMove.getValue());

    // values useful for writing to TT later
    Move bestMove;
    int flag = FailLow;
  
    // loop though all the moves
    for(int i = 0; i < totalMoves; i++) {
        if(board.makeMove(moves[i])) {
            nodes++;
            // searches from this node
            const int score = -qSearch(board, -beta, -alpha);
            board.undoMove();
            // time check
            if(timesUp) {
                return 0;
            }

            if(score > bestScore) {
                bestScore = score;
                bestMove = moves[i];

                // Improve alpha
                if(score > alpha) {
                    flag = Exact;
                    alpha = score;
                }

                // Fail-high
                if(score >= beta) {
                    flag = BetaCutoff;
                    break;
                }
            }
        }
    }

    // push to TT
    TT.setEntry(board.zobristHash, Transposition(board.zobristHash, bestMove, flag, bestScore, 0));

    return bestScore;  
}

void updateHistory(const int start, const int end, const int depth, const int colorToMove) {
    const int bonus = depth * depth;
    const int thingToAdd = bonus - historyTable[colorToMove][start][end] * std::abs(bonus) / historyCap;
    historyTable[colorToMove][start][end] += thingToAdd;
}

int negamax(Board &board, int depth, int alpha, int beta, int ply, bool nmpAllowed) {
    if(ply > 0 && board.isRepeated) return 0;
    // time check every 4096 nodes
    if(nodes % 4096 == 0) {
        if(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - begin).count() > timeToSearch) {
            timesUp = true;
            return 0 ;
        }
    }
    // activate q search if at the end of a branch
    if(depth <= 0) return qSearch(board, alpha, beta);
    const bool isPV = beta == alpha + 1;
    const bool inCheck = board.isInCheck();

    // TT check
    Transposition entry = TT.getEntry(board.zobristHash);

    if(ply > 0 && entry.zobristKey == board.zobristHash && entry.depth >= depth && (
            entry.flag == Exact // exact score
                || (entry.flag == BetaCutoff && entry.score >= beta) // lower bound, fail high
                || (entry.flag == FailLow && entry.score <= alpha) // upper bound, fail low
        )) {
        return entry.score;
    }

    // Reverse Futility Pruning
    // side note, A_randomnoob suggested returning just eval here, but it seems to be exactly equal after a 600 game test.
    const int eval = board.getEvaluation();
    if(eval - 80 * depth >= beta && !inCheck && depth < 9 && !isPV) return eval - 80 * depth;

    // nmp, "I could probably detect zugzwang here but ehhhhh" -Me, a few months ago
    // another thing suggested by A_randomnoob was having staticEval >= beta and !isPV as another condition, but the same story again.
    if(nmpAllowed && depth >= nmpMin && !inCheck) {
        board.changeColor();
        const int score = -negamax(board, depth - (depth+1)/3 - 1, 0-beta, 1-beta, ply + 1, false);
        board.undoChangeColor();
        if(score >= beta) {
            return score;
        }
    }

    // get the moves
    std::array<Move, 256> moves;
    const int totalMoves = board.getMoves(moves);
    orderMoves(board, moves, totalMoves, entry.bestMove.getValue());

    // values useful for writing to TT later
    int bestScore = -10000000;
    Move bestMove;
    int flag = FailLow;

    int extensions = 0;
    if(inCheck) {
        extensions++;
    }

    const int epIndex = board.getEnPassantIndex();
    const uint64_t capturable = board.getOccupiedBitboard() | (epIndex == 64 ? 0 : (1ULL << epIndex));
    // loop through the moves
    int legalMoves = 0;
    for(int i = 0; i < totalMoves; i++) {
        if(board.makeMove(moves[i])) {
            legalMoves++;
            nodes++;
            // passed pawn extension, also didn't gain.
            //int perMoveExtensions = 0;
            //int moveRank = moves[i].getEndSquare() / 8;
            //if(getType(board.pieceAtIndex(moves[i].getEndSquare())) == Pawn && (moveRank == 6 || moveRank == 1)) perMoveExtensions++;
            // Late Move Pruning (not working, needs moreit  testing)
            //if(depth < 4 && !isPV && bestScore > -10000000 + 256 && legalMoves > (3+depth*10)) break;
            bool isCapture = (capturable & (1ULL << moves[i].getEndSquare())) != 0;
            int score = 0;
            // Principal Variation Search
            if(legalMoves == 1) {
                // searches TT move, given first by the move ordering step.
                score = -negamax(board, depth + extensions - 1, -beta, -alpha, ply + 1, true);
            } else {
                // Late Move Reductions (LMR)
                int depthReduction = 0;
                if(extensions == 0 && depth > 1 && !isCapture) {
                    depthReduction = reductions[depth][legalMoves];
                }
                // this is more PVS stuff, searching with a reduced margin
                score = -negamax(board, depth + extensions - depthReduction - 1, -alpha - 1, -alpha, ply + 1, true);
                // and then if it fails high we search again with the better bounds
                if(score > alpha && (score < beta || depthReduction > 0)) {
                    score = -negamax(board, depth + extensions - 1, -beta, -alpha, ply + 1, true);
                }
            }
            board.undoMove();

            // backup time check
            if(timesUp) {
                return 0;
            }

            if(score > bestScore) {
                bestScore = score;
                bestMove = moves[i];
                if(ply == 0) rootBestMove = moves[i];

                // Improve alpha
                if(score > alpha) {
                    flag = Exact; 
                    alpha = score;
                }

                // Fail-high
                if(score >= beta) {
                    flag = BetaCutoff;
                    if(!isCapture) {
                        updateHistory(moves[i].getStartSquare(), moves[i].getEndSquare(), depth, board.getColorToMove());
                    }
                    break;
                }
            }
        }
    }

    // checkmate / stalemate detection
    if(legalMoves == 0) {
        if(inCheck) {
            return -10000000 + ply;
        }
        return 0;
    }

    // push to TT
    TT.setEntry(board.zobristHash, Transposition(board.zobristHash, bestMove, flag, bestScore, depth));

    return bestScore;
}

// yes cloning the board is intentional here.
std::string getPV(Board board) {
    std::string pv = "";
    if(TT.matchZobrist(board.zobristHash)) {
        Move bestMove = TT.getBestMove(board.zobristHash);
        if(bestMove.getValue() != 0 && board.makeMove(bestMove)) {
            std::string restOfPV = getPV(board);
            pv = toLongAlgebraic(bestMove) + " " + restOfPV;
        }
    }
    return pv;
}

Move think(Board board, int timeLeft) {
    clearHistory();
    nodes = 0;
    timeToSearch = timeLeft / 30;
    timesUp = false;

    begin = std::chrono::steady_clock::now();

    rootBestMove = Move();
    int score = 0;

    for(int depth = 1; depth < 100; depth++) {
        timesUp = false;
        int delta = 25;
        int alpha = std::max(-10000000, score - delta);
        int beta = std::min(10000000, score + delta);
        const Move previousBest = rootBestMove;
        if(depth > 3) {
            while (true) {
                score = negamax(board, depth, alpha, beta, 0, true);
                if(timesUp) {
                    return previousBest;
                }
                if (score >= beta) {
                    beta = std::min(beta + delta, 10000000);
                } else if (score <= alpha) {
                    beta = (alpha + beta) / 2;
                    alpha = std::max(alpha - delta, -10000000);
                } else break;

                delta *= 1.5;
            }
        } else {
            score = negamax(board, depth, -10000000, 10000000, 0, true);
        }
        const auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - begin).count();
        if(elapsedTime > timeToSearch || timesUp) {
            return previousBest;
        }
        //std::string pv = getPV(board);
        //std::cout << "info depth " << std::to_string(depth) << " nodes " << std::to_string(nodes) << " time " << std::to_string(elapsedTime) << " score cp " << std::to_string(score) << " pv " << pv << std::endl;
        std::cout << "info depth " << std::to_string(depth) << " nodes " << std::to_string(nodes) << " time " << std::to_string(elapsedTime) << " score cp " << std::to_string(score) << std::endl;
    }

    return rootBestMove;

    // thing left in for a while because I was having null move issues
    /*
    std::random_device rd;
    std::mt19937_64 gen(rd());

    if(rootBestMove.getValue() != 0) {
        return rootBestMove;
    } else {
        std::cout << "had to make a random move\n";
        // random mover as a backup
        std::array<Move, 256> moves;
        const int totalMoves = board.getMoves(moves);

        std::uniform_int_distribution distribution{0, totalMoves - 1};

        while(true) {
            const int index = distribution(gen);
            if(board.makeMove(moves[index])) {
                board.undoMove();
                return moves[index];
            }
        }
    }*/
}

int benchSearch(Board board, int depthToSearch) {
    clearHistory();
    nodes = 0;
    timeToSearch = 1215752192;
    
    int score = 0;

    for(int depth = 1; depth <= depthToSearch; depth++) {
        timesUp = false;
        int delta = 25;
        int alpha = std::max(-10000000, score - delta);
        int beta = std::min(10000000, score + delta);
        if(depth > 3) {
            while (true) {
                score = negamax(board, depth, alpha, beta, 0, true);
                
                if (score >= beta) {
                    beta = std::min(beta + delta, 10000000);
                } else if (score <= alpha) {
                    beta = (alpha + beta) / 2;
                    alpha = std::max(alpha - delta, -10000000);
                } else break;

                delta *= 1.5;
            }
        } else {
            score = negamax(board, depth, -10000000, 10000000, 0, true);
        }
    }
    return nodes;
}