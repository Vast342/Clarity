namespace Chess {
    public static class Mask {
        public const ulong FileMask = 0b100000001000000010000000100000001000000010000000100000001;
        public static ulong GetFileMask(int file) {
            return FileMask << file;
        }
    }
}