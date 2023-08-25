using System.Numerics;
using Chess;
using Bot.Essentials;

public class ChessBot {
    public Board board = new("8/8/8/8/8/8/8/8 w - - 0 0");
    public List<String> moves = new();
    public string name = "";
    public string author = "";
    public Move rootBestMove = Move.NullMove;
    public MoveTable moveTable = new();
    public static ulong mask = 0xFFFFFF;
    public Transposition[] TT = new Transposition[mask + 1];
    private const sbyte EXACT = 0, LOWERBOUND = -1, UPPERBOUND = 1, INVALID = -2;
    public struct Transposition {
        public Transposition(ulong zKey, Move m, int d, int s, sbyte f) {
            zobristKey = zKey;
            bestMove = m;
            depth = d;
            score = s;
            flag = f;
        }
        public ulong zobristKey;
        public Move bestMove;
        public int depth = 0, score = 0;
        public sbyte flag = INVALID;
    }
    /// <summary>
    /// Makes the bot identify itself with the values from Initialize()
    /// </summary>
    public void IdentifyUCI() {
        Console.WriteLine("id name " + name);
        Console.WriteLine("id author " + author);
    }
    /// <summary>
    /// initializes the bot, it's values, and the transposition TT
    /// </summary>
    /// <param name="n"></param>
    /// <param name="a"></param>
    public void Initialize(string n, string a) {
        name = n;
        author = a;
    }
    /// <summary>
    /// resets the bot and prepares for a new game
    /// </summary>
    public void NewGame() {
        board = new("8/8/8/8/8/8/8/8 w - - 0 0");
        moves.Clear();
        TT = new Transposition[mask + 1];
        moveTable.Clear();
    }
    /// <summary>
    /// Loads the position from a position command
    /// </summary>
    /// <param name="position">The position command</param>
    public void LoadPosition(string position) {
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
    public int universalDepth = 0;
    /// <summary>
    /// Thinks through the position it has been given, and writes the best move to the console once the search is done
    /// </summary>
    public void Think() {
        for(int i = 0; i < 5; i++) {
            universalDepth = i;
            Negamax(-int.MaxValue, int.MaxValue, universalDepth, 0);
        }
        
        moves.Add(rootBestMove.ConvertToLongAlgebraic());
        board.MakeMove(rootBestMove);
        Console.WriteLine("bestmove " + rootBestMove.ConvertToLongAlgebraic());
    }
    public int[] pieceValues = {0, 100, 310, 330, 500, 1000, 100000};
    /// <summary>
    /// A static evaluation of the position, currently only using material
    /// </summary>
    /// <returns>The number from the evaluation</returns>
    public int Evaluate() {
        int sum = 0;
        for(int i = 1; i < 6; i++) {
            sum += BitOperations.PopCount(board.coloredPieceBitboards[board.colorToMove, i]) * pieceValues[i];
            sum -= BitOperations.PopCount(board.coloredPieceBitboards[1 - board.colorToMove, i]) * pieceValues[i];
        }
        return sum;
    }
    /// <summary>
    /// Searches for the legal moves up to a certain depth and rates them using a Negamax search and Alpha-Beta pruning
    /// </summary>
    /// <param name="alpha">The lowest score the maximising player is guaranteed</param>
    /// <param name="beta">The Highest score the minimising player is guaranteed</param>
    /// <param name="depth">The depth being searched to </param>
    /// <returns>The result of the search</returns>
    public int Negamax(int alpha, int beta, int depth, int ply) {
        bool notRoot = ply > 0;
        ulong hash = board.CreateHash();
        int best = -30000;
        int originalAlpha = alpha;
        Move bestMove = Move.NullMove;
        List<Move> moves = board.GetLegalMoves();
        if(moves.Count == 0) {
            if(board.IsInCheck()) {
                return -10000000 + board.plyCount;
            }
            return 0;
        }
        Transposition entry = TT[hash & mask];

        if(notRoot && entry.zobristKey == hash && entry.depth >= depth && (
            entry.flag == EXACT
                || entry.flag == LOWERBOUND && entry.score >= beta
                || entry.flag == UPPERBOUND && entry.score <= alpha
        )) return entry.score;

        if(depth == 0) return QSearch(-int.MaxValue, int.MaxValue);
        OrderMoves(ref moves, hash);
        foreach(Move move in moves) {
            if(board.MakeMove(move)) {
                int score = -Negamax(-beta, -alpha, depth - 1, ply + 1);
                board.UndoMove(move);
                if(score > best) {
                    best = score;
                    bestMove = move;
                    if(!notRoot) rootBestMove = move;
                    alpha = Math.Max(alpha, score);
                    if(alpha >= beta) break;
                }
            }
        }
        sbyte bound = best >= beta ? LOWERBOUND : best > originalAlpha ? EXACT : LOWERBOUND;
        TT[hash & mask] = new Transposition(hash, bestMove, depth, best, bound);
        return best;
    }
    /// <summary>
    /// Looks at all the captures until there are none left, at the end of a branch.
    /// </summary>
    /// <param name="alpha">The lowest score</param>
    /// <param name="beta">The highest score</param>
    /// <returns>The total returned value of the search</returns>
    public int QSearch(int alpha, int beta) {
        Move bestMove = Move.NullMove;
        int best = -30000;
        List<Move> moves = board.GetLegalMovesQSearch();
        ulong hash = board.CreateHash();
        OrderMoves(ref moves, hash);
        int standPat = Evaluate();
        if(standPat >= beta) return beta;
        if(alpha < standPat) alpha = standPat;
        int originalAlpha = alpha;
        OrderMoves(ref moves, hash);
        foreach(Move move in moves) {
            if(board.MakeMove(move)) {
                int score = -QSearch(-beta, -alpha);
                board.UndoMove(move);
                if(score > best) {
                    best = score;
                    bestMove = move;
                    alpha = Math.Max(alpha, score);
                    if(alpha >= beta) break;
                }
            }
        }
        sbyte bound = best >= beta ? LOWERBOUND : best > originalAlpha ? EXACT : LOWERBOUND;
        TT[hash & mask] = new Transposition(hash, bestMove, 0, best, bound);
        return best;
    }
    /// <summary>
    /// Orders the moves using MVV-LVA
    /// </summary>
    /// <param name="moves">A reference to the list of legal moves being sorted</param>
    public void OrderMoves(ref List<Move> moves, ulong zobristHash) {
        int[] scores = new int[moves.Count];
        for(int i = 0; i < moves.Count; i++) {
            if(moves[i].IsCapture(board)) {
                scores[i] = pieceValues[Piece.GetType(board.squares[moves[i].endSquare])] - pieceValues[Piece.GetType(board.squares[moves[i].startSquare])];
            }
            if(Move.Equals(moves[i], TT[zobristHash & mask].bestMove)) {
                scores[i] += 1000000;
            }
        }
        Move[] sortedMoves = moves.ToArray();
        Array.Sort(scores, sortedMoves);
        Array.Reverse(sortedMoves);
        moves = sortedMoves.ToList();
    }
    /// <summary>
    /// Get's the fen string of the position currently being viewed by the bot
    /// </summary>
    public void GetFen() {
        Console.WriteLine(board.GetFenString());
    }
    /// <summary>
    /// detects a move from the command, and does it on the board.
    /// </summary>
    /// <param name="entry">The command being sent</param>
    public void MakeMove(string entry) {
        board.MakeMove(new Move(entry.Split(' ')[1], board));
        Console.WriteLine(board.GetFenString());
    }
}