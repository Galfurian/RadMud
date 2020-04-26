/// @file   enum_checker.hpp
/// @brief  Contains the implementation of an enums checker.
/// @author Enrico Fraccaroli
/// @date   7 Jun 2016
/// @copyright
/// Copyright (c) 2016 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
/// Permission to use, copy, modify, and distribute this software for any
/// purpose with or without fee is hereby granted, provided that the above
/// copyright notice and this permission notice appear in all copies.
///
/// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
/// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
/// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
/// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
/// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
/// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
/// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

#pragma once

#include <stdexcept>

/// @brief Class used to check whether a given enum is valid or not.
template <typename EnumType, EnumType... Values> class EnumCheck;

/// @brief Class used to check whether a given enum is valid or not.
template <typename EnumType> class EnumCheck<EnumType> {
public:
	/// @brief Checks whether the integral value is one of the valid enums.
	/// @return <b>True</b> if the value is a valid enum,<br>
	///         <b>False</b> otherwise.
	template <typename IntType> static bool constexpr is_value(IntType)
	{
		return false;
	}
};

/// @brief Class used to check whether a given enum is valid or not.
template <typename EnumType, EnumType V, EnumType... Next>
class EnumCheck<EnumType, V, Next...> : private EnumCheck<EnumType, Next...> {
private:
	/// The previous EnumCheck occurence.
	using super = EnumCheck<EnumType, Next...>;

public:
	/// @brief Checks whether the integral value is one of the valid enums.
	/// @param value The value to check.
	/// @return <b>True</b> if the value is a valid enum,<br>
	///         <b>False</b> otherwise.
	template <typename IntType> static bool constexpr is_value(IntType value)
	{
		return value == static_cast<IntType>(V) || super::is_value(value);
	}

	/// @brief Allows to convert the given value to one of the valid enums.
	/// @param value The value to convert.
	/// @return The enum version of the integral value.
	template <typename IntType> static EnumType convert(IntType value)
	{
		if (!is_value(value)) {
			throw std::runtime_error("Enum value out of range");
		}
		return static_cast<EnumType>(value);
	}
};
