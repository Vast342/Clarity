/*
    Clarity
    Copyright (C) 2025 Joseph Pasfield

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#include "globals.h"
#include "corrhist.h"
#include <cstdlib>
#include <stdatomic.h>

#include "rays.h"

template void Board::makeMove<false>(Move move);
template void Board::undoMove<false>();
template void Board::makeMove<true>(Move move);
template void Board::undoMove<true>();
template void Board::addPiece<false>(int square, int type);
template void Board::removePiece<false>(int square, int type);
template void Board::movePiece<false>(int square1, int type1, int square2, int type2);
template void Board::addPiece<true>(int square, int type);
template void Board::removePiece<true>(int square, int type);
template void Board::movePiece<true>(int square1, int type1, int square2, int type2);

// zobrist hashing values 
std::array<std::array<uint64_t, 14>, 64> zobTable;
// if black is to move this value is xor'ed
uint64_t zobColorToMove;

// masks for castling rights, used to update the castling rights faster after a rook or king move
constexpr std::array<uint8_t, 64> rookRightMasks = {
0b1101,255,255,255,255,255,255,0b1110,
255,   255,255,255,255,255,255,   255,
255,   255,255,255,255,255,255,   255,
255,   255,255,255,255,255,255,   255,
255,   255,255,255,255,255,255,   255,
255,   255,255,255,255,255,255,   255,
255,   255,255,255,255,255,255,   255,
0b0111,255,255,255,255,255,255,0b1011
};

constexpr std::array<uint8_t, 2> kingRightMasks = {
	0b0011, 0b1100
};

std::array<int, 8> directionalOffsets = {8, -8, 1, -1, 7, -7, 9, -9};

void Board::toString() {
    for(int rank = 7; rank >= 0; rank--) {
        for(int file = 0; file < 8; file++) {
            int piece = pieceAtIndex(rank * 8 + file);
            bool isBlack = getColor(piece) == 0;
            int pieceType = getType(piece);
            char pieceChar = ' ';
            switch(pieceType)
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
    std::cout << "Hash: " << std::to_string(stateHistory.back().zobristHash) << '\n';
    std::cout << "Castling rights: " << std::to_string(stateHistory.back().castlingRights & 1) << std::to_string((stateHistory.back().castlingRights >> 1) & 1) << std::to_string((stateHistory.back().castlingRights >> 2) & 1) << std::to_string((stateHistory.back().castlingRights >> 3) & 1) << '\n';
    if(stateHistory.back().enPassantIndex == 64) {
        std::cout << "En passant square: none\n";
    } else {
        std::cout << "En passant square: " << squareNames[stateHistory.back().enPassantIndex] << '\n';
    }
    std::cout << "Color to move: " << (colorToMove == 0 ? "black" : "white") << '\n';
    std::cout << "Evaluation: " << std::to_string(getEvaluation()) << '\n';
}

Board::Board(std::string fen) {
    stateHistory.clear();
    stateHistory.reserve(256);
    stateHistory.push_back(BoardState());
    //nnueState.reset();
    stateHistory.back().zobristHash = 0;
	stateHistory.back().pawnHash = 0;
    stateHistory.back().nonPawnHashes[0] = 0;
    stateHistory.back().nonPawnHashes[1] = 0;
    stateHistory.back().majorHash = 0;
    stateHistory.back().minorHash = 0;
    for(int i = 0; i < 64; i++) {
        stateHistory.back().mailbox[i] = None;
    }
    for(int i = 0; i < 6; i++) {
        stateHistory.back().pieceBitboards[i] = 0ULL;
    }
    for(int i = 0; i < 2; i++) {
        stateHistory.back().coloredBitboards[i] = 0ULL;
    }
    // main board state, segment 1
    std::vector<std::string> segments = split(fen, ' ');
    std::vector<std::string> ranks = split(segments[0], '/');
    std::ranges::reverse(ranks);
    int i = 0;
    for(const auto& rank : ranks) {
        for(char c : rank) {
            switch(c) {
                case 'p':
                    addPiece<false>(i, Pawn | Black);
                    i++;
                    break;
                case 'P':
                    addPiece<false>(i, Pawn | White);
                    i++;
                    break;
                case 'n':
                    addPiece<false>(i, Knight | Black);
                    i++;
                    break;
                case 'N':
                    addPiece<false>(i, Knight | White);
                    i++;
                    break;
                case 'b':
                    addPiece<false>(i, Bishop | Black);
                    i++;
                    break;
                case 'B':
                    addPiece<false>(i, Bishop | White);
                    i++;
                    break;
                case 'r':
                    addPiece<false>(i, Rook | Black);
                    i++;
                    break;
                case 'R':
                    addPiece<false>(i, Rook | White);
                    i++;
                    break;
                case 'q':
                    addPiece<false>(i, Queen | Black);
                    i++;
                    break;
                case 'Q':
                    addPiece<false>(i, Queen | White);
                    i++;
                    break;
                case 'k':
                    addPiece<false>(i, King | Black);
                    stateHistory.back().kingSquares[0] = i;
                    i++;
                    break;
                case 'K':
                    addPiece<false>(i, King | White);
                    stateHistory.back().kingSquares[1] = i;
                    i++;
                    break;
                default:
                    i += (c - '0');
                    break;
            }
        }
    }
    // convert color to move into 0 or 1, segment 2
    colorToMove = (segments[1] == "w" ? 1 : 0);
    if(colorToMove == 1) stateHistory.back().zobristHash ^= zobColorToMove;
    // decode the castling rights into the 4 bit number, segment 3
    stateHistory.back().castlingRights = 0;
    for(char c : segments[2]) {
        if(c == 'K') {
            stateHistory.back().castlingRights |= 1;
        } else if(c == 'Q') {
            stateHistory.back().castlingRights |= 2;
        } else if(c == 'k') {
            stateHistory.back().castlingRights |= 4;
        } else if(c == 'q') {
            stateHistory.back().castlingRights |= 8;
        }
    }
    // decode the en passant index, segment 4
    if(segments[3] != "-") {
        int i = 0;
        int num = 0;
        for(char c : segments[3]) {
            if(i == 0) {
                num += (c - 'a');
            } else {
                num += ((c - '1') * 8);
            }
            i++;
        }
        stateHistory.back().enPassantIndex = num;
    } else {
        stateHistory.back().enPassantIndex = 64;
    }
    // 50 move counter, segment 5
    stateHistory.back().hundredPlyCounter = 0;
    // ply count, segment 6
    plyCount = std::stoi(segments[5]) * 2 - colorToMove;
    //nnueState.refreshAccumulator(0, stateHistory.back(), stateHistory.back().kingSquares[0]);
    //.refreshAccumulator(1, stateHistory.back(), stateHistory.back().kingSquares[1]);
    stateHistory.back().threats = calculateThreats();
    updatePinsAndCheckers();
}

std::string Board::getFenString() {
    // essentially copy and pasted from my c# engine lol
    std::string fen = "";
    for(int rank = 7; rank >= 0; rank--) {
        int numEmptyFiles = 0;
        for(int file = 0; file < 8; file++) {
            int piece = pieceAtIndex(8*rank+file);
            if(piece != None) {
                if(numEmptyFiles != 0) {
                    fen += std::to_string(numEmptyFiles);
                    numEmptyFiles = 0;
                }
                bool isBlack = getColor(piece) == 0;
                int pieceType = getType(piece);
                char pieceChar = ' ';
                switch(pieceType)
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
        if(numEmptyFiles != 0) {
            fen += std::to_string(numEmptyFiles);
        }
        if(rank != 0) {
            fen += '/';
        }
    }

    // color to move
    fen += ' ';
    fen += (colorToMove == 0 ? 'b' : 'w');
    // castling rights
    fen += ' ';
    bool thingAdded = false;
    if((stateHistory.back().castlingRights & 1) != 0) {
        fen += 'K'; 
        thingAdded = true;
    }
    if((stateHistory.back().castlingRights & 2) != 0) {
        fen += 'Q'; 
        thingAdded = true;
    }
    if((stateHistory.back().castlingRights & 4) != 0) {
        fen += 'k'; 
        thingAdded = true;
    }
    if((stateHistory.back().castlingRights & 8) != 0) {
        fen += 'q'; 
        thingAdded = true;
    }
    if(thingAdded == false) fen += '-';
    // en passant square
    fen += ' ';
    if(stateHistory.back().enPassantIndex == 64) {
        fen += '-';
    } else {
        fen += squareNames[stateHistory.back().enPassantIndex];
    }
    // 50 move counter
    fen += ' ';
    fen += std::to_string(stateHistory.back().hundredPlyCounter / 2);
    // ply count
    fen += ' ';
    fen += std::to_string(plyCount / 2 + colorToMove);
    return fen;
}

template <bool UpdateNNUE> void Board::addPiece(int square, int type) {
    assert(type != None);
    assert(square < 64);
    assert(pieceAtIndex(square) == None);
    assert(square >= 0);
    const uint64_t bitboardSquare = squareToBitboard[square];
    //std::cout << "Adding piece of type " << std::to_string(type) << " at index " << std::to_string(square) << '\n';
    stateHistory.back().coloredBitboards[getColor(type)] ^= bitboardSquare;
    stateHistory.back().pieceBitboards[getType(type)] ^= bitboardSquare;
    stateHistory.back().mailbox[square] = type;
    assert(pieceAtIndex(square) == type);
    //if constexpr(UpdateNNUE) nnueState.activateFeature(square, type, stateHistory.back().kingSquares[0], stateHistory.back().kingSquares[1]);
    stateHistory.back().zobristHash ^= zobTable[square][type];
    if(getType(type) == Pawn) {
        stateHistory.back().pawnHash ^= zobTable[square][type];
    } else {
        stateHistory.back().nonPawnHashes[getColor(type)] ^= zobTable[square][type];
        if(getType(type) == Queen || getType(type) == Rook) {
            stateHistory.back().majorHash ^= zobTable[square][type];
        } else if(getType(type) != King) {
            stateHistory.back().minorHash ^= zobTable[square][type];
        }
    }
}

template <bool UpdateNNUE> void Board::removePiece(int square, int type) {
    assert(type != None);
    assert(square < 64);
    assert(pieceAtIndex(square) == type);
    assert(square >= 0);
    const uint64_t bitboardSquare = squareToBitboard[square];
    stateHistory.back().coloredBitboards[getColor(type)] ^= bitboardSquare;
    stateHistory.back().pieceBitboards[getType(type)] ^= bitboardSquare;
    stateHistory.back().mailbox[square] = None;
    //if constexpr(UpdateNNUE) nnueState.disableFeature(square, type, stateHistory.back().kingSquares[0], stateHistory.back().kingSquares[1]);
    stateHistory.back().zobristHash ^= zobTable[square][type];
    if(getType(type) == Pawn) {
        stateHistory.back().pawnHash ^= zobTable[square][type];
    } else {
        stateHistory.back().nonPawnHashes[getColor(type)] ^= zobTable[square][type];
        if(getType(type) == Queen || getType(type) == Rook) {
            stateHistory.back().majorHash ^= zobTable[square][type];
        } else if(getType(type) != King) {
            stateHistory.back().minorHash ^= zobTable[square][type];
        }
    }
    assert(pieceAtIndex(square) == None);
}

template <bool UpdateNNUE> void Board::movePiece(int square1, int type1, int square2, int type2) {
    assert(type1 != None);
    assert(square1 < 64);
    assert(square2 < 64);
    if(type2 != None) removePiece<UpdateNNUE>(square2, type2);
    addPiece<UpdateNNUE>(square2, type1);
    removePiece<UpdateNNUE>(square1, type1);
}

int Board::pieceAtIndex(int index) const {
    return stateHistory.back().mailbox[index];
}
int Board::colorAtIndex(int index) const {
    return stateHistory.back().mailbox[index] == None ? 2 : getColor(stateHistory.back().mailbox[index]);
}

uint64_t Board::getColoredPieceBitboard(int color, int piece) const {
    return stateHistory.back().pieceBitboards[piece] & stateHistory.back().coloredBitboards[color];
}

uint64_t Board::getOccupiedBitboard() const {
    return stateHistory.back().coloredBitboards[0] | stateHistory.back().coloredBitboards[1];
}

// fills up the array and then returns the number of moves to be looped through later.
int Board::getMoves(std::array<Move, 256> &moves) const {
    uint64_t occupiedBitboard = getOccupiedBitboard();
    int totalMoves = 0;
    // castling
    if((stateHistory.back().castlingRights & kingRightMasks[1 - colorToMove]) != 0) {
        if(!isInCheck()) {
            if(colorToMove == 1) {
                if((stateHistory.back().castlingRights & 1) != 0 && (occupiedBitboard & 0x60) == 0 && !squareIsUnderAttack(5)) {
                    moves[totalMoves] = Move(4, 6, castling[0]);
                    totalMoves++;
                }
                if((stateHistory.back().castlingRights & 2) != 0 && (occupiedBitboard & 0xE) == 0 && !squareIsUnderAttack(3)) {
                    moves[totalMoves] = Move(4, 2, castling[1]);
                    totalMoves++;
                }
            } else {
                if((stateHistory.back().castlingRights & 4) != 0 && (occupiedBitboard & 0x6000000000000000) == 0 && !squareIsUnderAttack(61)) {
                    moves[totalMoves] = Move(60, 62, castling[2]);
                    totalMoves++;
                }
                if((stateHistory.back().castlingRights & 8) != 0 && (occupiedBitboard & 0xE00000000000000) == 0 && !squareIsUnderAttack(59)) {
                    moves[totalMoves] = Move(60, 58, castling[3]);
                    totalMoves++; 
                }
            }
        }
    }
    uint64_t mask = stateHistory.back().coloredBitboards[colorToMove] ^ getColoredPieceBitboard(colorToMove, Pawn);
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
        total ^= (total & stateHistory.back().coloredBitboards[colorToMove]); 
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
    uint64_t capturable = stateHistory.back().coloredBitboards[1 - colorToMove];
    if(stateHistory.back().enPassantIndex != 64) {
        capturable |= squareToBitboard[stateHistory.back().enPassantIndex];
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
        moves[totalMoves] = Move(startSquare, index, (index == stateHistory.back().enPassantIndex ? EnPassant : Normal));
        totalMoves++;
    }
    while(rightCaptures != 0) {
        int index = popLSB(rightCaptures);
        int startSquare = index + (colorToMove == 0 ? 7 : -9);
        moves[totalMoves] = Move(startSquare, index, (index == stateHistory.back().enPassantIndex ? EnPassant : Normal));
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

int Board::getMovesQSearch(std::array<Move, 256> &moves) const {
    const uint64_t occupiedBitboard = getOccupiedBitboard();
    int totalMoves = 0;
    uint64_t mask = stateHistory.back().coloredBitboards[colorToMove] ^ getColoredPieceBitboard(colorToMove, Pawn);
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
        total &= stateHistory.back().coloredBitboards[1 - colorToMove]; 
        while(total != 0) {
            moves[totalMoves] = Move(startSquare, popLSB(total), Normal);
            totalMoves++;
        }
    }
    // pawn push promotions
    const uint64_t pawnBitboard = getColoredPieceBitboard(colorToMove, Pawn);
    const uint64_t emptyBitboard = ~occupiedBitboard;
    const uint64_t pawnPushes = getPawnPushes(pawnBitboard, emptyBitboard, colorToMove);
    uint64_t pawnPushPromotions = pawnPushes & getRankMask(7 * colorToMove);
    while(pawnPushPromotions != 0) {
        const uint8_t index = popLSB(pawnPushPromotions);
        const uint8_t startSquare = (index + directionalOffsets[colorToMove]);
        for(int type = Knight; type < King; type++) {
            moves[totalMoves] = Move(startSquare, index, promotions[type-1]);
            totalMoves++;
        }
    }

    // captures
    uint64_t capturable = stateHistory.back().coloredBitboards[1 - colorToMove];
    if(stateHistory.back().enPassantIndex != 64) {
        capturable |= squareToBitboard[stateHistory.back().enPassantIndex];
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
        moves[totalMoves] = Move(startSquare, index, (index == stateHistory.back().enPassantIndex ? EnPassant : Normal));
        totalMoves++;
    }
    while(rightCaptures != 0) {
        int index = popLSB(rightCaptures);
        int startSquare = index + (colorToMove == 0 ? 7 : -9);
        moves[totalMoves] = Move(startSquare, index, (index == stateHistory.back().enPassantIndex ? EnPassant : Normal));
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

uint8_t Board::getColorToMove() const {
    return colorToMove;
}

bool Board::isInCheck() const {
    return stateHistory.back().checkers;
}

// thanks ciekce, shoutout stormphrax
bool Board::squareIsUnderAttack(int square) const {
    if(stateHistory.back().threats != 0) {
        return (stateHistory.back().threats & (1ULL << square));
    } else {
        const auto opponent = 1 - colorToMove;

        const auto queens = getColoredPieceBitboard(opponent, Queen);

        uint64_t mask = (getRookAttacks(square, getOccupiedBitboard()) & (queens | getColoredPieceBitboard(opponent, Rook)));
        if(mask != 0)
            return true;

        mask = (getBishopAttacks(square, getOccupiedBitboard()) & (queens | getColoredPieceBitboard(opponent, Bishop)));
        if(mask != 0)
            return true;

        mask = getKnightAttacks(square) & getColoredPieceBitboard(opponent, Knight);
        if(mask != 0)
            return true;

        mask = getPawnAttacks(square, colorToMove) & getColoredPieceBitboard(opponent, Pawn);
        if(mask != 0)
            return true;

        mask = getKingAttacks(square) & getColoredPieceBitboard(opponent, King);
        if(mask != 0)
            return true;

        return false;
    }
}

template <bool PushNNUE> void Board::makeMove(Move move) {
    //std::cout << "move " << toLongAlgebraic(move) << " on position " << getFenString() << std::endl;
    //std::cout << "makemove " << toLongAlgebraic(move) << std::endl;
    // push to vectors
    stateHistory.push_back(stateHistory.back());
    NetworkUpdates updates;
    stateHistory.back().threats = 0;

    // get information
    int start = move.getStartSquare();
    int end = move.getEndSquare();
    int movedPiece = pieceAtIndex(move.getStartSquare());
    int victim = pieceAtIndex(move.getEndSquare());
    int flag = move.getFlag();
    assert(movedPiece != None);
    bool isCapture = victim != None;
    int movedPieceType = getType(movedPiece);
    
    // hundred Ply (fifty move) counter
    stateHistory.back().hundredPlyCounter++;
    if(isCapture || movedPieceType == Pawn) {
        stateHistory.back().hundredPlyCounter = 0;
    }

    // king square updates
    if(movedPieceType == King) {
        if(refreshRequired(colorToMove, start, end)) updates.pushBucket(end, colorToMove);
        stateHistory.back().kingSquares[colorToMove] = end;
    }

    // actually make the move
    if(isCapture) {
        removePiece<false>(end, victim);
        updates.pushSub(end, victim);
    }
    removePiece<false>(start, movedPiece);
    updates.pushSub(start, movedPiece);
    if(flag < promotions[0]) {
        addPiece<false>(end, movedPiece);
        updates.pushAdd(end, movedPiece);
    }

    // En Passant
    stateHistory.back().enPassantIndex = 64;
    
    // castling rights updates!
    if((stateHistory.back().castlingRights & kingRightMasks[1 - colorToMove]) != 0) {
        switch(movedPieceType) {
            case Rook:
                stateHistory.back().castlingRights &= rookRightMasks[start];
                break;
            case King:
                stateHistory.back().castlingRights &= kingRightMasks[colorToMove];
                break;
            default:
                break;
        }
    }
    if(getType(victim) == Rook) {
        switch(end) {
            case 7:
                stateHistory.back().castlingRights &= rookRightMasks[end];
                break;
            case 0:
                stateHistory.back().castlingRights &= rookRightMasks[end];
                break;
            case 63:
                stateHistory.back().castlingRights &= rookRightMasks[end];
                break;
            case 56:
                stateHistory.back().castlingRights &= rookRightMasks[end];
                break;
            default:
                break;
        }
    }

    // edge cases!
    switch(flag) {
        // castling cases
        case castling[0]:
            assert(pieceAtIndex(7) != None);
            movePiece<false>(7, Rook | White, 5, None);
            updates.pushSub(7, Rook | White);
            updates.pushAdd(5, Rook | White);
            break;
        case castling[1]:
            assert(pieceAtIndex(0) != None);
            movePiece<false>(0, Rook | White, 3, None);
            updates.pushSub(0, Rook | White);
            updates.pushAdd(3, Rook | White);
            break;
        case castling[2]:
            assert(pieceAtIndex(63) != None);
            movePiece<false>(63, Rook | Black, 61, None);
            updates.pushSub(63, Rook | Black);
            updates.pushAdd(61, Rook | Black);
            break;
        case castling[3]:
            assert(pieceAtIndex(56) != None);
            movePiece<false>(56, Rook | Black, 59, None);
            updates.pushSub(56, Rook | Black);
            updates.pushAdd(59, Rook | Black);
            break;
            // double pawn push
        case DoublePawnPush:
            stateHistory.back().enPassantIndex = end + directionalOffsets[colorToMove];
            break;
            // en passant
        case EnPassant:
            assert(pieceAtIndex(move.getEndSquare() + directionalOffsets[colorToMove]) != None);
            removePiece<false>(end + directionalOffsets[colorToMove], Pawn | (8 * !colorToMove));
            updates.pushSub(end + directionalOffsets[colorToMove], Pawn | (8 * !colorToMove));
            break;
            // promotion cases
        case promotions[0]:
            addPiece<false>(end, Knight | (8 * colorToMove));
            updates.pushAdd(end, Knight | (8 * colorToMove));
            break;
        case promotions[1]:
            addPiece<false>(end, Bishop | (8 * colorToMove));
            updates.pushAdd(end, Bishop | (8 * colorToMove));
            break;
        case promotions[2]:
            addPiece<false>(end, Rook | (8 * colorToMove));
            updates.pushAdd(end, Rook | (8 * colorToMove));
            break;
        case promotions[3]:
            addPiece<false>(end, Queen | (8 * colorToMove));
            updates.pushAdd(end, Queen | (8 * colorToMove));
            break;
        default:
            break;
    }
    plyCount++;
    if constexpr(PushNNUE) {
        //.performUpdatesAndPush(updates, stateHistory.back().kingSquares[0], stateHistory.back().kingSquares[1], stateHistory.back());
    } else {
        //nnueState.performUpdates(updates, stateHistory.back().kingSquares[0], stateHistory.back().kingSquares[1], stateHistory.back());
    }
    colorToMove = 1 - colorToMove;
    stateHistory.back().threats = calculateThreats();
    stateHistory.back().zobristHash ^= zobColorToMove;
    updatePinsAndCheckers();
}
template <bool PushNNUE> void Board::undoMove() {
    //std::cout << "undomove\n";
    stateHistory.pop_back();
    //if constexpr(PushNNUE) nnueState.pop();
    plyCount--;
    colorToMove = 1 - colorToMove;
    //std::cout << "position fen " << getFenString() << std::endl;
    //std::cout << "Changing Color To Move in undo move\n";
    // no zobrist update here because the saved zobrist hash is before the color changed
}

uint64_t Board::getCurrentPlayerBitboard() const {
    return stateHistory.back().coloredBitboards[colorToMove];
}

void Board::changeColor() {
    stateHistory.push_back(stateHistory.back());
    //nnueState.push();
    stateHistory.back().enPassantIndex = 64;
    stateHistory.back().hundredPlyCounter++;
    colorToMove = 1 - colorToMove;
    stateHistory.back().threats = calculateThreats();
    stateHistory.back().zobristHash ^= zobColorToMove;
}

void Board::undoChangeColor() {
    stateHistory.pop_back();
    //nnueState.pop();
    colorToMove = 1 - colorToMove;
}

int Board::getEvaluation() {   
    //std::cout << "position fen " << getFenString() << std::endl;
    //std::cout << "evaluate" << std::endl;
    int eval = int(double(nnueState.evaluate(colorToMove, __builtin_popcountll(getOccupiedBitboard()))));
    //nnueState.fullRefresh(stateHistory.back(), stateHistory.back().kingSquares[0], stateHistory.back().kingSquares[1]);
    //assert(eval == nnueState.evaluate(colorToMove, __builtin_popcountll(getOccupiedBitboard())));
    int phase =  3 * __builtin_popcountll(stateHistory.back().pieceBitboards[Knight])
               + 3 * __builtin_popcountll(stateHistory.back().pieceBitboards[Bishop])
               + 5 * __builtin_popcountll(stateHistory.back().pieceBitboards[Rook])
               + 10 * __builtin_popcountll(stateHistory.back().pieceBitboards[Queen]);    

    eval = eval * (206 + phase) / 256; 

    eval = eval * (200 - stateHistory.back().hundredPlyCounter) / 200;

    return eval;
}

int Board::getCastlingRights() const {
    return stateHistory.back().castlingRights;
}

int Board::getEnPassantIndex() const {
    return stateHistory.back().enPassantIndex;
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

bool Board::isRepeatedPosition() {
    int size = std::ssize(stateHistory);
    for(int i = size - 5; i >= size - stateHistory.back().hundredPlyCounter - 1; i -= 2) {
        if(stateHistory[i].zobristHash == stateHistory.back().zobristHash) {
            return true;
        }
    }
    return false;
}

uint64_t Board::getAttackers(int square) const {
    uint64_t occupied = getOccupiedBitboard();
    return (getPawnAttacks(square, 0) & getColoredPieceBitboard(1, Pawn))
        | (getPawnAttacks(square, 1) & getColoredPieceBitboard(0, Pawn))
        | (getKnightAttacks(square) & stateHistory.back().pieceBitboards[Knight])
        | (getRookAttacks(square, occupied) & (stateHistory.back().pieceBitboards[Rook] | stateHistory.back().pieceBitboards[Queen]))
        | (getBishopAttacks(square, occupied) & (stateHistory.back().pieceBitboards[Bishop] | stateHistory.back().pieceBitboards[Queen]))
        | (getKingAttacks(square) & stateHistory.back().pieceBitboards[King]);
}

uint64_t Board::getColoredBitboard(int color) const {
    return stateHistory.back().coloredBitboards[color];
}
uint64_t Board::getPieceBitboard(int piece) const {
    return stateHistory.back().pieceBitboards[piece];
}

int Board::getFiftyMoveCount() const {
    return stateHistory.back().hundredPlyCounter / 2;
}

uint64_t Board::getZobristHash() const {
    return stateHistory.back().zobristHash;
}

std::array<int, 2> Board::getNonPawnHash() {
    auto hashes = stateHistory.back().nonPawnHashes;
    std::array<int, 2> clipped = {int(hashes[0] & Corrhist::mask), int(hashes[1] & Corrhist::mask)};
    return clipped;
}

BoardState Board::getBoardState() const {
    return stateHistory.back();
}

Board::Board(BoardState s, int ctm) {
    stateHistory.push_back(s);
    colorToMove = ctm;
    stateHistory.back().hundredPlyCounter = 0;
    plyCount = 0;
    stateHistory.reserve(256);
}

// estimates the resulting zobrist key from a move
uint64_t Board::keyAfter(const Move move) const {
    const int startSquare = move.getStartSquare();
    const int endSquare = move.getEndSquare();
    
    const int moving = pieceAtIndex(startSquare);
    const int captured = pieceAtIndex(endSquare);

    uint64_t key = stateHistory.back().zobristHash;

    key ^= zobTable[startSquare][moving];
    key ^= zobTable[endSquare][moving];

    if (captured != None) {
        key ^= zobTable[endSquare][captured];
    }

    key ^= zobColorToMove;

    return key;
}

int Board::getPlyCount() const {
    return plyCount;
}

bool Board::isPKEndgame() const {
    uint64_t occupied = getOccupiedBitboard();
    uint64_t pk = getPieceBitboard(Pawn) | getPieceBitboard(King);
    return ((getColoredBitboard(colorToMove) & (occupied ^ pk)) == 0);
}

int Board::getPawnHashIndex() const {
	// last 15 bits
    return stateHistory.back().pawnHash & Corrhist::mask;
}

uint64_t Board::calculateThreats() {
    const int them = 1 - colorToMove;
    const uint64_t occupied = getOccupiedBitboard();

    uint64_t threats = 0;
    
    const uint64_t queens = getColoredPieceBitboard(them, Queen);

    uint64_t rooks = queens | getColoredPieceBitboard(them, Rook);
    while(rooks) {
        const int rook = popLSB(rooks);
        threats |= getRookAttacks(rook, occupied);
    }

    uint64_t bishops = queens | getColoredPieceBitboard(them, Bishop);
    while(bishops) {
        const int bishop = popLSB(bishops);
        threats |= getBishopAttacks(bishop, occupied);
    }

    uint64_t knights = getColoredPieceBitboard(them, Knight);
    while(knights) {
        const int knight = popLSB(knights);
        threats |= getKnightAttacks(knight);
    }

    const uint64_t pawns = getColoredPieceBitboard(them, Pawn);
    if(them == 0)
        threats |= ((pawns >> 9) & ~getFileMask(7)) | ((pawns >> 7) & ~getFileMask(0));
    else threats |= ((pawns << 7) & ~getFileMask(7)) | ((pawns << 9) & ~getFileMask(0));

    threats |= getKingAttacks(stateHistory.back().kingSquares[them]);

    return threats;
}

uint64_t Board::getThreats() const {
    return stateHistory.back().threats;
}

int Board::getMajorHash() const {
    return stateHistory.back().majorHash & Corrhist::mask;
}

int Board::getMinorHash() const {
    return stateHistory.back().minorHash & Corrhist::mask;
}

bool Board::isLegal(Move move) const {
    const auto from = move.getStartSquare();
    const auto to = move.getEndSquare();
    const auto flag = move.getFlag();

    const auto king = stateHistory.back().kingSquares[colorToMove];

    if(flag >= castling[0] && flag <= castling[3]) {
        const auto fromFile = from % 8;
        const auto toFile = to % 8;
        const auto fromRank = from / 8;
        const auto kingDestination = 8 * fromRank + (fromFile < toFile ? 6 : 2);
        return !(stateHistory.back().threats & (1ULL << kingDestination));
    } else if(flag == EnPassant) {
        auto rank = to / 8;
        const auto file = to % 8;
        rank = rank == 2 ? 3 : 4;
        const auto captureSquare = (8 * rank + file);
        const auto postMoveOcc = getOccupiedBitboard() ^ (1ULL << from) ^ (1ULL << to) ^ (1ULL << captureSquare);
        const auto oppQueens = getColoredPieceBitboard(1 - colorToMove, Queen);

        return ((getBishopAttacks(king, postMoveOcc) & (oppQueens | getColoredPieceBitboard(1 - colorToMove, Bishop))) == 0 &&
            (getRookAttacks(king, postMoveOcc) & (oppQueens | getColoredPieceBitboard(1 - colorToMove, Rook))) == 0);
    }

    const auto piece = pieceAtIndex(from);

    if(getType(piece) == King) {
        const auto kinglessOcc = getOccupiedBitboard() ^ (1ULL << king);
        const auto oppQueens = getColoredPieceBitboard(1 - colorToMove, Queen);

        return ((stateHistory.back().threats & (1ULL << to)) == 0) &&
               ((getBishopAttacks(to, kinglessOcc) & (oppQueens | getColoredPieceBitboard(1 - colorToMove, Bishop))) == 0) &&
               ((getRookAttacks(to, kinglessOcc) & (oppQueens | getColoredPieceBitboard(1 - colorToMove, Rook))) == 0);
    }

    // piece is guaranteed to not be king because prior condition
    if (__builtin_popcountll(stateHistory.back().checkers) >= 2 ||
    (((stateHistory.back().orthogonal_pins | stateHistory.back().diagonal_pins) & (1ULL << from)) &&
     (intersectingRays[from][to] & (1ULL << king)) == 0)) {
        return false;
     }

    if(stateHistory.back().checkers == 0) {
        return true;
    }

    const auto checker = std::countr_zero(stateHistory.back().checkers);
    return (betweenRays[king][checker] | (1ULL << checker)) & (1ULL << to);
}

void Board::updatePinsAndCheckers() {
    // while other engines were playing chess, CLARITY WAS PLAYING CHECKERS
    // (yes I'm bringing that bad joke back)
    stateHistory.back().checkers = getOppAttacks(stateHistory.back().kingSquares[colorToMove]);
    stateHistory.back().diagonal_pins = 0;
    stateHistory.back().orthogonal_pins = 0;

    // info gathering
    const auto opps = 1 - colorToMove;
    const auto us = getColoredBitboard(colorToMove);
    const auto them = getColoredBitboard(opps);
    const auto ourKing = stateHistory.back().kingSquares[colorToMove];

    const auto oppQueens = getColoredPieceBitboard(opps, Queen);
    const auto oppDiago = oppQueens | getColoredPieceBitboard(opps, Bishop);
    const auto oppOrtho = oppQueens | getColoredPieceBitboard(opps, Rook);

    auto potentialDiagoPinners = oppDiago & getBishopAttacks(ourKing, them);
    auto potentialOrthoPinners = oppOrtho & getRookAttacks(ourKing, them);

    while(potentialDiagoPinners != 0) {
        const auto pinner = popLSB(potentialDiagoPinners);
        const auto potentiallyPinned = betweenRays[ourKing][pinner] | (1ULL << pinner);
        if(__builtin_popcountll(potentiallyPinned & us) == 1) {
            stateHistory.back().diagonal_pins |= potentiallyPinned;
        }
    }

    while(potentialOrthoPinners != 0) {
        const auto pinner = popLSB(potentialOrthoPinners);
        const auto potentiallyPinned = betweenRays[ourKing][pinner] | (1ULL << pinner);
        if(__builtin_popcountll(potentiallyPinned & us) == 1) {
            stateHistory.back().orthogonal_pins |= potentiallyPinned;
        }
    }
}

uint64_t Board::getOppAttacks(int square) const {
    const auto opponent = 1 - colorToMove;
    const auto queens = getColoredPieceBitboard(opponent, Queen);

    return (getRookAttacks(square, getOccupiedBitboard()) & (queens | getColoredPieceBitboard(opponent, Rook)))
         | (getBishopAttacks(square, getOccupiedBitboard()) & (queens | getColoredPieceBitboard(opponent, Bishop)))
         | (getKnightAttacks(square) & getColoredPieceBitboard(opponent, Knight))
         | (getPawnAttacks(square, colorToMove) & getColoredPieceBitboard(opponent, Pawn))
         | (getKingAttacks(square) & getColoredPieceBitboard(opponent, King));
}

void Board::printPins() const {
    std::cout << "checkers: " << stateHistory.back().checkers << std::endl;
    std::cout << "ortho: " << stateHistory.back().orthogonal_pins << std::endl;
    std::cout << "diagonal: " << stateHistory.back().diagonal_pins << std::endl;
}

bool Board::isPseudolegal(Move move) const {
    const auto from = move.getStartSquare();
    const auto to = move.getEndSquare();
    const auto flag = move.getFlag();
    const auto piece = pieceAtIndex(from);
    const auto victim = pieceAtIndex(to);

    if(piece == None || getColor(piece) != colorToMove) {
        return false;
    }

    // ignoring another condition that would otherwise go here because my castling moves aren't encoded as king takes rook
    // if capturing own piece or king, that's a bad thing
    if(victim != None && (getColor(victim) == colorToMove || getType(victim) == King)) {
        return false;
    }


    if(flag >= castling[0] && flag <= castling[3]) {
        if(getType(piece) != King || isInCheck()) {
            return false;
        }

        // if the rank is wrong
        const auto home = colorToMove == 0 ? 7 : 0;
        if(from / 8 != home || to / 8 != home) {
            return false;
        }

        // movegen checks
        // Check if any castling rights exist for the current player
        if((stateHistory.back().castlingRights & kingRightMasks[1 - colorToMove]) == 0) {
            return false;
        }

        const auto occ = getOccupiedBitboard();
        // Check specific castling move types
        if(colorToMove == 1) { // White to move
            if(from == 4 && to == 6 && flag == castling[0]) { // White kingside castling
                return (stateHistory.back().castlingRights & 1) != 0 &&
                       (occ & 0x60) == 0 &&
                       !squareIsUnderAttack(5);
            }
            else if(from == 4 && to == 2 && flag == castling[1]) { // White queenside castling
                return (stateHistory.back().castlingRights & 2) != 0 &&
                       (occ & 0xE) == 0 &&
                       !squareIsUnderAttack(3);
            }
        } else { // Black to move
            if(from == 60 && to == 62 && flag == castling[2]) { // Black kingside castling
                return (stateHistory.back().castlingRights & 4) != 0 &&
                       (occ & 0x6000000000000000) == 0 &&
                       !squareIsUnderAttack(61);
            }
            else if(from == 60 && to == 58 && flag == castling[3]) { // Black queenside castling
                return (stateHistory.back().castlingRights & 8) != 0 &&
                       (occ & 0xE00000000000000) == 0 &&
                       !squareIsUnderAttack(59);
            }
        }
        return false;
    }

    if(getType(piece) == Pawn) {
        if(flag == EnPassant) {
            return to == stateHistory.back().enPassantIndex && (getPawnAttacks(from, colorToMove) & (1ULL << to)) != 0;
        }

        const auto fromRank = from / 8;
        const auto toRank = to / 8;
        // moving backwards is bad
        if((colorToMove == 0 && toRank >= fromRank) || (colorToMove == 1 && toRank <= fromRank)) {
            return false;
        }

        const auto promoRank = colorToMove == 0 ? 0 : 7;
        // promotion to wrong rank or nonpromotion on back rank
        if((flag >= promotions[0]) != (toRank == promoRank)) {
            return false;
        }

        const auto fromFile = from % 8;
        const auto toFile = to % 8;
        if(fromFile != toFile) {
            const auto pawnAttacks = getPawnAttacks(from, colorToMove);
            const auto enemyPieces = getColoredBitboard(1 - colorToMove);
            const auto attackedEnemies = pawnAttacks & enemyPieces;

            if (!(attackedEnemies & (1ULL << to))) {
                return false;
            }
        } else if(victim != None) {
            // forward capture
            return false;
        }

        const auto rankDelta = std::abs(toRank - fromRank);
        int maxDelta;
        if(colorToMove == 0) {
            maxDelta = fromRank == 6 ? 2 : 1;
        } else {
            maxDelta = fromRank == 1 ? 2 : 1;
        }

        if(rankDelta > maxDelta) {
            return false;
        }

        // incorrect delta
        if(rankDelta == 2 && flag != DoublePawnPush) {
            return false;
        }

        if(rankDelta == 1 && flag == DoublePawnPush) {
            return false;
        }

        // blocked double pawn push
        if(rankDelta == 2 && (getOccupiedBitboard() & (1ULL << (to + (colorToMove == 1 ? -8 : 8))))) {
            return false;
        }
    } else {
        // can't do ep or promote without a pawn
        if(flag == EnPassant || flag >= promotions[0] || flag == DoublePawnPush) {
            return false;
        }
        auto total = 0ULL;
        const auto occ = getOccupiedBitboard();
        const auto currentType = getType(piece);
        if(currentType == Knight) {
            total = getKnightAttacks(from);
        } else if(currentType == Bishop) {
            total = getBishopAttacks(from, occ);
        } else if(currentType == Rook) {
            total = getRookAttacks(from, occ);
        } else if(currentType == Queen) {
            total = getRookAttacks(from, occ) | getBishopAttacks(from, occ);
        } else if(currentType == King) {
            total = getKingAttacks(from);
        } else {
            __builtin_unreachable();
        }

        if((total & (1ULL << to)) == 0) {
            return false;
        }
    }

    return true;
}