namespace Chess {
    public static class Piece {
        public const int None = 0;
        public const int Pawn = 1;
        public const int Knight = 2;
        public const int Bishop = 3;
        public const int Rook = 4;
        public const int Queen = 5;
        public const int King = 6;

        public const int White = 8;
        public const int Black = 16;
        
    }
    public static class Board {
        public static int[] squares;
        public static Board() {
            squares = new int[64];

        }
        /// <summary>
        /// Loads the information contained within a string of Forsyth–Edwards Notation into the current board instance's values.*
        /// </summary>
        /// <param name="fen">The FEN string in question.</param>
        public static void LoadFenToPosition(string fen) {
            var pieceCharDictionary = new Dictionary<char, int>() {
                ['p'] = Piece.Pawn, ['n'] = Piece.Knight, ['b'] = Piece.Bishop, ['r'] = Piece.Rook, ['q'] = Piece.Queen, ['k'] = Piece.King
            };
            string boardSection = fen.Split(' ')[0];
            int file = 0;
            int rank = 7;
            foreach(char c in boardSection) {
                if(c == '/') {
                    file = 0;
                    rank--;
                } else {
                    if(char.IsDigit(c)) {
                        file += c - '0';
                    } else {
                        int pieceColor = char.IsUpper(c) ? Piece.White : Piece.Black;
                        int pieceType = pieceCharDictionary[char.ToLower(c)];
                        squares[rank * 8 + file] = pieceType | pieceColor;
                    }
                }
            }
        }
        static MoveData() {
            for(int file = 0; file < 8; file++) {
                for(int rank = 0; rank < 8; rank++) {
                    int north = 7 - rank;
                    int south = rank;
                    int west = file;
                    int east = 7 - file;
                    int index = rank * 8 + file;
                }
            }
        }
        public List<Move> GetLegalMoves() {
            List<Move> moves = new();
            for(int startSquare = 0; startSquare < 64; startSquare++) {

            }
        }
    }
    public struct Move {
        public int StartSquare;
        public int EndSquare;
    }
}