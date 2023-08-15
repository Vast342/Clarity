using Chess;

public class Tests {
    public static void BackendTests() {
        Console.WriteLine("IT'S ALIVE!!!");
        Board board = new("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        List<Move> moves = board.GetLegalMoves();
        if(moves.Count == 20) {
            Console.WriteLine("Move count test passed");
        } else {
            Console.WriteLine("Move count test Failed, outputted " + moves.Count + " legal moves");
        }
        // everything
        if(board.occupiedBitboard == 18446462598732906495) {
            Console.WriteLine("Occupied bitboard test passed");
        } else {
            Console.WriteLine("Occupied bitboard test failed, outputted " + board.occupiedBitboard);
        }
        // pawns
        if(board.coloredPieceBitboards[1, 0] == 0b1111111100000000) {
            Console.WriteLine("White pawn bitboard test passed");
        } else {
            Console.WriteLine("White pawn bitboard test failed, outputted " + board.coloredPieceBitboards[1,0]);
        }
        if(board.coloredPieceBitboards[0, 0] == 0b0000000011111111000000000000000000000000000000000000000000000000) {
            Console.WriteLine("Black pawn bitboard test passed");
        } else {
            Console.WriteLine("Black pawn bitboard test failed, outputted " + board.coloredPieceBitboards[0,0]);
        }
        // knights
        if(board.coloredPieceBitboards[1, 1] == 0b01000010) {
            Console.WriteLine("White knight bitboard test passed");
        } else {
            Console.WriteLine("White knight bitboard test failed, outputted " + board.coloredPieceBitboards[1,1]);
        }
        if(board.coloredPieceBitboards[0, 1] == 0b0100001000000000000000000000000000000000000000000000000000000000) {
            Console.WriteLine("Black knight bitboard test passed");
        } else {
            Console.WriteLine("Black knight bitboard test failed, outputted " + board.coloredPieceBitboards[0,1]);
        }
        // bishops
        if(board.coloredPieceBitboards[1, 2] == 0b00100100) {
            Console.WriteLine("White bishop bitboard test passed");
        } else {
            Console.WriteLine("White bishop bitboard test failed, outputted " + board.coloredPieceBitboards[1,2]);
        }
        if(board.coloredPieceBitboards[0, 2] == 0b0010010000000000000000000000000000000000000000000000000000000000) {
            Console.WriteLine("Black bishop bitboard test passed");
        } else {
            Console.WriteLine("Black bishop bitboard test failed, outputted " + board.coloredPieceBitboards[0,2]);
        }
        // rooks
        if(board.coloredPieceBitboards[1, 3] == 0b10000001) {
            Console.WriteLine("White rook bitboard test passed");
        } else {
            Console.WriteLine("White rook bitboard test failed, outputted " + board.coloredPieceBitboards[1,3]);
        }
        if(board.coloredPieceBitboards[0, 3] == 0b1000000100000000000000000000000000000000000000000000000000000000) {
            Console.WriteLine("Black rook bitboard test passed");
        } else {
            Console.WriteLine("Black rook bitboard test failed, outputted " + board.coloredPieceBitboards[0,3]);
        }
        // queens
        if(board.coloredPieceBitboards[1, 4] == 0b00001000) {
            Console.WriteLine("White queen bitboard test passed");
        } else {
            Console.WriteLine("White queen bitboard test failed, outputted " + board.coloredPieceBitboards[1,4]);
        }
        if(board.coloredPieceBitboards[0, 4] == 0b0000100000000000000000000000000000000000000000000000000000000000) {
            Console.WriteLine("Black queen bitboard test passed");
        } else {
            Console.WriteLine("Black queen bitboard test failed, outputted " + board.coloredPieceBitboards[0,4]);
        }
        // kings
        if(board.coloredPieceBitboards[1, 5] == 0b00010000) {
            Console.WriteLine("White king bitboard test passed");
        } else {
            Console.WriteLine("White king bitboard test failed, outputted " + board.coloredPieceBitboards[1,5]);
        }
        if(board.coloredPieceBitboards[0, 5] == 0b0001000000000000000000000000000000000000000000000000000000000000) {
            Console.WriteLine("Black king bitboard test passed");
        } else {
            Console.WriteLine("Black king bitboard test failed, outputted " + board.coloredPieceBitboards[0,5]);
        }
        if(board.GetFenString() == "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") {
            Console.WriteLine("Fen encoder test passed");
        } else {
            Console.WriteLine("Fen encoder test failed, outputted " + board.GetFenString());
        }
    
        if(board.IsInCheck()) {
            Console.WriteLine("Check test 1 failed");
        } else {
            Console.WriteLine("Check test 1 passed");
        }
        Board board2 = new("rnbqkbnr/pp3Bpp/3p4/2p1p3/4P3/5Q2/PPPP1PPP/RNB1K1NR b KQkq - 0 4");
        if(board2.IsInCheck()) {
            Console.WriteLine("Bishop check test passed");
        } else {
            Console.WriteLine("Bishop check test failed");
        }
        Board board3 = new("rnbqkbnr/p3pPpp/8/1ppp4/8/8/PPPP1PPP/RNBQKBNR b KQkq - 0 4");
        if(board3.IsInCheck()) {
            Console.WriteLine("Pawn check test passed");
        } else {
            Console.WriteLine("Pawn check test failed");
        }
        Board board4 = new("rnbqkbnr/ppppp1pp/8/5p1Q/4P3/8/PPPP1PPP/RNB1KBNR b KQkq - 1 2");
        if(board4.IsInCheck()) {
            Console.WriteLine("Queen check test passed");
        } else {
            Console.WriteLine("Queen check test failed");
        }
        Board board5 = new("rnbqkbnr/ppN2ppp/4p3/3p4/8/8/PPPPPPPP/R1BQKBNR b KQkq - 0 3");
        if(board5.IsInCheck()) {
            Console.WriteLine("Knight check test passed");
        } else {
            Console.WriteLine("Knight check test failed");
        }
        Board board6 = new("rnbqkbnr/pppp3p/6p1/6N1/8/4R2B/PPPPPK2/RNBQ4 b kq - 1 10");
        if(board6.IsInCheck()) {
            Console.WriteLine("Rook check test passed");
        } else {
            Console.WriteLine("Rook check test failed");
        }
        Board board7 = new("rnbqkbnr/ppp2Qpp/8/3pp3/4P3/8/PPPP1PPP/RNB1KBNR b KQkq - 0 3");
        if(board7.IsInCheck()) {
            Console.WriteLine("Queen check test 2 passed");
        } else {
            Console.WriteLine("Queen check test 2 failed");
        }
        Move move = new Move(12, 28, 0, new(board));
        board.MakeMove(move);
        if(board.GetFenString() == "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1") {
            Console.WriteLine("Moves test passed");
        } else {
            Console.WriteLine("Moves test failed, outputted " + board.GetFenString());
        }
        board.UndoMove(move);
        if(board.GetFenString() == "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") {
            Console.WriteLine("Undo test passed");
        } else {
            Console.WriteLine("Undo test failed, outputted " + board.GetFenString());
        }
        foreach(Move move1 in moves) {
            Console.WriteLine("Testing Move " + move1.ConvertToLongAlgebraic());
            board.MakeMove(move1);
            Console.WriteLine("Resulting FEN is " + board.GetFenString());
            board.UndoMove(move1);
            if(board.GetFenString() == "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") {
                Console.WriteLine("Final Fen String Is Proper");
            } else {
                Console.WriteLine("Undo failed, outputted " + board.GetFenString() + " after undoing move " + move1.ConvertToLongAlgebraic());
                break;
            }
        }
    }
    public static void MoveGenTests() {
        Board board = new("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        List<Move> moves = board.GetLegalMoves();
        if(moves.Count == 20) {
            Console.WriteLine("Move count test passed");
        } else {
            Console.WriteLine("Move count test Failed, outputted " + moves.Count + " legal moves");
        }
        int i = 0;
        int[] counts = new int[moves.Count];
        Console.WriteLine("perft 2");
        foreach(Move move in moves) {

            board.MakeMove(move);
            foreach(Move move2 in board.GetLegalMoves()) {
                counts[i]++;
            }
            board.UndoMove(move);
            if(board.GetFenString() != "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") {
                Console.WriteLine("Undo failed");
                break;
            }
            Console.WriteLine(move.ConvertToLongAlgebraic() + ": " + counts[i]);
            i++;
        }
        i = 0;
        counts = new int[moves.Count];
        Console.WriteLine("perft 3");
        foreach(Move move in moves) {

            board.MakeMove(move);
            foreach(Move move2 in board.GetLegalMoves()) {
                board.MakeMove(move2);
                foreach(Move move3 in board.GetLegalMoves()) {
                    counts[i]++;
                }
                board.UndoMove(move2);
            }
            board.UndoMove(move);
            if(board.GetFenString() != "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") {
                Console.WriteLine("Undo failed");
                break;
            }
            Console.WriteLine(move.ConvertToLongAlgebraic() + ": " + counts[i]);
            i++;
        }
        i = 0;
        counts = new int[moves.Count];
        Console.WriteLine("perft 4");
        foreach(Move move in moves) {

            board.MakeMove(move);
            foreach(Move move2 in board.GetLegalMoves()) {
                board.MakeMove(move2);
                foreach(Move move3 in board.GetLegalMoves()) {
                    board.MakeMove(move3);
                    foreach(Move move4 in board.GetLegalMoves()) {
                        counts[i]++;
                    } 
                    board.UndoMove(move3);
                }
                board.UndoMove(move2);
            }
            board.UndoMove(move);
            if(board.GetFenString() != "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") {
                Console.WriteLine("Undo failed");
                break;
            }
            Console.WriteLine(move.ConvertToLongAlgebraic() + ": " + counts[i]);
            i++;
        }
    }
}