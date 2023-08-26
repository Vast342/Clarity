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
        public static ulong AddPiece(ulong bitboard, int index) {
            return bitboard ^ 1UL << index;
        }
        public static ulong RemovePiece(ulong bitboard, int index) {
            return bitboard ^ 1UL << index;
        }
        public static ulong MovePiece(ulong bitboard, int index1, int index2) {
            ulong bb = bitboard;
            RemovePiece(bb, index1);
            AddPiece(bb, index2);
            return bb;
        }
    }
}