#pragma once

#include <cstdint>

enum class Player : uint8_t {
    White = 0U,
    Black,

    OOB, FIRST = White, LAST = Black
};

// Returns the opposing player.
constexpr Player otherPlayer(Player player) {
    return player == Player::White ? Player::Black : Player::White;
};