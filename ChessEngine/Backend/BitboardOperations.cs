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
    }
}