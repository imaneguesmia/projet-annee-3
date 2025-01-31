#pragma once

#include "bitboard.hpp"

#include "position.hpp"
#include "player.hpp"
#include "piece.hpp"
#include "move.hpp"

#include <iostream>
#include <bitset>
#include <string>

/* ---- DECLARE class Board ---- */

/**
 * @brief A class representing the current state of a board.
 * 
 * It uses a total of 12 bitboards, and possesses helper methods that operate on these bitboards to
 * quickly obtain the desired positions.
 * 
 * This class does not contain any info on legal moves, and only contains position info. It *will not*
 * check if a move is legal before playing it. It is essentially a simple chessboard.
 * 
 * See also: FEN Notation (https://www.chess.com/terms/fen-chess)
 */
class Board {
    // Lookup table to handle castling rights changes.
    static constexpr uint8_t castling_table[64] = {
         7, 15, 15, 15,  3, 15, 15, 11,
        15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15,
        13, 15, 15, 15, 12, 15, 15, 14
    };

    // Each bitboard represents the occupancy of a certain piece type or player.
    // For example, if the bit 32 is set on the bitboard for white bishops,
    // that means that there is a white bishop on position 32 of the board.
    BB::BitBoard piece_bb[2][6] {0ULL};     // [Player][Piece::Type]
    BB::BitBoard occupancy_bb[3] {0ULL};    // [Player | 2 (Both)]

    Player current_player {Player::White};
    uint8_t castling_rights {0};            // Castling rights bits: `0b[qkQK]`
    Position en_passant;

    // Halfmoves are incremented at the start of each turn, and reset after each capture or pawn move.
    // Fullmoves are incremented after black's turn.
    size_t halfmoves, fullmoves;

    // Sets pieces in the bitboards according to the given FEN string.
    void setPiecePositions(const std::string& piece_positions);
    // Sets the current valid en passant target.
    void setEnPassantPosition(const Position& position) { en_passant = position; };
    
    /**
     * @brief Sets castling rights bits according to the given string.
     * 
     * @param castling_indicators   The string indicating castling rights, i.e. "KQq" => `0b1011`.
     */
    void setCastlingRights(const std::string& castling_indicators);

    /**
     * @brief Determines the source and target positions of the rook when castling.
     * 
     * @param king_target           The target position of the king when castling.
     * @return A bitboard indicating the source and target positions of the rook.
     */
    BB::BitBoard rookMovementWhenCastling(const Position& king_target) const;

public:
    // Constructs a new Board object from the given FEN string.
    Board(const std::string& fen);
    // Construct a new Board object with the default starting position.
    Board() : Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") {};

    ~Board() {};

    /* -- Getters and setters -- */

    Player getCurrentPlayer() const { return current_player; };
    void setCurrentPlayer(const Player player) { current_player = player; };

    // Gets the current valid en passant target.
    const Position& getEnPassantPosition() const { return en_passant; };

    // Places a piece at a given position.
    void setPieceAt(const Position& position, const Piece& piece);

    // Makes the given move. Note that this modifies the board state.
    void makeMove(const Move move);
    
    /* -- Board operations -- */

    /**
     * @brief Gets the bitboard of the given piece.
     * 
     * @param piece     The piece to get the bitboard of.
     * @return A bitboard indicating the squares occupied by this piece.
     */
    BB::BitBoard bitboard(Piece piece) const { return piece_bb[int(piece.getPlayer())][piece.getType()]; };

    /**
     * @brief Gets the occupancy bitboard of all the given player's pieces.
     * 
     * @param player    The player to get the occupancy of.
     * @return A bitboard indicating the squares occupied by this player.
     */
    BB::BitBoard occupancy(Player player) const { return occupancy_bb[int(player)]; };
    /**
     * @brief Gets the global occupancy of all pieces.
     * 
     * @return A bitboard indicating all occupied squares on the board.
     */
    BB::BitBoard occupancy() const { return occupancy_bb[2]; };

    /**
     * @brief Gets the castling rights bits.
     * 
     * @return Bitflags indicating castling rights: `0b[qkQK]`.
     */
    uint8_t getCastlingRights() const { return castling_rights; };

    /* -- State exporting -- */

    // Gets the piece at the given position on the board.
    Piece pieceAt(const Position& position) const;

    // Gets the FEN symbol of the piece at the given position on the board.
    char pieceFenAt(const Position& position) const { return pieceAt(position).fen(); };
    // Translates the current board state into FEN notation.
    const std::string fen() const;
};

// Prints a board to the stream in a readable format.
std::ostream& operator<<(std::ostream& out, const Board& board);

/* ---- END DECLARE ---- */