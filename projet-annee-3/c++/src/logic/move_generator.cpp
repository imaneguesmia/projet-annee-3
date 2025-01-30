#include "move_generator.hpp"

#include "bitboard.hpp"

#include <cmath>
#include <vector>

// Algorithm by Kim Walisch
inline int leastSignificantBitIndex(uint64_t n) {
    static const uint64_t debruijn_hash_64 = 0x03f79d71b4cb0a89ULL;

    static const int index_64[64] = {
         0, 47,  1, 56, 48, 27,  2, 60,
        57, 49, 41, 37, 28, 16,  3, 61,
        54, 58, 35, 52, 50, 42, 21, 44,
        38, 32, 29, 23, 17, 11,  4, 62,
        46, 55, 26, 59, 40, 36, 15, 53,
        34, 51, 20, 43, 31, 22, 10, 45,
        25, 39, 14, 33, 19, 30,  9, 24,
        13, 18,  8, 12,  7,  6,  5, 63
    };

    return index_64[((n ^ (n-1)) * debruijn_hash_64) >> 58];
}

bool MoveGenerator::isSquareAttacked(
    const Position& position, Player player,
    const Board& board
) const {
    Player other_player = otherPlayer(player);

    // If a piece P is on a square S, it follows that a piece of the same type on any 
    // square that P is attacking is also attacking S.

    return (
        // Pawn attacks
        at.getPawnAttackBitboard(other_player, position) & board.bitboard(Piece(Piece::Pawn, player)) ||
        // Knight attacks
        at.getKnightAttackBitboard(position) & board.bitboard(Piece(Piece::Knight, player)) ||
        // King attacks
        at.getKingAttackBitboard(position) & board.bitboard(Piece(Piece::King, player)) ||

        // Bishop attacks
        // For example, here we check if there is a good bishop on the squares attacked by an opposing bishop on
        // this square.
        at.getBishopAttackBitboard(position, board.occupancy()) & board.bitboard(Piece(Piece::Bishop, player)) ||
        // Rook attacks
        at.getRookAttackBitboard(position, board.occupancy()) & board.bitboard(Piece(Piece::Rook, player)) ||
        // Queen attacks
        at.getQueenAttackBitboard(position, board.occupancy()) & board.bitboard(Piece(Piece::Queen, player))
    );
}

void MoveGenerator::generateMoves(const Board& board) const {


    Position from, to;
    // bitboard is the bitboard of the given piece
    // pseudo_legals is the bitboard of pseudo-legal moves
    // legals is the bitboard of legal moves (verified check)
    // specials is the bitboard of moves with a special flag (castling/double pawn push)
    BB::BitBoard bitboard, pseudo_legals, legals, specials;

    for (int id = 0; id < 12; id++) {
        Piece piece {Piece::Id(id)};

        Player player = piece.getPlayer();
        Player other = otherPlayer(player);

        bitboard = board.bitboard(piece);

        while (bitboard) {
            from = leastSignificantBitIndex(bitboard);

            switch (piece.getType()) {
                case Piece::Pawn: {
                    // Generate pushes on the fly because I can't be bothered T-T

                    BB::BitBoard single_push, double_push;

                    switch (piece.getId()) {
                        case Piece::W_Pawn:
                            single_push = (BB::new_at(from) >> 8) & ~board.occupancy();
                            double_push = ((single_push >> 8)) & ~board.occupancy() & row_4;
                        break;
                        case Piece::B_Pawn:
                            single_push = (BB::new_at(from)<< 8) & ~board.occupancy();
                            double_push = ((single_push << 8)) & ~board.occupancy() & row_5;
                        break;
                    }

                    // Special attack validation: can only do this move if attacking something, en passant included.
                    BB::BitBoard targets = board.occupancy(other) | (BB::new_at(board.getEnPassantPosition()));
                    BB::BitBoard attacks = at.getPawnAttackBitboard(player, from) & targets;

                    pseudo_legals = single_push | double_push | attacks;
                    specials = double_push;

                    std::cout << piece.fen() << " from " << from << '\n';
                    BB::out(std::cout << "Occupancy\n", board.occupancy());
                    // BB::out(std::cout << "Cast occupancy\n", cast_blocked);
                    BB::out(std::cout << "Targets\n", pseudo_legals);
                } break;
                case Piece::Knight:
                    pseudo_legals = at.getKnightAttackBitboard(from) & ~board.occupancy(player);
                break;
                case Piece::Bishop:
                    pseudo_legals = at.getBishopAttackBitboard(from, board.occupancy()) & ~board.occupancy(player);
                break;
                case Piece::Rook:
                    pseudo_legals = at.getRookAttackBitboard(from, board.occupancy()) & ~board.occupancy(player);
                break;
                case Piece::Queen:
                    pseudo_legals = at.getQueenAttackBitboard(from, board.occupancy()) & ~board.occupancy(player);
                break;
                case Piece::King: {
                    uint8_t castling_rights = board.getCastlingRights();
                    specials = 0ULL;

                    // This is not the most efficient way to do this, but by god am I tired it's 4 in the morning for crying out loud
                    for (int i = 0; castling_rights; i++, castling_rights >>= 1) {
                        auto [between, target] = relevant_castling_squares[i];

                        if (
                            (castling_rights & 1ULL) &&
                            !isSquareAttacked(from, other, board) && !isSquareAttacked(between, other, board) &&
                            !BB::get_bit(board.occupancy(), between) && !BB::get_bit(board.occupancy(), target)
                        ) {
                            BB::set_bit(specials, target);
                        }
                    }

                    pseudo_legals = (at.getKingAttackBitboard(from) & ~board.occupancy(player)) | specials;

                    std::cout << piece.fen() << " from " << from << '\n';
                    BB::out(std::cout << "Occupancy\n", board.occupancy());
                    // BB::out(std::cout << "Cast occupancy\n", cast_blocked);
                    BB::out(std::cout << "Targets\n", pseudo_legals);
                } break;
            }

            // Filter pseudo-legals

            // Calculate move integer

            BB::reset_bit(bitboard, from);
        }
    }
}