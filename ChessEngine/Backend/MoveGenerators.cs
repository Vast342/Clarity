using System.Numerics;
namespace Chess {
    public static class MaskGen {
        public static readonly int[] directionalOffsets = {8, -8, 1, -1, 7, -7, 9, -9};
        /// <summary>
        /// Get's the pushes a pawn can do from the startsquare
        /// </summary>
        /// <param name="startSquare">the square the pawn is on</param>
        /// <param name="colorToMove">the current color to move</param>
        /// <param name="emptyBitboard">the binary compliment (bitwise opposite) of the occupied bitboard</param>
        /// <returns>a bitmask of the pawn pushes</returns>
        public static ulong GetPawnPushes(int startSquare, int colorToMove, ulong emptyBitboard) {
            ulong attacks = 0;
            if(BitboardOperations.AtLocation(emptyBitboard, startSquare + directionalOffsets[1 - colorToMove])) {
                attacks |= ((ulong)1) << startSquare + directionalOffsets[1 - colorToMove];
                if(BitboardOperations.AtLocation(emptyBitboard, startSquare + directionalOffsets[1 - colorToMove] * 2) && startSquare >> 3 == (colorToMove == 1 ? 1 : 6)) {
                    attacks |= ((ulong)1) << startSquare + directionalOffsets[1 - colorToMove] * 2;
                }
            }
            return attacks;
        }
        /// <summary>
        /// Gets the rook attacks using the classical approach
        /// </summary>
        /// <param name="startSquare">the square the rook is on</param>
        /// <param name="occupiedBitboard">the bitboard of all pieces on the board</param>
        /// <returns>The bitboard of the moves</returns>
        public static ulong GetRookAttacks(int startSquare, ulong occupiedBitboard) {
            ulong total = 0;
            for(int direction = 0; direction < 4; direction++) {
                ulong attacks = Mask.slideyPieceRays[direction, startSquare];
                ulong potentialBlockers = occupiedBitboard & attacks;
                if(potentialBlockers != 0) {
                    if((direction & 1) == 0) {
                        int firstBlocker = BitOperations.TrailingZeroCount(potentialBlockers);
                        attacks ^= Mask.slideyPieceRays[direction, firstBlocker];
                    } else {
                        int firstBlocker = BitOperations.LeadingZeroCount(potentialBlockers);
                        attacks ^= Mask.slideyPieceRays[direction, 63-firstBlocker];
                    }
                }
                total |= attacks;
            }
            return total;
        }
        /// <summary>
        /// Gets the bishop attacks using the classical approach
        /// </summary>
        /// <param name="startSquare">the square the bishop is on</param>
        /// <param name="occupiedBitboard">the bitboard of all pieces on the board</param>
        /// <returns>The bitboard of the moves</returns>
        public static ulong GetBishopAttacks(int startSquare, ulong occupiedBitboard) {
            ulong total = 0;
            for(int direction = 4; direction < 8; direction++) {
                ulong attacks = Mask.slideyPieceRays[direction, startSquare];
                ulong potentialBlockers = occupiedBitboard & attacks;
                if(potentialBlockers != 0) {
                    if((direction & 1) == 0) {
                        int firstBlocker = BitOperations.TrailingZeroCount(potentialBlockers);
                        attacks ^= Mask.slideyPieceRays[direction, firstBlocker];
                    } else {
                        int firstBlocker = BitOperations.LeadingZeroCount(potentialBlockers);
                        attacks ^= Mask.slideyPieceRays[direction, 63-firstBlocker];
                    }
                }
                total |= attacks;
            }
            return total;
        }
        /// <summary>
        /// Gets a specific ray mask from the saved array
        /// </summary>
        /// <param name="startSquare">The square the ray comes from</param>
        /// <param name="direction">the direction it goes out</param>
        /// <returns>the saved mask</returns>
        public static ulong GetSlideRay(int startSquare, int direction) {
            return Mask.slideyPieceRays[direction, startSquare];
        }
        /// <summary>
        /// Gets the knight attacks from Homura's array
        /// </summary>
        /// <param name="startSquare">The square the knight is on</param>
        /// <returns>the kinght attack from the array</returns>
        public static ulong GetKnightAttacks(int startSquare) {
            return Mask.knightMasks[startSquare];
        }
        /// <summary>
        /// Gets the king attacks from Homura's array
        /// </summary>
        /// <param name="startSquare">The square the king is on</param>
        /// <returns>the king attack from the array</returns>
        public static ulong GetKingAttacks(int startSquare) {
            return Mask.kingMasks[startSquare];
        }
        /// <summary>
        /// Gets the pawn attacks from Homura's array
        /// </summary>
        /// <param name="startSquare">The square the pawn is on</param>
        /// <returns>the pawn captures from the array</returns>
        public static ulong GetPawnCaptures(int startSquare, int colorToMove) {
            return Mask.pawnAttackMasks[colorToMove, startSquare];        
        }
    }
}