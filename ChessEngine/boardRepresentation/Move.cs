namespace Chess {
    public class Move {
        public string name;
        public Square targetSquare;
        public Square startSquare;
        public Piece piece;
        public Piece capturedPiece;
        public Piece promotionPiece;
        public bool isCapture;
        public bool isPromotion;
        public Move(string n, Board board) {
            name = n;
            int[] sq = BoardFunctions.StartSquareFromMoveName(n);
            startSquare = board.squares[sq[1], sq[0]];
            piece = startSquare.piece;
            sq = BoardFunctions.TargetSquareFromMoveName(n);
            targetSquare = board.squares[sq[1], sq[0]];
            if(targetSquare.piece.type != PieceType.None) {
                isCapture = true;
                capturedPiece = targetSquare.piece;
            }
            isPromotion = BoardFunctions.MoveIsPromotionFromName(n);
            if(isPromotion) {
                promotionPiece = BoardFunctions.GetPromotionPiece(n, targetSquare, piece);
            }
        }
    }
}