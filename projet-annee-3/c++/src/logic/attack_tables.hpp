#pragma once

#include "bitboard.hpp"
#include "board.hpp"
#include "position.hpp"
#include "player.hpp"

#include <vector>
#include <memory>

/* ---- DECLARE class AttackTables ---- */

class AttackTables {
    // Constant bitboards used in leaping attack table generation
    const BB::BitBoard not_col_A {18374403900871474942ULL};
    const BB::BitBoard not_col_H {9187201950435737471ULL};
    const BB::BitBoard not_col_AB {18229723555195321596ULL};
    const BB::BitBoard not_col_GH {4557430888798830399ULL};
    const BB::BitBoard not_row_1 {72057594037927935ULL};
    const BB::BitBoard not_row_8 {18446744073709551360ULL};
    const BB::BitBoard not_row_12 {281474976710655ULL};
    const BB::BitBoard not_row_78 {18446744073709486080ULL};

    /* -- Leaping attacks -- */

    BB::BitBoard pawn_attacks[2][64];   // [player][position]
    BB::BitBoard knight_attacks[64];    // [position]
    BB::BitBoard king_attacks[64];      // [position]

    BB::BitBoard generatePawnAttacks(Player player, const Position& position);
    BB::BitBoard generateKnightAttacks(const Position& position);
    BB::BitBoard generateKingAttacks(const Position& position);

    void generateLeapingAttacks();

    /* -- Sliding attacks -- */

    // Helper class to handle advancing a position along a direction.
    class Direction {
        int dx, dy;

    public:
        enum D {
            NE, E, SE, S, SW, W, NW, N
        };

        Direction(D direction) {
            switch (direction) {
                case NE: dx =  1, dy = -1; break;
                case E:  dx =  1, dy =  0; break;
                case SE: dx =  1, dy =  1; break;
                case S:  dx =  0, dy =  1; break;
                case SW: dx = -1, dy =  1; break;
                case W:  dx = -1, dy =  0; break;
                case NW: dx = -1, dy = -1; break;
                case N:  dx =  0, dy = -1; break;
            }
        }

        Position advance(const Position& position) const {
            int new_x = position.getColumn() + dx;
            int new_y = position.getRow() + dy;

            try {
                return Position(new_y, new_x);
            } catch (const std::invalid_argument&) {
                return Position(-1);
            }
        }
    };

    // Generates bitboard of all squares seen by a piece in the given directions at
    // the given position, not considering any blockers. This is a relevancy mask, AKA
    // the possible blocker squares that we will consider when calculating, so it excludes
    // edge squares since blockers on those squares don't block anything anyway.
    BB::BitBoard generateRelevanceMask(const Position& position, uint8_t directions);

    // Bishop relevance mask: along diagonals
    constexpr BB::BitBoard bishopRelevanceMask(const Position& position) {
        return generateRelevanceMask(position, 0b01010101);
    };
    // Rook relevance mask: along rows/columns
    constexpr BB::BitBoard rookRelevanceMask(const Position& position) {
        return generateRelevanceMask(position, 0b10101010);
    };

    // Generates all possible occupancy configurations for the given relevance mask.
    std::vector<BB::BitBoard> generateOccupancies(const BB::BitBoard& relevance_mask);

    // Generates bitboard of all squares seen by a piece in the given directions at
    // the given position, given the blockers.
    BB::BitBoard generateRayMask(
        const Position& position, uint8_t directions,
        const BB::BitBoard& blockers
    );

    struct Magic {
        std::vector<BB::BitBoard> table;
        BB::BitBoard relevance_mask;
        uint64_t magic_number;
        uint8_t index_bits;
    };

    // It's literally magic.
    std::unique_ptr<Magic> generateMagicTableForPosition(
        const Position& position, uint8_t directions
    );

    std::unique_ptr<Magic> bishop_magics[64];   // [position]
    std::unique_ptr<Magic> rook_magics[64];     // [position]

    void generateMagicTables();

    BB::BitBoard getSlidingAttackTable(
        const Position& position, BB::BitBoard occupancy,
        const std::unique_ptr<Magic> * magics
    ) const;

public:
    AttackTables();
    ~AttackTables() {};

    constexpr BB::BitBoard getPawnAttackBitboard(
        Player player, const Position& position
    ) const {
        return pawn_attacks[player == Player::Black][position];
    }
    constexpr BB::BitBoard getKnightAttackBitboard(const Position& position) const {
        return knight_attacks[position];
    }
    constexpr BB::BitBoard getKingAttackBitboard(const Position& position) const {
        return king_attacks[position];
    }

    BB::BitBoard getBishopAttackBitboard(
        const Position& position, const BB::BitBoard& occupancy
    ) const {
        return getSlidingAttackTable(position, occupancy, bishop_magics);
    }
    BB::BitBoard getRookAttackBitboard(
        const Position& position, const BB::BitBoard& occupancy
    ) const {
        return getSlidingAttackTable(position, occupancy, rook_magics);
    }
    // Queen attack is just bishop + rook.
    BB::BitBoard getQueenAttackBitboard(
        const Position& position, const BB::BitBoard& occupancy
    ) const {
        return (
            getBishopAttackBitboard(position, occupancy) |
            getRookAttackBitboard(position, occupancy)
        );
    }
};

/* ---- END DECLARE ---- */