#include "chess/TranspositionTable.hpp"

namespace chess {

    std::optional<TTEntry> TranspositionTable::lookup(std::uint64_t key) const
    {
        auto it = table.find(key);
        if (it != table.end()) {
            return it->second;
        }
        return std::nullopt;
    }

    void TranspositionTable::store(std::uint64_t key, const TTEntry& entry)
    {
        table[key] = entry;
    }

} // namespace chess
