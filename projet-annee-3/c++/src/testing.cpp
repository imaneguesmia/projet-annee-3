/*
This is an entry point to test things. The actual final product will be a
C++ library and will not have any entry point.
*/

#include "logic/board.hpp"
#include "logic/attack_tables.hpp"
#include "logic/position.hpp"
#include "logic/player.hpp"

#include <iostream>
#include <vector>
#include <utility>

int main(int argc, char ** argv) {
    std::cout << "Hello worlderferf !" << std::endl;

    Board b;
    std::cout << b << std::endl;

    AttackTables a;

    std::vector<Position> white_pawn_tests {"e4", "h4", "a4"};
    std::vector<Position> black_pawn_tests {"e4", "h4", "a4"};
    std::vector<Position> knight_tests {"e4", "h4", "a4", "g4", "b4", "e8", "e1", "e7", "e2"};
    std::vector<Position> king_tests {"e4", "h4", "a4", "e8", "e1"};

    std::vector<Position> slider_test_positions {"e4", "e1", "a4"};
    BB::BitBoard occ {
        0b1110100110101011110000100010011001010001001110100001001010011100ULL
    };

    // Pawn tests
    for (const auto& p : white_pawn_tests) {
        BB::out(std::cout << "** White Pawn at " << p << " **\n", a.getPawnAttackBitboard(Player::White, p));
    }
    for (const auto& p : black_pawn_tests) {
        BB::out(std::cout << "** Black Pawn at " << p << " **\n", a.getPawnAttackBitboard(Player::Black, p));
    }
    // Knight tests
    for (const auto& p : knight_tests) {
        BB::out(std::cout << "** Knight at " << p << " **\n", a.getKnightAttackBitboard(p));
    }
    // King tests
    for (const auto& p : king_tests) {
        BB::out(std::cout << "** King at " << p << " **\n", a.getKingAttackBitboard(p));
    }
    for (const auto& p : slider_test_positions) {
        // Bishop tests
        std::cout << "** Bishop at " << p << " **\n";
        BB::out(std::cout << "Occupancy\n", occ);
        BB::out(std::cout << "Result\n", a.getBishopAttackBitboard(p, occ));
        // Rook tests
        std::cout << "** Rook at " << p << " **\n";
        BB::out(std::cout << "Occupancy\n", occ);
        BB::out(std::cout << "Result\n", a.getRookAttackBitboard(p, occ));
        // Queen tests
        std::cout << "** Queen at " << p << " **\n";
        BB::out(std::cout << "Occupancy\n", occ);
        BB::out(std::cout << "Result\n", a.getQueenAttackBitboard(p, occ));
    }

    return 0;
}