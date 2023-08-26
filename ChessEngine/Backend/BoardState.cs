namespace Chess {
    public struct BoardState {
        public SquareState[] squares = new SquareState[4];
        public byte[] kingSquares = new byte[2];  
        public int enPassantIndex;
        public bool[] castlingRights = new bool[4];
        public int fiftyMoveCounter;    
        public ulong occupiedBitboard = 0;
        public ulong emptyBitboard = 0;
        public ulong[] coloredBitboards = new ulong[2];
        public ulong[,] coloredPieceBitboards = new ulong[2,7];
        public BoardState(Board board) {
            Array.Copy(board.kingSquares, kingSquares, 2);
            enPassantIndex = board.enPassantIndex;
            Array.Copy(board.castlingRights, castlingRights, 4);
            fiftyMoveCounter = board.fiftyMoveCounter;
            occupiedBitboard = board.occupiedBitboard;
            emptyBitboard = board.emptyBitboard;
            Array.Copy(board.coloredBitboards, coloredBitboards, 2);
            Array.Copy(board.coloredPieceBitboards, coloredPieceBitboards, 14);
        }
        public BoardState(BoardState state) {
            Array.Copy(state.kingSquares, kingSquares, 2);
            enPassantIndex = state.enPassantIndex;
            Array.Copy(state.castlingRights, castlingRights, 4);
            fiftyMoveCounter = state.fiftyMoveCounter;
            occupiedBitboard = state.occupiedBitboard;
            emptyBitboard = state.emptyBitboard;
            Array.Copy(state.coloredBitboards, coloredBitboards, 2);
            Array.Copy(state.coloredPieceBitboards, coloredPieceBitboards, 14);
        }
    }
    public struct SquareState {
        public SquareState() {
            index = 64;
            value = 0;
            initialized = false;
        }
        public SquareState(int i, byte v) {
            index = i;
            value = v;
            initialized = true;
        }
        public int index;
        public byte value;
        public bool initialized = false;
    }
}