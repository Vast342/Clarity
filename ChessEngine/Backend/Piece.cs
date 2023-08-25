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
        /// <summary>
        /// Gets the color from a piece value
        /// </summary>
        /// <param name="piece">The piece value in question</param>
        /// <returns>the color of the piece</returns>
        public static int GetColor(byte piece) {
            return piece >> 3;
        }
        /// <summary>
        /// Gets the type from a piece value
        /// </summary>
        /// <param name="piece">The piece value oin question</param>
        /// <returns>the type of the piece</returns>
        public static int GetType(byte piece) {
            return piece & 7;
        }
    }
}