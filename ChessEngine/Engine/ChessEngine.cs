using Chess;
using Engine.Essentials;
using System.Diagnostics;
using System.Security.Cryptography.X509Certificates;
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
    public Move rootBestMove = Move.NullMove;
    /// <summary>
    /// Thinks through the position it has been given, and writes the best move to the console once the search is done
    /// </summary>
    public void Think(string entry, Board board) {
        MT.Clear();
        startTime = int.Parse(board.colorToMove == 1 ? entry.Split(' ')[2] : entry.Split(' ')[4]);
        nodes = 0;
        sw = Stopwatch.StartNew();
        bool check = board.IsInCheck();
        for(int i = 1; i <= 10; i++) {
            previousBestMove = rootBestMove;
            int eval = Negamax(board, -10000000, 10000000, i, 0, true);
            if(sw.ElapsedMilliseconds > startTime / timeRatio) {
                rootBestMove = previousBestMove;
                break;
            } 
            Console.WriteLine("info depth " + i + " time " + sw.ElapsedMilliseconds + " nodes " + nodes + " score cp " + eval);
        }
        numMoves++;
        board.MakeMove(rootBestMove);
        Console.WriteLine("bestmove " + rootBestMove.ConvertToLongAlgebraic());
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
                if((passedMask & board.GetColoredPieceBitboard(1 - Piece.GetColor(piece), (byte)Piece.GetType(piece))) == 0) {
                    mg += Tables.passedPawnBonuses[index ^ colorShifters[Piece.GetColor(piece)]];
                    eg += 2 * Tables.passedPawnBonuses[index ^ colorShifters[Piece.GetColor(piece)]];
                }
            }
        }
        int mgPhase = phase;
        if(mgPhase > 24) mgPhase = 24; // Clamping the value so that eg phase isn't negative if a pawn promotes early on
        int egPhase = 24 - mgPhase;
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
        if(sw.ElapsedMilliseconds > startTime / timeRatio) return 0;
        if(depth == 0) return QSearch(board, alpha, beta);
        int legalMoveCount = 0;
        ulong hash = board.CreateHash();
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
        OrderMoves(board, ref moves, hash);
        foreach(Move move in moves) {
            bool isCapture = move.IsCapture(board);
            bool isInCheck = board.IsInCheck();
            if(board.MakeMove(move)) { 
                int score = -Negamax(board, -beta, -alpha, depth + GetDepthDelta(board, depth, isCapture, isInCheck, mateThreat, legalMoveCount), ply + 1, true);
                legalMoveCount++;
                board.UndoMove(move);
                if(sw.ElapsedMilliseconds > startTime / timeRatio) return 0;
                if(score >= beta) {
                    TT.UpdateBestMove(move, hash);
                    if(!move.IsCapture(board)) MT.AddCutoff(move.ToNumber());
                    alpha = beta;
                    return score; // prune the branch
                }
                if(score > alpha) {
                    TT.UpdateBestMove(move, hash);
                    if(ply == 0) rootBestMove = move;
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
        if(sw.ElapsedMilliseconds > startTime / timeRatio) return 0;
        ulong hash = board.CreateHash();
        int standPat = Evaluate(board);
        if(standPat >= beta) return standPat;
        if(alpha < standPat) alpha = standPat;
        Span<Move> moves = stackalloc Move[256];
        board.GetMovesQSearch(ref moves);
        OrderMoves(board, ref moves, hash);
        foreach(Move move in moves) {
            if(board.MakeMove(move)) {
                int score = -QSearch(board, -beta, -alpha);
                board.UndoMove(move);
                if(sw.ElapsedMilliseconds > startTime / timeRatio) return 0;
                if(score >= beta) {
                    TT.UpdateBestMove(move, hash);
                    if(!move.IsCapture(board)) MT.AddCutoff(move.ToNumber());
                    alpha = beta;
                    return score;
                }
                if(score > alpha) {
                    TT.UpdateBestMove(move, hash);
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
    public void OrderMoves(Board board, ref Span<Move> moves, ulong zobristHash) {
        Span<int> scores = stackalloc int[moves.Length];
        for(int i = 0; i < moves.Length; i++) {
            if(moves[i].IsCapture(board)) {
                scores[i] = mvvlvaScore * pieceValuesMG[Piece.GetType(board.PieceAtIndex(moves[i].endSquare))] - pieceValuesMG[Piece.GetType(board.PieceAtIndex(moves[i].startSquare))];
            }
            if(Move.Equals(moves[i], TT.ReadBestMove(zobristHash))) {
                scores[i] += previousBestMoveScore;
            }
            scores[i] += historyScore * MT.ReadCutoff(moves[i].ToNumber());
        }
        scores.Sort(moves);
        moves.Reverse();
    }
    // this function handles extensions and reductions
    public int GetDepthDelta(Board board, int depth, bool isCapture, bool isInCheck, bool mateThreat, int number) {
        int delta = -1;
        bool isExtended = false;
        if(isInCheck) {
            delta++;
            isExtended = true;
        }
        if(mateThreat) {
            delta++;
            isExtended = true;
        }
        if(!isExtended && !board.IsInCheck() && depth > 3 && !isCapture) {
            // reductions
            delta -= Tables.reductions[number];
        }
        return delta;
    }
}   