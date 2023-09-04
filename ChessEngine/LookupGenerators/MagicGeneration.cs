using System.Numerics;
namespace Chess {
    public class MagicGeneration {
        public static readonly int[] directionalOffsets = {8, -8, 1, -1, 7, -7, 9, -9};
        public static readonly byte[,] squaresToEdge = new byte[64,8];
        public static readonly ulong[] rookMasks = new ulong[64];
        public static readonly ulong[] bishopMasks = new ulong[64];
        public static Random rng = new Random();
        public static void GenerateMasks() {
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
                                rookShifts[square] = (byte)(64-BitOperations.PopCount(rookMasks[square]));
                            }
                        }
                        for(int direction = 4; direction < 8; direction++) {
                            for(int i = 0; i < squaresToEdge[square, direction] - 1; i++) {
                                int targetSquareIndex = square + directionalOffsets[direction] * (i + 1);
                                bishopMasks[square] |= ((ulong)1) << targetSquareIndex;
                                bishopShifts[square] = (byte)(64-BitOperations.PopCount(bishopMasks[square]));
                            }
                        }
                    }
                }
            }
        }
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
        public static void InitializeSavedMagics() {
            for(int i = 0; i < 64; i++) {
                TryToMakeTable(i, bishopMagics[i], 0);
                TryToMakeTable(i, rookMagics[i], 1);
            }
        }
        public static void FindMagics() {
            for(int i = 0; i < 64; i++) {
                FindMagic(i, 0);
                FindMagic(i, 1);
            }
            //Console.WriteLine("Found magics for each piece");
        }
        public static void FindMagic(int square, int piece) {
            for(int i = 0; i < 1000000; i++) {
                ulong testingMagic = (ulong)(rng.NextInt64() & rng.NextInt64() & rng.NextInt64());
                if(TryToMakeTable(square, testingMagic, piece)) {
                    //Console.WriteLine("Found magic for square " + square + " and piece " + piece);
                    break;
                }
            }
        }
        public static bool TryToMakeTable(int square, ulong magic, int piece) {
            ulong mask = piece == 0 ? bishopMasks[square] : rookMasks[square];
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
            if(piece == 0) {
                CopyToAttackArray(table, square, 0);
            } else {
                CopyToAttackArray(table, square, 1);
            }
            return true;
        }
        public static void CopyToAttackArray(ulong[] table, int square, int piece) {
            for(int i = 0; i < 4096; i++) {
                if(piece == 0) {
                    bishopAttacks[(4096 * square) + i] = table[i];
                } else {
                    rookAttacks[(4096 * square) + i] = table[i];
                }
            }
        }
        public static ulong CalculateMagic(ulong magic, int shift, ulong blockers) {
            return (blockers * magic) >> (64-shift);
        }
        public static ulong[] rookMagics = {
            72075735998677760, 
            18023332042506304, 
            108103983779818624, 
            72066531865137152, 
            144119723829887240, 
            144186725084693528, 
            1297055418767639040, 
            4755805605696930305, 
            288371253226520584, 
            70506451714052, 
            6954965479253349632, 
            4613093736910424832, 
            306807862221344772, 
            18577417551479912, 
            83879689089195017, 
            4644338197856512, 
            35734132097428, 
            1328562164954304836, 
            5193354607824216068, 
            78817941416575360, 
            10186975365628036, 
            288371664234759168, 
            144119587465988129, 
            2341874005801436225, 
            1170937210933905732, 
            422495935201792, 
            576495956003913858, 
            4611731102701195264, 
            181269904329539712, 
            599542806356165632, 
            1445659882744185344, 
            90090692835052372, 
            36028934462128128, 
            4755871712808013824, 
            9042387938588352, 
            1441434523983153184, 
            8800404779020, 
            4612813035176724736, 
            563297879360520, 
            181270439086006348, 
            176196746772496, 
            725150596214308864, 
            18595498996924480, 
            292770259931299850, 
            4613938917837176848, 
            1214015993282880, 
            2882321358669479938, 
            2478741782532, 
            3458800798946066944, 
            36099167921113216, 
            281750125152512, 
            4629991805236019456, 
            281562084167936, 
            577588852642840704, 
            7205832006188467200, 
            2377900895318901248, 
            444342284337409, 
            2858801115972098, 
            4616471919007997955, 
            72885596069898, 
            73746582688238602, 
            2882585820810723873, 
            36178399328879876, 
            54043472589750530, 
        };
        public static byte[] rookShifts = new byte[64];
        public static ulong[] bishopMagics = {
            436853566263787536, 
            588287271204049024, 
            4648841823489917056, 
            2326395084922161280, 
            1858308860079505408, 
            33222989509305344, 
            1162001280766312712, 
            9081974921626248, 
            6861090206056516, 
            4757217446266429508, 
            571825553801536, 
            288234825746251792, 
            1130310972604416, 
            1189233989319660032, 
            72904288925663248, 
            4746977093183734304, 
            2986027429015847044, 
            225182249187050120, 
            562967201187880, 
            1125934352965636, 
            28292684756025537, 
            2306126726180176144, 
            10138050487686144, 
            5188675640127677444, 
            1153519640006623744, 
            1143492630483216, 
            41668201018359840, 
            2309225106993320064, 
            426610519990272, 
            72567784276736, 
            23081525797454088, 
            5649087278105600, 
            73189267454889988, 
            56308206696136996, 
            72796620538251264, 
            4611721340784214144, 
            18016606122811521, 
            288529445529124936, 
            2343002105260540160, 
            4613938956425281600, 
            5189888414396194816, 
            282093720441860, 
            2307531963361133568, 
            576461439783929864, 
            149537880540160, 
            144695798943318280, 
            2269499945396300, 
            4649975972190245381, 
            2324996520069824514, 
            4684312096622780434, 
            2478600945696, 
            4758377995823218692, 
            72198470076997632, 
            4828069975526768772, 
            167794974934075170, 
            45040396501926016, 
            294674552849408, 
            13517035275423744, 
            1170935921374400584, 
            2341891700523927552, 
            703722877682944, 
            580973182417666560, 
            13801654084437026, 
            90287509745566484, 
        };
        public static byte[] bishopShifts = new byte[64];
        public static ulong[] rookAttacks = new ulong[262144];
        public static ulong[] bishopAttacks = new ulong[262144];
    }
}