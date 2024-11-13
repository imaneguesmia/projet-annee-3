#include "board.hpp"

#include <format>

/* ---- DEFINE class Board ---- */

Board::Board(const std::string& fen) {
    int position = 0;
    int row = 0;  // Mostly just to check that the given FEN is valid.

    for (const auto& c : fen) {
        switch (c) {
            case W_Pawn:
                BB::set_bit(w_pawn, position);
                break;
            case W_Knight:
                BB::set_bit(w_knight, position);
                break;
            case W_Bishop:
                BB::set_bit(w_bishop, position);
                break;
            case W_Rook:
                BB::set_bit(w_rook, position);
                break;
            case W_Queen:
                BB::set_bit(w_queen, position);
                break;
            case W_King:
                BB::set_bit(w_king, position);
                break;
            case B_Pawn:
                BB::set_bit(b_pawn, position);
                break;
            case B_Knight:
                BB::set_bit(b_knight, position);
                break;
            case B_Bishop:
                BB::set_bit(b_bishop, position);
                break;
            case B_Rook:
                BB::set_bit(b_rook, position);
                break;
            case B_Queen:
                BB::set_bit(b_queen, position);
                break;
            case B_King:
                BB::set_bit(b_king, position);
                break;
            
            case '1': case '2': case '3':
            case '4': case '5': case '6':
            case '7': case '8':
                // ASCII hack; we subtract 1 extra to compensate for the increment
                position += c - '1';
                break;
            
            case '/':
                row++;
                continue;
            
            default:
                throw std::invalid_argument(std::format("Invalid FEN sequence: unknown character '{}'", c));
        }

        if (position / 8 != row) {
            throw std::invalid_argument("Invalid FEN sequence : bad column count");
        }

        position++;
    }
}

Board Board::initialState() {
    return Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
}

#define TO_LOWER_CASE 32

char Board::pieceAt(int position) const {
    char out {0};

    if (isOccupied(position)) {
        if (isBlack(position)) out += TO_LOWER_CASE;  // More ASCII hack

        if (isPawn(position)) out += W_Pawn;
        else if (isKnight(position)) out += W_Knight;
        else if (isBishop(position)) out += W_Bishop;
        else if (isRook(position)) out += W_Rook;
        else if (isQueen(position)) out += W_Queen;
        else if (isKing(position)) out += W_King;
    }
    else out = None;

    return out;
}

const std::string Board::fen() const {
    std::string out {""};

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            int position {i*8 + j};

            out += pieceAt(position);
        }
    }
}

std::ostream& operator<<(std::ostream& out, const Board& board) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            int position {i*8 + j};

            out << board.pieceAt(position) << " ";
        }

        out << std::endl;
    }

    return out;
}

/* ---- END DEFINE ---- */