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

std::vector<Move> MoveGenerator::generatePseudoLegals(const Board& board) const {
    std::vector<Move> moves;

    Position from, to;
    // bitboard is the bitboard of the given piece
    // pseudo_legals is the bitboard of pseudo-legal moves
    // legals is the bitboard of legal moves (verified check)
    // The next 5 bitboards are flags for the corresponding special moves
    BB::BitBoard bitboard, pseudo_legals,
                 capture, double_push, en_passant, castle, promotion;
    
    Move move;

    for (int id = 0; id < 12; id++) {
        Piece piece {Piece::Id(id)};

        Player player = piece.getPlayer();
        Player other = otherPlayer(player);

        bitboard = board.bitboard(piece);

        while (bitboard) {
            from = leastSignificantBitIndex(bitboard);

            // Reset flags
            capture = double_push = en_passant = castle = promotion = 0ULL;

            // Generate pseudo-legals

            switch (piece.getType()) {
                case Piece::Pawn: {
                    // Generate pushes on the fly because I can't be bothered T-T

                    BB::BitBoard single_push;

                    switch (piece.getId()) {
                        case Piece::W_Pawn:
                            single_push = (BB::new_at(from) >> 8) & ~board.occupancy();
                            double_push = ((single_push >> 8)) & ~board.occupancy() & row_4;
                            promotion = single_push & row_8;
                        break;
                        case Piece::B_Pawn:
                            single_push = (BB::new_at(from)<< 8) & ~board.occupancy();
                            double_push = ((single_push << 8)) & ~board.occupancy() & row_5;
                            promotion = single_push & row_1;
                        break;
                    }

                    // Special attack validation: can only do this move if attacking something, en passant included.
                    BB::BitBoard attacks = at.getPawnAttackBitboard(player, from) & board.occupancy(other);
                    en_passant = at.getPawnAttackBitboard(player, from) & BB::new_at(board.getEnPassantPosition());

                    capture = attacks | en_passant;
                    pseudo_legals = single_push | double_push | capture;
                } break;
                case Piece::Knight:
                    pseudo_legals = at.getKnightAttackBitboard(from) & ~board.occupancy(player);
                    capture = pseudo_legals & board.occupancy(other);
                break;
                case Piece::Bishop:
                    pseudo_legals = at.getBishopAttackBitboard(from, board.occupancy()) & ~board.occupancy(player);
                    capture = pseudo_legals & board.occupancy(other);
                break;
                case Piece::Rook:
                    pseudo_legals = at.getRookAttackBitboard(from, board.occupancy()) & ~board.occupancy(player);
                    capture = pseudo_legals & board.occupancy(other);
                break;
                case Piece::Queen:
                    pseudo_legals = at.getQueenAttackBitboard(from, board.occupancy()) & ~board.occupancy(player);
                    capture = pseudo_legals & board.occupancy(other);
                break;
                case Piece::King: {
                    uint8_t castling_rights = board.getCastlingRights();

                    // This is not the most efficient way to do this, but by god am I tired it's 4 in the morning for crying out loud
                    for (int i = 0; castling_rights; i++, castling_rights >>= 1) {
                        auto [between, target] = relevant_castling_squares[i];

                        if (
                            (castling_rights & 1ULL) &&
                            !isSquareAttacked(from, other, board) && !isSquareAttacked(between, other, board) &&
                            !BB::get_bit(board.occupancy(), between) && !BB::get_bit(board.occupancy(), target)
                        ) {
                            BB::set_bit(castle, target);
                        }
                    }

                    BB::BitBoard attacks = at.getKingAttackBitboard(from) & ~board.occupancy(player);

                    pseudo_legals = attacks | castle;
                    capture = attacks & board.occupancy(other);
                } break;
            }

            // Extract moves from pseudo-legal bitboard

            while (pseudo_legals) {
                to = leastSignificantBitIndex(pseudo_legals);
                BB::BitBoard target = BB::new_at(to);

                move.source      = from;
                move.target      = to;
                move.piece       = piece.getId();
                move.capture     = target & capture;
                move.double_push = target & double_push;
                move.en_passant  = target & en_passant;
                move.castle      = target & castle;

                if (target & promotion) {
                    for (int i = 0; i < 4; i++) {
                        uint8_t promoted_piece = (int(player)*6) + (i+1);

                        move.promotion = promoted_piece;
                        moves.push_back(move);
                    }
                } else {
                    move.promotion = Piece::NoneId;
                    moves.push_back(move);
                }

                pseudo_legals &= ~target;
            }

            BB::reset_bit(bitboard, from);
        }
    }

    return std::move(moves);
}

