#pragma once

#include "move_accepter.hpp"
#include "move_prompter.hpp"
#include "move_provider.hpp"

#include "../logic/attack_tables.hpp"
#include "../logic/game.hpp"
#include "../logic/move.hpp"
#include "../logic/player.hpp"

#include "../minimax/Search.hpp"

#include <memory>
#include <string>

/* ---- DECLARE class GameManager ---- */

class GameManager {
    const std::shared_ptr<const AttackTables> at;
    Game game;

public:
    GameManager();
    GameManager(const std::string& initial_position);
    ~GameManager() {};

    std::unique_ptr<AIMoveProvider> createMinimaxPlayer(int depth) {
        return std::make_unique<Beluga>(depth, at);
    }

    // MovePrompter promptNextMove();

    Player getCurrentPlayer() { return game.getCurrentPlayer(); };

    GameData& gameData() { return game; };
    ExtendedGameData& extendedGameData() { return game; };

    bool makeMove(Move move);
};

/* ---- END DECLARE ---- */