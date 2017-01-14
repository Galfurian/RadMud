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

#include "abilityModifier.hpp"

#include <string>

/// @brief The list of character's abilities.
class Ability
{
public:
    /// The possible abilities.
    enum Enum
    {
        None,
        Strength,
        Agility,
        Perception,
        Constitution,
        Intelligence
    };

    /// @brief Constructor from uint.
    Ability();

    /// @brief Constructor from uint.
    Ability(const unsigned int & _ability);

    /// @brief Constructor from enum.
    Ability(const Enum & _ability);

    /// @brief Constructor from string.
    Ability(const std::string & _ability);

    /// @brief Check is the given number is a valid ability.
    static bool isValid(const unsigned int & _ability);

    /// @brief Check is the given string is a valid ability.
    static bool isValid(const std::string & _ability);

    /// @brief Returns the ability as string.
    std::string toString() const;

    /// @brief Returns the description of the ability.
    std::string getDescription() const;

    /// @brief Returns the ability as number.
    unsigned int toUInt() const;

    /// @brief Return the modifier of the given ability.
    /// @param value The total ability value.
    /// @return The ability modifier.
    static unsigned int getModifier(const unsigned int & value);

    /// @brief Returns the modifier identifying an increase of the ability.
    AbilityModifier getIncreaseModifier() const;

    /// @brief Returns the modifier identifying a decrease of the ability.
    AbilityModifier getDecreaseModifier() const;

    /// @brief Equality operator w.r.t. a ability object.
    bool operator==(const Ability & rhs) const;

    /// @brief Equality operator w.r.t. a ability enum.
    bool operator==(const Ability::Enum & rhs) const;

    /// @brief Inequality operator w.r.t. a ability enum.
    bool operator!=(const Ability::Enum & rhs) const;

    /// @brief Equality operator w.r.t. a ability object.
    bool operator<(const Ability & rhs) const;

private:
    /// Internal ability value.
    Enum ability;
};
