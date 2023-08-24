using System.Numerics;
using Chess;
using Microsoft.Extensions.Diagnostics.HealthChecks;

public class TestBot {
    public static Board board = new("8/8/8/8/8/8/8/8 w - - 0 0");
    public static List<String> moves = new();
    public static Move bestMove;
    public static int[] nodeCounts = new int[20];
    public static void Initialize() {

    }
    public static void NewGame() {
        board = new("8/8/8/8/8/8/8/8 w - - 0 0");
        moves.Clear();
    }
    public static void LoadPosition(string position) {
        string[] segments = position.Split(' ');
        if(segments[1] == "startpos") {
            if(segments.Length > 2) {
                if(segments[2] == "moves") {
                    board.MakeMove(new Move(segments[3 + moves.Count], board));
                    moves.Add(segments[3 + moves.Count-1]);
                }
            } else {
                board = new("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
            }
        } else {
            if(segments.Length > 8) {
                board = new(segments[2] + " " + segments[3] + " " + segments[4] + " " + segments[5] + " " + segments[6] + " " + segments[7]);
                if(segments[8] == "moves") {
                    board.MakeMove(new Move(segments[9 + moves.Count], board));
                    moves.Add(segments[9 + moves.Count-1]);
                }
            } else {
                board = new(segments[2] + " " + segments[3] + " " + segments[4] + " " + segments[5] + " " + segments[6] + " " + segments[7]);
            }
        }
    }
    public static void Think() {
        bestMove = new Move(12, 28, 0, new(board));
        
        Negamax(-9999999, 9999999, 4);
        
        moves.Add(bestMove.ConvertToLongAlgebraic());
        board.MakeMove(bestMove);
        Console.WriteLine("bestmove " + bestMove.ConvertToLongAlgebraic());
    }
    public static int[] pieceValues = {100, 310, 330, 500, 1000, 100000};
    public static int Evaluate(List<Move> moves) {
        int sum = moves.Count;
        for(int i = 0; i < 6; i++) {
            sum += BitOperations.PopCount(board.coloredPieceBitboards[board.colorToMove == 1 ? 1 : 0, i]) * pieceValues[i];
            sum -= BitOperations.PopCount(board.coloredPieceBitboards[1 - board.colorToMove, i]) * pieceValues[i];
        }

        return sum;
    }
    public static int Negamax(int alpha, int beta, int depth) {
        nodeCounts[depth]++;
        List<Move> moves = board.GetLegalMoves();
        if(moves.Count == 0) {
            if(board.IsInCheck()) {
                return -10000000 + board.plyCount;
            }
            return 0;
        }
        if(depth == 0) return Evaluate(moves);
        foreach(Move move in moves) {
            if(board.MakeMove(move)) {
                int score = -Negamax(-beta, -alpha, depth - 1);
                board.UndoMove(move);
                if(score > beta) {
                    return beta;
                }
                if(score > alpha) {
                    if(depth == 4) {
                        bestMove = move;
                    }
                    alpha = score;
                }
            }
        }
        return alpha;
    }
    public static void OrderMoves(ref List<Move> moves) {
        int[] scores = new int[moves.Count];
        for(int i = 0; i < moves.Count; i++) {
            if(moves[i].IsCapture(board)) {
                scores[i] = pieceValues[Piece.GetType(board.squares[moves[i].endSquare])] - pieceValues[Piece.GetType(board.squares[moves[i].startSquare])];
            }
        }
        Move[] sortedMoves = moves.ToArray();
        Array.Sort(scores, sortedMoves);
        Array.Reverse(sortedMoves);
        moves = sortedMoves.ToList();
    }
    public static void GetFen() {
        Console.WriteLine(board.GetFenString());
    }
    public static void MakeMove(string entry) {
        board.MakeMove(new Move(entry.Split(' ')[1], board));
        Console.WriteLine(board.GetFenString());
    }
}