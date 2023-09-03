using System.Numerics;
namespace Chess {
    public class MagicGeneration {
        public static ulong[][] rookAttacks = new ulong[64][];
        public static ulong[][] bishopAttacks = new ulong[64][];
        public static readonly int[] directionalOffsets = {8, -8, 1, -1, 7, -7, 9, -9};
        public static readonly byte[,] squaresToEdge = new byte[64,8];
        public static readonly ulong[] rookMasks = new ulong[64];
        public static readonly ulong[] bishopMasks = new ulong[64];
        public static ulong[] rookMagics = new ulong[64];
        public static byte[] rookShifts = new byte[64];
        public static ulong[] bishopMagics = new ulong[64];
        public static byte[] bishopShifts = new byte[64];
        public static Random rng = new Random();
        public static ulong GetRookAttacks(int startSquare, ulong occupiedBitboard) {
            // North
            ulong north = Mask.slideyPieceRays[0, startSquare];
            ulong potentialBlockers = occupiedBitboard & north;
            int firstBlocker = BitOperations.TrailingZeroCount(potentialBlockers | (1UL << 63));
            north ^= Mask.slideyPieceRays[0, firstBlocker];
            // South
            ulong south = Mask.slideyPieceRays[1, startSquare];
            potentialBlockers = occupiedBitboard & south;
            firstBlocker = BitOperations.LeadingZeroCount(potentialBlockers | 1);
            south ^= Mask.slideyPieceRays[1, 63-firstBlocker];
            // East
            ulong east = Mask.slideyPieceRays[2, startSquare];
            potentialBlockers = occupiedBitboard & east;
            firstBlocker = BitOperations.TrailingZeroCount(potentialBlockers | (1UL << 63));
            east ^= Mask.slideyPieceRays[2, firstBlocker];
            // West
            ulong west = Mask.slideyPieceRays[3, startSquare];
            potentialBlockers = occupiedBitboard & west;
            firstBlocker = BitOperations.LeadingZeroCount(potentialBlockers | 1);
            west ^= Mask.slideyPieceRays[3, 63-firstBlocker];

            return north | south | east | west;
        }
        public static ulong GetBishopAttacks(int startSquare, ulong occupiedBitboard) {
            // North-West
            ulong northWest = Mask.slideyPieceRays[4, startSquare];
            ulong potentialBlockers = occupiedBitboard & northWest;
            int firstBlocker = BitOperations.TrailingZeroCount(potentialBlockers | (1UL << 63));
            northWest ^= Mask.slideyPieceRays[4, firstBlocker];
            // South-East
            ulong southEast = Mask.slideyPieceRays[5, startSquare];
            potentialBlockers = occupiedBitboard & southEast;
            firstBlocker = BitOperations.LeadingZeroCount(potentialBlockers | 1);
            southEast ^= Mask.slideyPieceRays[5, 63-firstBlocker];
            // North-East
            ulong northEast = Mask.slideyPieceRays[6, startSquare];
            potentialBlockers = occupiedBitboard & northEast;
            firstBlocker = BitOperations.TrailingZeroCount(potentialBlockers | (1UL << 63));
            northEast ^= Mask.slideyPieceRays[6, firstBlocker];
            // South-West
            ulong southWest = Mask.slideyPieceRays[7, startSquare];
            potentialBlockers = occupiedBitboard & southWest;
            firstBlocker = BitOperations.LeadingZeroCount(potentialBlockers | 1);
            southWest ^= Mask.slideyPieceRays[7, 63-firstBlocker];

            return northWest | southEast | northEast | southWest;
        }
        public static ulong[] CreateAllBlockerBitboards(ulong movementMask) {
            // creates a list of the squares in the movementMask
            List<int> moveableSquareIndices = new();
            for(int index = 0; index < 64; index++) {
                if(((movementMask >> index) & 1) == 1) {
                    moveableSquareIndices.Add(index);
                }
            }
            // calculates the total amount of possible blocker configuration
            int totalPatterns = 1 << moveableSquareIndices.Count;
            ulong[] blockerBitboards = new ulong[totalPatterns];

            // generates each possible configuration
            for(int patternNumber = 0; patternNumber < totalPatterns; patternNumber++) {
                for(int bitNumber = 0; bitNumber < moveableSquareIndices.Count; bitNumber++) {
                    int currentBit = (patternNumber >> bitNumber) & 1;
                    blockerBitboards[patternNumber] |= (ulong)currentBit << moveableSquareIndices[bitNumber];
                } 
            }  

            return blockerBitboards;
        }
        public static void FindMagics() {
            for(int i = 0; i < 64; i++) {
                FindMagic(i, 0);
                FindMagic(i, 1);
            }
            Console.WriteLine("Found magics for each piece");
        }
        public static ulong FindMagic(int square, int piece) {
            for(int i = 0; i < 1000000; i++) {
                ulong testingMagic = (ulong)(rng.NextInt64() & rng.NextInt64() & rng.NextInt64());
                if(TryToMakeTable(square, testingMagic, piece)) {
                    return testingMagic;
                }
            }
            Console.WriteLine("Failed to find a magic for piece " + piece + " at square " + square);
            return 0;
        }
        public static bool TryToMakeTable(int square, ulong magic, int piece) {
            ulong mask = piece == 0 ? bishopMasks[square] : bishopMasks[square];
            ulong[] table = new ulong[4096];
            for(int i = 0; i < 4096; i++) table[i] = 0;
            foreach(ulong blockers in CreateAllBlockerBitboards(mask)) {
                ulong moves = piece == 0 ? GetBishopAttacks(square, blockers) : GetRookAttacks(square, blockers);
                ulong entry = CalculateMagic(magic, BitOperations.PopCount(mask), blockers);
                if(table[entry] == 0) {
                    table[entry] = moves;
                } else if(table[entry] != moves) {
                    return false; // unhelpful overlap, not good
                }
            }
            Console.WriteLine("Magic number " + magic + " worked for square " + square);
            if(piece == 0) {
                Array.Copy(table, bishopAttacks[square], 4096);
                bishopShifts[square] = (byte)BitOperations.PopCount(mask);
                bishopMagics[square] = magic;
            } else {
                Array.Copy(table, rookAttacks[square], 4096);
                rookShifts[square] = (byte)BitOperations.PopCount(mask);
                rookMagics[square] = magic;
            }
            return true;
        }
        public static ulong CalculateMagic(ulong magic, int shift, ulong blockers) {
            return (blockers * magic) >> (64-shift);
        }
        public static void GenerateMasks() {
            for(int i = 0; i < 64; i++) {
                bishopAttacks[i] = new ulong[4096];
                rookAttacks[i] = new ulong[4096];
            }
            lock(bishopMasks) {
                lock(rookMasks) {
                    for(byte file = 0; file < 8; file++) {
                        for(byte rank = 0; rank < 8; rank++) {
                            byte north = (byte)(7 - rank);
                            byte south = rank;
                            byte east = (byte)(7 - file);
                            byte west = file;
                            byte index = (byte)(rank * 8 + file);
                            squaresToEdge[index, 0] = north;
                            squaresToEdge[index, 1] = south;
                            squaresToEdge[index, 2] = east;
                            squaresToEdge[index, 3] = west;
                            squaresToEdge[index, 4] = Math.Min(north, west);
                            squaresToEdge[index, 5] = Math.Min(south, east);
                            squaresToEdge[index, 6] = Math.Min(north, east);
                            squaresToEdge[index, 7] = Math.Min(south, west);
                        }
                    }
                    for(int square = 0; square < 64; square++) {
                        for(int direction = 0; direction < 4; direction++) {
                            for(int i = 0; i < squaresToEdge[square, direction] - 1; i++) {
                                int targetSquareIndex = square + directionalOffsets[direction] * (i + 1);
                                rookMasks[square] |= ((ulong)1) << targetSquareIndex;
                            }
                        }
                        for(int direction = 4; direction < 8; direction++) {
                            for(int i = 0; i < squaresToEdge[square, direction] - 1; i++) {
                                int targetSquareIndex = square + directionalOffsets[direction] * (i + 1);
                                bishopMasks[square] |= ((ulong)1) << targetSquareIndex;
                            }
                        }
                    }
                }
            }
        }
    }
}