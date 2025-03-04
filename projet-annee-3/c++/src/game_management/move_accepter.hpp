#pragma once

#include "../logic/move.hpp"

/* ---- DECLARE interface MoveAccepter ---- */

/**
 * @brief Interface for a manager class to accept a move from a MovePrompter.
 */
class MoveAccepter {

public:
    MoveAccepter() {};
    virtual ~MoveAccepter() {};

    virtual bool acceptMove(Move move) = 0;
};
    
/* ---- END DECLARE ---- */