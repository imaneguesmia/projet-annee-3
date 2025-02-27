#include "bitboard.hpp"

/* ---- DEFINE namespace BB ---- */

std::ostream& BB::out(std::ostream& stream, const BitBoard& bits) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            int position {i*8 + j};

            stream << (get_bit(bits, position) ? '1' : '.')  << ' ';
        }

        stream << '\n';
    }

    return stream;
}

uint8_t BB::popcount(BitBoard n) {
    // Brian Kernighan’s algorithm.
    // https://www.chessprogramming.org/Population_Count#Brian_Kernighan.27s_way

    uint8_t result = 0;

    // (n &= n-1) sets the rightmost 1 to 0. If we do this until n == 0,
    // the loop executes the same number of times as the number of set bits in
    // the original n.
    while (n) {
        n &= n - 1; 
        result++;
    }

    return result;
}

int BB::leastSignificantBitIndex(BitBoard n) {
    // Algorithm by Kim Walisch.
    // https://www.chessprogramming.org/BitScan#KimWalisch

    static const uint64_t debruijn_hash_64 = 0x03f79d71b4cb0a89ULL;

    static const int index_64[64] = {
         0, 47,  1, 56, 48, 27,  2, 60,
        57, 49, 41, 37, 28, 16,  3, 61,
        54, 58, 35, 52, 50, 42, 21, 44,
        38, 32, 29, 23, 17, 11,  4, 62,
        46, 55, 26, 59, 40, 36, 15, 53,
        34, 51, 20, 43, 31, 22, 10, 45,
        25, 39, 14, 33, 19, 30,  9, 24,
        13, 18,  8, 12,  7,  6,  5, 63
    };

    // Black magic hashery
    return index_64[((n ^ (n-1)) * debruijn_hash_64) >> 58];
}

/* ---- END DEFINE ---- */