namespace Chess {
    public class BoardFunctions {
        // basically subtracting the letter a from it to get a number from it because ascii
        public static int[] StartSquareFromMoveName(string name) {
            return new int[] {name[1] - '1', name[0] - 'a'};
        }
        public static int[] TargetSquareFromMoveName(string name) {
            return new int[] {name[3] - '1', name[2] - 'a'};
        }
        public static bool MoveIsPromotionFromName(string name) {
            return name.Length > 4;
        }
        public static string NameFromSquareLocation(int rank1, int file1, int rank2, int file2) {
            string name = "";
            name += (char)(file1 + 'a');
            name += rank1 + 1;
            name += (char)(file2 + 'a');
            name += rank2 + 1;
            return name;
        }
        public static string PromotionFromSquareLocation(int rank1, int file1, int rank2, int file2, PieceType promotionType) {
            string name = "";
            name += (char)(file1 + 'a');
            name += rank1 + 1;
            name += (char)(file2 + 'a');
            name += rank2 + 1;
            if(promotionType == PieceType.Knight) {
                name += 'n';
            } else if(promotionType == PieceType.Bishop) {
                name += 'b';
            } else if(promotionType == PieceType.Rook) {
                name += 'r';
            } else if(promotionType == PieceType.Queen) {
                name += 'q';
            }
            return name;
        }
        
        public static Piece GetPromotionPiece(string name, Square target, Piece piece) {
            if(piece.type == PieceType.Pawn) {
                if(name.Length < 4) {
                    Console.Error.Write("Move is not a promotion");
                    return new Piece(false, PieceType.None, target.rank, target.file);
                } else {
                    if(name[4] == 'n') {
                        return new Piece(piece.isWhite, PieceType.Knight, target.rank, target.file);
                    } else if(name[4] == 'b') {
                        return new Piece(piece.isWhite, PieceType.Bishop, target.rank, target.file);
                    } else if(name[4] == 'r') {
                        return new Piece(piece.isWhite, PieceType.Rook, target.rank, target.file);
                    } else if(name[4] == 'q') {
                        return new Piece(piece.isWhite, PieceType.Queen, target.rank, target.file);
                    } else {
                        Console.Error.Write("Piece To Promote To Is Invalid");
                        return new Piece(false, PieceType.None, target.rank, target.file);
                    }
                }
            } else {
                Console.Error.Write("Cannot Promote A Piece That Isn't A Pawn");
                return new Piece(false, PieceType.None, target.rank, target.file);
            }
        }
        public static void SwapSquares(ref Square[,] squares, int r1, int f1, int r2, int f2) {
            Square temp;
            temp = squares[r1,f1];
            squares[r1,f1] = squares[r2,f2];
            squares[r2,f2] = temp;
            squares[r1,f1].rank = r1;
            squares[r1,f1].file = f1;
            squares[r2,f2].rank = r2;
            squares[r2,f2].file = f2;
        }
    }
}