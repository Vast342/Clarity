namespace Chess {
    public struct Board {
        public Square[,] squares = new Square[8,8];
        public Square[] kingSquares = new Square[2];
        private readonly static ulong[,,] zobTable = new ulong[8,8,13];
        public bool isWhiteToMove;
        public int fiftyMoveCounter = 0;
        public int plyCount;
        public bool[] castlingRights = new bool[4];
        // 8 for black's pieces, 8 for white pieces
        public bool[] enPassantRights = new bool[16];
        public bool canEnPassant;
        public ulong occupiedBitboard = 0;
        public ulong[] pieceBitboards = new ulong[14];
        // first is white, second is black
        public ulong[] attackBitboards = new ulong[2];
        public ulong whiteBitboard = 0;
        public ulong blackBitboard = 0;
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
                            squares[j >> 3, j & 7] = new Square(j, PieceType.None, true);
                        }
                        i += c - '0';
                    }
                }
            }
            // whose turn it is
            isWhiteToMove = parts[1] == "w";
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
            // en passant rights
            if(parts[3] != "-") {
                int j = 0;
                int index = 0;
                foreach(char c in parts[3]) {
                    if(j == 0) {
                        index = c - 'a';
                    } else {
                        index += c == '3' ? 8 : 0;
                    }
                    j++;
                }
                enPassantRights[index] = true;
            }
            // fifty move counter and move counter
            fiftyMoveCounter = int.Parse(parts[4]);
            plyCount = int.Parse(parts[5]) * 2 - (isWhiteToMove ? 0 : 1);
            UpdateBitboards(1);
            InitializeZobrist();
        }
        public readonly int PieceIndexForHash(Piece p) {
            return (int)p.type + (p.isWhite ? 0 : 6);
        }
        public readonly void InitializeZobrist() {
            var rng = new Random();
            for(int i = 0; i < 8; i++) {
                for(int j = 0; j < 8; j++) {
                    for(int k = 1; k <= 12; k++) {
                        zobTable[i,j,k] = (ulong)rng.Next(0, (int)Math.Pow(2, 64)-1);
                    }
                }
            }
        }
        public readonly ulong ZobristHash() {
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
            // En Passant, nothing yet
            fen += ' ';
            if(canEnPassant) {
                for(int i = 0; i < 16; i++) {
                    if(enPassantRights[i]) {
                        if(i < 8) {
                            fen += (char)(i + 'a');
                            fen += '6';
                        } else {
                            fen += (char)(i - 8 + 'a');
                            fen += '3';
                        }
                    }
                }
            } else {
                fen += '-';
            }

            // 50 move counter
            fen += ' ';
            fen += fiftyMoveCounter;

            // Full-move count (should be one at start, and increase after each move by black)
            fen += ' ';
            fen += plyCount / 2 + (isWhiteToMove ? 1 : 0);

            return fen;
        }
        public void MakeMove(Move move) {
            // reset en passant rights
            for(int i = 0; i < 16; i++) {
                enPassantRights[i] = false;
                canEnPassant = false;
            }
            // add to 50 move counter
            fiftyMoveCounter++;
            // castling logic
            if(move.isCastle) {
                if(move.castleType == 1) {
                    BoardFunctions.SwapSquares(ref squares, 0, 4, 0, 6);
                    BoardFunctions.SwapSquares(ref squares, 0, 7, 0, 5);
                    castlingRights[0] = false;
                    castlingRights[1] = false;
                } else if(move.castleType == 2) {
                    BoardFunctions.SwapSquares(ref squares, 0, 4, 0, 2);
                    BoardFunctions.SwapSquares(ref squares, 0, 0, 0, 3);
                    castlingRights[0] = false;
                    castlingRights[1] = false;
                } else if(move.castleType == 3) {
                    BoardFunctions.SwapSquares(ref squares, 7, 4, 7, 6);
                    BoardFunctions.SwapSquares(ref squares, 7, 7, 7, 5);
                    castlingRights[2] = false;
                    castlingRights[3] = false;
                } else if(move.castleType == 4) {
                    BoardFunctions.SwapSquares(ref squares, 7, 4, 7, 2);
                    BoardFunctions.SwapSquares(ref squares, 7, 0, 7, 3);
                    castlingRights[2] = false;
                    castlingRights[3] = false;
                }
            } else if(move.isEnPassant) {
                // does en passant
                squares[move.enPassantSquare.rank, move.enPassantSquare.file] = new Square(move.enPassantSquare.rank, move.enPassantSquare.file, PieceType.None, false);
                BoardFunctions.SwapSquares(ref squares, move.startSquare.rank, move.startSquare.file, move.targetSquare.rank, move.targetSquare.file);
            } else {
                // does a regular move
                BoardFunctions.SwapSquares(ref squares, move.startSquare.rank, move.startSquare.file, move.targetSquare.rank, move.targetSquare.file);
            }
            // checks if it was a capture
            if(move.isCapture && !move.isEnPassant) {
                squares[move.startSquare.rank, move.startSquare.file].piece = new Piece(true, PieceType.None, move.startSquare.rank, move.startSquare.file);
                fiftyMoveCounter = 0;
            }
            if(move.isPromotion) {
                squares[move.targetSquare.rank, move.targetSquare.file].piece = move.promotionPiece;
            }
            // if it's a king move that side can't castle anymore
            if(move.piece.type == PieceType.King && move.piece.isWhite && !move.isCastle && (castlingRights[0] || castlingRights[1])) {
                castlingRights[0] = false;
                castlingRights[1] = false;
            }
            if(move.piece.type == PieceType.King && !move.piece.isWhite && !move.isCastle && (castlingRights[0] || castlingRights[1])) {
                castlingRights[2] = false;
                castlingRights[3] = false;
            }
            // rook move and rook detection (if it's moved off its starting square or has been captured it will revoke the castling rights)
            if(move.piece.type == PieceType.Rook && move.startSquare.file == 0 && castlingRights[1] && move.piece.isWhite) {
                castlingRights[1] = false;
            }
            if((pieceBitboards[4] & 0b1UL) == 0 && castlingRights[1]) {
                castlingRights[1] = false;
            }
            if(move.piece.type == PieceType.Rook && move.startSquare.file == 7 && castlingRights[0] && move.piece.isWhite) {
                castlingRights[0] = false;
            }
            if((pieceBitboards[4] & 0b1UL << 7) == 0 && castlingRights[0]) {
                castlingRights[0] = false;
            }
            if(move.piece.type == PieceType.Rook && move.startSquare.file == 0 && castlingRights[2] && !move.piece.isWhite) {
                castlingRights[2] = false;
            }
            if((pieceBitboards[11] & 0b1UL << 56) == 0 && castlingRights[3]) {
                castlingRights[3] = false;
            }
            if(move.piece.type == PieceType.Rook && move.startSquare.file == 7 && castlingRights[3] && !move.piece.isWhite) {
                castlingRights[3] = false;
            }
            if((pieceBitboards[11] & 0b1UL << 63) == 0 && castlingRights[2]) {
                castlingRights[2] = false;
            }
            // if it's a pawn move reset the 50 move count
            if(move.piece.type == PieceType.Pawn) {
                fiftyMoveCounter = 0;
                // update en passant rights
                if(Math.Abs(move.startSquare.rank - move.targetSquare.rank) > 1) {
                    enPassantRights[move.startSquare.file + (move.piece.isWhite ? 8 : 0)] = true;
                    canEnPassant = true;
                }
            }
            plyCount++;
            isWhiteToMove = !isWhiteToMove;
            UpdateBitboards(2);
        }
        public void UndoMove(Move move) {
            // reset en passant rights
            for(int i = 0; i < 16; i++) {
                enPassantRights[i] = false;
                canEnPassant = false;
            }
            // add to 50 move counter
            fiftyMoveCounter--;
            // castling logic
            if(move.isCastle) {
                if(move.castleType == 1) {
                    BoardFunctions.SwapSquares(ref squares, 0, 4, 0, 6);
                    BoardFunctions.SwapSquares(ref squares, 0, 7, 0, 5);
                    castlingRights[0] = false;
                    castlingRights[1] = false;
                } else if(move.castleType == 2) {
                    BoardFunctions.SwapSquares(ref squares, 0, 4, 0, 2);
                    BoardFunctions.SwapSquares(ref squares, 0, 0, 0, 3);
                    castlingRights[0] = false;
                    castlingRights[1] = false;
                } else if(move.castleType == 3) {
                    BoardFunctions.SwapSquares(ref squares, 7, 4, 7, 6);
                    BoardFunctions.SwapSquares(ref squares, 7, 7, 7, 5);
                    castlingRights[2] = false;
                    castlingRights[3] = false;
                } else if(move.castleType == 4) {
                    BoardFunctions.SwapSquares(ref squares, 7, 4, 7, 2);
                    BoardFunctions.SwapSquares(ref squares, 7, 0, 7, 3);
                    castlingRights[2] = false;
                    castlingRights[3] = false;
                }
            } else if(move.isEnPassant) {
                // does en passant
                squares[move.enPassantSquare.rank, move.enPassantSquare.file] = new Square(move.enPassantSquare.rank, move.enPassantSquare.file, PieceType.None, false);
                BoardFunctions.SwapSquares(ref squares, move.startSquare.rank, move.startSquare.file, move.targetSquare.rank, move.targetSquare.file);
            } else {
                // does a regular move
                BoardFunctions.SwapSquares(ref squares, move.startSquare.rank, move.startSquare.file, move.targetSquare.rank, move.targetSquare.file);
            }
            // checks if it was a capture
            if(move.isCapture && !move.isEnPassant) {
                squares[move.targetSquare.rank, move.targetSquare.file].piece = new Piece(true, move.capturedPiece.type, move.startSquare.rank, move.startSquare.file);
            }
            if(move.isPromotion) {
                squares[move.targetSquare.rank, move.targetSquare.file].piece = move.promotionPiece;
            }
            // if it's a king move that side can't castle anymore
            if(move.piece.type == PieceType.King && move.piece.isWhite && !move.isCastle && (castlingRights[0] || castlingRights[1])) {
                castlingRights[0] = false;
                castlingRights[1] = false;
            }
            if(move.piece.type == PieceType.King && !move.piece.isWhite && !move.isCastle && (castlingRights[0] || castlingRights[1])) {
                castlingRights[2] = false;
                castlingRights[3] = false;
            }
            // rook move and rook detection (if it's moved off its starting square or has been captured it will revoke the castling rights)
            if(move.piece.type == PieceType.Rook && move.startSquare.file == 0 && castlingRights[1] && move.piece.isWhite) {
                castlingRights[1] = false;
            }
            if((pieceBitboards[4] & 0b1UL) == 0 && castlingRights[1]) {
                castlingRights[1] = false;
            }
            if(move.piece.type == PieceType.Rook && move.startSquare.file == 7 && castlingRights[0] && move.piece.isWhite) {
                castlingRights[0] = false;
            }
            if((pieceBitboards[4] & 0b1UL << 7) == 0 && castlingRights[0]) {
                castlingRights[0] = false;
            }
            if(move.piece.type == PieceType.Rook && move.startSquare.file == 0 && castlingRights[2] && !move.piece.isWhite) {
                castlingRights[2] = false;
            }
            if((pieceBitboards[11] & 0b1UL << 56) == 0 && castlingRights[3]) {
                castlingRights[3] = false;
            }
            if(move.piece.type == PieceType.Rook && move.startSquare.file == 7 && castlingRights[3] && !move.piece.isWhite) {
                castlingRights[3] = false;
            }
            if((pieceBitboards[11] & 0b1UL << 63) == 0 && castlingRights[2]) {
                castlingRights[2] = false;
            }
            // if it's a pawn move reset the 50 move count
            if(move.piece.type == PieceType.Pawn) {
                fiftyMoveCounter = 0;
                // update en passant rights
                if(Math.Abs(move.startSquare.rank - move.targetSquare.rank) > 1) {
                    enPassantRights[move.startSquare.file + (move.piece.isWhite ? 8 : 0)] = true;
                    canEnPassant = true;
                }
            }
            plyCount++;
            isWhiteToMove = !isWhiteToMove;
            UpdateBitboards(2);
        }
        public readonly Square GetSquareFromIndex(int i) {
            return squares[i >> 3, i & 7];
        }
        public readonly Square GetSquareFromPosition(int r, int f) {
            return squares[r, f];
        }
        public void UpdateBitboards(int type) {
            // if type = 1, it's a full regeneration
            // if type = 2, a singular move happened.
            // nothing is done for now though, it just fully regenerates it
            occupiedBitboard = 0;
            if(type != 0) {
                int i = 0;
                foreach(Square square in squares) {
                    pieceBitboards[(int)square.piece.type + (square.piece.isWhite ? 0 : 7)] += ((ulong)1) << i;
                    i++;
                }
                occupiedBitboard |= pieceBitboards[1] | pieceBitboards[2] | pieceBitboards[3] | pieceBitboards[4] | pieceBitboards[5] | pieceBitboards[6] | pieceBitboards[8] | pieceBitboards[9] | pieceBitboards[10] | pieceBitboards[11] | pieceBitboards[12] | pieceBitboards[13]; 
                whiteBitboard |= pieceBitboards[1] | pieceBitboards[2] | pieceBitboards[3] | pieceBitboards[4] | pieceBitboards[5] | pieceBitboards[6];
                blackBitboard |= pieceBitboards[8] | pieceBitboards[9] | pieceBitboards[10] | pieceBitboards[11] | pieceBitboards[12] | pieceBitboards[13];
            }
        }
        // north east south west northeast northwest southwest southeast.
        readonly int[] directionOffsetsRank = {1, 0, -1, 0, 1, 1, -1, -1};
        readonly int[] directionOffsetsFile = {0, 1, 0, -1, 1, -1, -1, 1};
        readonly int[] knightOffsetsRank = {2, 2, 1, -1, -2, -2, 1, -1};
        readonly int[] knightOffsetsFile = {1, -1, -2, -2, 1, -1, 2, 2};
        public readonly Move[] GetLegalMoves() {
            List<Move> moves = new();
            // detect possible castles
            if(castlingRights[0] && (occupiedBitboard & 0x60) == 0) {
                moves.Add(new Move("e1g1", this));
            }
            if(castlingRights[1] && (occupiedBitboard & 0xE) == 0) {
                moves.Add(new Move("e1b1", this));
            }
            if(castlingRights[2] && (occupiedBitboard & 0x6000000000000000) == 0) {
                moves.Add(new Move("e8g8", this));
            }
            if(castlingRights[3] && (occupiedBitboard & 0xE00000000000000) == 0) {
                moves.Add(new Move("e8b8", this));
            }
            // other pieces
            for(int rank = 0; rank < 8; rank++) {
                attackBitboards[isWhiteToMove ? 1 : 0] = 0;
                for(int file = 0; file < 8; file++) {
                    Square currentSquare = squares[rank, file];
                    Piece currentPiece = currentSquare.piece;
                    if(currentPiece.isWhite == isWhiteToMove && currentPiece.type != PieceType.None) {
                        // long slidey piece moves
                        if(currentPiece.IsBishop || currentPiece.IsRook || currentPiece.IsQueen) {
                            int startDirection = currentPiece.IsBishop ? 4 : 0;
                            int endDirection = currentPiece.IsRook ? 4 : 8;
                            for(int directionIndex = startDirection; directionIndex < endDirection; directionIndex++) {
                                for(int i = 1; i < 8; i++) {
                                    int targetRank = rank + directionOffsetsRank[directionIndex] * i;
                                    int targetFile = file + directionOffsetsFile[directionIndex] * i;
                                    if(targetRank > -1 && targetRank < 8 && targetFile > -1 && targetFile < 8) {
                                        Square targetSquare = squares[targetRank, targetFile];
                                        if(targetSquare.piece.isWhite == isWhiteToMove && !targetSquare.piece.IsNull) {
                                            break;
                                        }
                                        moves.Add(new Move(BoardFunctions.NameFromSquareLocation(rank, file, targetSquare.rank, targetSquare.file), this));
                                        if(targetSquare.piece.isWhite != isWhiteToMove && !targetSquare.piece.IsNull) {
                                            break;
                                        }
                                    } else {
                                        break;
                                    }
                                }
                            }
                        }
                        // Knights
                        if(currentPiece.IsKnight) {
                            for(int directionIndex = 0; directionIndex < 8; directionIndex++) {
                                int targetRank = rank + knightOffsetsRank[directionIndex];
                                int targetFile = file + knightOffsetsFile[directionIndex];
                                if(targetRank > -1 && targetRank < 8 && targetFile > -1 && targetFile < 8) {
                                    if(squares[targetRank, targetFile].piece.type != PieceType.None) {
                                        if(squares[targetRank, targetFile].piece.isWhite != isWhiteToMove) {
                                            moves.Add(new Move(BoardFunctions.NameFromSquareLocation(rank, file, targetRank, targetFile), this));
                                        }
                                    } else {
                                        moves.Add(new Move(BoardFunctions.NameFromSquareLocation(rank, file, targetRank, targetFile), this));
                                    }
                                }
                            }
                        }
                        // Pawns
                        if(currentPiece.IsPawn) {
                            // NOTE TO SELF REMEMBER PROMOTION
                            // Future me: I didn't.
                            if(squares[rank + (isWhiteToMove ? 1 : -1), file].piece.type == PieceType.None) {
                                if(currentSquare.rank == (currentPiece.isWhite ? 1 : 6) && squares[rank + 2, file].piece.type == PieceType.None) {
                                    moves.Add(new Move(BoardFunctions.NameFromSquareLocation(rank, file, rank+(isWhiteToMove ? 2 : -2), file), this));
                                }
                                if(rank+(isWhiteToMove ? 1 : -1) == (isWhiteToMove ? 7 : 0)) {
                                    for(PieceType pt = PieceType.Knight; pt <= PieceType.Queen; pt++) {
                                        moves.Add(new Move(BoardFunctions.PromotionFromSquareLocation(rank, file, rank+(isWhiteToMove ? 1 : -1), file, pt), this));
                                    }
                                } else {
                                    moves.Add(new Move(BoardFunctions.NameFromSquareLocation(rank, file, rank+(isWhiteToMove ? 1 : -1), file), this));
                                }
                            }
                            // captures
                            if(rank > 0 && rank < 7 && file > 0 && file < 7) {
                                if(squares[rank + (isWhiteToMove ? 1 : -1), file + 1].piece.type != PieceType.None) {
                                    moves.Add(new Move(BoardFunctions.NameFromSquareLocation(rank, file, rank+(isWhiteToMove ? 1 : -1), file + 1), this));
                                }
                                if(squares[rank + (isWhiteToMove ? 1 : -1), file - 1].piece.type != PieceType.None) {
                                    moves.Add(new Move(BoardFunctions.NameFromSquareLocation(rank, file, rank+(isWhiteToMove ? 1 : -1), file - 1), this));
                                }
                            }
                            // En Passant
                            for(int i = -1; i < 2; i += 2) {
                                if(rank + i > 0 && rank + i < 7 && file + i > 0 && file + i < 7) {
                                    if(squares[rank, file + i].piece.IsPawn && squares[rank, file + i].piece.isWhite != isWhiteToMove && enPassantRights[file + i + (isWhiteToMove ? 0 : 8)]) {
                                        moves.Add(new Move(BoardFunctions.NameFromSquareLocation(rank, file, rank + (isWhiteToMove ? 1 : -1), file + i), this));
                                    }
                                }
                            }
                        }
                        // King
                        if(currentPiece.IsKing) {
                            for(int directionIndex = 0; directionIndex < 8; directionIndex++) {
                                int targetRank = rank + directionOffsetsRank[directionIndex];
                                int targetFile = file + directionOffsetsFile[directionIndex];
                                if(targetRank > -1 && targetRank < 8 && targetFile > -1 && targetFile < 8) {
                                    Square targetSquare = squares[targetRank, targetFile];
                                    if(targetSquare.piece.isWhite == isWhiteToMove && !targetSquare.piece.IsNull) {
                                        break;
                                    }
                                    moves.Add(new Move(BoardFunctions.NameFromSquareLocation(rank, file, targetSquare.rank, targetSquare.file), this));
                                    if(targetSquare.piece.isWhite != isWhiteToMove && !targetSquare.piece.IsNull) {
                                        break;
                                    }
                                } else {
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            return moves.ToArray();
        }
    }
}