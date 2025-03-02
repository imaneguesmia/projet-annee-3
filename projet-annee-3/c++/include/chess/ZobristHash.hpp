#pragma once

#include "../misc/enum_array.hpp"

#include "../logic/position.hpp"
#include "../logic/player.hpp"
#include "../logic/piece.hpp"
#include "../logic/game.hpp"

#include <cstdint>
#include <array>

/* ---- DECLARE class ZobristHash ---- */

class ZobristHash {
    static constexpr int SEED {1};

    enum_array<Square, enum_array<Player, enum_array<PType, uint64_t>>> table;

    const uint64_t black_to_move;
    const std::array<uint64_t, 16> castling;
    const std::array<uint64_t, 8> en_passant_column;


private:
    void generateKeys();

public:
    ZobristHash();

    uint64_t hash(const Game& game) const;
};

/* ---- END DECLARE ---- */