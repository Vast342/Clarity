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
        public static void AddPiece(ref ulong bitboard, int index) {
            bitboard ^= 1UL << index;
        }
        public static void RemovePiece(ref ulong bitboard, int index) {
            bitboard ^= 1UL << index;
        }
        public static void MovePiece(ref ulong bitboard, int index1, int index2) {
            RemovePiece(ref bitboard, index1);
            AddPiece(ref bitboard, index2);
        }
    }
}