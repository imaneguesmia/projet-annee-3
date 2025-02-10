#pragma once

#include <utility>
#include <stdexcept>
#include <type_traits>
#include <format>

/**
 * @brief Safely converts an integral value to an enum class value.
 * The given enum class type must have the identifiers `EnumType::FIRST` and `EnumType::LAST` for bounding.
 * 
 * @tparam EnumType         The enum class type to convert to.
 * @tparam IntegralType     The integral type to convert from.
 * @param n                 The integral value to convert.
 * @return The converted enum class value.
 * 
 * @throws `std::invalid_argument` if the given integral value is out of bounds.
 */
template <class EnumType, class IntegralType>
constexpr EnumType safely_to_enum_class(IntegralType n) 
    requires (std::is_scoped_enum_v<EnumType> && std::is_integral_v<IntegralType>)
{
    if (n < static_cast<int>(EnumType::FIRST) || n > static_cast<int>(EnumType::LAST)) {
        throw std::invalid_argument(std::format("Integral value {} out of bounds of enum class", n));
    } 
    return static_cast<EnumType>(n);
}
