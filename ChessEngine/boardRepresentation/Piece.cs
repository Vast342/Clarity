using System;

namespace Chess {
    public struct Piece {
        public bool isWhite;
        public PieceType type;

        public bool isNull => type is PieceType.None;
        public bool isRook => type is PieceType.Rook;
        public bool isKnight => type is PieceType.Knight;
        public bool isBishop => type is PieceType.Bishop;
        public bool isQueen => type is PieceType.Queen;
        public bool isKing => type is PieceType.King;
        public bool isPawn => type is PieceType.Pawn;
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