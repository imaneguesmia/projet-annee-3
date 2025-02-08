#pragma once

#include <utility>

// This only exists because enum classes piss me off.

/**
 * @brief Increments an enum class value.
 * 
 * @tparam EnumType     The enum class type of the value to increment. Must have the identifier
 *                      `EnumType::LAST` to be used as the maximum value.
 * @param value         The value to increment.
 */
template <class EnumType>
    requires(std::is_scoped_enum_v<EnumType>)
constexpr void increment_enum(EnumType& value) noexcept {
    int next = std::to_underlying(value) + 1;

    if (next <= static_cast<int>(EnumType::LAST)) value = static_cast<EnumType>(next); 
}
/**
 * @brief Gets the next enum class value.
 * 
 * @tparam EnumType     The enum class type of the given value. Must have the identifier
 *                      `EnumType::LAST` to be used as the maximum value.
 * @param value         The value to get the next value of.
 */
template <class EnumType>
    requires(std::is_scoped_enum_v<EnumType>)
constexpr EnumType next_in_enum(const EnumType& value) noexcept {
    int next = std::to_underlying(value) + 1;

    if (next <= static_cast<int>(EnumType::LAST)) return static_cast<EnumType>(next);
    else return value;
}

/**
 * @brief Decrements an enum class value.
 * 
 * @tparam EnumType     The enum class type of the value to decrement. Must have the identifier
 *                      `EnumType::FIRST` to be used as the minimum value.
 * @param value         The value to decrement.
 */
template <class EnumType>
    requires(std::is_scoped_enum_v<EnumType>)
constexpr void decrement_enum(EnumType& value) noexcept {
    int next = std::to_underlying(value) - 1;

    if (next >= static_cast<int>(EnumType::FIRST)) value = static_cast<EnumType>(next); 
}
/**
 * @brief Gets the previous enum class value.
 * 
 * @tparam EnumType     The enum class type of the given value. Must have the identifier
 *                      `EnumType::FIRST` to be used as the minimum value.
 * @param value         The value to get the next value of.
 */
template <class EnumType>
    requires(std::is_scoped_enum_v<EnumType>)
constexpr EnumType previous_in_enum(const EnumType& value) noexcept {
    int next = std::to_underlying(value) - 1;

    if (next >= static_cast<int>(EnumType::FIRST)) return static_cast<EnumType>(next);
    else return value;
}