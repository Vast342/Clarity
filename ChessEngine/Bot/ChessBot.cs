using Chess;
using Bot.Essentials;
using System.Diagnostics;

public class ChessBot {
    public Board board = new("8/8/8/8/8/8/8/8 w - - 0 0");
    public List<string> moves = new();
    public string name = "";
    public string author = "";
    public Move rootBestMove = Move.NullMove;
    public MoveTable moveTable = new();
    public static ulong mask = 0x7FFFFF;
    public TranspositionTable TT = new TranspositionTable();
    private const sbyte EXACT = 0, LOWERBOUND = -1, UPPERBOUND = 1, INVALID = -2;
    int nodes = 0;
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
        TT.Clear();
        moveTable.Clear();
        nodes = 0;
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
        color = board.colorToMove;
    }
    public int color;
    public int universalDepth = 0;
    public int startTime;
    public Move previousBestMove;
    public Stopwatch sw = new();
    /// <summary>
    /// Thinks through the position it has been given, and writes the best move to the console once the search is done
    /// </summary>
    public void Think(string entry) {
        startTime = int.Parse(color == 1 ? entry.Split(' ')[2] : entry.Split(' ')[4]);
        nodes = 0;
        sw = Stopwatch.StartNew();
        for(int i = 1; i <= 10; i++) {
            universalDepth = i;
            previousBestMove = rootBestMove;
            int eval = Negamax(-10000000, 10000000, universalDepth, 0);
            if(sw.ElapsedMilliseconds > startTime / 30) {
                rootBestMove = previousBestMove;
                break;
            }
            Console.WriteLine("info depth " + i + " time " + sw.ElapsedMilliseconds + " nodes " + nodes + " score cp " + eval);
        }
        moves.Add(rootBestMove.ConvertToLongAlgebraic());
        board.MakeMove(rootBestMove);
        Console.WriteLine("bestmove " + rootBestMove.ConvertToLongAlgebraic());
    }
    public int[] pieceValues = {100, 310, 330, 500, 1000, 100000, 0};
    public int[] colorMultipliers = {-1, 1};
    public int[] colorShifters = {0, 56};
    /// <summary>
    /// A static evaluation of the position, currently only using material
    /// </summary>
    /// <returns>The number from the evaluation</returns>
    public int Evaluate() {
        int sum = 0;
        ulong mask = board.GetOccupiedBitboard();
        while(mask != 0) {
            int index = BitboardOperations.PopLSB(ref mask);
            byte piece = board.PieceAtIndex(index);
            sum += Tables.tables[Piece.GetType(piece)][index ^ colorShifters[Piece.GetColor(piece)]] * colorMultipliers[Piece.GetColor(piece)];
            sum += pieceValues[Piece.GetType(piece)] * colorMultipliers[Piece.GetColor(piece)];
        }
        return sum * colorMultipliers[board.colorToMove];
    }
    /// <summary>
    /// Searches for the legal moves up to a certain depth and rates them using a Negamax search and Alpha-Beta pruning
    /// </summary>
    /// <param name="alpha">The lowest score the maximising player is guaranteed</param>
    /// <param name="beta">The Highest score the minimising player is guaranteed</param>
    /// <param name="depth">The depth being searched to </param>
    /// <returns>The result of the search</returns>
    public int Negamax(int alpha, int beta, int depth, int ply) {
        if(sw.ElapsedMilliseconds > startTime / 30) return 0;
        if(depth == 0) return QSearch(alpha, beta);
        ulong hash = board.CreateHash();
        int originalAlpha = alpha;
        Move bestMove = Move.NullMove;
        Move[] moves = board.GetMoves();
        int legalMoveCount = 0;
        OrderMoves(ref moves, hash);
        foreach(Move move in moves) {
           if(board.MakeMove(move)) { 
                legalMoveCount++;
                nodes++;
                int score = -Negamax(-beta, -alpha, depth - 1, ply + 1);
                board.UndoMove(move);
                if(sw.ElapsedMilliseconds > startTime / 30) return 0;
                if(score >= beta) {
                    alpha = beta;
                    break;
                }
                if(score > alpha) {
                    alpha = score;
                    bestMove = move;
                    if(ply == 0) rootBestMove = move;
                    alpha = Math.Max(alpha, score);
                }
           }    
        }
        if(legalMoveCount == 0) {
            if(board.IsInCheck()) {
                return -10000000 + ply;
            }
            return 0;
        }
        sbyte bound = alpha >= beta ? LOWERBOUND : alpha > originalAlpha ? EXACT : UPPERBOUND;
        TT.WriteEntry(new Transposition(hash, bestMove, depth, alpha, bound), hash);
        return alpha;
    }
    /// <summary>
    /// Looks at all the captures until there are none left, at the end of a branch.
    /// </summary>
    /// <param name="alpha">The lowest score</param>
    /// <param name="beta">The highest score</param>
    /// <returns>The total returned value of the search</returns>
    public int QSearch(int alpha, int beta) {
        if(sw.ElapsedMilliseconds > startTime / 30) return 0;
        Move bestMove = Move.NullMove;
        ulong hash = board.CreateHash();
        int standPat = Evaluate();
        if(standPat >= beta) return standPat;
        if(alpha < standPat) alpha = standPat;
        int originalAlpha = alpha;
        Move[] moves = board.GetMovesQSearch();
        OrderMoves(ref moves, hash);
        foreach(Move move in moves) {
            if(board.MakeMove(move)) {
                nodes++;
                int score = -QSearch(-beta, -alpha);
                board.UndoMove(move);
                if(sw.ElapsedMilliseconds > startTime / 30) return 0;
                if(score >= beta) {
                    alpha = beta;
                    break;
                }
                if(score > alpha) {
                    bestMove = move;
                    alpha = Math.Max(alpha, score);
                }
            }
        }
        sbyte bound = alpha >= beta ? LOWERBOUND : alpha > originalAlpha ? EXACT : UPPERBOUND;
        TT.WriteEntry(new Transposition(hash, bestMove, 0, alpha, bound), hash);
        return alpha;
    }
    /// <summary>
    /// Orders the moves using MVV-LVA
    /// </summary>
    /// <param name="moves">A reference to the list of legal moves being sorted</param>
    public void OrderMoves(ref Move[] moves, ulong zobristHash) {
        int[] scores = new int[moves.Length];
        for(int i = 0; i < moves.Length; i++) {
            if(moves[i].IsCapture(board)) {
                scores[i] = 10000 * pieceValues[Piece.GetType(board.PieceAtIndex(moves[i].endSquare))] - pieceValues[Piece.GetType(board.PieceAtIndex(moves[i].startSquare))];
            }
            if(Move.Equals(moves[i], TT.ReadBestMove(zobristHash))) {
                scores[i] += 1000000;
            }
        }
        Array.Sort(scores, moves);
        Array.Reverse(moves);
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