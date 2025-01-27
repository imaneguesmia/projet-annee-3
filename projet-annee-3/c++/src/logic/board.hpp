#pragma once

#include "bitboard.hpp"

#include "position.hpp"

#include <iostream>
#include <bitset>
#include <string>

/* ---- DECLARE class Board ---- */

/*
A class representing the current state of a board. It uses a total of 12 bitboards,
and possesses helper methods that operate on these bitboards to quickly obtain the
desired positions.

This class does not contain any info on legal moves, and only contains position info.

This class also contains a lot of repetitive code that could be easily avoided. This
is for the sake of efficiency, since these operations are going to be done often.
*/

class Board {

public:
    /* -- Enum definitions -- */

    // Not enum classes, because the C++ standards committee makes more bad decisions
    // than I do.

    enum Player {
        White = 'w',
        Black = 'b'
    };

    enum Piece {
        W_Pawn = 'P', W_Knight = 'N', W_Bishop = 'B',
        W_Rook = 'R', W_Queen = 'Q', W_King = 'K',

        B_Pawn = 'p', B_Knight = 'n', B_Bishop = 'b',
        B_Rook = 'r', B_Queen = 'q', B_King = 'k',

        None = '.'
    };

private:
    BB::BitBoard w_pawn {0ULL}, w_knight {0ULL}, w_bishop {0ULL},
                 w_rook {0ULL}, w_queen {0ULL}, w_king {0ULL};
    BB::BitBoard b_pawn {0ULL}, b_knight {0ULL}, b_bishop {0ULL}, 
                 b_rook {0ULL}, b_queen {0ULL}, b_king {0ULL};

    Player current_player {White};
    uint8_t castling_rights;
    std::optional<Position> en_passant;

    size_t halfmoves, fullmoves;

    void setPiecePositions(const std::string& piece_positions);

    

public:
    Board(const std::string& fen);
    /* 
        FEN : https://www.chess.com/terms/fen-chess 
        
        Format modified slightly : castling rights indicated by a 4-bit integer, and
        no en-passant indicated by two dashes instead of one.
    */
    Board() : Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w 0 -- 0 0") {};

    ~Board() {};

    /* -- Getters and setters -- */

    Player getCurrentPlayer() const { return current_player; };
    void setCurrentPlayer(Player player) { current_player = player; };  // Feels wrong to not have the argument be const, but c'mon it's a char we don't need that

    const std::optional<Position>& getEnPassantPosition() const { return en_passant; };
    void setEnPassantPosition(const std::optional<Position>& position) { en_passant = position; };
    
    /* -- Board operations -- */

    // Returns true if the given position is occupied by any piece.
    bool isOccupied(const Position& position) const {
        return BB::get_bit(
            w_pawn | w_knight | w_bishop | w_rook | w_queen | w_king |
            b_pawn | b_knight | b_bishop | b_rook | b_queen | b_king,
            position
        ); 
    };

    // Returns true if the given position is occupied by a white piece.
    bool isWhite(const Position& position) const { 
        return BB::get_bit(w_pawn | w_knight | w_bishop | w_rook | w_queen | w_king, position); 
    };
    // Returns true if the given position is occupied by a black piece.
    bool isBlack(const Position& position) const { 
        return BB::get_bit(b_pawn | b_knight | b_bishop | b_rook | b_queen | b_king, position); 
    };

    // The following return true if the given position is occupied by the specific piece type.
    bool isPawn(const Position& position) const { return BB::get_bit(w_pawn | b_pawn, position); };
    bool isKnight(const Position& position) const { return BB::get_bit(w_knight | b_knight, position); };
    bool isBishop(const Position& position) const { return BB::get_bit(w_bishop | b_bishop, position); };
    bool isRook(const Position& position) const { return BB::get_bit(w_rook | b_rook, position); };
    bool isQueen(const Position& position) const { return BB::get_bit(w_queen | b_queen, position); };
    bool isKing(const Position& position) const { return BB::get_bit(w_king | b_king, position); };

    /* -- State exporting -- */

    // Returns the FEN of the piece at the given position. Used for printing out
    // the board, as well as aiding in the translation to Python.
    char pieceAt(const Position& position) const;
    // Returns the FEN sequence representing the board state.
    const std::string fen() const;
};

std::ostream& operator<<(std::ostream& out, const Board& board);

/* ---- END DECLARE ---- */