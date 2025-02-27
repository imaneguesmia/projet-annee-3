#pragma once

#include "attack_tables.hpp"
#include "position.hpp"
#include "player.hpp"
#include "board.hpp"

#include <memory>

/* ---- DECLARE class BoardAnalysis ---- */

/**
 * @brief Class handling chess position analysis.
 */
class BoardAnalysis {
    // Shared pointer to the pre-initialized attack tables.
    const std::shared_ptr<const AttackTables> at;

public:
    BoardAnalysis(const std::shared_ptr<const AttackTables> at)
        : at(std::move(at))
    {};
    ~BoardAnalysis() {};

    /**
     * @brief Determines whether or not a position is attacked.
     * 
     * @param position  The position to test.
     * @param player    The attacking player.
     * @param board     The board to check.
     * @return `true` if the position is attacked by the player. 
     */
    bool isSquareAttacked(const Position& position, Player player, const Board& board) const;

    // Tests if a player is in check.
    bool isInCheck(const Player player, const Board& board) const;

    
};

/* ---- END DECLARE ---- */