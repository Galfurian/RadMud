/// @file   combatModifier.hpp
/// @author Enrico Fraccaroli
/// @date   Jan 13 2017
/// @copyright
/// Copyright (c) 2017 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
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

#include <string>

/// @brief The list of combat modifiers.
class CombatModifier
{
public:
    /// The possible combat modifiers.
    enum Enum
    {
        None,
        IncreaseUnarmedHitRoll,
        DecreaseUnarmedHitRoll,
        IncreaseUnarmedDamage,
        DecreaseUnarmedDamage,
        IncreaseMeleeWeaponHitRoll,
        DecreaseMeleeWeaponHitRoll,
        IncreaseMeleeWeaponDamage,
        DecreaseMeleeWeaponDamage,
        IncreaseRangedWeaponHitRoll,
        DecreaseRangedWeaponHitRoll,
        IncreaseRangedWeaponDamage,
        DecreaseRangedWeaponDamage
    };

    /// @brief Constructor from unsigned ind.
    CombatModifier();

    /// @brief Constructor from unsigned ind.
    CombatModifier(const unsigned int & _value);

    /// @brief Constructor from enum.
    CombatModifier(const Enum & _value);

    /// @brief Check is the given number is a valid enum.
    static bool isValid(const unsigned int & _value);

    /// @brief Returns the enumerator as string.
    std::string toString() const;

    /// @brief Returns the enumerator as number.
    unsigned int toUInt() const;

    /// @brief Returns the sign of the given modifier (+1 | -1).
    static int getSign(const CombatModifier & right);

    /// @brief Equality operator.
    bool operator==(const CombatModifier & right) const
    {
        return this->value == right.value;
    }

    /// @brief Inequality operator.
    bool operator!=(const CombatModifier & right) const
    {
        return this->value != right.value;
    }

    /// @brief Equality operator.
    bool operator<(const CombatModifier & right) const
    {
        return this->value < right.value;
    }

private:
    /// Internal value.
    Enum value;
};
