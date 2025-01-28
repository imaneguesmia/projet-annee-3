#include "attack_tables.hpp"

#ifndef FIND_NEW_MAGICS
#include "magic_numbers.hpp"
#endif

#include <iostream>
#include <ctime>
#include <random>

/* ---- Helpers ---- */

// Generate a pseudo-random uint64_t by generating 4 random uint16_t and concatenating them.
// Note that RAND_MAX is not guaranteed to be greater than or equal to MAX(uint16_t),
// but that case is irrelevant to this project as it only concerns machines made several
// decades ago.
inline uint64_t random64() {
    return (
        uint64_t(0xffff & std::rand()) |
        uint64_t(0xffff & std::rand()) << 16 |
        uint64_t(0xffff & std::rand()) << 32 |
        uint64_t(0xffff & std::rand()) << 48
    );
}

// Return the number of set bits in the given uint64_t.
inline uint8_t popcount(uint64_t n) {
    uint8_t result = 0;

    // Brian Kernighan’s algorithm.
    // (n &= n-1) sets the rightmost 1 to 0. If we do this until n == 0,
    // the loop executes the same number of times as the number of set bits in
    // the original n.
    while (n) {
        n &= n - 1; 
        result++;
    }

    return result;
}

/* ---- DEFINE class AttackTables ---- */

AttackTables::AttackTables() {
    generateLeapingAttacks();
    generateMagicTables();
}

/* -- Leaping attacks -- */

// These attack bitboards are easy since pieces cannot block their way. Therefore,
// it is enough to just set the bits that they can target (while taking into account
// the board edge).

BB::BitBoard AttackTables::generatePawnAttacks(Board::Player player, const Position& position) {
    BB::BitBoard piece = 0ULL;
    BB::BitBoard result = 0ULL;

    BB::set_bit(piece, position);

    if (player == Board::White) {
        result |= (piece & not_col_H) >> 7;
        result |= (piece & not_col_A) >> 9;
    } else {
        result |= (piece & not_col_A) << 7;
        result |= (piece & not_col_H) << 9;
    }

    return result;
}

BB::BitBoard AttackTables::generateKnightAttacks(const Position& position) {
    BB::BitBoard piece = 0ULL;
    BB::BitBoard result = 0ULL;

    BB::set_bit(piece, position);

    result |= (piece & not_col_A & not_row_78) >> 17;
    result |= (piece & not_col_A & not_row_12) << 15;

    result |= (piece & not_col_H & not_row_78) >> 15;
    result |= (piece & not_col_H & not_row_12) << 17;

    result |= (piece & not_col_AB & not_row_8) >> 10;
    result |= (piece & not_col_AB & not_row_1) << 6;

    result |= (piece & not_col_GH & not_row_8) >> 6;
    result |= (piece & not_col_GH & not_row_1) << 10;

    return result;
}

BB::BitBoard AttackTables::generateKingAttacks(const Position& position) {
    BB::BitBoard piece = 0ULL;
    BB::BitBoard result = 0ULL;

    BB::set_bit(piece, position);

    result |= (piece & not_col_A & not_row_8) >> 9;
    result |= (piece & not_col_A & not_row_1) << 7;
    result |= (piece & not_col_H & not_row_8) >> 7;
    result |= (piece & not_col_H & not_row_1) << 9;

    result |= (piece & not_col_A) >> 1;
    result |= (piece & not_col_H) << 1;
    result |= (piece & not_row_8) >> 8;
    result |= (piece & not_row_1) << 8;

    return result;
}

void AttackTables::generateLeapingAttacks() {
    for (int position = 0; position < 64; position++) {
        for (int player = 0; player < 2; player++) {
            pawn_attacks[player][position] = generatePawnAttacks((Board::Player) player, position);
        }

        knight_attacks[position] = generateKnightAttacks(position);
        king_attacks[position] = generateKingAttacks(position);
    }
}

/* -- Sliding attacks -- */

// These attack bitboards are much harder create a lookup table for, since pieces
// can block their path. This implementation uses the magic bitboard hashing technique.

BB::BitBoard AttackTables::generateRelevanceMask(const Position& position, uint8_t directions) {
    BB::BitBoard result = 0ULL;

    for (int d_index = 0; d_index < 8; d_index++) {
        if (directions & ((uint8_t) 1 << d_index)) {
            Direction direction {(Direction::D) d_index};
            Position current = position;

            while (
                (current = direction.advance(current)).isValid()
                && direction.advance(current).isValid()
            ) {
                BB::set_bit(result, current);
            }
        }
    }

    return result;
}

std::vector<BB::BitBoard> AttackTables::generateOccupancies(
    const BB::BitBoard& relevance_mask
) {
    BB::BitBoard bit_subset = 0ULL;
    std::vector<BB::BitBoard> result;

    do {
        // Actual black magic. Insane.
        bit_subset = (bit_subset - relevance_mask) & relevance_mask;
        result.push_back(bit_subset);
    } while (bit_subset);

    return result;
}

BB::BitBoard AttackTables::generateRayMask(
    const Position& position, uint8_t directions,
    const BB::BitBoard& blockers
) {
    BB::BitBoard result = 0ULL;

    for (int d_index = 0; d_index < 8; d_index++) {
        if (directions & (uint8_t(1) << d_index)) {
            Direction direction {(Direction::D) d_index};
            Position current = position;

            // Set bit along line of sight until board edge or blocker
            while ((current = direction.advance(current)).isValid()) {
                BB::set_bit(result, current);

                if (BB::get_bit(blockers, current) == 1) break;
            }
        }
    }

    return result;
}

std::unique_ptr<AttackTables::Magic> AttackTables::generateMagicTableForPosition(
    const Position& position, uint8_t directions
) {
    auto relevance_mask = generateRelevanceMask(position, directions);
    auto occupancies = generateOccupancies(relevance_mask);

    std::vector<BB::BitBoard> ray_masks;

    for (const auto& occupancy : occupancies) {
        ray_masks.push_back(generateRayMask(position, directions, occupancy));
    }

    auto magic = std::make_unique<Magic>();
    
    magic->relevance_mask = relevance_mask;
    // The number of bits of the table index is equal to the number of relevant blocker squares
    // since we store attack bitboards for each possible combination of blockers.
    magic->index_bits = popcount(relevance_mask);

    bool succeeded = false;
    const BB::BitBoard dummy = ~0ULL;

#ifndef FIND_NEW_MAGICS
    const uint64_t * magic_numbers = 
        directions == 0b01010101 
        ? bishop_magic_numbers 
        : rook_magic_numbers;
#endif

    // Try random magic numbers, see if it maps all occupancies without non-constructive
    // collision (basically, if the resulting ray masks are the same for two occupancies,
    // it doesn't matter if they collide in the hash table since they have the same result).
    while (!succeeded) {
        // Fill the table with dummy value ~0ULL.
        std::vector<BB::BitBoard> magic_table(1 << magic->index_bits, dummy);
        // Generate a random magic number. Biased towards 0 since magic numbers don't
        // require a lot of set bits.
#ifndef FIND_NEW_MAGICS
        uint64_t possible_magic_number = magic_numbers[position];
#else
        uint64_t possible_magic_number = random64() & random64() & random64();
#endif

        bool collision = false;

        for (size_t i = 0; i < occupancies.size(); i++) {
            const auto occupancy = occupancies[i];
            const auto ray_mask = ray_masks[i];
            
            // The index of the attack bitboard (ray mask) in the magic table follows this
            // basic hash formula. We take the most significant bits for the index because
            // of their high entropy (since occupancy is probably a rather large number, the
            // more significant bits are affected a lot more).
            const int index = (occupancy * possible_magic_number) >> (64 - magic->index_bits);

            if (magic_table[index] == dummy) {
                // No collision at all since the slot in the table is empty.
                magic_table[index] = ray_mask;
            } else if (magic_table[index] != ray_mask) {
                // Non-constructive collision since the ray masks aren't the same.
                collision = true;
                break;
            }
        }

        if (!collision) {
            magic->table = std::move(magic_table);
            magic->magic_number = possible_magic_number;
            // std::cout << magic->magic_number << std::endl;
            succeeded = true;
        }
    }

    return std::move(magic);
}

void AttackTables::generateMagicTables() {
    std::srand(std::time(0));

    for (int position = 0; position < 64; position++) {
        auto bishop_magic = generateMagicTableForPosition(position, 0b01010101);
        auto rook_magic = generateMagicTableForPosition(position, 0b10101010);

        bishop_magics[position] = std::move(bishop_magic);
        rook_magics[position] = std::move(rook_magic);
    }
}

BB::BitBoard AttackTables::getSlidingAttackTable(
    const Position& position, BB::BitBoard occupancy,
    const std::unique_ptr<Magic> * magics
) const {
    auto magic = *magics[position];

    // Apply the hashing calculation to find the index of the attack bitboard.
    occupancy &= magic.relevance_mask;      // Get the relevant blockers
    occupancy *= magic.magic_number;        // Multiply by the magic number
    occupancy >>= 64 - magic.index_bits;    // Shift to get the most significant bits

    return magic.table[occupancy];
}

/* ---- END DEFINE ---- */

