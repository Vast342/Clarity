namespace Chess {
    public static class Piece {
        public const byte None = 0;
        public const byte Pawn = 1;
        public const byte Knight = 2;
        public const byte Bishop = 3;
        public const byte Rook = 4;
        public const byte Queen = 5;
        public const byte King = 6;

        public const byte White = 8;
        public const byte Black = 0;
        public static int GetColor(byte piece) {
            return piece >> 3;
        }
        public static int GetType(byte piece) {
            return piece & 7;
        }
    }
}