#include "board_analysis.hpp"

#include "../misc/safely_to_enum_class.hpp"

/* ---- DEFINE class BoardAnalysis ---- */

bool BoardAnalysis::isSquareAttacked(
    const Position &position, Player player,
    const Board &board) const
{
    Player other_player = otherPlayer(player);

    // If a piece P is on a square S, it follows that a piece of the same type on any
    // square that P is attacking is also attacking S.

    return (
        // Pawn attacks
        at.getPawnAttackBitboard(other_player, position) & board.bitboard(Piece(PType::Pawn, player)) ||
        // Knight attacks
        at.getKnightAttackBitboard(position) & board.bitboard(Piece(PType::Knight, player)) ||
        // King attacks
        at.getKingAttackBitboard(position) & board.bitboard(Piece(PType::King, player)) ||

        // Bishop attacks
        // For example, here we check if there is a good bishop on the squares attacked by an opposing bishop on
        // this square.
        at.getBishopAttackBitboard(position, board.occupancy()) & board.bitboard(Piece(PType::Bishop, player)) ||
        // Rook attacks
        at.getRookAttackBitboard(position, board.occupancy()) & board.bitboard(Piece(PType::Rook, player)) ||
        // Queen attacks
        at.getQueenAttackBitboard(position, board.occupancy()) & board.bitboard(Piece(PType::Queen, player)));
}

bool BoardAnalysis::isInCheck(const Player player, const Board &board) const
{
    Position king_square = board.kingSquare(player);
    return isSquareAttacked(king_square, otherPlayer(player), board);
}

/* ---- END DEFINE ---- */