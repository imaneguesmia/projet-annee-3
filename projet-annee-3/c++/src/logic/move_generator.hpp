#pragma once

#include "attack_tables.hpp"
#include "position.hpp"
#include "player.hpp"
#include "board.hpp"
#include "move.hpp"

/* ---- DECLARE class MoveGenerator ---- */

class MoveGenerator {
    static constexpr BB::BitBoard row_4 {4278190080ULL};
    static constexpr BB::BitBoard row_5 {1095216660480ULL};
    static constexpr BB::BitBoard row_1 {18374686479671623680ULL};
    static constexpr BB::BitBoard row_8 {255ULL};

    // Truly the most space- vs time-complexity ever made. Lookup tables, lookup tables everywhere.
    static constexpr int relevant_castling_squares[4][2] {
        {Position::f1, Position::g1},   // King-side white
        {Position::e1, Position::d1},   // Queen-side white
        {Position::f8, Position::g8},   // King-side black
        {Position::e8, Position::d8}    // Queen-side black
    };

    static constexpr Piece::Type valid_promotions[4] {
        Piece::Knight, Piece::Bishop, Piece::Rook, Piece::Queen
    };

    AttackTables at;

    bool isSquareAttacked(const Position& position, Player player, const Board& board) const;

    void filterPseudoLegals(
        const Position& position, Player player, const Board& board, 
        BB::BitBoard& pseudo_legals
    ) const;
    std::vector<Move> generatePseudoLegals(const Board& board) const;

public:
    MoveGenerator() {};
    ~MoveGenerator() {};

    void _test() {
        std::cout << "hello world" << std::endl;
        Board b("8/8/8/8/3pP3/8/8/8 w K e3 0 0");
        std::cout << b << std::endl;

        auto moves = generatePseudoLegals(b);

        for (const auto& m : moves) {
            std::cout << m << '\n';
        }

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