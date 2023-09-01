namespace Chess {
    public struct Board {
/*
LIST OF IDEAS

make MakeMove and UndoMove better, rework readBoardState and GetMoves (remove bitboards from boardState)

Figure out TT

Memory management

Optimizations in general

Magic bitboard move generation

stuff with unsafe and fixed-size arrays
*/

        // board specific values
        public byte[] kingSquares = new byte[2];
        public int colorToMove = 1;
        // bitboards of the entire color: 0 is black 1 is white
        public ulong[] coloredBitboards = new ulong[2];
        // bitboards of specific pieces of specific color, using the numbers above.
        public ulong[] pieceBitboards = new ulong[6];
        // the index of the en-passantable square, from 0-63, and if it's 64 no en passant is legal.
        public int enPassantIndex = 64;
        // white kingside, white queenside, black kingside, black kingside
        public bool[] castlingRights = new bool[4];
        public int plyCount = 0;
        public byte fiftyMoveCounter;
        private readonly static ulong[,] zobTable = new ulong[64,15];
        private static ulong zobColorToMove;
        public static readonly int[] directionalOffsets = {8, -8, 1, -1, 7, -7, 9, -9};
        /// <summary>
        /// Reads from a boardstate
        /// </summary>
        /// <param name="state">the boardstate in question</param>
        public void ReadBoardState(BoardState state) {
            kingSquares[0] = state.kingSquareBlack;
            kingSquares[1] = state.kingSquareWhite;
            enPassantIndex = state.enPassantIndex;
            castlingRights[0] = state.wk;
            castlingRights[1] = state.wq;
            castlingRights[2] = state.bk;
            castlingRights[3] = state.bq;
            fiftyMoveCounter = state.fiftyMoveCounter;
            coloredBitboards[0] = state.blackBitboard;
            coloredBitboards[1] = state.whiteBitboard;
            pieceBitboards[Piece.Pawn] = state.pawnBitboard;
            pieceBitboards[Piece.Knight] = state.knightBitboard;
            pieceBitboards[Piece.Bishop] = state.bishopBitboard;
            pieceBitboards[Piece.Rook] = state.rookBitboard;
            pieceBitboards[Piece.Queen] = state.queenBitboard;
            pieceBitboards[Piece.King] = state.kingBitboard;
        }
        /// <summary>
        /// Establishes a new board instance, with the position and information contained within a FEN string.
        /// </summary>
        /// <param name="fen">The fen string in question.</param>
        public Board(string fen) {
            LoadFenToPosition(fen);
            InitializeZobrist();
        }
        /// <summary>
        /// Loads the information contained within a string of Forsyth–Edwards Notation into the current board instance's values.
        /// </summary>
        /// <param name="fen">The FEN string in question.</param>
        public void LoadFenToPosition(string fen) {
            byte[] squares = new byte[64];
            for(int j = 0; j < 64; j++) {
                squares[j] = Piece.None;
            }
            string[] parts = fen.Split(" ");
            string[] ranks = parts[0].Split('/');
            Array.Reverse(ranks);
            int i = 0;
            foreach(string rank in ranks) {
                foreach(char c in rank) {
                    if(c == 'p') {
                        squares[i] = Piece.Pawn | Piece.Black;
                        i++;
                    } else if(c == 'P') {
                        squares[i] = Piece.Pawn | Piece.White;
                        i++;
                    } else if(c == 'n') {
                        squares[i] = Piece.Knight | Piece.Black;
                        i++;
                    } else if(c == 'N') {
                        squares[i] = Piece.Knight | Piece.White;
                        i++;
                    } else if(c == 'b') {
                        squares[i] = Piece.Bishop | Piece.Black;
                        i++;
                    } else if(c == 'B') {
                        squares[i]= Piece.Bishop | Piece.White;
                        i++;
                    } else if(c == 'r') {
                        squares[i] = Piece.Rook | Piece.Black;
                        i++;
                    } else if(c == 'R') {
                        squares[i] = Piece.Rook | Piece.White;
                        i++;
                    } else if(c == 'q') {
                        squares[i] = Piece.Queen | Piece.Black;
                        i++;
                    } else if(c == 'Q') {
                        squares[i] = Piece.Queen | Piece.White;
                        i++;
                    } else if(c == 'k') {
                        squares[i] = Piece.King | Piece.Black;
                        kingSquares[1] = (byte)i;
                        i++;
                    } else if(c == 'K') {
                        squares[i] = Piece.King | Piece.White;
                        kingSquares[0] = (byte)i;
                        i++;
                    } else {
                        // for anyone wondering about this line, it's basically char.GetNumericValue but a bit simpler.
                        i += c - '0';
                    }
                }
            }
            // whose turn is it?
            colorToMove = parts[1] == "w" ? 1 : 0;
            // castling rights
            if(parts[2] != "-") {
                foreach(char right in parts[2]) {
                    if(right == 'Q') {
                        castlingRights[1] = true;
                    } else if(right == 'K') {
                        castlingRights[0] = true;
                    } else if(right == 'q') {
                        castlingRights[3] = true;
                    } else if(right == 'k') {
                        castlingRights[2] = true;
                    }
                }
            }
            // en passant rules
            if(parts[3] != "-") {
                char[] characters = parts[3].ToCharArray();
                enPassantIndex = characters[0] - 'a' + (characters[1] - '1') * 8;
            }
            // counters 
            fiftyMoveCounter = byte.Parse(parts[4]);
            plyCount = int.Parse(parts[5]) * 2 - colorToMove;
            GenerateBitboards(squares);
        }
        /// <summary>
        /// Outputs the fen string of the current position
        /// </summary>
        /// <returns>the fen string</returns>
        // NEEDS TO BE REWRITTEN TO USE BITBOARDS 
        public string GetFenString() {
            string fen = "";

            for (int rank = 7; rank >= 0; rank--)
            {
                int numEmptyFiles = 0;
                for (int file = 0; file < 8; file++)
                {
                    int piece = PieceAtIndex(8*rank+file);
                    if (piece != Piece.None)
                    {
                        if (numEmptyFiles != 0)
                        {
                            fen += numEmptyFiles;
                            numEmptyFiles = 0;
                        }
                        bool isBlack = Piece.GetColor((byte)piece) == 0;
                        int pieceType = Piece.GetType((byte)piece);
                        char pieceChar = ' ';
                        switch (pieceType)
                        {
                            case Piece.Rook:
                                pieceChar = 'R';
                                break;
                            case Piece.Knight:
                                pieceChar = 'N';
                                break;
                            case Piece.Bishop:
                                pieceChar = 'B';
                                break;
                            case Piece.Queen:
                                pieceChar = 'Q';
                                break;
                            case Piece.King:
                                pieceChar = 'K';
                                break;
                            case Piece.Pawn:
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

            fen += ' ';
            fen += colorToMove == 1 ? 'w' : 'b';

            // castling
            bool thingAdded = false;
            fen += ' ';
            if(castlingRights[0]) {
                fen += 'K';
                thingAdded = true;
            }
            if(castlingRights[1]) {
                fen += 'Q';
                thingAdded = true;
            }
            if(castlingRights[2]) {
                fen += 'k';
                thingAdded = true;
            }
            if(castlingRights[3]) {
                fen += 'q';
                thingAdded = true;
            }
            if(!thingAdded) {
                fen += '-';
            }

            // En Passant
            fen += ' ';
            if(enPassantIndex != 64) {
                fen += (char)((enPassantIndex & 7) + 'a');
                if(enPassantIndex < 25) {
                    fen += '3';
                } else {
                    fen += '6';
                }
            } else {
                fen += '-';
            }

            // 50 move counter
            fen += ' ';
            fen += fiftyMoveCounter;

            // Full-move count (should be one at start, and increase after each move by black)
            fen += ' ';
            fen += plyCount / 2 + colorToMove;
            return fen;
        }
        public static int[] colorMultipliers = {-1, 1};
        /// <summary>
        /// Generates a list of all the moves from the position, not including moves while in check. To filter the legal moves, check the return value of Board.MakeMove()
        /// </summary>
        /// <returns>A List of the legal moves</returns>
        public void GetMoves(ref System.Span<Move> moves) {
            BoardState state = new(this);
            ulong occupiedBitboard = GetOccupiedBitboard();
            int totalMoves = 0;
            // castling
            if(!IsInCheck()) {
                if(colorToMove == 1) {
                    if(castlingRights[0] && (occupiedBitboard & 0x60) == 0 && !SquareIsAttackedByOpponent(5) && PieceAtIndex(7) == (Piece.Rook | Piece.White)) {
                        moves[totalMoves] = new Move(4, 6, Piece.None, state);
                        totalMoves++;
                    }
                    if(castlingRights[1] && (occupiedBitboard & 0xE) == 0 && !SquareIsAttackedByOpponent(3) && PieceAtIndex(0) == (Piece.Rook | Piece.White)) {
                        moves[totalMoves] = new Move(4, 2, Piece.None, state);
                        totalMoves++;
                    }
                } else {
                    if(castlingRights[2] && (occupiedBitboard & 0x6000000000000000) == 0 && !SquareIsAttackedByOpponent(61) && PieceAtIndex(63) == Piece.Rook) {
                        moves[totalMoves] = new Move(60, 62, Piece.None, state);
                        totalMoves++;
                    }
                    if(castlingRights[3] && (occupiedBitboard & 0xE00000000000000) == 0 && !SquareIsAttackedByOpponent(59) && PieceAtIndex(56) == Piece.Rook) {
                        moves[totalMoves] = new Move(60, 58, Piece.None, state);
                        totalMoves++;   
                    }
                }
            }
            ulong mask = coloredBitboards[colorToMove];
            // the rest of the pieces
            while(mask != 0) {
                int startSquare = BitboardOperations.PopLSB(ref mask);
                byte currentPiece = PieceAtIndex(startSquare);
                ulong total = 0;
                ulong pawnCaptures = 0;
                if(Piece.GetType(currentPiece) == Piece.Bishop) {
                    total |= MaskGen.GetBishopAttacks(startSquare, occupiedBitboard);
                } else if(Piece.GetType(currentPiece) == Piece.Rook) {
                    total |= MaskGen.GetRookAttacks(startSquare, occupiedBitboard);
                } else if(Piece.GetType(currentPiece) == Piece.Queen) {
                    total |= MaskGen.GetRookAttacks(startSquare, occupiedBitboard); 
                    total |= MaskGen.GetBishopAttacks(startSquare, occupiedBitboard);
                } else if(Piece.GetType(currentPiece) == Piece.Pawn) {
                    pawnCaptures |= MaskGen.GetPawnCaptures(startSquare, colorToMove);
                } else if(Piece.GetType(currentPiece) == Piece.Knight) {
                    total |= MaskGen.GetKnightAttacks(startSquare);
                } else if(Piece.GetType(currentPiece) == Piece.King) {
                    total |= MaskGen.GetKingAttacks(startSquare);
                }
                // get rid of capturing your own pieces
                total ^= total & coloredBitboards[colorToMove];
                while(total != 0) {
                    moves[totalMoves] = new Move(startSquare, BitboardOperations.PopLSB(ref total), Piece.None, state);
                    totalMoves++;
                }
                // making sure it's just capturing the opponent's pieces or doing en passant
                pawnCaptures &= coloredBitboards[1 - colorToMove] | (1UL << enPassantIndex);
                while(pawnCaptures != 0) {
                    int index = BitboardOperations.PopLSB(ref pawnCaptures);
                    if(index >> 3 == 7 * colorToMove) {
                        // promotions
                        for(int type = Piece.Knight; type < Piece.King; type++) { 
                            moves[totalMoves] = new Move(startSquare, index, type | (colorToMove == 1 ? Piece.White : Piece.Black), state);
                            totalMoves++;
                        }
                    } else {
                        moves[totalMoves] = new Move(startSquare, index, Piece.None, state);
                        totalMoves++;
                    }
                }
            }
            // pawn pushes
            ulong pawnBitboard = GetColoredPieceBitboard(colorToMove, Piece.Pawn);
            ulong emptyBitboard = ~GetOccupiedBitboard();
            ulong pawnPushes = MaskGen.GetPawnPushes(pawnBitboard, emptyBitboard, colorToMove);
            ulong doublePawnPushes = MaskGen.GetDoublePawnPushes(pawnPushes, emptyBitboard, colorToMove);
            while(pawnPushes != 0) {
                int index = BitboardOperations.PopLSB(ref pawnPushes);
                int startSquare = index + directionalOffsets[colorToMove];
                if(index >> 3 == 7 * colorToMove) {
                     // promotions
                    for(int type = Piece.Knight; type < Piece.King; type++) { 
                        moves[totalMoves] = new Move(startSquare, index, type | (colorToMove == 1 ? Piece.White : Piece.Black), state);
                        totalMoves++;
                    }
                } else {
                    moves[totalMoves] = new Move(startSquare, index, Piece.None, state);
                    totalMoves++;
                }
            }
            while(doublePawnPushes != 0) {
                int index = BitboardOperations.PopLSB(ref doublePawnPushes);
                int startSquare = index + directionalOffsets[colorToMove] * 2;
                moves[totalMoves] = new Move(startSquare, index, Piece.None, state);
                totalMoves++;
            }
            moves = moves.Slice(0, totalMoves);
        }
        /// <summary>
        /// Gets the captures from a position, used in Q Search.
        /// </summary>
        /// <returns>A List of capture moves</returns>
        public void GetMovesQSearch(ref Span<Move> moves) {
            BoardState state = new(this);
            ulong occupiedBitboard = GetOccupiedBitboard();
            int totalMoves = 0;
            ulong mask = coloredBitboards[colorToMove];
            // the rest of the pieces
            while(mask != 0) {
                int startSquare = BitboardOperations.PopLSB(ref mask);
                byte currentPiece = PieceAtIndex(startSquare);
                ulong total = 0;
                ulong pawnCaptures = 0;
                if(Piece.GetType(currentPiece) == Piece.Bishop) {
                    total |= MaskGen.GetBishopAttacks(startSquare, occupiedBitboard);
                } else if(Piece.GetType(currentPiece) == Piece.Rook) {
                    total |= MaskGen.GetRookAttacks(startSquare, occupiedBitboard);
                } else if(Piece.GetType(currentPiece) == Piece.Queen) {
                    total |= MaskGen.GetRookAttacks(startSquare, occupiedBitboard); 
                    total |= MaskGen.GetBishopAttacks(startSquare, occupiedBitboard);
                } else if(Piece.GetType(currentPiece) == Piece.Pawn) {
                    pawnCaptures |= MaskGen.GetPawnCaptures(startSquare, colorToMove);
                } else if(Piece.GetType(currentPiece) == Piece.Knight) {
                    total |= MaskGen.GetKnightAttacks(startSquare);
                } else if(Piece.GetType(currentPiece) == Piece.King) {
                    total |= MaskGen.GetKingAttacks(startSquare);
                }
                while(total != 0) {
                    int index = BitboardOperations.PopLSB(ref total);
                    byte piece = PieceAtIndex(index);
                    if(Piece.GetColor(piece) != colorToMove) {
                        moves[totalMoves] = new Move(startSquare, index, Piece.None, state);
                        totalMoves++;
                    }
                }
                while(pawnCaptures != 0) {
                    int index = BitboardOperations.PopLSB(ref pawnCaptures);
                    byte piece = PieceAtIndex(index);
                    if(index >> 3 == 7 * colorToMove) {
                        // promotions
                        if(Piece.GetColor(piece) != colorToMove && Piece.GetType(piece) != Piece.None) {
                            for(int type = Piece.Knight; type < Piece.King; type++) { 
                                moves[totalMoves] = new Move(startSquare, index, type | (colorToMove == 1 ? Piece.White : Piece.Black), state);
                                totalMoves++;
                            }
                        }
                    } else {
                        if((Piece.GetColor(piece) != colorToMove && Piece.GetType(piece) != Piece.None) || index == enPassantIndex) {
                            moves[totalMoves] = new Move(startSquare, index, Piece.None, state);
                            totalMoves++;
                        }
                    }
                }
            }
            moves = moves.Slice(0, totalMoves);
        }

        /// <summary>
        /// initializes a table used later for zobrist hashing, done automatically if you create the board using a fen string.
        /// </summary>
        public void InitializeZobrist() {
            if(zobTable[0,0] != 0) {
                var rng = new Random();
                zobColorToMove = (ulong)rng.Next(0, int.MaxValue);
                for(int i = 0; i < 64; i++) {
                    for(int j = 0; j < 15; j++) {
                        zobTable[i,j] = (ulong)rng.Next(0, int.MaxValue);
                    }
                }
            }
        }
        /// <summary>
        /// The function to generate a zobrist hash of the current position
        /// </summary>
        /// <returns>The zobrist hash</returns>
        public ulong CreateHash() {
            ulong hash = 0;
            if(colorToMove == 0) {
                hash ^= zobColorToMove;
            }
            for(int i = 0; i < 64; i++) {
                if(PieceAtIndex(i) != 0) {
                    hash ^= zobTable[i, PieceAtIndex(i)];
                }
            }
            return hash;
        }
        /// <summary>
        /// Makes a move on the board
        /// </summary>
        /// <param name="move">The move to be made</param>
        public bool MakeMove(Move move) {
            byte startPiece = PieceAtIndex(move.startSquare);
            byte endPiece = PieceAtIndex(move.endSquare);
            if(colorToMove != 1) {
                fiftyMoveCounter++;
            }
            // fifty move counter
            if(endPiece != 0 || (startPiece & 7) == Piece.Pawn) {
                fiftyMoveCounter = 0;
            }
            // for each start one set the start square to 0
            if(castlingRights[0] && move.startSquare == 4 && move.endSquare == 6) {
                // castling 1
                // switch 4 with 6 and 7 with 5
                castlingRights[0] = false;
                castlingRights[1] = false;
                // update the bitboards
                MovePiece(4, PieceAtIndex(4), 6, Piece.None);
                MovePiece(7, PieceAtIndex(7), 5, Piece.None);
                enPassantIndex = 64;
            } else if(castlingRights[1] && move.startSquare == 4 && move.endSquare == 2) {
                // castling 
                // switch 4 with 2 and 0 with 3
                castlingRights[0] = false;
                castlingRights[1] = false;
                // update the bitboards
                MovePiece(4, PieceAtIndex(4), 2, Piece.None);
                MovePiece(0, PieceAtIndex(0), 3, Piece.None);
                enPassantIndex = 64;
            } else if(castlingRights[2] && move.startSquare == 60 && move.endSquare == 62) {
                // castling 3
                // switch 60 with 62 and 63 with 61
                castlingRights[2] = false;
                castlingRights[3] = false;
                // update the bitboards
                MovePiece(60, PieceAtIndex(60), 62, Piece.None);
                MovePiece(63, PieceAtIndex(63), 61, Piece.None);
                enPassantIndex = 64;
            } else if(castlingRights[3] && move.startSquare == 60 && move.endSquare == 58) {
                // castling 4
                // switch 60 with 58 and 56 with 59
                castlingRights[2] = false;
                castlingRights[3] = false;
                // update the bitboards
                MovePiece(60, PieceAtIndex(60), 58, Piece.None);
                MovePiece(56, PieceAtIndex(56), 59, Piece.None);
                enPassantIndex = 64;
            } else if(move.endSquare != 0 && move.endSquare == enPassantIndex && Piece.GetType(startPiece) == Piece.Pawn) {
                // en passant
                // switch first square with second square and set the square either ahead or behind with colorToMove ? 8 : -8 to 0
                enPassantIndex = 64;
                // update the bitboards
                MovePiece(move.startSquare, startPiece, move.endSquare, endPiece);
                RemovePiece(move.endSquare + (colorToMove == 1 ? -8 : 8), (byte)(Piece.Pawn | (colorToMove == 1 ? Piece.Black : Piece.White)));
            } else {
                // move  normally
                // switch first square with second
                if((startPiece & 7) == Piece.Pawn && move.endSquare == move.startSquare + (colorToMove == 1 ? 16 : -16)) {
                    // its a double move, make the first square the en passant index
                    enPassantIndex = move.startSquare + (colorToMove == 1 ? 8 : -8);
                } else {
                    enPassantIndex = 64;
                }
                // update the bitboards
                MovePiece(move.startSquare, startPiece, move.endSquare, endPiece);
            }
            // promotions
            if(move.promotionType != Piece.None && move.promotionType != 0) {
                // update the bitboards
                RemovePiece(move.endSquare, (byte)(Piece.Pawn | (colorToMove == 1 ? Piece.White : Piece.Black)));
                AddPiece(move.endSquare, (byte)move.promotionType);
            }
            // king square updates
            if(Piece.GetType(PieceAtIndex(move.endSquare)) == Piece.King) {
                kingSquares[colorToMove] = (byte)move.endSquare;
                castlingRights[colorToMove == 1 ? 0 : 2] = false;
                castlingRights[colorToMove == 1 ? 1 : 3] = false;
            }
            if(Piece.GetType(PieceAtIndex(move.endSquare)) == Piece.Rook) {
                switch (move.startSquare) {
                    case 0:
                        castlingRights[1] = false;
                        break;
                    case 7:
                        castlingRights[0] = false;
                        break;
                    case 56:
                        castlingRights[3] = false;
                        break;
                    case 63:
                        castlingRights[2] = false;
                        break;
                }
            }
            plyCount++;
            if(IsInCheck()) {
                UndoMove(move);
                colorToMove = 1 - colorToMove;
                return false;
            } else {
                colorToMove = 1 - colorToMove;
                return true;
            }
}
        /// <summary>
        /// undoes a move by reading from the move's board state
        /// </summary>
        /// <param name="move"></param>
        public void UndoMove(Move move) {
            ReadBoardState(move.state); 
            plyCount--;
            colorToMove = 1 - colorToMove;
        } 
        /// <summary>
        /// Generates the bitboards
        /// </summary>
        public void GenerateBitboards(byte[] squares) {
            for(int index = 0; index < 64; index++) {
                if(squares[index] != Piece.None) {
                    AddPiece(index, squares[index]);
                }
                if(squares[index] == Piece.King || squares[index] == (Piece.King | Piece.White)) {
                    kingSquares[squares[index] == (Piece.King | Piece.White) ? 1 : 0] = (byte)index;
                } 
            }
        }
        /// <summary>
        /// Checks if the given square is under attack, using the various masks.
        /// </summary>
        /// <param name="square">The index of the square</param>
        /// <returns>Is the square under attack?</returns>
        public bool SquareIsAttackedByOpponent(int square) {
            ulong rook = MaskGen.GetRookAttacks(square, GetOccupiedBitboard());
            ulong bishop = MaskGen.GetBishopAttacks(square, GetOccupiedBitboard());
            ulong totalMask = rook & GetColoredPieceBitboard(1 - colorToMove, Piece.Rook);
            if(totalMask == 0) {
                totalMask |= bishop & GetColoredPieceBitboard(1 - colorToMove, Piece.Bishop);
            }
            if(totalMask == 0) {
                totalMask |= bishop & GetColoredPieceBitboard(1 - colorToMove, Piece.Queen);
            }
            if(totalMask == 0) {
                totalMask |= rook & GetColoredPieceBitboard(1 - colorToMove, Piece.Queen);
            }
            if(totalMask == 0) {
                totalMask |= MaskGen.GetKnightAttacks(square) & GetColoredPieceBitboard(1 - colorToMove, Piece.Knight);
            }
            if(totalMask == 0) {
                totalMask |= MaskGen.GetPawnCaptures(square, colorToMove) & GetColoredPieceBitboard(1 - colorToMove, Piece.Pawn);
            }
            if(totalMask == 0) {
                totalMask |= MaskGen.GetKingAttacks(square) & GetColoredPieceBitboard(1 - colorToMove, Piece.King);
            }
            return totalMask != 0;
        }
        /// <summary>
        /// Checks if the current color to move is under attack.
        /// </summary>
        /// <returns>Are you in check?</returns>
        public bool IsInCheck() {
            return SquareIsAttackedByOpponent(kingSquares[colorToMove]);
        }
        public byte PieceAtIndex(int index) {
            for(byte i = 0; i < 6; i++) {
                if((GetColoredPieceBitboard(0, i) & 1UL << index) != 0) {
                    return (byte)(i | Piece.Black);
                }
                if((GetColoredPieceBitboard(1, i) & 1UL << index) != 0) {
                    return (byte)(i | Piece.White);
                }
            }
            return Piece.None;
        }
        public void AddPiece(int index, byte type) {
            coloredBitboards[Piece.GetColor(type)] ^= 1UL << index; 
            pieceBitboards[Piece.GetType(type)] ^=  1UL << index;
        }
        public void RemovePiece(int index, byte type) {
            coloredBitboards[Piece.GetColor(type)] ^= 1UL << index; 
            pieceBitboards[Piece.GetType(type)] ^=  1UL << index;
        }
        public void MovePiece(int index1, byte type1, int index2, byte type2) {
            RemovePiece(index1, type1);
            if(Piece.GetType(type2) < Piece.None) {
                RemovePiece(index2, type2);
            }
            AddPiece(index2, type1);
        }
        public ulong GetColoredPieceBitboard(int color, byte piece) {
            return pieceBitboards[piece] & coloredBitboards[color];
        }
        public ulong GetOccupiedBitboard() {
            return coloredBitboards[0] | coloredBitboards[1];
        }
    }
} 
