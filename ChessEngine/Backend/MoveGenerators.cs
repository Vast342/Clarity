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
        public static ulong GetRookAttacks(int startSquare, ulong occupiedBitboard) {
            return MagicGeneration.rookAttacks[(4096 * startSquare) + (int)(((occupiedBitboard & MagicGeneration.rookMasks[startSquare]) * MagicGeneration.rookMagics[startSquare]) >> MagicGeneration.rookShifts[startSquare])];
        }
        public static ulong GetBishopAttacks(int startSquare, ulong occupiedBitboard) {
            return MagicGeneration.bishopAttacks[(4096 * startSquare) + (int)(((occupiedBitboard & MagicGeneration.bishopMasks[startSquare]) * MagicGeneration.bishopMagics[startSquare]) >> MagicGeneration.bishopShifts[startSquare])];
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
