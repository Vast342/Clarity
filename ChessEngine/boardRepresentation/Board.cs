namespace Chess {
    public struct Board {
        public Square[,] squares = new Square[8,8];
        public ulong zobristKey;
        public Square[] kingSquares = new Square[2];
        private static ulong[,,] zobTable = new ulong[8,8,13];
        public bool isWhiteToMove;
        public int fiftyMoveCounter;
        public int plyCount;
        // for the >> 3 and & 7,  I learned those things from Ellie M, the developer of the chess engine Homura.
        // its the same as / 8 and % 8
        // future me, if you don't remember just look up bitwise functions
        public Board(string fen) {
            string[] parts = fen.Split(" ");
            string[] ranks = parts[0].Split('/');
            Array.Reverse(ranks);
            int i = 0;
            foreach(string rank in ranks) {
                foreach(char c in rank) {
                    if(c == 'p') {
                        squares[i >> 3, i & 7] = new Square(i, PieceType.Pawn, false);
                        i++;
                    } else if(c == 'P') {
                        squares[i >> 3, i & 7] = new Square(i, PieceType.Pawn, true);
                        i++;
                    } else if(c == 'n') {
                        squares[i >> 3, i & 7] = new Square(i, PieceType.Knight, false);
                        i++;
                    } else if(c == 'N') {
                        squares[i >> 3, i & 7] = new Square(i, PieceType.Knight, true);
                        i++;
                    } else if(c == 'b') {
                        squares[i >> 3, i & 7] = new Square(i, PieceType.Bishop, false);
                        i++;
                    } else if(c == 'B') {
                        squares[i >> 3, i & 7] = new Square(i, PieceType.Bishop, true);
                        i++;
                    } else if(c == 'r') {
                        squares[i >> 3, i & 7] = new Square(i, PieceType.Rook, false);
                        i++;
                    } else if(c == 'R') {
                        squares[i >> 3, i & 7] = new Square(i, PieceType.Rook, true);
                        i++;
                    } else if(c == 'q') {
                        squares[i >> 3, i & 7] = new Square(i, PieceType.Queen, false);
                        i++;
                    } else if(c == 'Q') {
                        squares[i >> 3, i & 7] = new Square(i, PieceType.Queen, true);
                        i++;
                    } else if(c == 'k') {
                        squares[i >> 3, i & 7] = new Square(i, PieceType.King, false);
                        kingSquares[0] = squares[i >> 3, i & 7];
                        i++;
                    } else if(c == 'K') {
                        squares[i >> 3, i & 7] = new Square(i, PieceType.King, true);
                        kingSquares[1] = squares[i >> 3, i & 7];
                        i++;
                    } else {
                        for(int j = i; j <  i + c - '0'; j++) {
                            squares[j >> 3, j & 7] = new Square(i, PieceType.None, true);
                        }
                        i += c - '0';
                    }
                }
            }
            isWhiteToMove = parts[1] == "w";
            plyCount = int.Parse(parts[5]);
        }
        public int PieceIndexForHash(Piece p) {
            return (int)p.type + (p.isWhite ? 0 : 6);
        }
        public void InitializeZobrist() {
            var rng = new Random();
            for(int i = 0; i < 8; i++) {
                for(int j = 0; j < 8; j++) {
                    for(int k = 1; k <= 12; k++) {
                        zobTable[i,j,k] = (ulong)rng.Next(0, (int)Math.Pow(2, 64)-1);
                    }
                }
            }
        }
        public ulong ZobristHash() {
            ulong hash = 0;
            for(int i = 0; i < 8; i++) {
                for(int j = 0; j < 8; j++) {
                    int type = PieceIndexForHash(squares[i,j].piece);
                    hash ^= zobTable[i,j,type];
                }
            }
            return hash;
        }
        readonly public string GetFenString() {
            string fen = "";
            for (int rank = 7; rank >= 0; rank--)
            {
                int numEmptyFiles = 0;
                for (int file = 0; file < 8; file++)
                {
                    Piece piece = squares[rank, file].piece;
                    if (piece.type != PieceType.None)
                    {
                        if (numEmptyFiles != 0)
                        {
                            fen += numEmptyFiles;
                            numEmptyFiles = 0;
                        }
                        bool isBlack = !piece.isWhite;
                        PieceType pieceType = piece.type;
                        char pieceChar = ' ';
                        switch (pieceType)
                        {
                            case PieceType.Rook:
                                pieceChar = 'R';
                                break;
                            case PieceType.Knight:
                                pieceChar = 'N';
                                break;
                            case PieceType.Bishop:
                                pieceChar = 'B';
                                break;
                            case PieceType.Queen:
                                pieceChar = 'Q';
                                break;
                            case PieceType.King:
                                pieceChar = 'K';
                                break;
                            case PieceType.Pawn:
                                pieceChar = 'P';
                                break;
                        }
                        fen += isBlack ? pieceChar.ToString().ToLower() : pieceChar.ToString();
                    }
                    else
                    {
                        numEmptyFiles++;
                    }

                }
                if (numEmptyFiles != 0)
                {
                    fen += numEmptyFiles;
                }
                if (rank != 0)
                {
                    fen += '/';
                }
            }

            // Side to move
            fen += ' ';
            fen += isWhiteToMove ? 'w' : 'b';

            // castling, nothing yet
            fen += ' ';
            fen += "-";

            // En Passant, nothing yet
            fen += ' ';
            fen += '-';

            // 50 move counter
            fen += ' ';
            fen += fiftyMoveCounter;

            // Full-move count (should be one at start, and increase after each move by black)
            fen += ' ';
            fen += (plyCount / 2) + 1;

            return fen;
        }
        public void MakeMove(Move move) {
            Console.WriteLine(move.startSquare.rank + ", " + move.startSquare.file + ", " + move.targetSquare.rank + ", " + move.targetSquare.file);
            plyCount++;
            BoardFunctions.SwapSquares(ref squares, move.startSquare.rank, move.startSquare.file, move.targetSquare.rank, move.targetSquare.file);
            // checks if it was a capture
            if(move.isCapture) {
                squares[move.startSquare.rank, move.startSquare.file].piece = new Piece(true, PieceType.None, move.startSquare.rank, move.startSquare.file);
            }
            isWhiteToMove = !isWhiteToMove;
        }
        public Square GetSquareFromIndex(int i) {
            return squares[i >> 3, i & 7];
        }
        public Square GetSquareFromPosition(int r, int f) {
            return squares[r, f];
        }
    }
}