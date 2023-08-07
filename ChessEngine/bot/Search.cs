using System;
using Chess;
public class Bot {
    public static void Main() {
        Console.WriteLine("IT'S ALIVE");
        Board board = new("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        board.MakeMove(new Move("e2e4", board));
        board.MakeMove(new Move("e7e5", board));
        foreach(Move move in board.GetLegalMoves()) {
            Console.WriteLine(move.name);
        }
    }
}