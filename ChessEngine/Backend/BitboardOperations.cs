namespace Chess {
    public static class BitboardOperations {
        public static bool AtLocation(ulong bitboard, int index) {
            return (bitboard & ((ulong)1 << index)) != 0;
        }
    }
}