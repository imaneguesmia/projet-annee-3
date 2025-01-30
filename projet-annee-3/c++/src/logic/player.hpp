#pragma once

enum class Player {
    White,
    Black
};

constexpr Player otherPlayer(Player player) {
    return Player(1 - int(player));
}