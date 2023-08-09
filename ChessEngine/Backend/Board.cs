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
        public const int Black = 0;
    }
    public class Board {
        public static int[] squares = new int[64];
        public static int colorToMove = 1;
        public Board(string fen) {
            LoadFenToPosition(fen);
            MoveData();
        }
        /// <summary>
        /// Loads the information contained within a string of Forsyth–Edwards Notation into the current board instance's values.*
        /// </summary>
        /// <param name="fen">The FEN string in question.</param>
        public static void LoadFenToPosition(string fen) {
            var pieceCharDictionary = new Dictionary<char, int>() {
                ['p'] = Piece.Pawn, ['n'] = Piece.Knight, ['b'] = Piece.Bishop, ['r'] = Piece.Rook, ['q'] = Piece.Queen, ['k'] = Piece.King
            };
            string[] fenSections = fen.Split(' ');
            int file = 0;
            int rank = 7;
            foreach(char c in fenSections[0]) {
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
            colorToMove = fenSections[1] == "w" ? 1 : 0;
        }
        public static readonly int[] directionalOffsets = {8, -8, -1, 1, 7, -7, 9, -9};
        public static int[,] squaresToEdge = new int[64,8];
        static void MoveData() {
            for(int file = 0; file < 8; file++) {
                for(int rank = 0; rank < 8; rank++) {
                    int north = 7 - rank;
                    int south = rank;
                    int west = file;
                    int east = 7 - file;
                    int index = rank * 8 + file;
                    squaresToEdge[index, 0] = north;
                    squaresToEdge[index, 1] = south;
                    squaresToEdge[index, 0] = west;
                    squaresToEdge[index, 0] = east;
                    squaresToEdge[index, 0] = Math.Min(north, west);
                    squaresToEdge[index, 0] = Math.Min(south, east);
                    squaresToEdge[index, 0] = Math.Min(north, east);
                    squaresToEdge[index, 0] = Math.Min(south, west);
                }
            }
        }
        public List<Move> GetLegalMoves() {
            List<Move> moves = new();
            for(int startSquare = 0; startSquare < 64; startSquare++) {
                int currentPiece = squares[startSquare];
                // checks if it's the right color
                if((currentPiece & 0b1000) == colorToMove) {
                    // a check if it's a sliding piece
                    if((currentPiece & 0b0111) > 2 && (currentPiece & 0b0111) != 6) {
                        int startDirection = (currentPiece & 0b0111) == 3 ? 4 : 0;
                        int endDirection = (currentPiece & 0b0111) == 4 ? 8 : 4;
                        for(int direction = startDirection; direction < endDirection; direction++) {
                            for(int i = 0; i < squaresToEdge[startSquare, direction]; i++) {
                                int targetSquareIndex = startSquare + directionalOffsets[direction] * (i + 1);
                                int targetPiece = squares[targetSquareIndex];
                                // can't capture the piece at the end of the line, end the cycle
                                if((targetPiece & 0b1000) == colorToMove) {
                                    break;
                                }
                                moves.Add(new Move(startSquare, targetSquareIndex));
                                // the piece at the end of the line can be captured, you just added the capture to the list, end the cycle
                                if((targetPiece & 0b1000) != colorToMove) {
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            return moves;
        }
    }
    public struct Move {
        public int StartSquare;
        public int EndSquare;
        /// <summary>
        /// Makes a new move with the starting and ending squares.
        /// </summary>
        /// <param name="start">Starting square of the move</param>
        /// <param name="end">Ending square of the move</param>
        public Move(int start, int end) {
            StartSquare = start;
            EndSquare = end;
        }
    }
}