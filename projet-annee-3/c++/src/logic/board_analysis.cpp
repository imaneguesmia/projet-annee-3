#include "board_analysis.hpp"

/* ---- DEFINE class BoardAnalysis ---- */

bool BoardAnalysis::isSquareAttacked(
    const Position& position, Player player,
    const Board& board
) const {
    Player other_player = otherPlayer(player);

    // If a piece P is on a square S, it follows that a piece of the same type on any 
    // square that P is attacking is also attacking S.

    return (
        // Pawn attacks
        at->getPawnAttackBitboard(other_player, position) & board.bitboard(Piece(Piece::Pawn, player)) ||
        // Knight attacks
        at->getKnightAttackBitboard(position) & board.bitboard(Piece(Piece::Knight, player)) ||
        // King attacks
        at->getKingAttackBitboard(position) & board.bitboard(Piece(Piece::King, player)) ||

        // Bishop attacks
        // For example, here we check if there is a good bishop on the squares attacked by an opposing bishop on
        // this square.
        at->getBishopAttackBitboard(position, board.occupancy()) & board.bitboard(Piece(Piece::Bishop, player)) ||
        // Rook attacks
        at->getRookAttackBitboard(position, board.occupancy()) & board.bitboard(Piece(Piece::Rook, player)) ||
        // Queen attacks
        at->getQueenAttackBitboard(position, board.occupancy()) & board.bitboard(Piece(Piece::Queen, player))
    );
}

bool BoardAnalysis::isInCheck(const Player player, const Board& board) const {
    Position king_square = BB::leastSignificantBitIndex(board.bitboard(Piece(Piece::King, player)));

    return isSquareAttacked(king_square, otherPlayer(player), board);
}

/* ---- END DEFINE ---- */