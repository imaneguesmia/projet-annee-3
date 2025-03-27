#pragma once

#include "move.hpp"
#include "piece.hpp"

#include <iostream>
#include <cstdint>

/* ---- DECLARE struct UnmakeMove ---- */

/**
 * @brief Struct containing undo data. Used for the make-unmake approach, and is more efficient
 * for undoing than saving a copy of the entire state.
 */
struct UnmakeMove {
    Move move;                      // The made move.
    PType captured          : 4;    // `PType` of captured piece (`PType::NoneType` if no capture).

    uint8_t castling_rights : 4;    // Bitflags indicating previous castling rights: `0b[qkQK]`.
    uint8_t en_passant;             // Previous valid en passant target.

    size_t halfmoves;               // Halfmove clock before the move.

    uint64_t board_hash;            // Zobrist hash of the previous position.
};

std::ostream& operator<<(std::ostream& out, const UnmakeMove& unmake_move);

/* ---- END DECLARE ---- */