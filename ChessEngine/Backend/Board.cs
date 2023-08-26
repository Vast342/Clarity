using System.Runtime.CompilerServices;

namespace Chess {
    public class Board {
        // board specific valus
        public byte[] squares = new byte[64];
        public byte[] kingSquares = new byte[2];
        public int colorToMove = 1;
        public ulong occupiedBitboard = 0;
        public ulong emptyBitboard = 0;
        // bitboards of the entire color: 0 is black 1 is white
        public ulong[] coloredBitboards = new ulong[2];
        // bitboards of specific pieces of specific color, using the numbers above.
        public ulong[,] coloredPieceBitboards = new ulong[2,7];
        // the index of the en-passantable square, from 0-63, and if it's 64 no en passant is legal.
        public int enPassantIndex = 64;
        // white kingside, white queenside, black kingside, black kingside
        public bool[] castlingRights = new bool[4];
        public int plyCount = 0;
        public int fiftyMoveCounter;
        // random values needed later
        private readonly static ulong[,] zobTable = new ulong[64,15];
        public static readonly int[] directionalOffsets = {8, -8, 1, -1, 7, -7, 9, -9};
        /// <summary>
        /// Reads from a boardstate
        /// </summary>
        /// <param name="state">the boardstate in question</param>
        public void ReadBoardState(BoardState state) {
            Array.Copy(state.squares, squares, 64);
            Array.Copy(state.kingSquares, kingSquares, 2);
            enPassantIndex = state.enPassantIndex;
            Array.Copy(state.castlingRights, castlingRights, 4);
            fiftyMoveCounter = state.fiftyMoveCounter;
        }
        /// <summary>
        /// Establishes a new board instance, with the position and information contained within a FEN string.
        /// </summary>
        /// <param name="fen">The fen string in question.</param>
        public Board(string fen) {
            LoadFenToPosition(fen);
            UpdateBitboards();
            InitializeZobrist();
        }
        /// <summary>
        /// Loads the information contained within a string of Forsyth–Edwards Notation into the current board instance's values.
        /// </summary>
        /// <param name="fen">The FEN string in question.</param>
        public void LoadFenToPosition(string fen) {
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
                        squares[i] = Piece.Bishop | Piece.White;
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
            fiftyMoveCounter = int.Parse(parts[4]);
            plyCount = int.Parse(parts[5]) * 2 - colorToMove;
        }
        /// <summary>
        /// Outputs the fen string of the current position
        /// </summary>
        /// <returns>the fen string</returns>
        public string GetFenString() {
            string fen = "";

            for (int rank = 7; rank >= 0; rank--)
            {
                int numEmptyFiles = 0;
                for (int file = 0; file < 8; file++)
                {
                    int piece = squares[8*rank+file];
                    if (piece != 0)
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
        /// <summary>
        /// Generates a list of all the moves from the position, not including moves while in check. To filter the legal moves, check the return value of Board.MakeMove()
        /// </summary>
        /// <returns>A List of the legal moves</returns>
        public List<Move> GetLegalMoves() {
            BoardState state = new(this);
            List<Move> moves = new();
            // castlingi;
            if(castlingRights[0] && (occupiedBitboard & 0x60) == 0 && colorToMove == 1) {
                moves.Add(new Move(4, 6, 0, state));
            }
            if(castlingRights[1] && (occupiedBitboard & 0xE) == 0 && colorToMove == 1) {
                moves.Add(new Move(4, 2, 0, state));
            }
            if(castlingRights[2] && (occupiedBitboard & 0x6000000000000000) == 0 && colorToMove == 0) {
                moves.Add(new Move(60, 62, 0, state));
            }
            if(castlingRights[3] && (occupiedBitboard & 0xE00000000000000) == 0 && colorToMove == 0) {
                moves.Add(new Move(60, 58, 0, state));
            }
            // the rest of the pieces
            for(int startSquare = 0; startSquare < 64; startSquare++) {
                byte currentPiece = squares[startSquare];
                // checks if it's the right color
                if(Piece.GetColor(currentPiece) == colorToMove) {
                    // a check if it's a sliding piece
                    ulong total = 0;
                    ulong pawnCaptures = 0;
                    ulong pawnPushes = 0;
                    if(Piece.GetType(currentPiece) == Piece.Bishop) {
                        total |= MaskGen.GetBishopAttacks(startSquare, occupiedBitboard);
                    } else if(Piece.GetType(currentPiece) == Piece.Rook) {
                        total |= MaskGen.GetRookAttacks(startSquare, occupiedBitboard);
                    } else if(Piece.GetType(currentPiece) == Piece.Queen) {
                        total |= MaskGen.GetRookAttacks(startSquare, occupiedBitboard); 
                        total |= MaskGen.GetBishopAttacks(startSquare, occupiedBitboard);
                    } else if(Piece.GetType(currentPiece) == Piece.Pawn) {
                        pawnPushes |= MaskGen.GetPawnPushes(startSquare, colorToMove, emptyBitboard);
                        pawnCaptures |= MaskGen.GetPawnCaptures(startSquare, colorToMove);
                    } else if(Piece.GetType(currentPiece) == Piece.Knight) {
                        total |= MaskGen.GetKnightAttacks(startSquare);
                    } else if(Piece.GetType(currentPiece) == Piece.King) {
                        total |= MaskGen.GetKingAttacks(startSquare);
                    }
                    if(total != 0) {
                        for(int i = 0; i < 64; i++) {
                            if(BitboardOperations.AtLocation(total, i) && (Piece.GetColor(squares[i]) != colorToMove || Piece.GetType(squares[i]) == Piece.None)) {
                                moves.Add(new Move(startSquare, i, 0, new(this)));
                            }
                        }
                    } else if(Piece.GetType(currentPiece) == Piece.Pawn) {
                        for(int i = 0; i < 64; i++) {
                            if(i >> 3 == 7 * colorToMove) {
                                // promotions
                                if(BitboardOperations.AtLocation(pawnPushes, i) && Piece.GetType(squares[i]) == Piece.None) {
                                    for(int type = Piece.Knight; type < Piece.King; type++) { 
                                        moves.Add(new Move(startSquare, startSquare + directionalOffsets[1 - colorToMove], type, state));
                                    }
                                }
                            } else {
                                if(BitboardOperations.AtLocation(pawnPushes, i) && Piece.GetType(squares[i]) == Piece.None) {
                                    moves.Add(new Move(startSquare, i, 0, new(this)));
                                }
                                if(BitboardOperations.AtLocation(pawnCaptures, i) && ((Piece.GetColor(squares[i]) != colorToMove && Piece.GetType(squares[i]) != Piece.None) || i == enPassantIndex)) {
                                    moves.Add(new Move(startSquare, i, 0, new(this)));
                                }
                            }
                        }
                    }
                }
            }
            return moves;
        }
        /// <summary>
        /// Gets the captures from a position, used in Q Search.
        /// </summary>
        /// <returns>A List of capture moves</returns>
        public List<Move> GetLegalMovesQSearch() {
            BoardState state = new(this);
            List<Move> moves = new();
            // the rest of the pieces
            for(int startSquare = 0; startSquare < 64; startSquare++) {
                byte currentPiece = squares[startSquare];
                // checks if it's the right color
                if(Piece.GetColor(currentPiece) == colorToMove) {
                    // a check if it's a sliding piece
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
                    if(total != 0) {
                        for(int i = 0; i < 64; i++) {
                            if(BitboardOperations.AtLocation(total, i) && Piece.GetColor(squares[i]) != colorToMove) {
                                moves.Add(new Move(startSquare, i, 0, state));
                            }
                        }
                    } else if(Piece.GetType(currentPiece) == Piece.Pawn) {
                        for(int i = 0; i < 64; i++) {
                            if(BitboardOperations.AtLocation(pawnCaptures, i) && ((Piece.GetColor(squares[i]) != colorToMove && Piece.GetType(squares[i]) != Piece.None) || i == enPassantIndex)) {
                                moves.Add(new Move(startSquare, i, 0, state));
                            }
                        }
                    }
                }
            }
            return moves;
        }

        /// <summary>
        /// initializes a table used later for zobrist hashing, done automatically if you create the board using a fen string.
        /// </summary>
        public void InitializeZobrist() {
            var rng = new Random();
            for(int i = 0; i < 64; i++) {
                for(int j = 0; j < 15; j++) {
                    zobTable[i,j] = (ulong)rng.Next(0, int.MaxValue);
                }
            }
        }
        /// <summary>
        /// The function to generate a zobrist hash of the current position
        /// </summary>
        /// <returns>The zobrist hash</returns>
        public ulong CreateHash() {
            ulong hash = 0;
            for(int i = 0; i < 64; i++) {
                hash ^= zobTable[i, squares[i]];
            }
            return hash;
        }
        /// <summary>
        /// Makes a move on the board
        /// </summary>
        /// <param name="move">The move to be made</param>
        public bool MakeMove(Move move) {
            if(colorToMove != 1) {
                fiftyMoveCounter++;
            }
            // fifty move counter
            if(squares[move.endSquare] != 0 || (squares[move.startSquare] & 7) == Piece.Pawn) {
                fiftyMoveCounter = 0;
            }
            // for each start one set the start square to 0
            if(castlingRights[0] && move.startSquare == 4 && move.endSquare == 6) {
                // castling 1
                // switch 4 with 6 and 7 with 5
                squares[6] = squares[4];
                squares[4] = 0;
                squares[5] = squares[7];
                squares[7] = 0;
                castlingRights[0] = false;
                castlingRights[1] = false;
                // update the bitboards
                coloredPieceBitboards[1, Piece.King] = BitboardOperations.MovePiece(coloredPieceBitboards[1, Piece.King], 4, 6);
                coloredPieceBitboards[1, Piece.Rook] = BitboardOperations.MovePiece(coloredPieceBitboards[1, Piece.Rook], 7, 5);
            } else if(castlingRights[1] && move.startSquare == 4 && move.endSquare == 2) {
                // castling 
                // switch 4 with 2 and 0 with 3
                squares[2] = squares[4];
                squares[4] = 0;
                squares[3] = squares[0];
                squares[0] = 0;
                castlingRights[0] = false;
                castlingRights[1] = false;
                // update the bitboards
                coloredPieceBitboards[1, Piece.King] = BitboardOperations.MovePiece(coloredPieceBitboards[1, Piece.King], 4, 2);
                coloredPieceBitboards[1, Piece.Rook] = BitboardOperations.MovePiece(coloredPieceBitboards[1, Piece.Rook], 0, 3);
            } else if(castlingRights[2] && move.startSquare == 60 && move.endSquare == 62) {
                // castling 3
                // switch 60 with 62 and 63 with 61
                squares[62] = squares[60];
                squares[60] = 0;
                squares[61] = squares[63];
                squares[63] = 0;
                castlingRights[2] = false;
                castlingRights[3] = false;
                // update the bitboards
                coloredPieceBitboards[0, Piece.King] = BitboardOperations.MovePiece(coloredPieceBitboards[0, Piece.King], 60, 62);
                coloredPieceBitboards[0, Piece.Rook] = BitboardOperations.MovePiece(coloredPieceBitboards[0, Piece.Rook], 63, 61);
            } else if(castlingRights[3] && move.startSquare == 60 && move.endSquare == 58) {
                // castling 4
                // switch 60 with 58 and 56 with 59
                squares[58] = squares[60];
                squares[4] = 0;
                squares[59] = squares[56];
                squares[56] = 0;
                castlingRights[2] = false;
                castlingRights[3] = false;
                // update the bitboards
                coloredPieceBitboards[0, Piece.King] = BitboardOperations.MovePiece(coloredPieceBitboards[0, Piece.King], 60, 58);
                coloredPieceBitboards[0, Piece.Rook] = BitboardOperations.MovePiece(coloredPieceBitboards[0, Piece.Rook], 56, 59);
            } else if(move.endSquare != 0 && move.endSquare == enPassantIndex && Piece.GetType(squares[move.startSquare]) == Piece.Pawn) {
                // en passant
                // switch first square with second square and set the square either ahead or behind with colorToMove ? 8 : -8 to 0
                squares[move.endSquare] = squares[move.startSquare];
                squares[move.startSquare] = 0;
                squares[move.endSquare + (colorToMove == 1 ? -8 : 8)] = 0;
                enPassantIndex = 64;
                // update the bitboards
                coloredPieceBitboards[colorToMove, Piece.Pawn] = BitboardOperations.MovePiece(coloredPieceBitboards[colorToMove, Piece.Pawn], move.startSquare, move.endSquare);
                coloredPieceBitboards[1 - colorToMove, Piece.Pawn] = BitboardOperations.RemovePiece(coloredPieceBitboards[1 - colorToMove, Piece.Pawn], move.endSquare + (colorToMove == 1 ? -8 : 8));
            } else {
                // move  normally
                // switch first square with second
                if((squares[move.startSquare] & 7) == Piece.Pawn && move.endSquare == move.startSquare + (colorToMove == 1 ? 16 : -16)) {
                    // its a double move, make the first square the en passant index
                    enPassantIndex = move.startSquare + (colorToMove == 1 ? 8 : -8);
                } else {
                    enPassantIndex = 64;
                }
                int captureType = Piece.GetType(squares[move.endSquare]);
                squares[move.endSquare] = squares[move.startSquare];
                squares[move.startSquare] = 0;
                // update the bitboards
                coloredPieceBitboards[1 - colorToMove, captureType] = BitboardOperations.RemovePiece(coloredPieceBitboards[1 - colorToMove, captureType], move.endSquare);
                coloredPieceBitboards[colorToMove, Piece.GetType(squares[move.endSquare])] = BitboardOperations.MovePiece(coloredPieceBitboards[colorToMove, Piece.GetType(squares[move.endSquare])], move.startSquare, move.endSquare);
            }
            // promotions
            if(move.promotionType != 0) {
                squares[move.endSquare] = (byte)(move.promotionType + colorToMove * 8);
                // update the bitboards
                coloredPieceBitboards[colorToMove, Piece.Pawn] = BitboardOperations.RemovePiece(coloredPieceBitboards[colorToMove, Piece.Pawn], move.endSquare);
                coloredPieceBitboards[colorToMove, move.promotionType] = BitboardOperations.AddPiece(coloredPieceBitboards[colorToMove, move.promotionType], move.endSquare);
            }
            // king square updates
            if(Piece.GetColor(squares[move.startSquare]) == Piece.King) {
                kingSquares[colorToMove] = (byte)move.endSquare;
                castlingRights[colorToMove == 1 ? 0 : 2] = false;
                castlingRights[colorToMove == 1 ? 1 : 3] = false;
            }
            plyCount++;
            coloredBitboards[0] = coloredPieceBitboards[0,1] | coloredPieceBitboards[0,2] | coloredPieceBitboards[0,3] | coloredPieceBitboards[0,4] | coloredPieceBitboards[0,5] | coloredPieceBitboards[0,6];
            coloredBitboards[1] = coloredPieceBitboards[1,1] | coloredPieceBitboards[1,2] | coloredPieceBitboards[1,3] | coloredPieceBitboards[1,4] | coloredPieceBitboards[1,5] | coloredPieceBitboards[1,6];
            occupiedBitboard = coloredBitboards[0] | coloredBitboards[1];
            emptyBitboard = ~occupiedBitboard;
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
        /// Updates the bitboards, in a horribly inefficient manner
        /// </summary>
        public void UpdateBitboards() {
            occupiedBitboard = 0;
            coloredBitboards[0] = 0;
            coloredBitboards[1] = 0;
            for(int i = 0; i < 7; i++) {
                coloredPieceBitboards[0,i] = 0;
                coloredPieceBitboards[1,i] = 0;
            }
            for(int index = 0; index < 64; index++) {
                if(squares[index] != 0) {
                    coloredPieceBitboards[squares[index] >> 3, squares[index] & 7] |= (ulong)1 << index; 
                }
                if(squares[index] == 6 || squares[index] == 14) {
                    kingSquares[squares[index] == 14 ? 1 : 0] = (byte)index;
                } 
            }
            coloredBitboards[0] = coloredPieceBitboards[0,1] | coloredPieceBitboards[0,2] | coloredPieceBitboards[0,3] | coloredPieceBitboards[0,4] | coloredPieceBitboards[0,5] | coloredPieceBitboards[0,6];
            coloredBitboards[1] = coloredPieceBitboards[1,1] | coloredPieceBitboards[1,2] | coloredPieceBitboards[1,3] | coloredPieceBitboards[1,4] | coloredPieceBitboards[1,5] | coloredPieceBitboards[1,6];
            occupiedBitboard = coloredBitboards[0] | coloredBitboards[1];
            emptyBitboard = ~occupiedBitboard;
        }
        /// <summary>
        /// Checks if the given square is under attack, using the various masks.
        /// </summary>
        /// <param name="square">The index of the square</param>
        /// <returns>Is the square under attack?</returns>
        public bool SquareIsAttackedByOpponent(int square) {
            ulong rook = MaskGen.GetRookAttacks(square, occupiedBitboard);
            ulong bishop = MaskGen.GetBishopAttacks(square, occupiedBitboard);
            ulong totalMask = rook & coloredPieceBitboards[1 - colorToMove, Piece.Rook];
            if(totalMask == 0) {
                totalMask |= bishop & coloredPieceBitboards[1 - colorToMove, Piece.Bishop];
            }
            if(totalMask == 0) {
                totalMask |= bishop & coloredPieceBitboards[1 - colorToMove, Piece.Queen];
            }
            if(totalMask == 0) {
                totalMask |= rook & coloredPieceBitboards[1 - colorToMove, Piece.Queen];
            }
            if(totalMask == 0) {
                totalMask |= MaskGen.GetKnightAttacks(square) & coloredPieceBitboards[1 - colorToMove, Piece.Knight];
            }
            if(totalMask == 0) {
                totalMask |= MaskGen.GetPawnCaptures(square, colorToMove) & coloredPieceBitboards[1 - colorToMove, Piece.Pawn];
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
    }
} 