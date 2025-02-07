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
 * It uses a total of 15 bitboards, and possesses helper methods that operate on these bitboards to
 * quickly obtain the desired positions.
 * 
 * This class does not contain any info on legal moves, and only contains position info. It *will not*
 * check if a move is legal before playing it. It is essentially a simple chessboard.
 * 
 * See also: FEN Notation (https://www.chess.com/terms/fen-chess)
 */
class Board {
    // Sets pieces in the bitboards according to the given FEN string.
    void setPiecePositions(const std::string& piece_positions);

public:
    // Constructs a new Board object from the given position string.
    Board(const std::string& piece_positions) { setPiecePositions(piece_positions); };
    // Construct a new Board object with the default starting position.
    // Board() : Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") {};
    Board() : Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR") {};

    ~Board() {};

    /* -- Bitboards -- */

    // Each bitboard represents the occupancy of a certain piece type or player.
    // For example, if the bit 32 is set on the bitboard for white bishops,
    // that means that there is a white bishop on position 32 of the board.
    BB::BitBoard piece_bb[2][6] {0ULL};     // [Player][Piece::Type]
    BB::BitBoard occupancy_bb[3] {0ULL};    // [Player | 2 (Both)]

    /* -- Getters and setters -- */

    // Places a piece at a given position.
    void setPieceAt(const Position& position, const Piece& piece);

    // Makes the given move. Note that this modifies the board state.
    void makeMove(const Move move);
    
    /* -- Board operations -- */

    /**
     * @brief Shorthand to get the bitboard of the given piece.
     * 
     * @param piece     The piece to get the bitboard of.
     * @return A bitboard indicating the squares occupied by this piece.
     */
    BB::BitBoard bitboard(Piece piece) const { return piece_bb[int(piece.getPlayer())][piece.getType()]; };

    /**
     * @brief Shorthand to get the occupancy bitboard of all the given player's pieces.
     * 
     * @param player    The player to get the occupancy of.
     * @return A bitboard indicating the squares occupied by this player.
     */
    BB::BitBoard occupancy(Player player) const { return occupancy_bb[int(player)]; };
    /**
     * @brief Shorthand to get the global occupancy of all pieces.
     * 
     * @return A bitboard indicating all occupied squares on the board.
     */
    BB::BitBoard occupancy() const { return occupancy_bb[2]; };

    /* -- State exporting -- */

    // Gets the piece at the given position on the board.
    Piece pieceAt(const Position& position) const;

    // Gets the FEN symbol of the piece at the given position on the board.
    char pieceFenAt(const Position& position) const { return pieceAt(position).fen(); };

    // Gets the string representation of the board.
    std::string getPositionString() const;
};

// Prints a board to the stream in a readable format.
std::ostream& operator<<(std::ostream& out, const Board& board);

/* ---- END DECLARE ---- */