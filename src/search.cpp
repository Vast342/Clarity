#include "search.h"
#include "tt.h"
#include <cstring>

constexpr int hardNodeCap = 400000;

constexpr int startDepth = 3;

constexpr int historyCap = 16384;

constexpr int nmpMin = 2;

constexpr int depthLimit = 100;

constexpr int badCaptureScore = -500000;

// The main search functions

// resets the history, done when ucinewgame is sent, and at the start of each turn
// thanks zzzzz
void Engine::clearHistory() {
    std::memset(historyTable.data(), 0, sizeof(historyTable));
    std::memset(captureHistoryTable.data(), 0, sizeof(captureHistoryTable));
    std::memset(conthistTable.data(), 0, sizeof(conthistTable));
}

// resizes the transposition table
void Engine::resizeTT(int newSize) {
    TT.resize(newSize);
}

// resets the engine, done when ucinewgame is sent
void Engine::resetEngine() {
    TT.clearTable();
    clearHistory(); 
}

int Engine::estimateMoveValue(const Board& board, const int end, const int flag) {
    // starting with the end square piece
    int value = eg_value[getType(board.pieceAtIndex(end))];
    // promotions! pawn--, newpiece++
    for(int i = 0; i < 4; i++) {
        if(flag == promotions[i]) {
            value = eg_value[i + 1] - eg_value[Pawn];
            return value;
        }
    }

    // Target square is empty for en passant, but you still capture a pawn
    if(flag == EnPassant) {
        value = eg_value[Pawn];
    }
    // castling can't capture and is never encoded as such so we don't care.
    return value;
}

bool Engine::see(const Board& board, Move move, int threshold) {
    // establishing stuff
    const int start = move.getStartSquare();
    const int end = move.getEndSquare();
    const int flag = move.getFlag();

    int nextVictim = getType(board.pieceAtIndex(start));
    // handle promotions
    // promotion flags are the 4 highest numbers, so this saves a loop if it's not necessary
    if(flag > DoublePawnPush) {
        for(int i = 0; i < 4; i++) {
            if(flag == promotions[i]) {
                nextVictim = i + 1;
                break;
            }
        }
    }
    int balance = estimateMoveValue(board, end, flag) - threshold;
    // best case still doesn't beat threshold, not good
    if(balance < 0) return false;
    // worst case, we lose the piece here
    balance -= eg_value[nextVictim];
    // if it's still winning in the best case scenario, we can just cut it off
    if(balance >= 0) return true;
    // make sure occupied bitboard knows we did the first move
    uint64_t occupied = board.getOccupiedBitboard();
    occupied = (occupied ^ (1ULL << start)) | (1ULL << end);
    if (flag == EnPassant) occupied ^= (1ULL << board.getEnPassantIndex());
    int color = 1 - board.getColorToMove();
    // get the pieces, for detecting revealed attackers
    const uint64_t bishops = board.getPieceBitboard(Bishop) | board.getPieceBitboard(Queen);
    const uint64_t rooks = board.getPieceBitboard(Rook) | board.getPieceBitboard(Queen);
    // generate the attackers (not including the first move)
    uint64_t attackers = board.getAttackers(end) & occupied;
    while(true) {
        // get the attackers
        uint64_t myAttackers = attackers & board.getColoredBitboard(color);
        // if no attackers, you're done
        if (myAttackers == 0ULL) break;
        // find lowest value attacker
        for (nextVictim = Pawn; nextVictim <= Queen; nextVictim++) {
            if ((myAttackers & board.getColoredPieceBitboard(color, nextVictim)) != 0) {
                break;
            }
        }
        // make the move
        occupied ^= (1ULL << std::countr_zero(myAttackers & board.getColoredPieceBitboard(color, nextVictim)));
        // diagonal moves may reveal more attackers
        if(nextVictim == Pawn || nextVictim == Bishop || nextVictim == Queen) {
            attackers |= (getBishopAttacks(end, occupied) & bishops);
        }
        // orthogonal moves may reveal more attackers
        if(nextVictim == Rook || nextVictim == Queen) {
            attackers |= (getRookAttacks(end, occupied) & rooks);
        }
        attackers &= occupied;
        color = 1 - color;
        // update balance
        balance = -balance - 1 - eg_value[nextVictim];
        // if you are ahead
        if(balance >= 0) {
            // speedrunning legality check
            if(nextVictim == King && ((attackers & board.getColoredBitboard(color)) != 0)) {
                color = 1 - color;
            }
            break;
        }
    }
    // if color is different, than you either ran out of attackers and lost the exchange or the opponent won
    return board.getColorToMove() != color;
}

/* orders the moves based on the following order:
    1: TT Best move: the result of a previous search, if any
    2: Good Captures: Sorted by the victim piece value + value from capture history
    3: Killer moves: moves that are proven to be good from earlier, being indexed by ply
    4: History: scores of how many times a move has caused a beta cutoff
    5: Bad captures: captures that result in bad exchanges.
*/
void Engine::scoreMoves(const Board& board, std::array<Move, 256> &moves, std::array<int, 256> &values, int numMoves, Move ttMove, int ply, bool inQSearch) {
    const uint64_t occupied = board.getOccupiedBitboard();
    const int colorToMove = board.getColorToMove();
    for(int i = 0; i < numMoves; i++) {
        Move move = moves[i];
        const int end = move.getEndSquare();
        const int start = move.getStartSquare();
        const int piece = getType(board.pieceAtIndex(start));
        if(move == ttMove) {
            values[i] = 1000000000;
        } else if((occupied & (1ULL << end)) != 0) {
            // Captures!
            const int victim = getType(board.pieceAtIndex(end));
            // see!
            if(see(board, move, 0)) {
                // good captures
                // mvv lva
                values[i] = 500 * eg_value[victim] - eg_value[piece];
            } else {
                // bad captures
                values[i] = badCaptureScore;
            }
            // capthist, I'll be back soon
            /*values[i] =  200 * eg_value[victim] + captureHistoryTable[colorToMove][piece][end][victim];
            if(see(board, move, 0)) {
                // good captures
                values[i] += 1000000;
            }*/
        } else {
            // read from history
            values[i] = historyTable[colorToMove][start][end]
                + (ply > 0 ? (*stack[ply - 1].ch_entry)[colorToMove][piece][end] : 0)
                + (ply > 1 ? (*stack[ply - 2].ch_entry)[colorToMove][piece][end] : 0);
            // if not in qsearch, killers
            if(!inQSearch) {
                if(move == stack[ply].killers[0]) {
                    values[i] = 54000;
                } else if(move == stack[ply].killers[1]) {
                    values[i] = 53000;
                }  else if(move == stack[ply].killers[2]) {
                    values[i] = 52000;
                }
            }
        }
    }
}

// Quiecense search, searching all the captures until there aren't anymore so that you can get an accurate eval
int Engine::qSearch(Board &board, int alpha, int beta, int ply) {
    //if(board.isRepeatedPosition()) return 0;
    // time check every 4096 nodes
    if(nodes % 4096 == 0) {
        if(dataGeneration) {
            if(nodes > hardNodeCap) {
                timesUp = true;
                return 0;
            }
        } else {
            if(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - begin).count() > hardLimit) {
                timesUp = true;
                return 0;
            }
        }
    }
    if(ply > seldepth) seldepth = ply;
    const uint64_t hash = board.getZobristHash();
    // TT check
    Transposition* entry = TT.getEntry(hash);

    if(entry->zobristKey == hash && (
        entry->flag == Exact // exact score
            || (entry->flag == BetaCutoff && entry->score >= beta) // lower bound, fail high
            || (entry->flag == FailLow && entry->score <= alpha) // upper bound, fail low
    )) {
        return entry->score;
    }

    // stand pat shenanigans
    int bestScore = board.getEvaluation();
    if(bestScore >= beta) return bestScore;
    if(alpha < bestScore) alpha = bestScore;

    // get the legal moves and sort them
    std::array<Move, 256> moves;
    const int totalMoves = board.getMovesQSearch(moves);
    std::array<int, 256> moveValues;
    scoreMoves(board, moves, moveValues, totalMoves, entry->bestMove, -1, true);

    // values useful for writing to TT later
    Move bestMove;
    int flag = FailLow;

    // loop though all the moves
    for(int i = 0; i < totalMoves; i++) {
        for (int j = i + 1; j < totalMoves; j++) {
            if (moveValues[j] > moveValues[i]) {
                std::swap(moveValues[j], moveValues[i]);
                std::swap(moves[j], moves[i]);
            }
        }
        Move move = moves[i];
        if(moveValues[i] == badCaptureScore) {
            continue;
        }
        if(!board.makeMove(move)) {
            continue;
        }
        nodes++;
        // searches from this node
        const int score = -qSearch(board, -beta, -alpha, ply + 1);
        board.undoMove();
        // time check
        if(timesUp) return 0;

        if(score > bestScore) {
            bestScore = score;

            // Improve alpha
            if(score > alpha) {
                flag = Exact;
                alpha = score;
                bestMove = move;
            }

            // Fail-high
            if(score >= beta) {
                flag = BetaCutoff;
                bestMove = move;
                break;
            }
        }
    }

    // push to TT
    TT.setEntry(hash, Transposition(hash, bestMove, flag, bestScore, 0));

    return bestScore;  
}

// adds to the history of a particular move
void Engine::updateHistory(const int colorToMove, const int start, const int end, const int piece, const int bonus, const int ply) {
    int thingToAdd = bonus - historyTable[colorToMove][start][end] * std::abs(bonus) / historyCap;
    historyTable[colorToMove][start][end] += thingToAdd;
    if (ply > 0) {
        thingToAdd = bonus - (*stack[ply - 1].ch_entry)[colorToMove][piece][end] * std::abs(bonus) / historyCap;
        (*stack[ply - 1].ch_entry)[colorToMove][piece][end] += thingToAdd;
    }

    if (ply > 1) {
        thingToAdd = bonus - (*stack[ply - 2].ch_entry)[colorToMove][piece][end] * std::abs(bonus) / historyCap;
        (*stack[ply - 2].ch_entry)[colorToMove][piece][end] += thingToAdd;
    }
}

void Engine::updateCaptureHistory(const int colorToMove, const int piece, const int end, const int victim, const int bonus) {
    const int thingToAdd = bonus - captureHistoryTable[colorToMove][piece][end][victim] * std::abs(bonus) / historyCap;
    captureHistoryTable[colorToMove][piece][end][victim] += thingToAdd;
}

// The main search function
int Engine::negamax(Board &board, int depth, int alpha, int beta, int ply, bool nmpAllowed) {
    // if it's a repeated position, it's a draw
    if(ply > 0 && board.isRepeatedPosition()) return 0;
    // time check every 4096 nodes
    if(nodes % 4096 == 0) {
        if(dataGeneration) {
            if(nodes > hardNodeCap) {
                timesUp = true;
                return 0;
            }
        } else {
            if(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - begin).count() > hardLimit) {
                timesUp = true;
                return 0;
            }
        }
    }
    
    // activate q search if at the end of a branch
    if(depth <= 0) return qSearch(board, alpha, beta, ply);
    const bool isPV = beta > alpha + 1;
    const bool inCheck = board.isInCheck();
    stack[ply].inCheck = inCheck;
    const uint64_t hash = board.getZobristHash();

    // TT check
    Transposition* entry = TT.getEntry(hash);

    // if it meets these criteria, it's done the search exactly the same way before, if not more throuroughly in the past and you can skip it
    // it would make sense to add !isPV here, however from my testing that makes it about 80 elo worse
    // turns out that score above was complete bs lol, my isPV was broken
    if(!isPV && ply > 0 && entry->zobristKey == hash && entry->depth >= depth && (
            entry->flag == Exact // exact score
                || (entry->flag == BetaCutoff && entry->score >= beta) // lower bound, fail high
                || (entry->flag == FailLow && entry->score <= alpha) // upper bound, fail low
        )) {
        return entry->score; 
    }

    // Internal Iterative Reduction (IIR)
    // Things to test: alternative depth
    if((entry->zobristKey != hash || entry->bestMove == Move()) && depth > 3) depth--;

    // Reverse Futility Pruning
    const int staticEval = board.getEvaluation();
    stack[ply].staticEval = staticEval;
    const bool improving = (ply > 1 && !inCheck && staticEval > stack[ply - 2].staticEval && !stack[ply - 2].inCheck);
    if(staticEval - 80 * (depth - improving) >= beta && !inCheck && depth < 9 && !isPV) return staticEval;

    // Null Move Pruning (NMP)
    // Things to test: !isPV, alternate formulas, etc
    // "I could probably detect zugzwang here but ehhhhh" -Me, a few months ago
    if(nmpAllowed && depth >= nmpMin && !inCheck && staticEval >= beta) {
        stack[ply].ch_entry = &conthistTable[0][0][0];
        board.changeColor();
        const int score = -negamax(board, depth - (depth+1)/3 - 2, 0-beta, 1-beta, ply + 1, false);
        board.undoChangeColor();
        if(score >= beta) {
            return score;
        }
    }

    // get the moves
    std::array<Move, 256> moves;
    std::array<Move, 256> testedQuiets;
    int quietCount = 0;
    const int totalMoves = board.getMoves(moves);
    std::array<int, 256> moveValues;
    scoreMoves(board, moves, moveValues, totalMoves, entry->bestMove, ply, false);

    // values useful for writing to TT later
    int bestScore = mateScore;
    Move bestMove;
    int flag = FailLow;

    // extensions, currently only extending if you are in check
    depth += inCheck;

    // Mate Distance Pruning (I will test it at some point I swear)
    /*if (!isPV) {
        // my mateScore is a large negative number and that is what I return, people seem to get confused by that when I talk with other devs.
        const auto mdAlpha = std::max(alpha, mateScore + ply);
        const auto mdBeta = std::min(beta, -mateScore - ply - 1);
        if (mdAlpha >= mdBeta) {
            return mdAlpha;
        }
    }*/
    // capturable squares to determine if a move is a capture.
    const uint64_t capturable = board.getOccupiedBitboard();
    // loop through the moves
    int legalMoves = 0;
    for(int i = 0; i < totalMoves; i++) {
        for (int j = i + 1; j < totalMoves; j++) {
            if (moveValues[j] > moveValues[i]) {
                std::swap(moveValues[j], moveValues[i]);
                std::swap(moves[j], moves[i]);
            }
        }
        Move move = moves[i];
        int moveStartSquare = move.getStartSquare();
        int moveEndSquare = move.getEndSquare();
        int moveFlag = move.getFlag();
        bool isCapture = ((capturable & (1ULL << moveEndSquare)) != 0) || moveFlag == EnPassant;
        bool isQuiet = (!isCapture && (moveFlag <= DoublePawnPush));
        bool isQuietOrBadCapture = (moveValues[i] <= historyCap * 3);
        // futility pruning
        if(bestScore > mateScore && !inCheck && depth <= 8 && staticEval + 250 + depth * 60 <= alpha) break;
        // Late Move Pruning
        if(depth < 7 && !isPV && isQuiet && bestScore > mateScore + 256 && quietCount > 5 + depth * depth / (2 - improving)) continue;
        // see pruning
        if (depth <= 8 && isQuietOrBadCapture && bestScore > mateScore + 256 && !see(board, move, depth * (!isCapture ? -50 : -90))) continue;
        if(!board.makeMove(move)) {
            continue;
        }
        stack[ply].ch_entry = &conthistTable[board.getColorToMove()][getType(board.pieceAtIndex(moveEndSquare))][moveEndSquare];
        legalMoves++;
        nodes++;
        if(isQuiet) {
            testedQuiets[quietCount] = move;
            quietCount++;
        }
        int score = 0;
        // Principal Variation Search
        int presearchNodeCount = nodes;
        if(legalMoves == 1) {
            // searches TT move at full depth, no reductions or anything, given first by the move ordering step.
            score = -negamax(board, depth - 1, -beta, -alpha, ply + 1, true);
        } else {
            // Late Move Reductions (LMR)
            int depthReduction = 0;
            if(!inCheck && depth > 1 && isQuiet) {
                depthReduction = reductions[depth][legalMoves];
                depthReduction -= isPV;
                if(moveValues[i] < 53000) {
                    depthReduction -= moveValues[i] / 8192;
                }

                depthReduction = std::clamp(depthReduction, 0, depth - 2);
            }
            // this is more PVS stuff, searching with a reduced margin
            score = -negamax(board, depth - depthReduction - 1, -alpha - 1, -alpha, ply + 1, true);
            // and then if it fails high or low we search again with the original bounds
            if(score > alpha && (score < beta || depthReduction > 0)) {
                score = -negamax(board, depth - 1, -beta, -alpha, ply + 1, true);
            }
        }
        board.undoMove();

        if(ply == 0) nodeTMTable[moveStartSquare][moveEndSquare] += nodes - presearchNodeCount;

        // backup time check
        if(timesUp) return 0;

        if(score > bestScore) {
            bestScore = score;

            // Improve alpha
            if(score > alpha) {
                flag = Exact; 
                alpha = score;
                bestMove = move;
                if(ply == 0) rootBestMove = move;
            }

            // Fail-high
            if(score >= beta) {
                flag = BetaCutoff;
                bestMove = move;
                if(ply == 0) rootBestMove = move;
                if(isQuiet) {
                    // adds to the move's history and adjusts the killer table accordingly
                    int start = moveStartSquare;
                    int end = moveEndSquare;
                    int piece = getType(board.pieceAtIndex(start));
                    // testing berserk history bonus
                    int bonus = std::min(1896, 4 * depth * depth + 120 * depth - 120);
                    const int colorToMove = board.getColorToMove();
                    updateHistory(colorToMove, start, end, piece, bonus, ply);
                    bonus = -bonus;
                    // malus!
                    for(int quiet = 0; quiet < quietCount - 1; quiet++) {
                        start = testedQuiets[quiet].getStartSquare();
                        end = testedQuiets[quiet].getEndSquare();
                        piece = getType(board.pieceAtIndex(start));
                        updateHistory(colorToMove, start, end, piece, bonus, ply);
                    }
                    if(stack[ply].killers[0] != move && stack[ply].killers[1] != move) {
                        stack[ply].killers[2] = stack[ply].killers[1];
                        stack[ply].killers[1] = stack[ply].killers[0];
                        stack[ply].killers[0] = move;
                    }
                } /*else { NEED TO MAKE MALUS BEFORE MAKING THIS
                    const int end = move.getEndSquare();
                    const int piece = getType(board.pieceAtIndex(move.getStartSquare()));
                    const int victim = getType(board.pieceAtIndex(end));
                    updateCaptureHistory(board.getColorToMove(), piece, end, victim, bonus);
                }*/
                break;
            }
        }
    }

    // checkmate / stalemate detection, if I did legal move generation instead of pseudolegal I could probably do this first
    if(legalMoves == 0) {
        if(inCheck) {
            return mateScore + ply;
        }
        return 0;
    }

    // push to TT
    //bool pushToTT = ((flag == Exact && entry->flag != Exact) || depth + 4 >= entry->depth || entry->zobristKey != hash);
    if(entry->zobristKey == hash && entry->bestMove != Move() && bestMove == Move()) bestMove = entry->bestMove;
    /*if(pushToTT)*/ TT.setEntry(hash, Transposition(hash, bestMove, flag, bestScore, depth));

    return bestScore;
}

// gets the PV from the TT, has some inconsistencies or illegal moves, and will be replaced with a triangular PV table eventually
std::string Engine::getPV(Board board, std::vector<uint64_t> &hashVector, int numEntries) {
    std::string pv;
    const uint64_t hash = board.getZobristHash();
    for(int i = numEntries; i > -1; i--) {
        if(hashVector[i] == hash) {
            // repitition, GET THAT OUTTA HERE
            return pv;
        }
    }
    hashVector.push_back(hash);
    numEntries++;
    if(TT.matchZobrist(hash)) {
        Move bestMove = TT.getBestMove(hash);
        if(bestMove != Move() && board.makeMove(bestMove)) {
            std::string restOfPV = getPV(board, hashVector, numEntries);
            pv = toLongAlgebraic(bestMove) + " " + restOfPV;
        }
    }
    return pv;
}

void Engine::outputInfo(const Board& board, int score, int depth, int elapsedTime) {
    std::string scoreString = " score ";
    if(abs(score) < abs(mateScore + 256)) {
        scoreString += "cp ";
        scoreString += std::to_string(score);
    } else {
        // score is checkmate in score - mateScore ply
        // position fen rn1q2rk/pp3p1p/2p4Q/3p4/7P/2NP2R1/PPP3P1/4RK2 w - - 0 1
        // ^^ mate in 3 test position
        int colorMultiplier = score > 0 ? 1 : -1;
        scoreString += "mate ";
        scoreString += std::to_string((abs(abs(score) + mateScore) / 2 + board.getColorToMove()) * colorMultiplier);
    }
    if(depth > 6) {
        std::vector<uint64_t> hashVector;
        hashVector.reserve(128);
        std::cout << "info depth " << std::to_string(depth) << " seldepth " << std::to_string(seldepth) << " nodes " << std::to_string(nodes) << " time " << std::to_string(elapsedTime) << " nps " << std::to_string(int(double(nodes) / (elapsedTime == 0 ? 1 : elapsedTime) * 1000)) << scoreString << " pv " << getPV(board, hashVector, 0) << std::endl;
    } else {
        std::cout << "info depth " << std::to_string(depth) << " seldepth " << std::to_string(seldepth) << " nodes " << std::to_string(nodes) << " time " << std::to_string(elapsedTime) << " nps " << std::to_string(int(double(nodes) / (elapsedTime == 0 ? 1 : elapsedTime) * 1000)) << scoreString << " pv " << toLongAlgebraic(rootBestMove) << std::endl;
    }
}

// the usual search function, where you give it the amount of time it has left, and it will search in increasing depth steps until it runs out of time
Move Engine::think(Board board, int softBound, int hardBound, bool info) {
    //ageHistory();
    //clearHistory();
    std::memset(nodeTMTable.data(), 0, sizeof(nodeTMTable));
    nodes = 0;
    hardLimit = hardBound;
    seldepth = 0;
    timesUp = false;

    begin = std::chrono::steady_clock::now();

    rootBestMove = Move();
    int score = 0;

    // Iterative Deepening, searches to increasing depths, which sounds like it would slow things down but it makes it much better
    for(int depth = 1; depth < 100; depth++) {
        // Aspiration Windows, searches with reduced bounds until it doesn't fail high or low
        seldepth = depth;
        timesUp = false;
        int delta = 25;
        int alpha = std::max(mateScore, score - delta);
        int beta = std::min(-mateScore, score + delta);
        const Move previousBest = rootBestMove;
        if(depth > 3) {
            while (true) {
                score = negamax(board, depth, alpha, beta, 0, true);
                if(timesUp) {
                    return previousBest;
                } 
                if (score >= beta) {
                    beta = std::min(beta + delta, -mateScore);
                } else if (score <= alpha) {
                    beta = (alpha + beta) / 2;
                    alpha = std::max(alpha - delta, mateScore);
                } else break;

                delta *= 1.5;
            }
        } else {
            score = negamax(board, depth, mateScore, -mateScore, 0, true);
        }
        const auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - begin).count();
        if(timesUp) {
            return previousBest;
        }
        // outputs info which is picked up by the user
        if(info) outputInfo(board, score, depth, elapsedTime);
        // soft time bounds check
        double frac = nodeTMTable[rootBestMove.getStartSquare()][rootBestMove.getEndSquare()] / static_cast<double>(nodes);
        if(elapsedTime >= softBound * (depth > 8 ? (1.5 - frac) * 1.35 : 1.00)) break;
        //if(elapsedTime > softBound) break;
    }

    return rootBestMove;
}

// searches done for bench, returns the number of nodes searched.
int Engine::benchSearch(Board board, int depthToSearch) {
    //clearHistory();
    nodes = 0;
    hardLimit = 1215752192;
    seldepth = 0;
    timesUp = false;
    
    begin = std::chrono::steady_clock::now();

    rootBestMove = Move();
    int score = 0;
    // Iterative Deepening, searches to increasing depths, which sounds like it would slow things down but it makes it much better
    for(int depth = 1; depth <= depthToSearch; depth++) {
        // Aspiration Windows, searches with reduced bounds until it doesn't fail high or low
        seldepth = depth;
        int delta = 25;
        int alpha = std::max(mateScore, score - delta);
        int beta = std::min(-mateScore, score + delta);
        if(depth > 3) {
            while (true) {
                score = negamax(board, depth, alpha, beta, 0, true);
                
                if (score >= beta) {
                    beta = std::min(beta + delta, -mateScore);
                } else if (score <= alpha) {
                    beta = (alpha + beta) / 2;
                    alpha = std::max(alpha - delta, mateScore);
                } else break;

                delta *= 1.5;
            }
        } else {
            score = negamax(board, depth, mateScore, -mateScore, 0, true);
        }
        //const auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - begin).count();
        // outputs info which is picked up by the user
        //outputInfo(board, score, depth, elapsedTime);
    }
    return nodes;
}

// searches to a fixed depth when the user says go depth x
Move Engine::fixedDepthSearch(Board board, int depthToSearch, bool info) {
    //ageHistory();
    //clearHistory();
    nodes = 0;
    seldepth = 0;
    hardLimit = 1215752192;
    begin = std::chrono::steady_clock::now();

    int score = 0;

    for(int depth = 1; depth <= depthToSearch; depth++) {
        seldepth = 0;
        timesUp = false;
        int delta = 25;
        int alpha = std::max(mateScore, score - delta);
        int beta = std::min(-mateScore, score + delta);
        if(depth > 3) {
            while (true) {
                score = negamax(board, depth, alpha, beta, 0, true);
                
                if (score >= beta) {
                    beta = std::min(beta + delta, -mateScore);
                } else if (score <= alpha) {
                    beta = (alpha + beta) / 2;
                    alpha = std::max(alpha - delta, mateScore);
                } else break;

                delta *= 1.5;
            }
        } else {
            score = negamax(board, depth, mateScore, -mateScore, 0, true);
        }
        const auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - begin).count();
        if(info) outputInfo(board, score, depth, elapsedTime);
    }
    return rootBestMove;
}

std::pair<Move, int> Engine::dataGenSearch(Board board, int nodeCap) {
    //clearHistory();
    dataGeneration = true;
    nodes = 0;
    hardLimit = 1215752192;
    seldepth = 0;
    timesUp = false;

    begin = std::chrono::steady_clock::now();

    rootBestMove = Move();
    int score = 0;
    // Iterative Deepening, searches to increasing depths, which sounds like it would slow things down but it makes it much better
    for(int depth = 1; depth <= 100; depth++) {
        // Aspiration Windows, searches with reduced bounds until it doesn't fail high or low
        seldepth = depth;
        int delta = 25;
        int alpha = std::max(mateScore, score - delta);
        int beta = std::min(-mateScore, score + delta);
        if(depth > 3) {
            while (true) {
                score = negamax(board, depth, alpha, beta, 0, true);
                
                if (score >= beta) {
                    beta = std::min(beta + delta, -mateScore);
                } else if (score <= alpha) {
                    beta = (alpha + beta) / 2;
                    alpha = std::max(alpha - delta, mateScore);
                } else break;
                if(nodes > nodeCap) break;
                delta *= 1.5;
            }
        } else {
            score = negamax(board, depth, mateScore, -mateScore, 0, true);
        }
        //const auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - begin).count();
        // outputs info which is picked up by the user
        //outputInfo(board, score, depth, elapsedTime);
        if(nodes > nodeCap) break;
    }
    return std::pair<Move, int>(rootBestMove, score);
}
