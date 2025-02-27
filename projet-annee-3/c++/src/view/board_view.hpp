#pragma once

#include "../logic/board.hpp"
#include "../logic/piece.hpp"
#include "../logic/position.hpp"

#include "../misc/enum_array.hpp"

/* ---- DECLARE class BoardView ---- */

/**
 * @brief Class used to easily access board data for display purposes.
 */
class BoardView {
    enum_array<Square, Piece> board_data;

public:
    BoardView(const Board& board);
    ~BoardView() {}

    Piece pieceAt(const Position& position) const { return board_data[position.getPositionSquare()]; };
};

/* ---- END DECLARE ---- */