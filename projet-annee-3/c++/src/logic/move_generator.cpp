#include "move_generator.hpp"

#include "bitboard.hpp"

#include <cmath>
#include <vector>

/* ---- DEFINE class MoveGenerator ---- */

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

std::vector<Move> MoveGenerator::generatePseudoLegals(
    const Player player, const Board& board
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

    for (int p_type = Piece::Pawn; p_type < Piece::NoneType; p_type++) {
        Piece piece {Piece::Type(p_type), player};
        Player other = otherPlayer(player);

        bitboard = board.bitboard(piece);

        // Loop over all set bits of the bitboard, aka all friendly pieces of this type on the board.
        while (bitboard) {
            from = BB::leastSignificantBitIndex(bitboard);

            // Reset flags
            capture = double_push = en_passant = castle = promotion = 0ULL;

            /* Generate pseudo-legals for piece */

            switch (piece.getType()) {
                case Piece::Pawn: {
                    // Generate pushes on the fly because I can't be bothered.

                    BB::BitBoard single_push;

                    if (player == Player::White) {
                        single_push = (BB::new_at(from) >> 8) & ~board.occupancy();
                        double_push = ((single_push >> 8)) & ~board.occupancy() & row_4;
                        promotion = single_push & row_8;
                    } else {
                        single_push = (BB::new_at(from) << 8) & ~board.occupancy();
                        double_push = ((single_push << 8)) & ~board.occupancy() & row_5;
                        promotion = single_push & row_1;
                    }

                    // Special attack validation: can only do this move if attacking something.
                    BB::BitBoard attacks = at.getPawnAttackBitboard(player, from) & board.occupancy(other);

                    en_passant = board.getEnPassantPosition() != Position::Invalid;
                    en_passant *= at.getPawnAttackBitboard(player, from) & BB::new_at(board.getEnPassantPosition());

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

                    // [TODO] This is not the most efficient way to do this.
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

            /* Extract moves from pseudo-legal bitboard */

            while (pseudo_legals) {
                to = BB::leastSignificantBitIndex(pseudo_legals);
                BB::BitBoard target = BB::new_at(to);

                move.source      = from;
                move.target      = to;
                move.piece       = piece.getId();
                move.capture     = target & capture;
                move.double_push = target & double_push;
                move.en_passant  = target & en_passant;
                move.castle      = target & castle;

                if (target & promotion) {
                    // Loop through all possible promotions.
                    for (int i = Piece::Knight; i <= Piece::Queen; i++) {
                        uint8_t promoted_piece = int(player)*6 + i;

                        move.promotion = promoted_piece;
                        moves.push_back(move);
                    }
                } else {
                    move.promotion = Piece::NoneId;
                    moves.push_back(move);
                }

                pseudo_legals &= ~target;
            }

            /* Clear piece from bitboard */

            BB::reset_bit(bitboard, from);
        }
    }

    return std::move(moves);
}

bool MoveGenerator::isInCheck(const Player player, const Board& board) const {
    Position king_square = BB::leastSignificantBitIndex(board.bitboard(Piece(Piece::King, player)));

    return isSquareAttacked(king_square, otherPlayer(player), board);
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
        copy->makeMove(move);

        if (!isInCheck(player, *copy)) legals.push_back(move);
    }

    return std::move(legals);
};

std::vector<Move> MoveGenerator::generateMoves(const Player player, const Board& board) const {
    std::vector<Move> legals = filterPseudoLegals(
        player, board, 
        generatePseudoLegals(player, board)
    );

    return std::move(legals);
}

/* ---- END DEFINE ---- */