#include "attack_tables.hpp"

#include "../misc/increment_enum.hpp"

#ifndef FIND_NEW_MAGICS
#include "magic_numbers.hpp"
#include "../misc/random64.hpp"
#endif

#include <iostream>
#include <ctime>
#include <random>

/* ---- DEFINE class AttackTables ---- */

AttackTables::AttackTables() {
    generateLeapingAttacks();
    generateMagicTables();
}

/* -- Leaping attacks -- */

// These attack bitboards are easy since pieces cannot block their way. Therefore,
// it is enough to just set the bits that they can target (while taking into account
// the board edge).

BB::BitBoard AttackTables::generatePawnAttacksFromPosition(Player player, const Position& position) {
    BB::BitBoard piece = 0ULL;
    BB::BitBoard result = 0ULL;

    BB::set_bit(piece, position);

    if (player == Player::White) {
        result |= (piece & not_col_H) >> 7;
        result |= (piece & not_col_A) >> 9;
    } else {
        result |= (piece & not_col_A) << 7;
        result |= (piece & not_col_H) << 9;
    }

    return result;
}

BB::BitBoard AttackTables::generateKnightAttacksFromPosition(const Position& position) {
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

BB::BitBoard AttackTables::generateKingAttacksFromPosition(const Position& position) {
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
    for (Square position = Square::FIRST; position != Square::OOB; increment_enum(position)) {
        for (Player player = Player::FIRST; player != Player::OOB; increment_enum(player)) {
            pawn_attacks[player][position] = generatePawnAttacksFromPosition(Player(player), position);
        }

        knight_attacks[position] = generateKnightAttacksFromPosition(position);
        king_attacks[position] = generateKingAttacksFromPosition(position);
    }
}

/* -- Sliding attacks -- */

// These attack bitboards are much harder create a lookup table for, since pieces
// can block their path. This implementation uses the magic bitboard hashing technique.

BB::BitBoard AttackTables::generateRelevanceMask(const Position& position, uint8_t directions) {
    BB::BitBoard result = 0ULL;

    for (int d_index = 0; d_index < 8; d_index++) {
        if (directions & (uint8_t(1) << d_index)) {
            Direction direction {Direction::D(d_index)};
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
    magic->index_bits = BB::popcount(relevance_mask);

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

#ifndef FIND_NEW_MAGICS
        uint64_t possible_magic_number = magic_numbers[position];
#else
        // Generate a random magic number. Biased towards 0 since magic numbers don't
        // require a lot of set bits.
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
            // No collision, hash table successfully generated.
            magic->table = std::move(magic_table);
            magic->magic_number = possible_magic_number;
            succeeded = true;
        }
    }

    return magic;
}

void AttackTables::generateMagicTables() {
    std::srand(std::time(0));

    for (Square position = Square::FIRST; position != Square::OOB; increment_enum(position)) {
        auto bishop_magic = generateMagicTableForPosition(position, 0b01010101);
        auto rook_magic = generateMagicTableForPosition(position, 0b10101010);

        bishop_magics[position] = std::move(bishop_magic);
        rook_magics[position] = std::move(rook_magic);
    }
}

BB::BitBoard AttackTables::getSlidingAttackTable(
    const Position& position, BB::BitBoard occupancy,
    const enum_array<Square, std::unique_ptr<Magic>>& magics
) const {
    auto magic = *magics[position.getPositionSquare()];

    // Apply the hashing calculation to find the index of the attack bitboard.
    occupancy &= magic.relevance_mask;      // Get the relevant blockers
    occupancy *= magic.magic_number;        // Multiply by the magic number
    occupancy >>= 64 - magic.index_bits;    // Shift to get the most significant bits

    return magic.table[occupancy];
}

/* ---- END DEFINE ---- */