#include "move_generator.hpp"

#include "bitboard.hpp"

#include "../misc/increment_enum.hpp"

#include <cmath>
#include <vector>

/* ---- DEFINE class MoveGenerator ---- */

std::vector<Move> MoveGenerator::generatePseudoLegals(
    const Player player, const Board& board,
    const Position& en_passant_position, const uint8_t castling_rights,
    bool only_captures
) const {
    std::vector<Move> moves;

    Position from, to;
    // bitboard is the bitboard of the given piece
    // pseudo_legals is the bitboard of pseudo-legal moves
    // legals is the bitboard of legal moves (verified check)
    // The next 5 bitboards are flags for the corresponding special moves
    BB::BitBoard bitboard, pseudo_legals,
                 capture, double_push, en_passant, castle, promotion;
    
    Move move;

    for (PType p_type = PType::FIRST; p_type != PType::OOB; increment_enum(p_type)) {
        Piece piece {p_type, player};
        Player other = otherPlayer(player);

        bitboard = board.bitboard(piece);

        // Loop over all set bits of the bitboard, aka all friendly pieces of this type on the board.
        while (bitboard) {
            from = BB::leastSignificantBitIndex(bitboard);

            // Reset flags
            capture = double_push = en_passant = castle = promotion = 0ULL;

            /* Generate pseudo-legals for piece */

            switch (piece.getType()) {
                case PType::Pawn: {
                    // Generate pushes on the fly because I can't be bothered.

                    // Special attack validation: can only do this move if attacking something.
                    BB::BitBoard attacks = at->getPawnAttackBitboard(player, from) & board.occupancy(other);

                    BB::BitBoard single_push;

                    if (player == Player::White) {
                        single_push = (BB::new_at(from) >> 8) & ~board.occupancy();
                        double_push = ((single_push >> 8)) & ~board.occupancy() & row_4;
                        promotion = (single_push | attacks) & row_8;
                    } else {
                        single_push = (BB::new_at(from) << 8) & ~board.occupancy();
                        double_push = ((single_push << 8)) & ~board.occupancy() & row_5;
                        promotion = (single_push | attacks) & row_1;
                    }

                    en_passant = en_passant_position.isValid();
                    en_passant *= at->getPawnAttackBitboard(player, from) & BB::new_at(en_passant_position);

                    capture = attacks | en_passant;
                    pseudo_legals = single_push | double_push | capture;
                } break;
                case PType::Knight:
                    pseudo_legals = at->getKnightAttackBitboard(from) & ~board.occupancy(player);
                    capture = pseudo_legals & board.occupancy(other);
                break;
                case PType::Bishop:
                    pseudo_legals = at->getBishopAttackBitboard(from, board.occupancy()) & ~board.occupancy(player);
                    capture = pseudo_legals & board.occupancy(other);
                break;
                case PType::Rook:
                    pseudo_legals = at->getRookAttackBitboard(from, board.occupancy()) & ~board.occupancy(player);
                    capture = pseudo_legals & board.occupancy(other);
                break;
                case PType::Queen:
                    pseudo_legals = at->getQueenAttackBitboard(from, board.occupancy()) & ~board.occupancy(player);
                    capture = pseudo_legals & board.occupancy(other);
                break;
                case PType::King: {
                    uint8_t relevant_castling_bits = castling_rights >> (2 * static_cast<int>(player));

                    for (int i = 0; i < 2; i++) {
                        auto [between, target] = relevant_castling_squares[static_cast<int>(player)][i];

                        if (
                            (relevant_castling_bits & (1 << i)) &&
                            !board_analysis.isSquareAttacked(from, other, board) && 
                            !board_analysis.isSquareAttacked(between, other, board) &&
                            !BB::get_bit(board.occupancy(), static_cast<int>(between)) &&
                            !BB::get_bit(board.occupancy(), static_cast<int>(target))
                        ) {
                            BB::set_bit(castle, static_cast<int>(target));
                        }
                    }

                    BB::BitBoard attacks = at->getKingAttackBitboard(from) & ~board.occupancy(player);

                    pseudo_legals = attacks | castle;
                    capture = attacks & board.occupancy(other);
                } break;
                default: break;
            }

            /* Extract moves from pseudo-legal bitboard */

            while (pseudo_legals) {
                to = BB::leastSignificantBitIndex(pseudo_legals);
                BB::BitBoard target = BB::new_at(to);

                move.source      = from;
                move.target      = to;
                move.player      = player;
                move.p_type      = p_type;
                move.capture     = target & capture;
                move.double_push = target & double_push;
                move.en_passant  = target & en_passant;
                move.castle      = target & castle;
                
                // If `only_captures`, don't add a move if it's not a capture.
                if (!only_captures || move.capture) {
                    if (target & promotion) {
                        // Loop through all possible promotions.
                        for (PType new_type = PType::Knight; new_type <= PType::Queen; increment_enum(new_type)) {
                            move.promotion = new_type;
                            moves.push_back(move);
                        }
                    } else {
                        move.promotion = PType::NoneType;
                        moves.push_back(move);
                    }
                }

                pseudo_legals &= ~target;
            }

            /* Clear piece from bitboard */

            BB::reset_bit(bitboard, from);
        }
    }

    return moves;
}

std::vector<Move> MoveGenerator::filterPseudoLegals(
    const Player player, const Board& board,
    const std::vector<Move>& pseudo_legals
) const {
    std::vector<Move> legals;
    std::unique_ptr<Board> copy;

    for (const auto& move : pseudo_legals) {
        // [TODO] The laziest implementation of copy-and-make I've ever seen. Can probably be improved.
        copy = std::make_unique<Board>(board);
        copy->movePiece(move);

        if (!board_analysis.isInCheck(player, *copy)) legals.push_back(move);
    }

    return legals;
};

std::vector<Move> MoveGenerator::generateMoves(
    const Player player, const Board& board,
    const Position& en_passant_position, const uint8_t castling_rights,
    bool only_captures
) const {
    std::vector<Move> legals = filterPseudoLegals(
        player, board, 
        generatePseudoLegals(player, board, en_passant_position, castling_rights, only_captures)
    );

    return legals;
}

/* ---- END DEFINE ---- */