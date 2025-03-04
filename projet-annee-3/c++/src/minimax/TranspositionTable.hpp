#pragma once

#include "ZobristHash.hpp"

#include "../logic/move.hpp"

#include <cstdint>
#include <unordered_map>
#include <optional>

/**
 * @enum Bound
 * @brief Represents the type of bound stored in the transposition table.
 */
enum class Bound { EXACT, LOWER, UPPER };

/**
 * @struct TTEntry
 * @brief Represents an entry in the transposition table.
 */
struct TTEntry {
    int score;      ///< Evaluation score of the position.
    int depth;      ///< Search depth at which this evaluation was obtained.
    Bound bound;    ///< Bound type (Exact, Lower, Upper).
    Move bestMove;  ///< Best move found at this position.
};

/**
 * @class TranspositionTable
 * @brief Implements a transposition table using an unordered map for caching evaluations.
 * @link https://www.chessprogramming.org/Transposition_Table
 */
class TranspositionTable {
public:
    /**
     * @brief Default constructor for the transposition table.
     */
    TranspositionTable() = default;

    /**
     * @brief Looks up a position in the transposition table.
     * @param key The Zobrist hash key representing the board position.
     * @return An optional TTEntry containing the stored evaluation if found, or std::nullopt if not found.
     */
    std::optional<TTEntry> lookup(uint64_t key) const;

    /**
     * @brief Stores an entry in the transposition table.
     * @param key The Zobrist hash key representing the board position.
     * @param entry The TTEntry containing evaluation and best move data.
     */
    void store(uint64_t key, const TTEntry& entry);



private:
    std::unordered_map<uint64_t, TTEntry> table; ///< Hash table storing board evaluations.
};
