#include "globals.h"
#include "psqt.h"
#include <cstdlib>

// zobrist hashing values 
std::array<std::array<uint64_t, 14>, 64> zobTable;
// if black is to move this value is xor'ed
uint64_t zobColorToMove;

int directionalOffsets[] = {8, -8, 1, -1, 7, -7, 9, -9};

void Board::toString() {
    for(int rank = 7; rank >= 0; rank--) {
        for(int file = 0; file < 8; file++) {
            int piece = pieceAtIndex(rank * 8 + file);
            bool isBlack = getColor(piece) == 0;
            int pieceType = getType(piece);
            char pieceChar = ' ';
            switch (pieceType)
            {
                case Rook:
                    pieceChar = 'R';
                    break;
                case Knight:
                    pieceChar = 'N';
                    break;
                case Bishop:
                    pieceChar = 'B';
                    break;
                case Queen:
                    pieceChar = 'Q';
                    break;
                case King:
                    pieceChar = 'K';
                    break;
                case Pawn:
                    pieceChar = 'P';
                    break;
                default:
                    pieceChar = ' ';
                    break;
            }
            std::cout << static_cast<char>(isBlack ? tolower(pieceChar) : pieceChar) << " ";
        }
        std::cout << '\n';
    }
    std::cout << "Ply count: " << std::to_string(plyCount) << '\n';
    std::cout << "Fifty-move count: " << std::to_string(fiftyMoveCounter) << '\n';
    std::cout << "Hash: " << std::to_string(zobristHash) << '\n';
    std::cout << "Castling rights: " << std::to_string(castlingRights & 1) << std::to_string((castlingRights >> 1) & 1) << std::to_string((castlingRights >> 2) & 1) << std::to_string((castlingRights >> 3) & 1) << '\n';
    if(enPassantIndex == 64) {
        std::cout << "En passant square: none\n";
    } else {
        std::cout << "En passant square: " << squareNames[enPassantIndex] << '\n';
    }
    std::cout << "Color to move: " << (colorToMove == 0 ? "black" : "white") << '\n';
    std::cout << "Midgame eval: " << std::to_string(mgEval) << '\n';
    std::cout << "Endgame eval: " << std::to_string(egEval) << '\n';
    std::cout << "Phase: " << std::to_string(phase) << '\n';
    std::cout << "Total eval: " << std::to_string(getEvaluation()) << '\n';
}

Board::Board(std::string fen) {
    zobristHistory.clear();
    stateHistory.clear();
    mgEval = 0;
    egEval = 0;
    phase = 0;
    zobristHash = 0;
    for(int i = 0; i < 6; i++) {
        pieceBitboards[i] = 0ULL;
    }
    for(int i = 0; i < 2; i++) {
        coloredBitboards[i] = 0ULL;
    }
    auto segments = views::split(fen, ' ');
    auto backwardRanks = views::split(segments.front(), '/');
    std::vector ranks(backwardRanks.begin(), backwardRanks.end());
    std::ranges::reverse(ranks);
    int i = 0;
    for (const auto& rank : ranks) {
        for(char c : rank) {
            switch(c) {
                case 'p':
                    addPiece(i, Pawn | Black);
                    i++;
                    break;
                case 'P':
                    addPiece(i, Pawn | White);
                    i++;
                    break;
                case 'n':
                    addPiece(i, Knight | Black);
                    i++;
                    break;
                case 'N':
                    addPiece(i, Knight | White);
                    i++;
                    break;
                case 'b':
                    addPiece(i, Bishop | Black);
                    i++;
                    break;
                case 'B':
                    addPiece(i, Bishop | White);
                    i++;
                    break;
                case 'r':
                    addPiece(i, Rook | Black);
                    i++;
                    break;
                case 'R':
                    addPiece(i, Rook | White);
                    i++;
                    break;
                case 'q':
                    addPiece(i, Queen | Black);
                    i++;
                    break;
                case 'Q':
                    addPiece(i, Queen | White);
                    i++;
                    break;
                case 'k':
                    addPiece(i, King | Black);
                    kingSquares[0] = i;
                    i++;
                    break;
                case 'K':
                    addPiece(i, King | White);
                    kingSquares[1] = i;
                    i++;
                    break;
                default:
                    i += (c - '0');
                    break;
            }
        }
    }
    auto segmentIter = segments.begin();
    ++segmentIter; // we've already parsed the board position
    auto colorSegment = *segmentIter;
    std::string color(colorSegment.begin(), colorSegment.end());
    ++segmentIter;
    auto castlingSegment = *segmentIter;
    std::string castling(castlingSegment.begin(), castlingSegment.end());
    ++segmentIter;
    auto enPassantSegment = *segmentIter;
    std::string enPassant(enPassantSegment.begin(), enPassantSegment.end());
    ++segmentIter;
    auto fiftyMoveSegment = *segmentIter;
    std::string fiftyMoveString(fiftyMoveSegment.begin(), fiftyMoveSegment.end());
    ++segmentIter;
    auto plyCountSegment = *segmentIter;
    std::string plyString(plyCountSegment.begin(), plyCountSegment.end());
    int ply = std::stoi(plyString);
    // convert color to move into 0 or 1, segment 2
    colorToMove = (color == "w" ? 1 : 0);
    if(colorToMove == 1) zobristHash ^= zobColorToMove;
    // decode the castling rights into the 4 bit number, segment 3
    castlingRights = 0;
    for(char c : castling) {
        if(c == 'K') {
            castlingRights |= 1;
        } else if(c == 'Q') {
            castlingRights |= 2;
        } else if(c == 'k') {
            castlingRights |= 4;
        } else if(c == 'q') {
            castlingRights |= 8;
        }
    }
    // decode the en passant index, segment 4
    if(enPassant != "-") {
        int i = 0;
        int num = 0;
        for(char c : enPassant) {
            if(i == 0) {
                num += (c - 'a');
            } else {
                num += ((c - '1') * 8);
            }
            i++;
        }
        enPassantIndex = num;
    } else {
        enPassantIndex = 64;
    }
    // 50 move counter, segment 5
    fiftyMoveCounter = std::stoi(fiftyMoveString);
    hundredPlyCounter = 0;
    // ply count, segment 6
    plyCount = ply * 2 - colorToMove;
    //std::cout << "Parsed fen, hash is now " << std::to_string(zobristHash) << '\n';
}

std::string Board::getFenString() {
    // essentially copy and pasted from my c# engine lol
    std::string fen = "";
    for (int rank = 7; rank >= 0; rank--) {
        int numEmptyFiles = 0;
        for (int file = 0; file < 8; file++) {
            int piece = pieceAtIndex(8*rank+file);
            if (piece != None) {
                if (numEmptyFiles != 0) {
                    fen += std::to_string(numEmptyFiles);
                    numEmptyFiles = 0;
                }
                bool isBlack = getColor(piece) == 0;
                int pieceType = getType(piece);
                char pieceChar = ' ';
                switch (pieceType)
                {
                    case Rook:
                        pieceChar = 'R';
                        break;
                    case Knight:
                        pieceChar = 'N';
                        break;
                    case Bishop:
                        pieceChar = 'B';
                        break;
                    case Queen:
                        pieceChar = 'Q';
                        break;
                    case King:
                        pieceChar = 'K';
                        break;
                    case Pawn:
                        pieceChar = 'P';
                        break;
                }
                fen += isBlack ? tolower(pieceChar) : pieceChar;
            }
            else {
                numEmptyFiles++;
            }

        }
        if (numEmptyFiles != 0) {
            fen += std::to_string(numEmptyFiles);
        }
        if (rank != 0) {
            fen += '/';
        }
    }

    // color to move
    fen += ' ';
    fen += (colorToMove == 0 ? 'b' : 'w');
    // castling rights
    fen += ' ';
    bool thingAdded = false;
    if((castlingRights & 1) != 0) {
        fen += 'K'; 
        thingAdded = true;
    }
    if((castlingRights & 2) != 0) {
        fen += 'Q'; 
        thingAdded = true;
    }
    if((castlingRights & 4) != 0) {
        fen += 'k'; 
        thingAdded = true;
    }
    if((castlingRights & 8) != 0) {
        fen += 'q'; 
        thingAdded = true;
    }
    if(thingAdded == false) fen += '-';
    // en passant square
    fen += ' ';
    if(enPassantIndex == 64) {
        fen += '-';
    } else {
        fen += squareNames[enPassantIndex];
    }
    // 50 move counter
    fen += ' ';
    fen += std::to_string(fiftyMoveCounter);
    // ply count
    fen += ' ';
    fen += std::to_string(plyCount / 2 + colorToMove);
    return fen;
}

void Board::addPiece(int square, int type) {
    assert(type != None);
    assert(square < 64);
    assert(pieceAtIndex(square) == None);
    assert(square >= 0);
    //std::cout << "Adding piece of type " << std::to_string(type) << " at index " << std::to_string(square) << '\n';
    coloredBitboards[getColor(type)] ^= (1ULL << square);
    pieceBitboards[getType(type)] ^= (1ULL << square);
    assert(pieceAtIndex(square) == type);
    phase += phaseIncrements[getType(type)];
    int index = getColor(type) == 1 ? flipIndex(square) : square;
    int colorMultiplier = 2 * getColor(type) - 1;
    mgEval += mgTables[getType(type)][index] * colorMultiplier;
    egEval += egTables[getType(type)][index] * colorMultiplier;
    zobristHash ^= zobTable[square][type];
}

void Board::removePiece(int square, int type) {
    assert(type != None);
    assert(square < 64);
    assert(pieceAtIndex(square) == type);
    assert(square >= 0);
    //std::cout << "Removing piece of type " << std::to_string(type) << " at index " << std::to_string(square) << '\n';
    coloredBitboards[getColor(type)] ^= (1ULL << square);
    pieceBitboards[getType(type)] ^= (1ULL << square);
    phase -= phaseIncrements[getType(type)];
    int index = getColor(type) == 1 ? flipIndex(square) : square;
    int colorMultiplier = 2 * getColor(type) - 1;
    mgEval -= mgTables[getType(type)][index] * colorMultiplier;
    egEval -= egTables[getType(type)][index] * colorMultiplier;
    zobristHash ^= zobTable[square][type];
    assert(pieceAtIndex(square) == None);
}

void Board::movePiece(int square1, int type1, int square2, int type2) {
    assert(type1 != None);
    assert(square1 < 64);
    assert(square2 < 64);
    if(type2 != None) removePiece(square2, type2);
    addPiece(square2, type1);
    removePiece(square1, type1);
}

int Board::pieceAtIndex(int index) const {
    if((getOccupiedBitboard() & (1ULL << index)) != 0) {
        for(int i = Pawn; i < None; i++) {
            if((getColoredPieceBitboard(0, i) & 1ULL << index) != 0) {
                return i | Black;
            }
            if((getColoredPieceBitboard(1, i) & 1ULL << index) != 0) {
                return i | White;
            }
        }
    }
    return None;
}
int Board::colorAtIndex(int index) const {
    if((coloredBitboards[0] & (1ULL << index)) != 0) {
        return 0;
    }
    if((coloredBitboards[1] & (1ULL << index)) != 0) {
        return 1;
    }
    // invalid result, bad
    return 2;
}

uint64_t Board::getColoredPieceBitboard(int color, int piece) const {
    return pieceBitboards[piece] & coloredBitboards[color];
}

uint64_t Board::getOccupiedBitboard() const {
    return coloredBitboards[0] | coloredBitboards[1];
}

// fills up the array and then returns the number of moves to be looped through later.
int Board::getMoves(std::array<Move, 256> &moves) {
    uint64_t occupiedBitboard = getOccupiedBitboard();
    int totalMoves = 0;
    // castling
    if(castlingRights != 0) {
        if(!isInCheck()) {
            if(colorToMove == 1) {
                if((castlingRights & 1) != 0 && (occupiedBitboard & 0x60) == 0 && !squareIsUnderAttack(5) && pieceAtIndex(7) == (Rook | White)) {
                    moves[totalMoves] = Move(4, 6, castling[0]);
                    totalMoves++;
                }
                if((castlingRights & 2) != 0 && (occupiedBitboard & 0xE) == 0 && !squareIsUnderAttack(3) && pieceAtIndex(0) == (Rook | White)) {
                    moves[totalMoves] = Move(4, 2, castling[1]);
                    totalMoves++;
                }
            } else {
                if((castlingRights & 4) != 0 && (occupiedBitboard & 0x6000000000000000) == 0 && !squareIsUnderAttack(61) && pieceAtIndex(63) == Rook) {
                    moves[totalMoves] = Move(60, 62, castling[2]);
                    totalMoves++;
                }
                if((castlingRights & 8) != 0 && (occupiedBitboard & 0xE00000000000000) == 0 && !squareIsUnderAttack(59) && pieceAtIndex(56) == Rook) {
                    moves[totalMoves] = Move(60, 58, castling[3]);
                    totalMoves++;   
                }
            }
        }
    }
    uint64_t mask = coloredBitboards[colorToMove] ^ getColoredPieceBitboard(colorToMove, Pawn);
    // the rest of the pieces
    while(mask != 0) {
        uint8_t startSquare = popLSB(mask);
        uint8_t currentType = getType(pieceAtIndex(startSquare));
        uint64_t total = 0;
        if(currentType == Knight) {
            total = getKnightAttacks(startSquare);
        } else if(currentType == Bishop) {
            total = getBishopAttacks(startSquare, occupiedBitboard);
        } else if(currentType == Rook) {
            total = getRookAttacks(startSquare, occupiedBitboard);
        } else if(currentType == Queen) {
            total = getRookAttacks(startSquare, occupiedBitboard) | getBishopAttacks(startSquare, occupiedBitboard);
        } else if(currentType == King) {
            total = getKingAttacks(startSquare);
        }
        // get rid of capturing your own pieces
        total ^= (total & coloredBitboards[colorToMove]); 
        while(total != 0) {
            moves[totalMoves] = Move(startSquare, popLSB(total), Normal);
            totalMoves++;
        }
    }
    // pawn pushes
    uint64_t pawnBitboard = getColoredPieceBitboard(colorToMove, Pawn);
    uint64_t emptyBitboard = ~occupiedBitboard;
    uint64_t pawnPushes = getPawnPushes(pawnBitboard, emptyBitboard, colorToMove);
    uint64_t doublePawnPushes = getDoublePawnPushes(pawnPushes, emptyBitboard, colorToMove);
    uint64_t pawnPushPromotions = pawnPushes & getRankMask(7 * colorToMove);
    pawnPushes ^= pawnPushPromotions;
    while(pawnPushes != 0) {
        uint8_t index = popLSB(pawnPushes);
        uint8_t startSquare = (index + directionalOffsets[colorToMove]);
        moves[totalMoves] = Move(startSquare, index, Normal);
        totalMoves++;
    }
    while(doublePawnPushes != 0) {
        uint8_t index = popLSB(doublePawnPushes);
        uint8_t startSquare = (index + (directionalOffsets[colorToMove] * 2));
        assert(getType(pieceAtIndex(startSquare)) == Pawn);
        moves[totalMoves] = Move(startSquare, index, DoublePawnPush);
        totalMoves++;
    }
    while(pawnPushPromotions != 0) {
        uint8_t index = popLSB(pawnPushPromotions);
        uint8_t startSquare = (index + directionalOffsets[colorToMove]);
        for(int type = Knight; type < King; type++) {
            moves[totalMoves] = Move(startSquare, index, promotions[type-1]);
            totalMoves++;
        }
    }
    // pawn captures
    uint64_t capturable = coloredBitboards[1 - colorToMove];
    if(enPassantIndex != 64) {
        capturable |= (1ULL << enPassantIndex);
    }

    uint64_t leftCaptures = (colorToMove == 0 ? pawnBitboard >> 9 : pawnBitboard << 7);
    leftCaptures &= ~getFileMask(7);
    leftCaptures &= capturable;
    uint64_t leftCapturePromotions = leftCaptures & getRankMask(7 * colorToMove);
    leftCaptures ^= leftCapturePromotions;

    uint64_t rightCaptures = (colorToMove == 0 ? pawnBitboard >> 7 : pawnBitboard << 9);
    rightCaptures &= ~getFileMask(0);
    rightCaptures &= capturable;
    uint64_t rightCapturePromotions = rightCaptures & getRankMask(7 * colorToMove);
    rightCaptures ^= rightCapturePromotions;

    while(leftCaptures != 0) {
        int index = popLSB(leftCaptures);
        int startSquare = index + (colorToMove == 0 ? 9 : -7);
        moves[totalMoves] = Move(startSquare, index, (index == enPassantIndex ? EnPassant : Normal));
        totalMoves++;
    }
    while(rightCaptures != 0) {
        int index = popLSB(rightCaptures);
        int startSquare = index + (colorToMove == 0 ? 7 : -9);
        moves[totalMoves] = Move(startSquare, index, (index == enPassantIndex ? EnPassant : Normal));
        totalMoves++;
    }
    while(leftCapturePromotions != 0) {
        int index = popLSB(leftCapturePromotions);
        int startSquare = index + (colorToMove == 0 ? 9 : -7);
        for(int type = Knight; type < King; type++) {
            moves[totalMoves] = Move(startSquare, index, promotions[type-1]);
            totalMoves++;
        }
    }
    while(rightCapturePromotions != 0) {
        int index = popLSB(rightCapturePromotions);
        int startSquare = index + (colorToMove == 0 ? 7 : -9);
        for(int type = Knight; type < King; type++) {
            moves[totalMoves] = Move(startSquare, index, promotions[type-1]);
            totalMoves++;
        }
    }
    return totalMoves;
}

int Board::getMovesQSearch(std::array<Move, 256> &moves) {
    const uint64_t occupiedBitboard = getOccupiedBitboard();
    int totalMoves = 0;
    uint64_t mask = coloredBitboards[colorToMove] ^ getColoredPieceBitboard(colorToMove, Pawn);
    // the rest of the pieces
    while(mask != 0) {
        const uint8_t startSquare = popLSB(mask);
        const uint8_t currentType = getType(pieceAtIndex(startSquare));
        uint64_t total = 0;
        if(currentType == Knight) {
            total = getKnightAttacks(startSquare);
        } else if(currentType == Bishop) {
            total = getBishopAttacks(startSquare, occupiedBitboard);
        } else if(currentType == Rook) {
            total = getRookAttacks(startSquare, occupiedBitboard);
        } else if(currentType == Queen) {
            total = getRookAttacks(startSquare, occupiedBitboard) | getBishopAttacks(startSquare, occupiedBitboard);
        } else if(currentType == King) {
            total = getKingAttacks(startSquare);
        }
        // Only captures or (not done yet) checks
        total &= coloredBitboards[1 - colorToMove]; 
        while(total != 0) {
            moves[totalMoves] = Move(startSquare, popLSB(total), Normal);
            totalMoves++;
        }
    }
    // pawn captures
    const uint64_t pawnBitboard = getColoredPieceBitboard(colorToMove, Pawn);
    uint64_t capturable = coloredBitboards[1 - colorToMove];
    if(enPassantIndex != 64) {
        capturable |= (1ULL << enPassantIndex);
    }

    uint64_t leftCaptures = (colorToMove == 0 ? pawnBitboard >> 9 : pawnBitboard << 7);
    leftCaptures &= ~getFileMask(7);
    leftCaptures &= capturable;
    uint64_t leftCapturePromotions = leftCaptures & getRankMask(7 * colorToMove);
    leftCaptures ^= leftCapturePromotions;

    uint64_t rightCaptures = (colorToMove == 0 ? pawnBitboard >> 7 : pawnBitboard << 9);
    rightCaptures &= ~getFileMask(0);
    rightCaptures &= capturable;
    uint64_t rightCapturePromotions = rightCaptures & getRankMask(7 * colorToMove);
    rightCaptures ^= rightCapturePromotions;

    while(leftCaptures != 0) {
        int index = popLSB(leftCaptures);
        int startSquare = index + (colorToMove == 0 ? 9 : -7);
        moves[totalMoves] = Move(startSquare, index, (index == enPassantIndex ? EnPassant : Normal));
        totalMoves++;
    }
    while(rightCaptures != 0) {
        int index = popLSB(rightCaptures);
        int startSquare = index + (colorToMove == 0 ? 7 : -9);
        moves[totalMoves] = Move(startSquare, index, (index == enPassantIndex ? EnPassant : Normal));
        totalMoves++;
    }
    while(leftCapturePromotions != 0) {
        int index = popLSB(leftCapturePromotions);
        int startSquare = index + (colorToMove == 0 ? 9 : -7);
        for(int type = Knight; type < King; type++) {
            moves[totalMoves] = Move(startSquare, index, promotions[type-1]);
            totalMoves++;
        }
    }
    while(rightCapturePromotions != 0) {
        int index = popLSB(rightCapturePromotions);
        int startSquare = index + (colorToMove == 0 ? 7 : -9);
        for(int type = Knight; type < King; type++) {
            moves[totalMoves] = Move(startSquare, index, promotions[type-1]);
            totalMoves++;
        }
    }
    return totalMoves;
}

uint8_t Board::getColorToMove() {
    return colorToMove;
}

bool Board::isInCheck() {
    return squareIsUnderAttack(kingSquares[colorToMove]);
}

// thanks ciekce, shoutout stormphrax
bool Board::squareIsUnderAttack(int square) {
    const auto opponent = 1 - colorToMove;

    const auto queens = getColoredPieceBitboard(opponent, Queen);

    uint64_t mask = (getRookAttacks(square, getOccupiedBitboard()) & (queens | getColoredPieceBitboard(opponent, Rook)));
    if (mask != 0)
        return true;

    mask = (getBishopAttacks(square, getOccupiedBitboard()) & (queens | getColoredPieceBitboard(opponent, Bishop)));
    if (mask != 0)
        return true;

    mask = getKnightAttacks(square) & getColoredPieceBitboard(opponent, Knight);
    if (mask != 0)
        return true;

    mask = getPawnAttacks(square, colorToMove) & getColoredPieceBitboard(opponent, Pawn);
    if (mask != 0)
        return true;

    mask = getKingAttacks(square) & getColoredPieceBitboard(opponent, King);
    if (mask != 0)
        return true;

    return false;
}

bool Board::makeMove(Move move) {
    stateHistory.push_back(generateBoardState());
    if(colorToMove != 1) fiftyMoveCounter++;
    hundredPlyCounter++;
    int movedPiece = pieceAtIndex(move.getStartSquare());
    assert(movedPiece != None);
    if(pieceAtIndex(move.getEndSquare()) != None || getType(movedPiece) == Pawn) {
        hundredPlyCounter = 0;
        fiftyMoveCounter = 0;
    }
    movePiece(move.getStartSquare(), movedPiece, move.getEndSquare(), pieceAtIndex(move.getEndSquare()));
    enPassantIndex = 64;
    if(move.getFlag() == DoublePawnPush) enPassantIndex = move.getEndSquare() + directionalOffsets[colorToMove];
    if(getType(movedPiece) == King) {
        kingSquares[colorToMove] = move.getEndSquare();
    }
    if(castlingRights != 0) {
        if(getType(movedPiece) == King) {
            castlingRights ^= ((colorToMove == 1 ? 1 : 4) & castlingRights);
            castlingRights ^= ((colorToMove == 1 ? 2 : 8) & castlingRights);
        }
        if(getType(movedPiece) == Rook) {
            switch (move.getStartSquare()) {
                case 7:
                    if((castlingRights & 1) != 0) castlingRights ^= 1;
                    break;
                case 0:
                    if((castlingRights & 2) != 0) castlingRights ^= 2;
                    break;
                case 63:
                    if((castlingRights & 4) != 0) castlingRights ^= 4;
                    break;
                case 56:
                    if((castlingRights & 8) != 0) castlingRights ^= 8;
                    break;
            }
        }
    }
    if(move.getFlag() == castling[0]) {
        assert(pieceAtIndex(7) != None);
        movePiece(7, pieceAtIndex(7), 5, None);
        castlingRights ^= (1 & castlingRights);
        castlingRights ^= (2 & castlingRights);
    } else if(move.getFlag() == castling[1]) {
        assert(pieceAtIndex(0) != None);
        movePiece(0, pieceAtIndex(0), 3, None);
        castlingRights ^= (1 & castlingRights);
        castlingRights ^= (2 & castlingRights);
    } else if(move.getFlag() == castling[2]) {
        assert(pieceAtIndex(63) != None);
        movePiece(63, pieceAtIndex(63), 61, None);
        castlingRights ^= (4 & castlingRights);
        castlingRights ^= (8 & castlingRights);
    } else if(move.getFlag() == castling[3]) {
        assert(pieceAtIndex(56) != None);
        movePiece(56, pieceAtIndex(56), 59, None);
        castlingRights ^= (4 & castlingRights);
        castlingRights ^= (8 & castlingRights);
    } else if(move.getFlag() == promotions[0]) {
        removePiece(move.getEndSquare(), movedPiece);
        addPiece(move.getEndSquare(), Knight | (colorToMove == 1 ? White : Black));
    } else if(move.getFlag() == promotions[1]) {
        removePiece(move.getEndSquare(), movedPiece);
        addPiece(move.getEndSquare(), Bishop | (colorToMove == 1 ? White : Black));
    } else if(move.getFlag() == promotions[2]) {
        removePiece(move.getEndSquare(), movedPiece);
        addPiece(move.getEndSquare(), Rook | (colorToMove == 1 ? White : Black));
    } else if(move.getFlag() == promotions[3]) {
        removePiece(move.getEndSquare(), movedPiece);
        addPiece(move.getEndSquare(), Queen | (colorToMove == 1 ? White : Black));
    } else if(move.getFlag() == EnPassant) {
        assert(pieceAtIndex(move.getEndSquare() + directionalOffsets[colorToMove]) != None);
        removePiece(move.getEndSquare() + directionalOffsets[colorToMove], pieceAtIndex(move.getEndSquare() + directionalOffsets[colorToMove]));
    }
    plyCount++;
    isRepeated = isRepeatedPosition();
    zobristHistory.push_back(zobristHash);
    if(isInCheck()) {
        undoMove();
        colorToMove = 1 - colorToMove;
        //std::cout << "Changing Color To Move, move was illegal\n";
        return false;
    } else {
        colorToMove = 1 - colorToMove;
        //std::cout << "Changing Color To Move, move was legal\n";
        zobristHash ^= zobColorToMove;
        return true;
    }
}

void Board::undoMove() {
    loadBoardState(stateHistory.back());
    plyCount--;
    stateHistory.pop_back();  
    zobristHistory.pop_back();
    colorToMove = 1 - colorToMove;
    //std::cout << "Changing Color To Move in undo move\n";
    // no zobrist update here because the saved zobrist hash is before the color changed
}

void Board::loadBoardState(BoardState state) {
    std::copy(state.coloredBitboards.begin(), state.coloredBitboards.end(), coloredBitboards.begin());
    std::copy(state.pieceBitboards.begin(), state.pieceBitboards.end(), pieceBitboards.begin());
    std::copy(state.kingSquares.begin(), state.kingSquares.end(), kingSquares.begin());
    enPassantIndex = state.enPassantIndex;
    fiftyMoveCounter = state.fiftyMoveCounter;
    castlingRights = state.castlingRights;
    zobristHash = state.zobristHash;
    mgEval = state.mgEval;
    egEval = state.egEval;
    phase = state.phase;
    isRepeated = state.isRepeated;
    hundredPlyCounter = state.hundredPlyCounter;
}

BoardState Board::generateBoardState() {
    BoardState state;
    std::copy(coloredBitboards.begin(), coloredBitboards.end(), state.coloredBitboards.begin());
    std::copy(pieceBitboards.begin(), pieceBitboards.end(), state.pieceBitboards.begin());
    std::copy(kingSquares.begin(), kingSquares.end(), state.kingSquares.begin());
    state.enPassantIndex = enPassantIndex;
    state.fiftyMoveCounter = fiftyMoveCounter;
    state.castlingRights = castlingRights;
    state.zobristHash = zobristHash;
    state.mgEval = mgEval;
    state.egEval = egEval;
    state.phase = phase;
    state.isRepeated = isRepeated;
    state.hundredPlyCounter = hundredPlyCounter;
    return state;
}

uint64_t Board::getCurrentPlayerBitboard() const {
    return coloredBitboards[colorToMove];
}

void Board::changeColor() {
    stateHistory.push_back(generateBoardState());
    enPassantIndex = 0;
    colorToMove = 1 - colorToMove;
    zobristHash ^= zobColorToMove;
    isRepeated = isRepeatedPosition();
    zobristHistory.push_back(zobristHash);
}

void Board::undoChangeColor() {
    enPassantIndex = stateHistory.back().enPassantIndex;
    isRepeated = stateHistory.back().isRepeated;
    stateHistory.pop_back();
    zobristHistory.pop_back();
    colorToMove = 1 - colorToMove;
    zobristHash ^= zobColorToMove;
}

int Board::getEvaluation() {   
    // currently disabled passed pawn bonuses, as it wasn't gaining any elo
    int egPhase = 24 - phase;
    return ((mgEval * phase + egEval * egPhase) / 24) * ((2 * colorToMove) - 1);
    //return (((mgEval * phase + egEval * egPhase) / 24) + getPassedPawnBonuses()) * ((2 * colorToMove) - 1);
}

int Board::getCastlingRights() const {
    return castlingRights;
}

int Board::getEnPassantIndex() const {
    return enPassantIndex;
}

void initializeZobrist() {
    // random number stuff
    //std::random_device rd;
    std::mt19937_64 gen(0xABBABA5ED);
    std::uniform_int_distribution<uint64_t> dis;
    // color to move
    zobColorToMove = dis(gen);
    // squares
    for(int i = 0; i < 64; i++) {
        for(int j = 0; j < 14; j++) {
            zobTable[i][j] = dis(gen);
        }
    }
}

int Board::taperValue(int mg, int eg) {
    int egPhase = 24 - phase;
    return ((mg * phase + eg * egPhase) / 24);
}

int Board::fullEvalRegen() {
    uint64_t mask = getOccupiedBitboard();
    int midgame = 0;
    int endgame = 0;
    int currentPhase = 0;
    while(mask != 0) {
        int index = popLSB(mask);
        int piece = pieceAtIndex(index);
        int pieceType = getType(piece);
        int pieceColor = getColor(piece);
        midgame += mgTables[pieceType][pieceColor == 1 ? flipIndex(index) : index] * ((2 * pieceColor) - 1);
        endgame += egTables[pieceType][pieceColor == 1 ? flipIndex(index) : index] * ((2 * pieceColor) - 1);
        currentPhase += phaseIncrements[pieceType];
    }
    int egPhase = 24 - currentPhase;
    return ((midgame * currentPhase + endgame * egPhase) / 24) * ((2 * colorToMove) - 1);
}

uint64_t Board::fullZobristRegen() {
    //std::cout << "Beginning Full Regen\n";
    uint64_t mask = getOccupiedBitboard();
    uint64_t result = 0;
    while(mask != 0) {
        int index = popLSB(mask);
        int piece = pieceAtIndex(index);
        //std::cout << "Adding piece of type " << std::to_string(piece) << " at index " << std::to_string(index) << '\n';
        result ^= zobTable[index][piece];
    }
    if(colorToMove == 1) {
        //std::cout << "Changing Color To Move in full regen\n";
        result ^= zobColorToMove;
    }
    //std::cout << "Result: " << std::to_string(result) << '\n';
    return result;
}

int Board::getPassedPawnBonuses() {
    int mgBonuses = 0;
    int egBonuses = 0;
    uint64_t mask = pieceBitboards[Pawn];
    uint64_t allPawns = mask;
    while(mask != 0) {
        int index = popLSB(mask);
        int color = colorAtIndex(index);
        assert(color != 2);
        
        if((getPassedPawnMask(index, color) & allPawns) == 0) {
            int square = color == 1 ? flipIndex(index) : index;
            int colorMultiplier = 2 * color - 1;
            mgBonuses += mgPassedPawnBonusTable[square] * colorMultiplier;
            egBonuses += egPassedPawnBonusTable[square] * colorMultiplier;
        }
    }

    return taperValue(mgBonuses, egBonuses);
}

bool Board::isRepeatedPosition() {
    for(int i = std::ssize(zobristHistory) - 2; i >= std::ssize(zobristHistory) - hundredPlyCounter; i--) {
        if(zobristHistory[i] == zobristHash) {
            return true;
        }
    }
    return false;
}