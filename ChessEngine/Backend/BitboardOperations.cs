using System.Numerics;

namespace Chess {
    public static class BitboardOperations {
        /// <summary>
        /// outputs true if the bit in the bitboard at the index is 1.
        /// </summary>
        /// <param name="bitboard">the bitboard in question</param>
        /// <param name="index">The index in question</param>
        /// <returns>if the bit on the bitboard at the index is 1</returns>
        public static bool AtLocation(ulong bitboard, int index) {
            return (bitboard & ((ulong)1 << index)) != 0;
        }
        public static int PopLSB(ref ulong bitboard) {
            int lsb = BitOperations.TrailingZeroCount(bitboard);
            bitboard ^= 1UL << lsb;
            return lsb;
        }
        public static int PopMSB(ref ulong bitboard) {
            int msb = BitOperations.LeadingZeroCount(bitboard);
            bitboard ^= 1UL << msb;
            return msb;
        }
        public static ulong GetPassedPawnMask(int square, int colorToMove) {
            int file = square & 7;
            ulong centralFileMask = Mask.GetFileMask(file);
            ulong leftFileMask = file == 0 ? 0 : Mask.GetFileMask(file + 1);
            ulong rightFileMask = file == 7 ? 0 : Mask.GetFileMask(file - 1);
            ulong mask = centralFileMask | leftFileMask | rightFileMask;
            if(colorToMove == 1) {
                return mask << 8 * (square >> 3);
            } else {
                return mask >> 8 * (square >> 3);
            }
        }

    }
}