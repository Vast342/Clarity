namespace Chess {
    public static class MaskGenerator {
        public static void MaskAdjuster() {
            // adapting Homura's masks for my own personal gain
            // homura uses backwards files, from h to a, whereas we use a to h. as such I need to adapt the masks
            for(int rank = 0; rank < 8; rank++) {
                for(int file = 7; file >= 0; file -= 2) {
                    Console.WriteLine(Mask.pawnAttackMasks[0, rank * 8 + file] + ", " + Mask.pawnAttackMasks[0, rank * 8 + file - 1] + ",");
                }
            }
            Console.WriteLine("Part 2");
            for(int rank = 0; rank < 8; rank++) {
                for(int file = 7; file >= 0; file -= 2) {
                    Console.WriteLine(Mask.pawnAttackMasks[1, rank * 8 + file] + ", " + Mask.pawnAttackMasks[1, rank * 8 + file - 1] + ",");
                }
            }
        }
        public static ulong[] kingMasks = new ulong[64];
        public static ulong[] knightMasks = new ulong[64];
        public static readonly byte[,] squaresToEdge = new byte[64,8];
        public static ulong[,] slidingMasks = new ulong[64,8];
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
            for(int rank = 0; rank < 8; rank++) {
                for(int file = 0; file < 8; file++) {
                    for(int direction = 0; direction < 8; direction++) {
                        for(int i = 0; i < squaresToEdge[rank * 8 + file, direction]; i++) {
                            int targetSquareIndex = rank * 8 + file + Board.directionalOffsets[direction] * (i + 1);
                            if(i == 0) kingMasks[rank * 8 + file] |= ((ulong)1) << targetSquareIndex;
                            slidingMasks[rank * 8 + file, direction] |= ((ulong)1) << targetSquareIndex;
                        }
                    }
                }
            }
            for(int i = 0; i < 8; i++) {
                Console.Write("{");
                for(int j = 0; j < 8; j++) {
                    for(int k = 7; k >= 0; k--) {
                        Console.Write(slidingMasks[j * 8 + k, i] + ", ");
                    }
                }
                Console.WriteLine("},");
            }
        }
    }
}