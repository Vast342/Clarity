namespace Chess {
    // I spent a decent while trying to replace this format but it's probably good for now
    // potential optimisations could include detecting if it's en passant, or writing that the move was en passant when it's made, and updating the  board that way instead of through the BoardState
    public struct Move {
        public byte startSquare;
        public byte endSquare;
        public byte promotionType = Piece.None;
        public BoardState state;
        /// <summary>
        /// Makes a new move with the starting and ending squares.
        /// </summary>
        /// <param name="start">Starting square of the move</param>
        /// <param name="end">Ending square of the move</param>
        /// <param name="pType">The promotion piece type, if any</param>
        public Move(byte start, byte end, byte pType, BoardState s) {
            startSquare = start;
            endSquare = end;
            promotionType = pType;
            state = s;
        }
        /// <summary>
        /// Converts the move to long algebraic form
        /// </summary>
        /// <returns>the long algebraic form</returns>
        public string ConvertToLongAlgebraic() {
            int startRank = startSquare >> 3;
            int startFile = startSquare & 7;
            int endRank = endSquare >> 3;
            int endFile = endSquare & 7;
            string name = "";
            name += (char)(startFile + 'a');
            name += startRank + 1;
            name += (char)(endFile + 'a');
            name += endRank + 1;
            switch(Piece.GetType((byte)promotionType)) {
                    case Piece.Knight:
                        name += 'n';
                        break;
                    case Piece.Bishop:
                        name += 'b';
                        break;
                    case Piece.Rook:
                        name += 'r';
                        break;
                    case Piece.Queen:
                        name += 'q';
                        break;
                }
            return name;
        }
        public Move(string longAlgebraicForm, Board board) {
            char[] chars = longAlgebraicForm.ToCharArray();
            byte startRank = (byte)(chars[1] - '1');
            byte startFile = (byte)(chars[0] - 'a');
            byte endRank = (byte)(chars[3] - '1');
            byte endFile = (byte)(chars[2] - 'a');
            if(chars.Length > 4) {
                switch(chars[4]) {
                    case 'n':
                        promotionType = (byte)(Piece.Knight | (board.colorToMove == 1 ? Piece.White : Piece.Black));
                        break;
                    case 'b':
                        promotionType = (byte)(Piece.Bishop | (board.colorToMove == 1 ? Piece.White : Piece.Black));
                        break;
                    case 'r':
                        promotionType = (byte)(Piece.Rook | (board.colorToMove == 1 ? Piece.White : Piece.Black));
                        break;
                    case 'q':
                        promotionType = (byte)(Piece.Queen | (board.colorToMove == 1 ? Piece.White : Piece.Black));
                        break;
                }
            }
            startSquare = (byte)(startRank * 8 + startFile);
            endSquare = (byte)(endRank * 8 + endFile);

            state = new(board);
        }
        /// <summary>
        /// Checks if the move is a capture on it's board
        /// </summary>
        /// <param name="board">The board the move is used on</param>
        /// <returns>is it a capture?</returns>
        public bool IsCapture(Board board) {
            return board.PieceAtIndex(endSquare) != Piece.None;
        }
        /// <summary>
        /// Converts the move to a number, to be used by the move table
        /// </summary>
        /// <returns></returns>
        public int ToNumber() {
            return (promotionType << 12) | (startSquare << 6) | endSquare;
        }
        public static Move NullMove => new Move(0, 0, 0, new(new Board("8/8/8/8/8/8/8/8 - - - 0 1")));
        public static bool Equals(Move move1, Move move2) {
            return move1.startSquare == move2.startSquare && move1.endSquare == move2.endSquare && move1.promotionType == move2.promotionType;
        }
    }
}