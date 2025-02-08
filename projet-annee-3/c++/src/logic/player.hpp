#pragma once

enum class Player {
    White,
    Black,

    OOB, FIRST = White, LAST = Black
};

// Returns the opposing player.
constexpr Player otherPlayer(Player player) {
    return Player(1 - int(player));
}