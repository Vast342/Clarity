namespace Chess {
    public struct BoardState {
        public byte[] squares = new byte[64];
        public byte[] kingSquares = new byte[2];  
        public int enPassantIndex;
        public bool[] castlingRights = new bool[4];
        public int fiftyMoveCounter;    
        public BoardState(Board board) {
            Array.Copy(board.squares, squares, 64);
            Array.Copy(board.kingSquares, kingSquares, 2);
            enPassantIndex = board.enPassantIndex;
            Array.Copy(board.castlingRights, castlingRights, 4);
            fiftyMoveCounter = board.fiftyMoveCounter;
        }
        public BoardState(BoardState state) {
            Array.Copy(state.squares, squares, 64);
            Array.Copy(state.kingSquares, kingSquares, 2);
            enPassantIndex = state.enPassantIndex;
            Array.Copy(state.castlingRights, castlingRights, 4);
            fiftyMoveCounter = state.fiftyMoveCounter;
        }
    }
}