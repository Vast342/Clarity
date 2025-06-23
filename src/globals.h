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
// something something header guards, I don't actually know much about this
#pragma once
// libraries that I think are probably what I need to use
#include <iostream>
#include <cstdint>
#include <cassert>
#include <string_view>
#include <array>
#include <bit>
#include <ranges>
#include <algorithm>
#include <vector>
#include <string>
#include <sstream>
#include <random>
#include <chrono>
#include <fstream>
#include <thread>
#include "nwupdate.h"
#include <memory>
#include <cstring>
#include <atomic>
#include <span>

extern bool useSyzygy;

// declaring things i guess
enum flags {
    Undefined, FailLow, BetaCutoff, Exact
};
constexpr std::array<std::string_view, 64> squareNames = {
    "a1","b1","c1","d1","e1","f1","g1","h1",
    "a2","b2","c2","d2","e2","f2","g2","h2",
    "a3","b3","c3","d3","e3","f3","g3","h3",
    "a4","b4","c4","d4","e4","f4","g4","h4",
    "a5","b5","c5","d5","e5","f5","g5","h5",
    "a6","b6","c6","d6","e6","f6","g6","h6",
    "a7","b7","c7","d7","e7","f7","g7","h7",
    "a8","b8","c8","d8","e8","f8","g8","h8",
};
enum Pieces {
    Pawn, Knight, Bishop, Rook, Queen, King, None
};
constexpr int Black = 0;
constexpr int White = 8;

extern std::array<std::array<uint8_t, 218>, 150> reductions;

// structs and stuff
struct Board;

// holds the state of the board, specifically the factors that can't be brought back after a move is made
struct BoardState {
    std::array<uint64_t, 2> coloredBitboards;
    std::array<uint64_t, 6> pieceBitboards;
    std::array<uint8_t, 64> mailbox;
    uint8_t enPassantIndex;
    std::array<uint8_t, 2> kingSquares;
    uint8_t hundredPlyCounter;
    uint8_t castlingRights;
    uint64_t zobristHash;
    uint64_t pawnHash;
    std::array<uint64_t, 2> nonPawnHashes;
    uint64_t majorHash;
    uint64_t minorHash;
    uint64_t threats;
};

// a single move, stored in 16 bits
struct Move {
    public:
        int getStartSquare() const;
        int getEndSquare() const;
        int getFlag() const;
        Move(int startSquare, int endSquare, int flag);
        Move();
        Move(std::string longAlgebraic, const Board& board);
        Move(int startSquare, int endSquare, int promote, int ep, const Board &board);
        [[nodiscard]] constexpr auto operator==(const Move &other) const -> bool = default;
        [[nodiscard]] explicit constexpr operator bool() const {
            return value != 0;
        }
    private: 
        uint16_t value;
};

#include "eval.h"

// the board itself
struct Board {
    public:
        Board(std::string fen);
        Board(BoardState s, int ctm);
        template <bool PushNNUE> bool makeMove(Move move);
        template <bool PushNNUE> void undoMove();
        int getMoves(std::array<Move, 256> &moves) const;
        int getMovesQSearch(std::array<Move, 256> &moves) const;
        std::string getFenString();
        bool isInCheck() const;
        bool squareIsUnderAttack(int square) const;
        void toString();
        uint8_t getColorToMove() const;
        uint64_t getCurrentPlayerBitboard() const;
        uint64_t getOccupiedBitboard() const;
        uint64_t getColoredPieceBitboard(int color, int piece) const;
        int pieceAtIndex(int index) const;
        int colorAtIndex(int index) const;
        void changeColor();
        void undoChangeColor();
        int getEvaluation();
        int getCastlingRights() const;
        int getEnPassantIndex() const;
        uint64_t fullZobristRegen();
        bool isRepeatedPosition();
        uint64_t getAttackers(int square) const;
        uint64_t getColoredBitboard(int color) const;
        uint64_t getPieceBitboard(int piece) const;
        int getFiftyMoveCount() const;
        uint64_t getZobristHash() const;
        std::array<int, 2> getNonPawnHash();
        int getMajorHash() const;
        int getMinorHash() const;
        BoardState getBoardState() const;
        uint64_t keyAfter(const Move move) const;
        int getPlyCount() const;
        bool isPKEndgame() const;
        int getPawnHashIndex() const;
        uint64_t getThreats() const;
        uint64_t calculateThreats();
        bool isLegal(Move move);
    private:
        int plyCount;
        uint8_t colorToMove;
        std::vector<BoardState> stateHistory;
        NetworkState nnueState;
        template <bool UpdateNNUE> void addPiece(int square, int type);
        template <bool UpdateNNUE> void removePiece(int square, int type);
        template <bool UpdateNNUE> void movePiece(int square1, int type1, int square2, int type2);
};

// the eternal functions, can be used everywhere
[[nodiscard]]int getType(int value);
[[nodiscard]]int getColor(int value);
[[nodiscard]]int popLSB(uint64_t &bitboard);
[[nodiscard]]std::string toLongAlgebraic(Move move);
[[nodiscard]]uint64_t getRankMask(int rank);
[[nodiscard]]uint64_t getFileMask(int file);
[[nodiscard]]uint64_t getRookAttacksOld(int square, uint64_t occupiedBitboard);
[[nodiscard]]uint64_t getBishopAttacksOld(int square, uint64_t occupiedBitboard);
[[nodiscard]]uint64_t getRookAttacks(int square, uint64_t occupiedBitboard);
[[nodiscard]]uint64_t getBishopAttacks(int square, uint64_t occupiedBitboard);
[[nodiscard]]uint64_t getPawnPushes(uint64_t pawnBitboard, uint64_t emptyBitboard, int colorToMove);
[[nodiscard]]uint64_t getDoublePawnPushes(uint64_t pawnAttacks, uint64_t emptyBitboard, int colorToMove);
[[nodiscard]]uint64_t getPawnAttacks(int square, int colorToMove);
[[nodiscard]]uint64_t getKnightAttacks(int square);
[[nodiscard]]uint64_t getKingAttacks(int square);
void initializeZobrist();
void initialize();
std::vector<std::string> split(const std::string &string, const char seperator);
int flipIndex(int index);
uint64_t getPassedPawnMask(int square, int colorToMove);
extern std::array<uint64_t, 64> squareToBitboard;

// conthist hehe
using CHEntry = std::array<std::array<std::array<int16_t, 64>, 7>, 2>;
using CHTable = std::array<std::array<std::array<std::array<CHEntry, 7>, 64>, 7>, 2>;

// flags for moves
constexpr uint8_t Normal = 0b0000;
constexpr std::array<uint8_t, 4> castling = {0b0001, 0b0010, 0b0011, 0b0100};
constexpr uint8_t EnPassant = 0b0101;
constexpr uint8_t DoublePawnPush = 0b0110;
constexpr std::array<uint8_t, 4> promotions = {0b0111, 0b1000, 0b1001, 0b1010};