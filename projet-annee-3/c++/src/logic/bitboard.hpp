#include <ostream>

/* ---- DECLARE namespace BB ---- */

/*
A namespace that defines bitboard operations. A bitboard is a 64-bit unsigned integer
where each bit represents a square on an 8x8 chessboard. It can represent several
elements of the game, such as the positions of each pawn, the positions of all the
black pieces, etc.
*/

namespace BB {
    typedef uint64_t BitBoard;

    // const BitBoard COL_A {0x8080808080808080ULL};
    // const BitBoard COL_A {0x8080808080808080ULL};

    constexpr bool get_bit(const BitBoard& bits, int position) { return bits & (1ULL << position); };
    inline void set_bit(BitBoard& bits, int position) { bits |= (1ULL << position); };
    inline void reset_bit(BitBoard& bits, int position) { bits &= ~(1ULL << position); };

    // Write bits to the stream in a readable format.
    void out(std::ostream& stream, const BitBoard& bits);
}

/* ---- END DECLARE ---- */