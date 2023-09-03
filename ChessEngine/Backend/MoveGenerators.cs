using System.Numerics;
namespace Chess {
    public static class MaskGen {
        public static ulong GetPawnPushes(ulong pawnBitboard, ulong emptyBitboard, int colorToMove) {
            ulong attacks;
            if(colorToMove == 1) {
                attacks = (pawnBitboard << 8) & emptyBitboard; 
            } else {
                attacks = (pawnBitboard >> 8) & emptyBitboard; 
            }
            return attacks;
        }
        public static ulong GetDoublePawnPushes(ulong pawnAttacks, ulong emptyBitboard, int colorToMove) {
            ulong attacks;
            if(colorToMove == 1) {
                attacks = ((pawnAttacks & Mask.GetRankMask(5 - (3 * colorToMove))) << 8) & emptyBitboard;
            } else {
                attacks = ((pawnAttacks & Mask.GetRankMask(5 - (3 * colorToMove))) >> 8) & emptyBitboard;
            }
            return attacks;
        }
        public static ulong GetRookAttacksOld(int startSquare, ulong occupiedBitboard) {
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
        public static ulong GetBishopAttacksOld(int startSquare, ulong occupiedBitboard) {
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
        public static ulong GetRookAttacks(int startSquare, ulong occupiedBitboard) {
            return MagicGeneration.rookAttacks[startSquare][((occupiedBitboard & MagicGeneration.rookMasks[startSquare]) * MagicGeneration.rookMagics[startSquare]) >> (64-MagicGeneration.rookShifts[startSquare])];
        }
        public static ulong GetBishopAttacks(int startSquare, ulong occupiedBitboard) {
            return MagicGeneration.bishopAttacks[startSquare][((occupiedBitboard & MagicGeneration.bishopMasks[startSquare]) * MagicGeneration.bishopMagics[startSquare]) >> (64-MagicGeneration.bishopShifts[startSquare])];
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
