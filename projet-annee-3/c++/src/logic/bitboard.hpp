#pragma once

#include <ostream>

/* ---- DECLARE namespace BB ---- */

/**
 * @brief A namespace that defines bitboard operations.
 */
namespace BB {
    /**
     * @brief Bitboard type alias.
     * 
     * A bitboard is a 64-bit unsigned integer where each bit represents a square on an 8x8 chessboard. 
     * It can represent several elements of the game, such as the positions of each pawn, the positions 
     * of all attacked squares etc.
     */
    typedef uint64_t BitBoard;

    // Gets the bit of the bitboard at the given position.
    constexpr bool get_bit(const BitBoard& bits, int position) { return bits & (1ULL << position); };

    // Sets the bit at the given position to 1.
    inline void set_bit(BitBoard& bits, int position) { bits |= (1ULL << position); };
    // Resets the bit at the given position to 0.
    inline void reset_bit(BitBoard& bits, int position) { bits &= ~(1ULL << position); };

    // Returns a new bitboard that is empty except for the given position.
    inline BitBoard new_at(int position) { return 1ULL << position; };

    // Returns the number of set bits in the given bitboard.
    uint8_t popcount(BitBoard n);

    // Gets the index of the least significant set bit in the given bitboard.
    int leastSignificantBitIndex(BitBoard n);

    // Write bits to the stream in a readable format.
    std::ostream& out(std::ostream& stream, const BitBoard& bits);
}

/* ---- END DECLARE ---- */