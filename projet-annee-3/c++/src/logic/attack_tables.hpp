#pragma once

#include "bitboard.hpp"
#include "board.hpp"
#include "position.hpp"
#include "player.hpp"

#include <vector>
#include <memory>

/* ---- DECLARE class AttackTables ---- */

/**
 * @brief Helper class to generate piece attack lookup tables in advance.
 * 
 * Used to avoid having to expensively generate attack bitboards on the fly. Leaper pieces use simple lookup
 * tables, while slider pieces use the magic bitboard hashing technique (https://www.chessprogramming.org/Magic_Bitboards).
 */
class AttackTables {
    // Constant bitboards used in leaping attack table generation.
    static constexpr BB::BitBoard not_col_A {18374403900871474942ULL};
    static constexpr BB::BitBoard not_col_H {9187201950435737471ULL};
    static constexpr BB::BitBoard not_col_AB {18229723555195321596ULL};
    static constexpr BB::BitBoard not_col_GH {4557430888798830399ULL};
    static constexpr BB::BitBoard not_row_1 {72057594037927935ULL};
    static constexpr BB::BitBoard not_row_8 {18446744073709551360ULL};
    static constexpr BB::BitBoard not_row_12 {281474976710655ULL};
    static constexpr BB::BitBoard not_row_78 {18446744073709486080ULL};

    /* -- Leaping attacks -- */

    // Lookup tables for leaper pieces.
    BB::BitBoard pawn_attacks[2][64];   // [Player][Position]
    BB::BitBoard knight_attacks[64];    // [Position]
    BB::BitBoard king_attacks[64];      // [Position]

    /**
     * @brief Generates all possible pawn attacks from the given position for the given player.
     * 
     * @param player    The player to generate attacks for, as pawns are not symetrical in behavior.
     * @param position  The position to generate attacks from.
     * @return A bitboard indicating all attack positions.
     */
    BB::BitBoard generatePawnAttacks(Player player, const Position& position);
    /**
     * @brief Generates all possible knight attacks from the given position.
     * 
     * @param position  The position to generate attacks from.
     * @return A bitboard indicating all attack positions.
     */
    BB::BitBoard generateKnightAttacks(const Position& position);
    /**
     * @brief Generates all possible king attacks from the given position.
     * 
     * @param position  The position to generate attacks from.
     * @return A bitboard indicating all attack positions.
     */
    BB::BitBoard generateKingAttacks(const Position& position);

    /**
     * @brief Generates attacks for each leaper piece type and stores them in the relevant lookup tables.
     */
    void generateLeapingAttacks();

    /* -- Sliding attacks -- */

    /**
     * @brief Helper class to handle advancing a position along a direction.
     */
    class Direction {
        // x- and y-difference for advancing along this direction.
        int dx, dy;

    public:
        // Enum over all 8 cardinal directions.
        enum D {
            NE, E, SE, S, SW, W, NW, N
        };

        /**
         * @brief Construct a new Direction object.
         * 
         * @param direction The direction to will handle.
         */
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

        /**
         * @brief Advances the given position along this direction.
         * 
         * @param position  The position to advance.
         * @return The new position obtained after advancing.
         */
        Position advance(const Position& position) const {
            int new_x = position.getColumn() + dx;
            int new_y = position.getRow() + dy;

            try {
                return Position(new_y, new_x);
            } catch (const std::invalid_argument&) {
                return Position();
            }
        }
    };

    // 8 bitflags for each cardinal direction, indicating the directions that each piece can attack in.
    static constexpr uint8_t BISHOP_DIRECTIONS {0b01010101};
    static constexpr uint8_t ROOK_DIRECTIONS {0b01010101};

    /**
     * @brief Generates bitboard of all possible relevant blocker positions for a piece at the
     * given position in the given directions.
     * 
     * This method generates a relevance mask — the possible blocker squares that we will consider
     * when calculating sliding attacks. It excludes edge squares since blockers on those squares
     * do not block anything behind them.
     * 
     * @param position      The position to generate the relevance mask for.
     * @param directions    Bitflags for the directions that the relevance mask considers – what the 
     *                      piece can "see". 
     * @return A bitboard indicating all possible relevant blocker positions.
     */
    BB::BitBoard generateRelevanceMask(const Position& position, uint8_t directions);

    /**
     * @brief Generates a bishop relevance mask for the given position.
     * 
     * @param position      The position to generate the relevance mask for.
     * @return A bitboard indicating all possible relevant blocker positions.
     */
    constexpr BB::BitBoard bishopRelevanceMask(const Position& position) {
        return generateRelevanceMask(position, BISHOP_DIRECTIONS);
    };
    /**
     * @brief Generates a rook relevance mask for the given position.
     * 
     * @param position      The position to generate the relevance mask for.
     * @return A bitboard indicating all possible relevant blocker positions.
     */
    constexpr BB::BitBoard rookRelevanceMask(const Position& position) {
        return generateRelevanceMask(position, ROOK_DIRECTIONS);
    };

    /**
     * @brief Generates all possible configurations of relevant blockers for the given relevance mask.
     * 
     * @param relevance_mask    The relevance mask to consider.
     * @return A vector containing bitboards indicating each possible occupancy configuration.
     */
    std::vector<BB::BitBoard> generateOccupancies(const BB::BitBoard& relevance_mask);

    /**
     * @brief Generates a bitboard of all squares seen by a piece in the given directions at the given 
     * position, considering blockers.
     * 
     * This method generates a ray mask – the squares visible in each direction by a piece at a given
     * position. It uses a naive "casting" approach of advancing the piece's position in each direction until
     * a blocker is encountered. This method is too expensive to be used on the fly, so it is used
     * purely for initialization purposes.
     * 
     * @param position      The position to generate the ray mask for.
     * @param directions    Bitflags for the directions that the ray mask considers.
     * @param blockers      Bitboard indicating the blockers to consider when casting.
     * @return A bitboard indicating all visible squares, including blocking blockers.
     */
    BB::BitBoard generateRayMask(
        const Position& position, uint8_t directions,
        const BB::BitBoard& blockers
    );

    /**
     * @brief Magic table entry, containing all the necessary information for magic bitboard hashing.
     * 
     * Since it isn't possible to generate a naive lookup table for all possible combinations of blockers,
     * we reduce the size of the lookup table by using ONLY the relevant blockers as the key to a hash table.
     */
    struct Magic {
        std::vector<BB::BitBoard> table;    // Hash table of all possible ray masks to be used as attack bitboards.
        BB::BitBoard relevance_mask;        // Relevance mask of all blockers considered during ray mask generation.
        uint64_t magic_number;              // Magic number used for hashing over the hash table.
        uint8_t index_bits;                 // Number of bits used to index the hash table.
    };

    /**
     * @brief Generates a magic table entry for the given position and directions.
     * 
     * Uses a brute-force generate-and-test approach for finding a working magic number for this piece's
     * ray mask hash table.
     * 
     * @param position      The position to generate the magic table entry for.
     * @param directions    Bitflags for the directions that the piece can attack in.
     * @return Pointer to a magic table entry.
     */
    std::unique_ptr<Magic> generateMagicTableForPosition(
        const Position& position, uint8_t directions
    );

    // Lookup table for magic entries.
    std::unique_ptr<Magic> bishop_magics[64];   // [Position]
    std::unique_ptr<Magic> rook_magics[64];     // [Position]

    /**
     * @brief Generates magic tables for rook and bishop slider pieces and stores them in the relevant
     * lookup tables.
     * 
     * Queen magic tables are unnecessary, as queen moves are merely a composite of rook and bishop moves.
     */
    void generateMagicTables();

    /**
     * @brief Gets the attack bitboard of a sliding piece.
     * 
     * @param position      The position of the attacker.
     * @param occupancy     Bitboard indicating the occupied squares of the board.
     * @param magics        The lookup table used to find the relevant magic table.
     * @return A bitboard indicating all attack positions.
     */
    BB::BitBoard getSlidingAttackTable(
        const Position& position, BB::BitBoard occupancy,
        const std::unique_ptr<Magic> * magics
    ) const;

public:
    /**
     * @brief Constructs a new AttackTables object.
     * 
     * All attack tables are generated on construction.
     */
    AttackTables();
    ~AttackTables() {};

    /**
     * @brief Gets the attack bitboard of a pawn.
     * 
     * @param player    The player who owns the pawn.
     * @param position  The position of the pawn.
     * @return A bitboard indicating all attack positions of the pawn.
     */
    constexpr BB::BitBoard getPawnAttackBitboard(
        Player player, const Position& position
    ) const {
        return pawn_attacks[int(player)][position];
    }
    /**
     * @brief Gets the attack bitboard of a knight.
     * 
     * @param position  The position of the knight.
     * @return A bitboard indicating all attack positions of the knight.
     */
    constexpr BB::BitBoard getKnightAttackBitboard(const Position& position) const {
        return knight_attacks[position];
    }
    /**
     * @brief Gets the attack bitboard of a king.
     * 
     * @param position  The position of the king.
     * @return A bitboard indicating all attack positions of the king.
     */
    constexpr BB::BitBoard getKingAttackBitboard(const Position& position) const {
        return king_attacks[position];
    }

    /**
     * @brief Gets the attack bitboard of a bishop.
     * 
     * @param position  The position of the bishop.
     * @param occupancy Bitboard indicating the occupied squares of the board.
     * @return A bitboard indicating all attack positions of the bishop.
     */
    BB::BitBoard getBishopAttackBitboard(
        const Position& position, const BB::BitBoard& occupancy
    ) const {
        return getSlidingAttackTable(position, occupancy, bishop_magics);
    }
    /**
     * @brief Gets the attack bitboard of a rook.
     * 
     * @param position  The position of the rook.
     * @param occupancy Bitboard indicating the occupied squares of the board.
     * @return A bitboard indicating all attack positions of the rook.
     */
    BB::BitBoard getRookAttackBitboard(
        const Position& position, const BB::BitBoard& occupancy
    ) const {
        return getSlidingAttackTable(position, occupancy, rook_magics);
    }
    /**
     * @brief Gets the attack bitboard of a queen.
     * 
     * This is just the composite of a bishop and rook attack bitboard.
     * 
     * @param position  The position of the queen.
     * @param occupancy Bitboard indicating the occupied squares of the board.
     * @return A bitboard indicating all attack positions of the queen.
     */
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