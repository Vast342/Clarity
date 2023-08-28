namespace Chess {
    public struct BoardState {
        public byte[] kingSquares = new byte[2];  
        public int enPassantIndex;
        public bool[] castlingRights = new bool[4];
        public int fiftyMoveCounter;    
        public ulong[] coloredBitboards = new ulong[2];
        public ulong[] pieceBitboards = new ulong[6];
        public BoardState(Board board) {
            Array.Copy(board.kingSquares, kingSquares, 2);
            enPassantIndex = board.enPassantIndex;
            Array.Copy(board.castlingRights, castlingRights, 4);
            fiftyMoveCounter = board.fiftyMoveCounter;
            Array.Copy(board.coloredBitboards, coloredBitboards, 2);
            Array.Copy(board.pieceBitboards, pieceBitboards, 6);
        }
        public BoardState(BoardState state) {
            Array.Copy(state.kingSquares, kingSquares, 2);
            enPassantIndex = state.enPassantIndex;
            Array.Copy(state.castlingRights, castlingRights, 4);
            fiftyMoveCounter = state.fiftyMoveCounter;
            Array.Copy(state.coloredBitboards, coloredBitboards, 2);
            Array.Copy(state.pieceBitboards, pieceBitboards, 6);
        }
    }
}