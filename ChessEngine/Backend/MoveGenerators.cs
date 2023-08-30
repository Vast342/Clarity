using System.Globalization;
using System.Numerics;
namespace Chess {
    public static class MaskGen {
        public static readonly int[] directionalOffsets = {8, -8, 1, -1, 7, -7, 9, -9};
        public static ulong GetPawnPushesOld(int startSquare, int colorToMove) {
            ulong attacks = 0;
            attacks |= ((ulong)1) << startSquare + directionalOffsets[1 - colorToMove];
            if(startSquare >> 3 == (colorToMove == 1 ? 1 : 6)) {
                attacks |= ((ulong)1) << startSquare + directionalOffsets[1 - colorToMove] * 2;
            }
            return attacks;
        }
        public static ulong GetPawnPushes(int startSquare, int colorToMove) {
            return Mask.pawnPushMasks[colorToMove, startSquare];
        }
        public static ulong GetRookAttacks(int startSquare, ulong occupiedBitboard) {
            // North
            ulong north = Mask.slideyPieceRays[0, startSquare];
            ulong potentialBlockers = occupiedBitboard & north;
            int firstBlocker = BitOperations.TrailingZeroCount(potentialBlockers | (1UL << 63));
            north ^= Mask.slideyPieceRays[0, firstBlocker];
            // South
            ulong south = Mask.slideyPieceRays[1, startSquare];
            potentialBlockers = occupiedBitboard & south;
            firstBlocker = BitOperations.LeadingZeroCount(potentialBlockers | 1);
            south ^= Mask.slideyPieceRays[1, 63-firstBlocker];
            // East
            ulong east = Mask.slideyPieceRays[2, startSquare];
            potentialBlockers = occupiedBitboard & east;
            firstBlocker = BitOperations.TrailingZeroCount(potentialBlockers | (1UL << 63));
            east ^= Mask.slideyPieceRays[2, firstBlocker];
            // West
            ulong west = Mask.slideyPieceRays[3, startSquare];
            potentialBlockers = occupiedBitboard & west;
            firstBlocker = BitOperations.LeadingZeroCount(potentialBlockers | 1);
            west ^= Mask.slideyPieceRays[3, 63-firstBlocker];

            return north | south | east | west;
        }
        public static ulong GetBishopAttacks(int startSquare, ulong occupiedBitboard) {
            // North-West
            ulong northWest = Mask.slideyPieceRays[4, startSquare];
            ulong potentialBlockers = occupiedBitboard & northWest;
            int firstBlocker = BitOperations.TrailingZeroCount(potentialBlockers | (1UL << 63));
            northWest ^= Mask.slideyPieceRays[4, firstBlocker];
            // South-East
            ulong southEast = Mask.slideyPieceRays[5, startSquare];
            potentialBlockers = occupiedBitboard & southEast;
            firstBlocker = BitOperations.LeadingZeroCount(potentialBlockers | 1);
            southEast ^= Mask.slideyPieceRays[5, 63-firstBlocker];
            // North-East
            ulong northEast = Mask.slideyPieceRays[6, startSquare];
            potentialBlockers = occupiedBitboard & northEast;
            firstBlocker = BitOperations.TrailingZeroCount(potentialBlockers | (1UL << 63));
            northEast ^= Mask.slideyPieceRays[6, firstBlocker];
            // South-West
            ulong southWest = Mask.slideyPieceRays[7, startSquare];
            potentialBlockers = occupiedBitboard & southWest;
            firstBlocker = BitOperations.LeadingZeroCount(potentialBlockers | 1);
            southWest ^= Mask.slideyPieceRays[7, 63-firstBlocker];

            return northWest | southEast | northEast | southWest;
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
