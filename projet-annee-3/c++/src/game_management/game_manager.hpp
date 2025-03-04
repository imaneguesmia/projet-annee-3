#pragma once

#include "move_accepter.hpp"
#include "move_prompter.hpp"

#include "../logic/game.hpp"
#include "../logic/move.hpp"

#include <memory>
#include <string>

/* ---- DECLARE class GameManager ---- */

class GameManager : public MoveAccepter {
    Game game;

public:
    GameManager() {};
    GameManager(const std::string& initial_position) : game(initial_position) {}
    ~GameManager() {};

    MovePrompter promptNextMove();

    bool acceptMove(Move move) override;
};

/* ---- END DECLARE ---- */