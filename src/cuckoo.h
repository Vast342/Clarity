/*
    Clarity
    Copyright (C) 2026 Joseph Pasfield

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
#pragma once

#include "globals.h"

// https://web.archive.org/web/20201107002606/https://marcelk.net/2013-04-06/paper/upcoming-rep-v2.pdf
// Implementation based on Stormphrax's (which was based on Stockfish's)

// these 2 are explicitly from stormphrax
[[nodiscard]] constexpr size_t h1(uint64_t key) {
    return static_cast<size_t>(key & 0x1FFF);
}

[[nodiscard]] constexpr size_t h2(uint64_t key) {
    return static_cast<size_t>((key >> 16) & 0x1FFF);
}

inline std::array<uint64_t, 8192> keys{};
inline std::array<Move, 8192> moves{};

inline void initCuckoo() {
    for(int pieceType = 1; pieceType < 6; pieceType++) {
        for(int pieceColor = 0; pieceColor <= 1; pieceColor++) {
            const auto piece = pieceColor * 8 + pieceType;
            for(int square0 = 0; square0 < 64; square0++) {
                // generate attacks for that piece from that square
                uint64_t attacks = 0;
                if (pieceType == Knight)      attacks = getKnightAttacks(square0);
                else if (pieceType == Bishop) attacks = getBishopAttacks(square0, 0);
                else if (pieceType == Rook)   attacks = getRookAttacks(square0, 0);
                else if (pieceType == Queen)  attacks = getRookAttacks(square0, 0) | getBishopAttacks(square0, 0);
                else if (pieceType == King)   attacks = getKingAttacks(square0);

                // dramatic sigh, back to square 1
                for(int square1 = square0 + 1; square1 < 64; square1++) {
                    // ensure the move here is possible
                    if((attacks & (1ULL << square1)) == 0) {
                        continue;
                    }

                    auto move = Move(square0, square1, 0);
                    auto key = zobTable[square0][piece] ^ zobTable[square1][piece] ^ zobColorToMove;
                    uint32_t slot = h1(key);

                    while(true) {
                        std::swap(keys[slot], key);
                        std::swap(moves[slot], move);
                        if(move == Move()) break;
                        slot = slot == h1(key) ? h2(key) : h1(key);
                    }
                }   
            }
        }
    }
}