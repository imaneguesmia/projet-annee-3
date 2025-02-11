#ifndef CHESS_TRANSPOSITIONTABLE_HPP
#define CHESS_TRANSPOSITIONTABLE_HPP

#include <cstdint>
#include <unordered_map>
#include <optional>
#include "chess.hpp"

namespace chess {

    // Types de bound
    enum class Bound { EXACT, LOWER, UPPER };

    // Entrée de la TT
    struct TTEntry {
        int      score;
        int      depth;
        Bound    bound;
        Move     bestMove;
    };

    class TranspositionTable
    {
    public:
        TranspositionTable() = default;

        // Recherche d'une entrée
        std::optional<TTEntry> lookup(std::uint64_t key) const;

        // Stockage d'une entrée
        void store(std::uint64_t key, const TTEntry& entry);

    private:
        std::unordered_map<std::uint64_t, TTEntry> table;
    };

} // namespace chess

#endif // CHESS_TRANSPOSITIONTABLE_HPP
