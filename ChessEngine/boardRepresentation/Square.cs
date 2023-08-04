namespace Chess {
    public struct Square {
        // square's values
        // side note rank is row and file is column, and index is 8r+f (position in the board counting from top left to bottom right)
        public int rank;
        public int file;
        public int index;
        public Piece piece;
        // defines the square's values with the rank and file
        public Square(int r, int f) {
            rank = r;
            file = f;
            index = 8*r+f;
        }
        // defines the square's values with just the index
        public Square(int i) {
            index = i;
            file = i & 7;
            rank = i >> 3;
        }
        // defines the square's values with just the index
        public Square(int i, PieceType t, bool w) {
            index = i;
            file = i & 7;
            rank = i >> 3;
            piece = new Piece(w, t, rank, file);
        }
        public Square(int r, int f, PieceType t, bool w) {
            index = 8*r + f;
            file = f;
            rank = r;
            piece = new Piece(w, t, rank, file);
        }
    }
}