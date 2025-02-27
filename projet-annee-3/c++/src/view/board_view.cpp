#include "board_view.hpp"

#include "../misc/increment_enum.hpp"

/* ---- DEFINE class BoardView ---- */

BoardView::BoardView(const Board& board) {
    for (Square s = Square::FIRST; s <= Square::LAST; increment_enum(s)) {
        board_data[s] = board.pieceAt(s);
    }
}

/* ---- END DEFINE ---- */