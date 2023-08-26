using Chess;
public class Tests {
    /// <summary>
    /// Performs the suite of backend tests
    /// </summary>
    public static void BackendTests() {
        Console.WriteLine("IT'S ALIVE!!!");
        Board board = new("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        List<Move> moves = board.GetLegalMoves();
        if(moves.Count == 20) {
            Console.ForegroundColor = ConsoleColor.Green;
            Console.WriteLine("Move count test passed");
            Console.ResetColor();
        } else {
            Console.ForegroundColor = ConsoleColor.Red;
            Console.WriteLine("Move count test Failed, outputted " + moves.Count + " legal moves");
            Console.ResetColor();
        }
        // everything
        if(board.occupiedBitboard == 18446462598732906495) {
            Console.ForegroundColor = ConsoleColor.Green;
            Console.WriteLine("Occupied bitboard test passed");
            Console.ResetColor();
        } else {
            Console.ForegroundColor = ConsoleColor.Red;
            Console.WriteLine("Occupied bitboard test failed, outputted " + board.occupiedBitboard);
            Console.ResetColor();
        }
        // pawns
        if(board.coloredPieceBitboards[1, 1] == 0b1111111100000000) {
            Console.ForegroundColor = ConsoleColor.Green;
            Console.WriteLine("White pawn bitboard test passed");
            Console.ResetColor();
        } else {
            Console.ForegroundColor = ConsoleColor.Red;
            Console.WriteLine("White pawn bitboard test failed, outputted " + board.coloredPieceBitboards[1,0]);
            Console.ResetColor();
        }
        if(board.coloredPieceBitboards[0, 1] == 0b0000000011111111000000000000000000000000000000000000000000000000) {
            Console.ForegroundColor = ConsoleColor.Green;
            Console.WriteLine("Black pawn bitboard test passed");
            Console.ResetColor();
        } else {
            Console.ForegroundColor = ConsoleColor.Red;
            Console.WriteLine("Black pawn bitboard test failed, outputted " + board.coloredPieceBitboards[0,0]);
            Console.ResetColor();
        }
        // knights
        if(board.coloredPieceBitboards[1, 2] == 0b01000010) {
            Console.ForegroundColor = ConsoleColor.Green;
            Console.WriteLine("White knight bitboard test passed");
            Console.ResetColor();
        } else {
            Console.ForegroundColor = ConsoleColor.Red;
            Console.WriteLine("White knight bitboard test failed, outputted " + board.coloredPieceBitboards[1,1]);
            Console.ResetColor();
        }
        if(board.coloredPieceBitboards[0, 2] == 0b0100001000000000000000000000000000000000000000000000000000000000) {
            Console.ForegroundColor = ConsoleColor.Green;
            Console.WriteLine("Black knight bitboard test passed");
            Console.ResetColor();
        } else {
            Console.ForegroundColor = ConsoleColor.Red;
            Console.WriteLine("Black knight bitboard test failed, outputted " + board.coloredPieceBitboards[0,1]);
            Console.ResetColor();
        }
        // bishops
        if(board.coloredPieceBitboards[1, 3] == 0b00100100) {
            Console.ForegroundColor = ConsoleColor.Green;
            Console.WriteLine("White bishop bitboard test passed");
            Console.ResetColor();
        } else {
            Console.ForegroundColor = ConsoleColor.Red;
            Console.WriteLine("White bishop bitboard test failed, outputted " + board.coloredPieceBitboards[1,2]);
            Console.ResetColor();
        }
        if(board.coloredPieceBitboards[0, 3] == 0b0010010000000000000000000000000000000000000000000000000000000000) {
            Console.ForegroundColor = ConsoleColor.Green;
            Console.WriteLine("Black bishop bitboard test passed");
            Console.ResetColor();
        } else {
            Console.ForegroundColor = ConsoleColor.Red;
            Console.WriteLine("Black bishop bitboard test failed, outputted " + board.coloredPieceBitboards[0,2]);
            Console.ResetColor();
        }
        // rooks
        if(board.coloredPieceBitboards[1, 4] == 0b10000001) {
            Console.ForegroundColor = ConsoleColor.Green;
            Console.WriteLine("White rook bitboard test passed");
            Console.ResetColor();
        } else {
            Console.ForegroundColor = ConsoleColor.Red;
            Console.WriteLine("White rook bitboard test failed, outputted " + board.coloredPieceBitboards[1,3]);
            Console.ResetColor();
        }
        if(board.coloredPieceBitboards[0, 4] == 0b1000000100000000000000000000000000000000000000000000000000000000) {
            Console.ForegroundColor = ConsoleColor.Green;
            Console.WriteLine("Black rook bitboard test passed");
            Console.ResetColor();
        } else {
            Console.ForegroundColor = ConsoleColor.Red;
            Console.WriteLine("Black rook bitboard test failed, outputted " + board.coloredPieceBitboards[0,3]);
            Console.ResetColor();
        }
        // queens
        if(board.coloredPieceBitboards[1, 5] == 0b00001000) {
            Console.ForegroundColor = ConsoleColor.Green;
            Console.WriteLine("White queen bitboard test passed");
            Console.ResetColor();
        } else {
            Console.ForegroundColor = ConsoleColor.Red;
            Console.WriteLine("White queen bitboard test failed, outputted " + board.coloredPieceBitboards[1,4]);
            Console.ResetColor();
        }
        if(board.coloredPieceBitboards[0, 5] == 0b0000100000000000000000000000000000000000000000000000000000000000) {
            Console.ForegroundColor = ConsoleColor.Green;
            Console.WriteLine("Black queen bitboard test passed");
            Console.ResetColor();
        } else {
            Console.ForegroundColor = ConsoleColor.Red;
            Console.WriteLine("Black queen bitboard test failed, outputted " + board.coloredPieceBitboards[0,4]);
            Console.ResetColor();
        }
        // kings
        if(board.coloredPieceBitboards[1, 6] == 0b00010000) {
            Console.ForegroundColor = ConsoleColor.Green;
            Console.WriteLine("White king bitboard test passed");
            Console.ResetColor();
        } else {
            Console.ForegroundColor = ConsoleColor.Red;
            Console.WriteLine("White king bitboard test failed, outputted " + board.coloredPieceBitboards[1,5]);
            Console.ResetColor();
        }
        if(board.coloredPieceBitboards[0, 6] == 0b0001000000000000000000000000000000000000000000000000000000000000) {
            Console.ForegroundColor = ConsoleColor.Green;
            Console.WriteLine("Black king bitboard test passed");
            Console.ResetColor();
        } else {
            Console.ForegroundColor = ConsoleColor.Red;
            Console.WriteLine("Black king bitboard test failed, outputted " + board.coloredPieceBitboards[0,5]);
            Console.ResetColor();
        }
        if(board.GetFenString() == "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") {
            Console.ForegroundColor = ConsoleColor.Green;
            Console.WriteLine("Fen encoder test passed");
            Console.ResetColor();
        } else {
            Console.ForegroundColor = ConsoleColor.Red;
            Console.WriteLine("Fen encoder test failed, outputted " + board.GetFenString());
            Console.ResetColor();
        }
    
        if(board.IsInCheck()) {
            Console.ForegroundColor = ConsoleColor.Red;
            Console.WriteLine("Check test 1 failed");
            Console.ResetColor();
        } else {
            Console.ForegroundColor = ConsoleColor.Green;
            Console.WriteLine("Check test 1 passed");
            Console.ResetColor();
        }
        Board board2 = new("rnbqkbnr/pp3Bpp/3p4/2p1p3/4P3/5Q2/PPPP1PPP/RNB1K1NR b KQkq - 0 4");
        if(board2.IsInCheck()) {
            Console.ForegroundColor = ConsoleColor.Green;
            Console.WriteLine("Bishop check test passed");
            Console.ResetColor();
        } else {
            Console.ForegroundColor = ConsoleColor.Red;
            Console.WriteLine("Bishop check test failed");
            Console.ResetColor();
        }
        Board board3 = new("rnbqkbnr/p3pPpp/8/1ppp4/8/8/PPPP1PPP/RNBQKBNR b KQkq - 0 4");
        if(board3.IsInCheck()) {
            Console.ForegroundColor = ConsoleColor.Green;
            Console.WriteLine("Pawn check test passed");
            Console.ResetColor();
        } else {
            Console.ForegroundColor = ConsoleColor.Red;
            Console.WriteLine("Pawn check test failed");
            Console.ResetColor();
        }
        Board board4 = new("rnbqkbnr/ppppp1pp/8/5p1Q/4P3/8/PPPP1PPP/RNB1KBNR b KQkq - 1 2");
        if(board4.IsInCheck()) {
            Console.ForegroundColor = ConsoleColor.Green;
            Console.WriteLine("Queen check test passed");
            Console.ResetColor();
        } else {
            Console.ForegroundColor = ConsoleColor.Red;
            Console.WriteLine("Queen check test failed");
            Console.ResetColor();
        }
        Board board5 = new("rnbqkbnr/ppN2ppp/4p3/3p4/8/8/PPPPPPPP/R1BQKBNR b KQkq - 0 3");
        if(board5.IsInCheck()) {
            Console.ForegroundColor = ConsoleColor.Green;
            Console.WriteLine("Knight check test passed");
            Console.ResetColor();
        } else {
            Console.ForegroundColor = ConsoleColor.Red;
            Console.WriteLine("Knight check test failed");
            Console.ResetColor();
        }
        Board board6 = new("rnbqkbnr/pppp3p/6p1/6N1/8/4R2B/PPPPPK2/RNBQ4 b kq - 1 10");
        if(board6.IsInCheck()) {
            Console.ForegroundColor = ConsoleColor.Green;
            Console.WriteLine("Rook check test passed");
            Console.ResetColor();
        } else {
            Console.ForegroundColor = ConsoleColor.Red;
            Console.WriteLine("Rook check test failed");
            Console.ResetColor();
        }
        Board board8 = new("rnbqkbnr/ppp2ppp/8/3P4/3p4/8/PPP1QPPP/RNB1KBNR b KQkq - 1 4");
        if(board8.IsInCheck()) {
            Console.ForegroundColor = ConsoleColor.Green;
            Console.WriteLine("Orthagonal queen check test passed");
            Console.ResetColor();
        } else {
            Console.ForegroundColor = ConsoleColor.Red;
            Console.WriteLine("Orthagonal queen check test failed");
            Console.ResetColor();
        }
        Board board7 = new("rnbqkbnr/ppp2Qpp/8/3pp3/4P3/8/PPPP1PPP/RNB1KBNR b KQkq - 0 3");
        if(board7.IsInCheck()) {
            Console.ForegroundColor = ConsoleColor.Green;
            Console.WriteLine("Queen check test 2 passed");
            Console.ResetColor();
        } else {
            Console.ForegroundColor = ConsoleColor.Red;
            Console.WriteLine("Queen check test 2 failed");
            Console.ResetColor();
        }
        Move move = new Move(12, 28, 0, new(board));
        board.MakeMove(move);
        if(board.GetFenString() == "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1") {
            Console.ForegroundColor = ConsoleColor.Green;
            Console.WriteLine("Moves test passed");
            Console.ResetColor();
        } else {
            Console.ForegroundColor = ConsoleColor.Red;
            Console.WriteLine("Moves test failed, outputted " + board.GetFenString());
            Console.ResetColor();
        }
        board.UndoMove(move);
        if(board.GetFenString() == "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") {
            Console.ForegroundColor = ConsoleColor.Green;
            Console.WriteLine("Undo test passed");
            Console.ResetColor();
        } else {
            Console.ForegroundColor = ConsoleColor.Red;
            Console.WriteLine("Undo test failed, outputted " + board.GetFenString());
            Console.ResetColor();
        }
        foreach(Move move1 in moves) {
            Console.WriteLine("Testing Move " + move1.ConvertToLongAlgebraic() + " with piece " + board.squares[move1.startSquare]);
            board.MakeMove(move1);
            Console.WriteLine("Resulting FEN is " + board.GetFenString());
            board.UndoMove(move1);
            if(board.GetFenString() == "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") {
                Console.ForegroundColor = ConsoleColor.Green;
                Console.WriteLine("Final Fen String Is Proper");
                Console.ResetColor();
            } else {
                Console.ForegroundColor = ConsoleColor.Red;
                Console.WriteLine("Undo failed, outputted " + board.GetFenString() + " after undoing move " + move1.ConvertToLongAlgebraic());
                Console.ResetColor();
                break;
            }
        }
    }
    /// <summary>
    /// performs a very limited set of tests on the move generation
    /// </summary>
    public static void MoveGenTests() {
        Board board = new("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        List<Move> moves = board.GetLegalMoves();
        if(moves.Count == 20) {
            Console.ForegroundColor = ConsoleColor.Green;
            Console.WriteLine("Move count test passed");
            Console.ResetColor();
        } else {
            Console.ForegroundColor = ConsoleColor.Red;
            Console.WriteLine("Move count test Failed, outputted " + moves.Count + " legal moves");
            Console.ResetColor();
        }
    }
    /// <summary>
    /// tests the outliers, such as castling and en passant
    /// </summary>
    public static void OutlierTests() {
        Board epBoard = new("rnbqkbnr/ppp1p1pp/8/3pPp2/8/8/PPPP1PPP/RNBQKBNR w KQkq d6 0 3");
        epBoard.MakeMove(new Move("e5d6", epBoard));
        Console.WriteLine(epBoard.GetFenString());
        Board WKCBoard = new("rnbqkbnr/ppp2ppp/8/4p3/2p1P3/5N2/PPPP1PPP/RNBQK2R w KQkq - 0 4");
        WKCBoard.MakeMove(new Move("e1g1", WKCBoard));
        Console.WriteLine(WKCBoard.GetFenString());
        Board WQCBoard = new("rnbqkbnr/ppp2ppp/8/3p4/3PpB2/2NQ4/PPP1PPPP/R3KBNR b KQkq - 1 4");
        WQCBoard.MakeMove(new Move("e1c1", WQCBoard));
        Console.WriteLine(WQCBoard.GetFenString());
        Board BKCBoard = new("rnbqk2r/pppp1ppp/5n2/2b1P3/4P3/8/PPPP2PP/RNBQKBNR w KQkq - 1 4");
        BKCBoard.MakeMove(new Move("e8g8", BKCBoard));
        Console.WriteLine(BKCBoard.GetFenString());
        Board BQCBoard = new("r3kbnr/ppp1pppp/2nq4/3p1b2/2PP1B2/3Q4/PP2PPPP/RN2KBNR w KQkq - 1 5");
        BQCBoard.MakeMove(new Move("e8c8", BQCBoard));
        Console.WriteLine(BQCBoard.GetFenString());

    }
    /// <summary>
    /// Performs the perft tests at a certain depth
    /// </summary>
    /// <param name="depth">The depth in question</param>
    /// <param name="board">The board that it gets done on</param>
    /// <returns>The total legal moves to that </returns>
    public static int Perft(int depth, Board board) { 
        List<Move> moves = board.GetLegalMoves();
        int count = 0;
        if(depth == 0) {
            return 1;
        }
        foreach(Move move in moves) {
            if(board.MakeMove(move)) {
                count += Perft(depth - 1, board);
                board.UndoMove(move);
            }
        }
        return count;
    }
}