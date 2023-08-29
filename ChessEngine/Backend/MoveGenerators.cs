using System.Numerics;
namespace Chess {
    public static class MaskGen {
        public static readonly int[] directionalOffsets = {8, -8, 1, -1, 7, -7, 9, -9};
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
        // This is a concept, I don't know if it will get used but I think it would be more efficient
        public static ulong GetPawnPushesRow(int colorToMove, ulong coloredPawnBitboard) {
            ulong attacks = coloredPawnBitboard;
            attacks <<= directionalOffsets[1 - colorToMove];
            return attacks;
        }
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
        public static ulong GetSlideRay(int startSquare, int direction) {
            return Mask.slideyPieceRays[direction, startSquare];
        }
        public static ulong GetKnightAttacks(int startSquare) {
            return Mask.knightMasks[startSquare];
        }
        public static ulong GetKingAttacks(int startSquare) {
            return Mask.kingMasks[startSquare];
        }
        public static ulong GetPawnCaptures(int startSquare, int colorToMove) {
            return Mask.pawnAttackMasks[colorToMove, startSquare];        
        }
    }
}