using System;
using Chess;
public class Bot {
    public static void Main() {
        Console.WriteLine("IT'S ALIVE");
        // en passant testing itself
        Board board = new("rnbqkbnr/ppp1p1pp/8/3pPp2/8/8/PPPP1PPP/RNBQKBNR w KQkq d6 0 3");
        Move move = new("e5d6", board);
        board.MakeMove(move);
        if(board.GetFenString() == "rnbqkbnr/ppp1p1pp/3P4/5p2/8/8/PPPP1PPP/RNBQKBNR b KQkq - 0 3") {
            Console.WriteLine("Test 1 Passed");
        } else {
            Console.WriteLine("Test 1 Failed, outputted " + board.GetFenString());
        }

        // en passant rights testing
        /*
        Board board = new("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        board.InitializeZobrist();
        board.GenerateMoveData();
        Move move = new("e2e4", board);
        board.MakeMove(move);
        if(board.GetFenString() == "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1") {
            Console.WriteLine("Test 1 Passed");
        } else {
            Console.WriteLine("Test 1 Failed, outputted " + board.GetFenString());
        }
        Move move2 = new("e7e5", board);
        board.MakeMove(move2);
        if(board.GetFenString() == "rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR w KQkq e6 0 3") {
            Console.WriteLine("Test 2 Passed");
        } else {
            Console.WriteLine("Test 2 Failed, outputted " + board.GetFenString());
        }
        // castling tests
        // test 1
        Board board = new("r1bqkbnr/pppp2pp/2n2p2/4p3/2B1P3/5N2/PPPP1PPP/RNBQK2R w KQkq - 1 3");
        board.InitializeZobrist();
        board.GenerateMoveData();
        Move move = board.GetLegalMoves()[0];
        board.MakeMove(move);
        if(board.GetFenString() == "r1bqkbnr/pppp2pp/2n2p2/4p3/2B1P3/5N2/PPPP1PPP/RNBQ1RK1 b kq - 2 3") {
            Console.WriteLine("Test 1 Success");
        } else {
            Console.WriteLine("Test 1 Failed, outputed " + board.GetFenString());
        }
        // test 2
        Board board2 = new("rnb1kbnr/pppp1p1p/8/4p2p/4P2q/2NP4/PPP2PPP/R3KBNR w KQkq - 2 6");
        board2.InitializeZobrist();
        board2.GenerateMoveData();
        Move move2 = board2.GetLegalMoves()[0];
        board2.MakeMove(move2);
        if(board2.GetFenString() == "rnb1kbnr/pppp1p1p/8/4p2p/4P2q/2NP4/PPP2PPP/2KR1BNR b kq - 3 6") {
            Console.WriteLine("Test 2 Success");
        } else {
            Console.WriteLine("Test 2 Failed, outputed " + board2.GetFenString());
        }
        // test 1
        Board board3 = new("rnbqk2r/pppp1ppp/5n2/2b1p3/2B1P3/5PP1/PPPP3P/RNBQK1NR b KQkq - 2 4");
        board3.InitializeZobrist();
        board3.GenerateMoveData();
        Move move3 = board3.GetLegalMoves()[0];
        board3.MakeMove(move3);
        if(board3.GetFenString() == "rnbq1rk1/pppp1ppp/5n2/2b1p3/2B1P3/5PP1/PPPP3P/RNBQK1NR w KQ - 3 5") {
            Console.WriteLine("Test 3 Success");
        } else {
            Console.WriteLine("Test 3 Failed, outputed " + board3.GetFenString());
        }
        // test 2
        Board board4 = new("r3kbnr/ppp2ppp/2np4/4p1N1/3PP1P1/8/PPP3PP/RNBQKB1R b KQkq d3 0 6");
        board4.InitializeZobrist();
        board4.GenerateMoveData();
        Move move4 = board4.GetLegalMoves()[0];
        board4.MakeMove(move4);
        if(board4.GetFenString() == "2kr1bnr/ppp2ppp/2np4/4p1N1/3PP1P1/8/PPP3PP/RNBQKB1R w KQ - 1 7") {
            Console.WriteLine("Test 4 Success");
        } else {
            Console.WriteLine("Test 4 Failed, outputed " + board4.GetFenString());
        }
        */
    }
}