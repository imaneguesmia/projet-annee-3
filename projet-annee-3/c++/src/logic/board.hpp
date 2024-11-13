#include "bitboard.hpp"

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
    BB::BitBoard w_pawn {0ULL}, w_knight {0ULL}, w_bishop {0ULL},
                 w_rook {0ULL}, w_queen {0ULL}, w_king {0ULL};
    BB::BitBoard b_pawn {0ULL}, b_knight {0ULL}, b_bishop {0ULL}, 
                 b_rook {0ULL}, b_queen {0ULL}, b_king {0ULL};

public:
    Board() {};
    Board(const std::string& fen);
    ~Board() {};

    static Board initialState();

    // Not an enum class, because the C++ standards committee makes more bad decisions
    // than I do.
    enum Position {
        a8, b8, c8, d8, e8, f8, g8, h8,
        a7, b7, c7, d7, e7, f7, g7, h7,
        a6, b6, c6, d6, e6, f6, g6, h6,
        a5, b5, c5, d5, e5, f5, g5, h5,
        a4, b4, c4, d4, e4, f4, g4, h4,
        a3, b3, c3, d3, e3, f3, g3, h3,
        a2, b2, c2, d2, e2, f2, g2, h2,
        a1, b1, c1, d1, e1, f1, g1, h1
    };

    enum Piece {
        W_Pawn = 'P', W_Knight = 'N', W_Bishop = 'B',
        W_Rook = 'R', W_Queen = 'Q', W_King = 'K',

        B_Pawn = 'p', B_Knight = 'n', B_Bishop = 'b',
        B_Rook = 'r', B_Queen = 'q', B_King = 'k',

        None = '.'
    };

    // Returns true if the given position is occupied by any piece.
    bool isOccupied(int position) const {
        return BB::get_bit(
            w_pawn | w_knight | w_bishop | w_rook | w_queen | w_king |
            b_pawn | b_knight | b_bishop | b_rook | b_queen | b_king,
            position
        ); 
    };

    // Returns true if the given position is occupied by a white piece.
    bool isWhite(int position) const { 
        return BB::get_bit(w_pawn | w_knight | w_bishop | w_rook | w_queen | w_king, position); 
    };
    // Returns true if the given position is occupied by a black piece.
    bool isBlack(int position) const { 
        return BB::get_bit(b_pawn | b_knight | b_bishop | b_rook | b_queen | b_king, position); 
    };

    // The following return true if the given position is occupied by the specific piece type.
    bool isPawn(int position) const { return BB::get_bit(w_pawn | b_pawn, position); };
    bool isKnight(int position) const { return BB::get_bit(w_knight | b_knight, position); };
    bool isBishop(int position) const { return BB::get_bit(w_bishop | b_bishop, position); };
    bool isRook(int position) const { return BB::get_bit(w_rook | b_rook, position); };
    bool isQueen(int position) const { return BB::get_bit(w_queen | b_queen, position); };
    bool isKing(int position) const { return BB::get_bit(w_king | b_king, position); };

    // Returns the FEN of the piece at the given position. Used for printing out
    // the board, as well as aiding in the translation to Python.
    char pieceAt(int position) const;
    // Returns the FEN sequence representing the board state.
    const std::string fen() const;
};

std::ostream& operator<<(std::ostream& out, const Board& board);

/* ---- END DECLARE ---- */