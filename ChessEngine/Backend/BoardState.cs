namespace Chess {
    public struct BoardState {
        public byte kingSquareWhite;
        public byte kingSquareBlack;
        public int enPassantIndex;
        // the 4 castlings
        public bool wk;
        public bool wq;
        public bool bk;
        public bool bq;
        public byte fiftyMoveCounter;    
        public ulong whiteBitboard;
        public ulong blackBitboard;
        public ulong pawnBitboard;
        public ulong knightBitboard;
        public ulong bishopBitboard;
        public ulong rookBitboard;
        public ulong queenBitboard;
        public ulong kingBitboard;
        public ulong zobristHash;
        public BoardState(Board board) {
            kingSquareBlack = board.kingSquares[0];
            kingSquareWhite = board.kingSquares[1];
            enPassantIndex = board.enPassantIndex;
            wk = board.castlingRights[0];
            wq = board.castlingRights[1];
            bk = board.castlingRights[2];
            bq = board.castlingRights[3];
            fiftyMoveCounter = board.fiftyMoveCounter;
            blackBitboard = board.coloredBitboards[0];
            whiteBitboard = board.coloredBitboards[1];
            pawnBitboard = board.pieceBitboards[Piece.Pawn];
            knightBitboard = board.pieceBitboards[Piece.Knight];
            bishopBitboard = board.pieceBitboards[Piece.Bishop];
            rookBitboard = board.pieceBitboards[Piece.Rook];
            queenBitboard = board.pieceBitboards[Piece.Queen];
            kingBitboard = board.pieceBitboards[Piece.King];
            zobristHash = board.zobristHash;
        }
    }
}