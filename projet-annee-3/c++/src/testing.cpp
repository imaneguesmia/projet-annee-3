/*
This is an entry point to test things. The actual final product will be a
C++ library and will not have any entry point.
*/

#include "logic/board.hpp"
#include "logic/attack_tables.hpp"
#include "logic/position.hpp"
#include "logic/player.hpp"
#include "logic/move_generator.hpp"
#include "logic/game.hpp"

#include "misc/safely_to_enum_class.hpp"

#include "minimax/Search.hpp"

#include "game_management/game_manager.hpp"

#include <iostream>
#include <vector>
#include <memory>
#include <stdexcept>

int main(int argc, char ** argv) {
    try {
        auto gm = GameManager("r1bqkbnr/pppppppp/2n5/8/4P3/2N5/PPPP1PPP/R1BQKBNR b KQkq - 1 2");
        // auto gm = GameManager("r1bqkbnr/pppppppp/2n5/8/4P3/2N5/PPPPPPPP/R1BQKBNR b KQkq - 1 2");
        auto at = std::make_shared<const AttackTables>();

        // auto g1 = Game(at);
        // auto g2 = Game(at);

        // std::cout << g1.fen() << ' ' << g1.getHash() << '\n';
        // std::cout << g2.fen() << ' ' << g2.getHash() << '\n';

        // std::cout <<(int)gm.getCurrentPlayer()<<'\n';

        auto s = gm.createMinimaxPlayer(1);

        auto m = s->getMove(gm.extendedGameData());

        std::cout << "Move:\n" << m << std::endl;

        std::cout << (m == Move::NO_MOVE) << '\n';
    } catch (const std::exception& e) {
        std::cout << e.what() << '\n';
        return 1;
    }
    
    // safely_to_enum_class<Player>(2);

    // std::cout << Position("e4") << '\n';
    // return 0;

    // std::cout << "Initial state :" << std::endl;

    // Board b("6k1/1P6/5q2/3Pp3/3P4/8/8/R3K2R w KQ - 0 1");
    // std::cout << b << std::endl;

    // Game b("6k1/1P6/5q2/3Pp3/3P4/8/8/R3K2R w KQ e6 0 1");
    // Game b;
    // Game b("3r1r2/8/8/8/8/8/r7/4K3 w - - 0 1");
    // Game b("3r1r2/8/8/8/7b/8/r7/4K3 w - - 0 1");
    // Game b("3r4/8/8/8/7b/8/r7/4K3 w - - 0 1");

    // b.printBoard();
    // std::cout << b.fen() << '\n';

    // enum_array<GameState, std::string> outtt;
    // outtt[GameState::INGAME] = "INGAME";
    // outtt[GameState::CHECKMATE] = "CHECKMATE";
    // outtt[GameState::STALEMATE] = "STALEMATE";

    // std::cout << outtt[b.getGameState()] << '\n';

    // // b.move({
    // //     Position::e1, Position::g1,
    // //     Player::White, PType::King,
    // //     PType::NoneType,
    // //     false, false, false, true
    // // });

    return 0;

    // Flags : capture, double_push, en_passant, castle

    // std::vector<Move> m {
    //     // Castling (this isn't technically legal, but it's for testing purposes)
    //     Move {
    //         Position(Square::e1), Position(Square::g1),
    //         Player::White, PType::King,
    //         PType::NoneType,
    //         false, false, false, true
    //     },
    //     // En passant
    //     Move {
    //         Position(Square::e5), Position(Square::d4),
    //         Player::Black, PType::Pawn,
    //         PType::NoneType,
    //         true, false, false, false
    //     },
    //     // Illegal move
    //     Move {
    //         Position(Square::g1), Position(Square::f2),
    //         Player::White, PType::King,
    //         PType::NoneType,
    //         false, false, false, false
    //     },
    //     // Promotion
    //     Move {
    //         Position(Square::b7), Position(Square::b8),
    //         Player::White, PType::Pawn,
    //         PType::Queen,
    //         false, false, false, false
    //     },
    //     // Illegal move (black king was checked last turn)
    //     Move {
    //         Position(Square::f6), Position(Square::f1),
    //         Player::Black, PType::Queen,
    //         PType::NoneType,
    //         true, false, false, false
    //     },
    //     // Legal move (blocking the check)
    //     Move {
    //         Position(Square::f6), Position(Square::f8),
    //         Player::Black, PType::Queen,
    //         PType::NoneType,
    //         false, false, false, false
    //     },
    // };

    // for (const auto& move : m) {
    //     if (b.move(move)) {
    //         std::cout << "\nDid legal move\n" << move;

    //         b.printBoard();
    //         std::cout << b.fen() << '\n';
    //     } else {
    //         std::cout << "\nMove was illegal\n" << move;
    //     }
    // }

    // for (auto lm = b.undoLastMove(); lm != std::nullopt; lm = b.undoLastMove()) {
    //     std::cout << "\nUndid last move\n";

    //     b.printBoard();
    //     std::cout << b.fen() << '\n';
    // }

    // std::cout << "Done undoing all moves!\n";

    // b.makeMove({
    //     Position::e1, Position::g1,
    //     PType::W_King,
    //     PType::NoneId,
    //     false, false, false, true
    // });

    // b.makeMove({
    //     Position::c7, Position::c8,
    //     PType::W_Pawn,
    //     PType::W_Queen,
    //     false, false, false, false
    // });

    // std::cout << b << std::endl;

    // b.makeMove({
    //     Position::e2, Position::e4,
    //     PType::W_Pawn,
    //     PType::NoneId,
    //     false, true, false, false
    // });

    // std::cout << b << std::endl;

    // b.makeMove({
    //     Position::d7, Position::d5,
    //     PType::B_Pawn,
    //     PType::NoneId,
    //     false, true, false, false
    // });

    // std::cout << b << std::endl;

    // uint8_t source      : 6;
    // uint8_t target      : 6;
    // uint8_t piece       : 4;
    // uint8_t promotion   : 4;
    // bool capture        : 1;
    // bool double_push    : 1;
    // bool en_passant     : 1;
    // bool castle         : 1;

    // AttackTables a;

    // std::vector<Position> white_pawn_tests {"e4", "h4", "a4", "e2", "e7"};
    // std::vector<Position> white_pawn_tests {"e4", "h4", "a4", "e2", "e7"};
    // std::vector<Position> black_pawn_tests {"e4", "h4", "a4", "e2", "e7"};
    // std::vector<Position> knight_tests {"e4", "h4", "a4", "g4", "b4", "e8", "e1", "e7", "e2"};
    // std::vector<Position> king_tests {"e4", "h4", "a4", "e8", "e1"};

    // std::vector<Position> slider_test_positions {"e4", "e1", "a4"};
    // BB::BitBoard occ {
    //     0b1110100110101011110000100010011001010001001110100001001010011100ULL
    // };

    // Pawn tests
    // for (const auto& p : white_pawn_tests) {
    //     BB::out(std::cout << "** White Pawn at " << p << " **\n", a.getPawnAttackBitboard(Player::White, p) | a.getPawnPushBitboard(Player::White, p));
    // }
    // for (const auto& p : black_pawn_tests) {
    //     BB::out(std::cout << "** Black Pawn at " << p << " **\n", a.getPawnAttackBitboard(Player::Black, p) | a.getPawnPushBitboard(Player::Black, p));
    // }
    // // Knight tests
    // for (const auto& p : knight_tests) {
    //     BB::out(std::cout << "** Knight at " << p << " **\n", a.getKnightAttackBitboard(p));
    // }
    // // King tests
    // for (const auto& p : king_tests) {
    //     BB::out(std::cout << "** King at " << p << " **\n", a.getKingAttackBitboard(p));
    // }
    // for (const auto& p : slider_test_positions) {
    //     // Bishop tests
    //     std::cout << "** Bishop at " << p << " **\n";
    //     BB::out(std::cout << "Occupancy\n", occ);
    //     BB::out(std::cout << "Result\n", a.getBishopAttackBitboard(p, occ));
    //     // Rook tests
    //     std::cout << "** Rook at " << p << " **\n";
    //     BB::out(std::cout << "Occupancy\n", occ);
    //     BB::out(std::cout << "Result\n", a.getRookAttackBitboard(p, occ));
    //     // Queen tests
    //     std::cout << "** Queen at " << p << " **\n";
    //     BB::out(std::cout << "Occupancy\n", occ);
    //     BB::out(std::cout << "Result\n", a.getQueenAttackBitboard(p, occ));
    // }

    // *const* AttackTables. This baby ain't ever changin', boaïe.
    // auto at = std::make_shared<const AttackTables>();

    // MoveGenerator mg(at);
    
    // auto moves = mg.generateMoves(Player::White, b);

    // for (const auto& m : moves) {
    //     std::cout << m << '\n';
    // }

    return 0;
}