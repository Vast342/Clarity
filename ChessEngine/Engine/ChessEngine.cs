using Chess;
using Engine.Essentials;
using Microsoft.VisualBasic;
using System.Diagnostics;
using System.Runtime.CompilerServices;
public struct ChessEngine {
    public ChessEngine(int i) {
        NewGame();
        InitializeTables();
    }
    public int timeRatio = 25;
    public string name = "Vast-Test";
    public string author = "Vast";
    public TranspositionTable TT = new TranspositionTable();
    public MoveTable MT = new MoveTable();
    int nodes = 0;
    int numMoves = 0;
    // depth reduction for nmp
    int nmpR = 2;
    int nmpMin = 3;
    public static int depthLimit = 100;
    public Move[] pv = new Move[depthLimit];
    /// <summary>
    /// Makes the bot identify itself
    /// </summary>
    public void IdentifyUCI() {
        Console.WriteLine("id name " + name);
        Console.WriteLine("id author " + author);
    }
    /// <summary>
    /// resets the bot and prepares for a new game
    /// </summary>
    public void NewGame() {
        TT.Clear();
        nodes = 0;
        numMoves = 0;
    }
    public int[][] MGTables = new int[6][];
    public int[][] EGTables = new int[6][];
    public void InitializeTables() {
        for(int i = 0; i < 6; i++) {
            MGTables[i] = new int[64];
            EGTables[i] = new int[64];
            Array.Copy(Tables.MGTables[i], MGTables[i], 64);
            Array.Copy(Tables.EGTables[i], EGTables[i], 64);
            for(int j = 0; j < 64; j++) {
                MGTables[i][j] += pieceValuesMG[i];
                EGTables[i][j] += pieceValuesEG[i];
            }
        }
    }
    public int startTime;
    public Move previousBestMove;
    public Stopwatch sw = new();
    /// <summary>
    /// Thinks through the position it has been given, and writes the best move to the console once the search is done
    /// </summary>
    public void Think(string entry, Board board) {
        pv = new Move[depthLimit];
        MT.Clear();
        startTime = int.Parse(board.colorToMove == 1 ? entry.Split(' ')[2] : entry.Split(' ')[4]);
        nodes = 0;
        sw = Stopwatch.StartNew();
        for(int depth = 1; depth <= 100; depth++) {
            previousBestMove = TT.ReadBestMove(board.zobristHash);
            int eval = Negamax(board, -10000000, 10000000, depth, 0, true);
            if(sw.ElapsedMilliseconds > startTime / timeRatio) {
                TT.UpdateBestMove(previousBestMove, board.zobristHash);
                break;
            }
            string pvText = " pv ";
            for(int i = 0; i < depth; i++) {
                pvText += pv[i].ConvertToLongAlgebraic() + " ";
            } 
            Console.WriteLine("info depth " + depth + " time " + sw.ElapsedMilliseconds + " nodes " + nodes + " score cp " + eval + pvText);
        }
        numMoves++;
        Console.WriteLine("bestmove " + pv[0].ConvertToLongAlgebraic());
        board.MakeMove(pv[0]);
    }
    public int[] pieceValuesMG = { 82, 337, 365, 477, 1025,  0};
    public int[] pieceValuesEG = { 94, 281, 297, 512,  936,  0};
    public int[] colorMultipliers = {-1, 1};
    public int[] colorShifters = {0, 56};
    public int[] phaseIncrements = {0, 1, 1, 2, 4, 0};
    /// <summary>
    /// A static evaluation of the position, currently only using material
    /// </summary>
    /// <returns>The number from the evaluation</returns>
    public int Evaluate(Board board) {
        int mg = 0;
        int eg = 0;
        int phase = 0;
        ulong mask = board.GetOccupiedBitboard();
        while(mask != 0) {
            int index = BitboardOperations.PopLSB(ref mask);
            byte piece = board.PieceAtIndex(index);
            mg += MGTables[Piece.GetType(piece)][index ^ colorShifters[Piece.GetColor(piece)]] * colorMultipliers[Piece.GetColor(piece)];
            eg += EGTables[Piece.GetType(piece)][index ^ colorShifters[Piece.GetColor(piece)]] * colorMultipliers[Piece.GetColor(piece)];
            phase += phaseIncrements[Piece.GetType(piece)];
            if(Piece.GetType(piece) == Piece.Pawn) {
                ulong passedMask = BitboardOperations.GetPassedPawnMask(index, Piece.GetColor(piece));
                if((passedMask & board.GetColoredPieceBitboard(1 - Piece.GetColor(piece), Piece.Pawn)) == 0) {
                    mg += Tables.passedPawnBonuses[index ^ colorShifters[Piece.GetColor(piece)]];
                    eg += 2 * Tables.passedPawnBonuses[index ^ colorShifters[Piece.GetColor(piece)]];
                }
            }
        }
        int mgPhase = phase;
        if(mgPhase > 24) mgPhase = 24; // Clamping the value so that eg phase isn't negative if a pawn promotes early on
        int egPhase = 24 - mgPhase;
        eg -= 5 * MagicGeneration.minDistanceToEdge[board.kingSquares[1 - board.colorToMove]];
        return (mg * mgPhase + eg * egPhase) / 24 * colorMultipliers[board.colorToMove];
    }
    /// <summary>
    /// Searches for the legal moves up to a certain depth and rates them using a Negamax search and Alpha-Beta pruning
    /// </summary>
    /// <param name="alpha">The lowest score the maximising player is guaranteed</param>
    /// <param name="beta">The Highest score the minimising player is guaranteed</param>
    /// <param name="depth">The depth being searched to </param>
    /// <returns>The result of the search</returns>
    public int Negamax(Board board, int alpha, int beta, int depth, int ply, bool nmpAllowed) {
        nodes++;
        if(nodes % 4096 == 0) if(sw.ElapsedMilliseconds > startTime / timeRatio) return 0;
        if(depth == 0) return QSearch(board, alpha, beta);
        bool isPV = beta - alpha > 1; // line from A_randomnoob
        int legalMoveCount = 0;
        // could potentially detect zugzwang here, but ehhhhhh
        bool mateThreat = false;
        if(nmpAllowed & depth > nmpMin) {
            board.ChangeColor();
            int score = -Negamax(board, -beta, -alpha, depth - nmpR - 1, ply + 1, false);
            if(score >= beta) {
                board.ChangeColor();
                return score;
            } else {
                // mate threat for extensions
                if(score >= 10000000 - ply - 15) mateThreat = true;
                board.ChangeColor();
            }
        }
        Span<Move> moves = stackalloc Move[256];
        board.GetMoves(ref moves);
        OrderMoves(board, ref moves);
        int extensions = 0;
        if(board.IsInCheck()) extensions++;
        if(mateThreat) extensions++;
        foreach(Move move in moves) {
            if(board.MakeMove(move)) { 
                int score = -Negamax(board, -beta, -alpha, depth + extensions + GetDepthDelta(board, depth, legalMoveCount, extensions), ply + 1, true);
                legalMoveCount++;
                board.UndoMove(move);
                if(nodes % 4096 == 0) if(sw.ElapsedMilliseconds > startTime / timeRatio) return 0;
                if(score >= beta) {
                    if(TT.ReadFlag(board.zobristHash) != 0 && TT.IsEntryEqual(board.zobristHash)) TT.WriteEntry(new(board.zobristHash, move, 1, depth + extensions + GetDepthDelta(board, depth, legalMoveCount, extensions)), board.zobristHash); // the 1 here is the flag for a beta cutoff
                    if(!move.IsCapture(board)) MT.AddCutoff(move.ToNumber());
                    alpha = beta;
                    return score; // prune the branch
                }
                if(score > alpha) {
                    // current idea, write the flag and only overwrite it if it's not 0, and then the idea is that I can save the zobrist board.zobristHash so if the flag is zero and the board.zobristHash is different I don't overwrite it
                    if(TT.ReadFlag(board.zobristHash) == 0 && !TT.IsEntryEqual(board.zobristHash)) TT.WriteEntry(new(board.zobristHash, move, 0, depth + extensions + GetDepthDelta(board, depth, legalMoveCount, extensions)), board.zobristHash);  
                    if(isPV) pv[ply] = move;
                    alpha = score;
                }
           }    
        }
        if(legalMoveCount == 0) {
            if(board.IsInCheck()) {
                return -10000000 + ply;
            }
            return 0;
        }
        return alpha;
    }
    /// <summary>
    /// Looks at all the captures until there are none left, at the end of a branch.
    /// </summary>
    /// <param name="alpha">The lowest score</param>
    /// <param name="beta">The highest score</param>
    /// <returns>The total returned value of the search</returns>
    public int QSearch(Board board, int alpha, int beta) {
        nodes++;
        if(nodes % 4096 == 0) if(sw.ElapsedMilliseconds > startTime / timeRatio) return 0;
        int standPat = Evaluate(board);
        if(standPat >= beta) return standPat;
        if(alpha < standPat) alpha = standPat;
        Span<Move> moves = stackalloc Move[256];
        board.GetMovesQSearch(ref moves);
        OrderMoves(board, ref moves);
        foreach(Move move in moves) {
            if(board.MakeMove(move)) {
                int score = -QSearch(board, -beta, -alpha);
                board.UndoMove(move);
                if(nodes % 4096 == 0) if(sw.ElapsedMilliseconds > startTime / timeRatio) return 0;
                if(score >= beta) {
                    if(!move.IsCapture(board)) MT.AddCutoff(move.ToNumber());
                    alpha = beta;
                    return score;
                }
                if(score > alpha) {
                    TT.UpdateBestMove(move, board.zobristHash);
                    TT.WriteFlag(0, board.zobristHash); // exact is 0
                    alpha = score;
                }
            }
        }
        return alpha;
    }
    public int previousBestMoveScore = 100000000;
    public int mvvlvaScore = 1000000;
    public int historyScore = 1000;
    /// <summary>
    /// Orders the moves using MVV-LVA
    /// </summary>
    /// <param name="moves">A reference to the list of legal moves being sorted</param>
    public void OrderMoves(Board board, ref Span<Move> moves) {
        Span<int> scores = stackalloc int[moves.Length];
        for(int i = 0; i < moves.Length; i++) {
            if(moves[i].IsCapture(board)) {
                scores[i] = mvvlvaScore * pieceValuesMG[Piece.GetType(board.PieceAtIndex(moves[i].endSquare))] - pieceValuesMG[Piece.GetType(board.PieceAtIndex(moves[i].startSquare))];
            }
            if(Move.Equals(moves[i], TT.ReadBestMove(board.zobristHash))) {
                scores[i] += previousBestMoveScore;
            }
            scores[i] += historyScore * MT.ReadCutoff(moves[i].ToNumber());
        }
        scores.Sort(moves);
        moves.Reverse();
    }
    // this function handles extensions and reductions
    public int GetDepthDelta(Board board, int depth, int number, int extensions) {
        if(extensions == 0) {
            int delta = -1;
            if(!board.IsInCheck() && depth > 3) {
                delta += Tables.reductions[number];
            }
            return delta;
        } else return -1;
    }
}   