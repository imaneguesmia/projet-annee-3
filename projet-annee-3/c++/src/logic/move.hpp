#pragma once

#include <cstdint>
#include <iostream>

/**
 * @brief Structure containing move data.
 * 
 * Bitfields? In *my* modern C++ application? It's more likely than you'd think.
 */
struct Move {
    uint8_t source      : 6;    // The source position of the move (where the piece comes from).
    uint8_t target      : 6;    // The target position of the move (where the piece is going).
    uint8_t piece       : 4;    // The `Piece::Id` of the piece that moved. 
    uint8_t promotion   : 4;    // The `Piece::Id` of the piece to promote to (`Piece::NoneId` if no promotion).
    bool capture        : 1;    // Capture flag. 
    bool double_push    : 1;    // Double push flag. 
    bool en_passant     : 1;    // En passant flag. 
    bool castle         : 1;    // Castle flag. 
};

// Prints the move data to the stream in a readable format.
std::ostream& operator<<(std::ostream& out, const Move& move);