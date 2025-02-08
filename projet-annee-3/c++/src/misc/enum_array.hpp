#pragma once

#include <utility>

// This only exists because enum classes piss me off. 
// Not very efficient, but nyeh I prefer readable code.

/**
 * @brief Class encapsulating an array, allowing for indexing using a given enum class.
 * The given enum class must have the identifier `IndexEnum::LAST`, used as the last index in the
 * array.
 */
template <class IndexEnum, class ValueType>
    requires(std::is_scoped_enum_v<IndexEnum>)
class enum_array {
    ValueType array[static_cast<int>(IndexEnum::LAST) + 1] {};

public:
    ValueType& operator[](IndexEnum index) {
        return array[static_cast<int>(index)];
    };

    const ValueType& operator[](IndexEnum index) const {
        return array[static_cast<int>(index)];
    };
};