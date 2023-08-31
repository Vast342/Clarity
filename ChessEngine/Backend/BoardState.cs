namespace Chess {
    public struct BoardState {
        public byte[] kingSquares = new byte[2];  
        public int enPassantIndex;
        public bool[] castlingRights = new bool[4];
        public byte fiftyMoveCounter;    
        public BoardState(Board board) {
            Array.Copy(board.kingSquares, kingSquares, 2);
            enPassantIndex = board.enPassantIndex;
            Array.Copy(board.castlingRights, castlingRights, 4);
            fiftyMoveCounter = board.fiftyMoveCounter;
        }
        public BoardState(BoardState state) {
            Array.Copy(state.kingSquares, kingSquares, 2);
            enPassantIndex = state.enPassantIndex;
            Array.Copy(state.castlingRights, castlingRights, 4);
            fiftyMoveCounter = state.fiftyMoveCounter;
        }
    }
}