using System;

namespace Chess {
    public struct Piece {
        public bool isWhite;
        public PieceType type;

        public bool IsNull => type is PieceType.None;
        public bool IsRook => type is PieceType.Rook;
        public bool IsKnight => type is PieceType.Knight;
        public bool IsBishop => type is PieceType.Bishop;
        public bool IsQueen => type is PieceType.Queen;
        public bool IsKing => type is PieceType.King;
        public bool IsPawn => type is PieceType.Pawn;
        public int rank;
        public int file;
        public int index;
        public ulong legalMoves;
        public Piece(bool w, PieceType t, int r, int f) {
            isWhite = w;
            type = t;
            rank = r;
            file = f;
            index = 8*r + f;
        }
    }
}