using System.Numerics;
namespace Chess {
    public class RayGenerator {
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
        public static Dictionary<(int, ulong), ulong> rookAttacks = new Dictionary<(int, ulong), ulong>();
        public static Dictionary<(int, ulong), ulong> bishopAttacks = new Dictionary<(int, ulong), ulong>();
        public static void GetAllRookAttacks() {
            for(int i = 0; i < 64; i++) {
                ulong attackRay = 0;
                for(int j = 0; j < 4; j++) {
                    attackRay |= Mask.slideyPieceRays[j, i];
                }
                ulong[] possibleBlockers = CreateAllBlockerBitboards(attackRay);
                foreach(ulong blockers in possibleBlockers) {
                    rookAttacks.Add((i, blockers), GetRookAttacks(i, blockers));
                }
            }
            //Console.WriteLine("Rook Lookup Initialized");
        }
        public static void GetAllBishopAttacks() {
            for(int i = 0; i < 64; i++) {
                ulong attackRay = 0;
                for(int j = 4; j < 8; j++) {
                    attackRay |= Mask.slideyPieceRays[j, i];
                }
                ulong[] possibleBlockers = CreateAllBlockerBitboards(attackRay);
                foreach(ulong blockers in possibleBlockers) {
                    bishopAttacks.Add((i, blockers), GetBishopAttacks(i, blockers));
                }
            }
            //Console.WriteLine("Bishop Lookup Initialized");
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
        public static ulong[] rookMagics = new ulong[64];
        public static byte[] rookShifts = new byte[64];
        public static ulong[] bishopMagics = new ulong[64];
        public static byte[] bishopShifts = new byte[64];
        public static bool isTesting = false;
        public static Random rng = new Random();
        public static void FindMagics() {
            for(int i = 0; i < 64; i++) {
                FindMagic(i);
            }
        }
        public static void FindMagic(int square) {
            Console.WriteLine(bishopMasks[square].ToString());
            Console.WriteLine(square.ToString());
            ulong bishopMask = bishopMasks[square];
            ulong rookMask = rookMasks[square];
            int bishopBits = BitOperations.PopCount(bishopMask);
            int rookBits = BitOperations.PopCount(rookMask);
            ulong[] used = new ulong[4096];
            bool outcome = true;
            Console.WriteLine("got to the loop");
            for(int i = 0; i < 1000000000; i++) {
                ulong decidedMagic = (ulong)(rng.Next(0, int.MaxValue) & rng.Next(0, int.MaxValue) & rng.Next(0, int.MaxValue));
                for(int j = 0; j < 4096; j++) used[j] = 0;
                for(int j = 0; j < 1 << bishopBits; j++) {
                    int result = (int)(bishopMask * decidedMagic) >> (64-bishopBits);
                    if(used[result] == 0) used[result] = 1;
                    else if(used[result] == 1) {
                        outcome = false;
                        break;
                    }
                }
                if(outcome)  {
                    bishopMagics[square] = decidedMagic;
                    bishopShifts[square] = (byte)bishopBits;
                }
                outcome = true;
                for(int j = 0; j < 1 << rookBits; j++) {
                    int result = (int)(rookMask * decidedMagic) >> (64-rookBits);
                    if(used[result] == 0) used[result] = 1;
                    else if(used[result] == 1) {
                        outcome = false;
                        break;
                    }
                }
                if(outcome)  {
                    rookMagics[square] = decidedMagic;
                    rookShifts[square] = (byte)rookBits;
                }
            }
            Console.WriteLine("Finished the loop");
        }
        public static void OutputMagics() {
            isTesting = false;
            Console.WriteLine("Rook Magics");

            Console.WriteLine("Rook Shifts");

            Console.WriteLine("Bishop Magics");

            Console.WriteLine("Bishop Shifts");

        }
        public static readonly int[] directionalOffsets = {8, -8, 1, -1, 7, -7, 9, -9};
        public static readonly byte[,] squaresToEdge = new byte[64,8];
        public static ulong[] rookMasks = new ulong[64];
        public static ulong[] bishopMasks = new ulong[64];
        public static void GenerateMasks() {
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