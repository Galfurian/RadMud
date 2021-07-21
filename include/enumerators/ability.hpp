/// @file   ability.hpp
/// @brief  Define the ability class.
/// @author Enrico Fraccaroli
/// @date   Nov 19 2016
/// @copyright
/// Copyright (c) 2016 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
/// Permission is hereby granted, free of charge, to any person obtaining a
/// copy of this software and associated documentation files (the "Software"),
/// to deal in the Software without restriction, including without limitation
/// the rights to use, copy, modify, merge, publish, distribute, sublicense,
/// and/or sell copies of the Software, and to permit persons to whom the
/// Software is furnished to do so, subject to the following conditions:
///     The above copyright notice and this permission notice shall be included
///     in all copies or substantial portions of the Software.
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
/// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
/// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
/// DEALINGS IN THE SOFTWARE.

#pragma once

#include "baseEnumerator.hpp"

/// @brief The list of character's abilities.
class Ability : public BaseEnumerator {
public:
	/// The possible abilities.
	enum Enum {
		None,
		Strength,
		Agility,
		Perception,
		Constitution,
		Intelligence
	};

	/// @brief Constructor.
	Ability() :
		BaseEnumerator()
	{
		// Nothing to do.
	}

	/// @brief Constructor from unsigned int.
	explicit Ability(const unsigned int &_value) :
		BaseEnumerator(_value)
	{
		if (_value > Intelligence)
			value = None;
	}

	/// @brief Constructor from enum.
	Ability(const Enum &_value) :
		BaseEnumerator(_value)
	{
		// Nothing to do.
	}

	/// @brief Returns the ability as string.
	std::string toString() const override;

	/// @brief Returns the abbreviation of the ability name.
	std::string getAbbreviation() const;

	/// @brief Returns the description of the ability.
	std::string getDescription() const;

	/// @brief Return the modifier of the given ability.
	/// @param _value The total ability value.
	/// @return The ability modifier.
	static inline int getModifier(const unsigned int &_value)
	{
		return (static_cast<int>(_value) - 10) / 2;
	}

	inline Ability &operator++()
	{
		++value;
		return (*this);
	}

	inline Ability &operator--()
	{
		--value;
		return (*this);
	}
};
