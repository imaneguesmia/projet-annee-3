#pragma once

#include "attack_tables.hpp"
#include "position.hpp"
#include "player.hpp"
#include "board.hpp"
#include "move.hpp"
#include "board_analysis.hpp"

#include <memory>

/* ---- DECLARE class MoveGenerator ---- */

/**
 * @brief Class handling legal move generation.
 */
class MoveGenerator {
    // Constant bitboards used for pawn double pushes and promotions.
    static constexpr BB::BitBoard row_4 {1095216660480ULL};
    static constexpr BB::BitBoard row_5 {4278190080ULL};
    static constexpr BB::BitBoard row_1 {18374686479671623680ULL};
    static constexpr BB::BitBoard row_8 {255ULL};

    // Lookup table of relevant board squares to check when castling.
    static constexpr Square relevant_castling_squares[4][2] {
        {Square::f1, Square::g1},   // King-side white
        {Square::d1, Square::c1},   // Queen-side white
        {Square::f8, Square::g8},   // King-side black
        {Square::d8, Square::c8}    // Queen-side black
    };

    // Shared pointer to the pre-initialized attack tables.
    const std::shared_ptr<const AttackTables> at;
    const BoardAnalysis board_analysis;

    /**
     * @brief Filters pseudo-legals to produce true legal moves.
     * 
     * @param player    The player making each move.
     * @param board     The current board state.
     * @param pseudo_legals Vector containing pseudo-legal moves.
     * @return A vector containing all legal moves.
     */
    std::vector<Move> filterPseudoLegals(
        const Player player, const Board& board, 
        const std::vector<Move>& pseudo_legals
    ) const;

public:
    MoveGenerator(const std::shared_ptr<const AttackTables> at) 
        : at(at)
        , board_analysis(std::move(at))
    {};
    ~MoveGenerator() {};

    /**
     * @brief Generates all possible pseudo-legal moves from the given board state.
     * 
     * @param player                The player making each move.
     * @param board                 The current board state.
     * @param en_passant_position   The current valid en passant target.
     * @param castling_rights       Bitflags indicating castling rights.
     * @return A vector containing all found pseudo-legal moves.
     */
    std::vector<Move> generatePseudoLegals(
        const Player player, const Board& board,
        const Position& en_passant_position, const uint8_t castling_rights
    ) const;

    /**
     * @brief Generates all legal moves that a player can make from the current position.
     * 
     * @param player                The player making each move.
     * @param board                 The current board state.
     * @param en_passant_position   The current valid en passant target.
     * @param castling_rights       Bitflags indicating castling rights.
     * @return A vector containing all found legal moves.
     */
    std::vector<Move> generateMoves(
        const Player player, const Board& board,
        const Position& en_passant_position, const uint8_t castling_rights
    ) const;

    void _test() {
        // std::cout << "hello world" << std::endl;
        // // Board b("8/8/8/8/8/8/8/8 w K e3 0 0");
        // Board b("6k1/1P6/5q2/3Pp3/3P4/8/8/R3K2R w KQ - 0 1");
        // // Board b("8/8/8/8/8/8/8/R3K3 w Q - 0 1");
        // std::cout << b << std::endl;

        // auto moves = generatePseudoLegals(b.getCurrentPlayer(), b);
        // auto legals = filterPseudoLegals(b.getCurrentPlayer(), b, moves);

        // for (const auto& m : legals) {
        //     std::cout << m << '\n';
        // }

        // std::cout << "Squares attacked by White" << std::endl;

        // Position position {0};

        // for (int row = 0; row < 8; row++) {
        //     for (int column = 0; column < 8; column++) {
        //         std::cout << (isSquareAttacked(position++, Player::White, b) ? 'X' : '.') << ' ';
        //     }

        //     std::cout << std::endl;
        // }

        // std::cout << "Squares attacked by Black" << std::endl;

        // position = {0};

        // for (int row = 0; row < 8; row++) {
        //     for (int column = 0; column < 8; column++) {
        //         std::cout << (isSquareAttacked(position++, Player::Black, b) ? 'X' : '.') << ' ';
        //     }

        //     std::cout << std::endl;
        // }
    };
};

/* ---- END DECLARE ---- */