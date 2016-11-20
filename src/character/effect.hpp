/// @file   effect.hpp
/// @brief  Define the effect class.
/// @author Enrico Fraccaroli
/// @date   May 2 2016
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

#include "defines.hpp"
#include "ability.hpp"
#include "utils.hpp"

#include <functional>

class Character;

/// @brief Allows to define an effect which can alter the status of a character.
class Effect
{
public:
    /// The character affected by the effect.
    Character * affected;
    /// Name of the modifier.
    std::string name;
    /// How many tic until it expires.
    unsigned int remainingTic;
    /// Message to show when the effect begins.
    std::string messageActivate;
    /// Message to show when the effect ends.
    std::string messageExpire;
    /// Function executed when the effect expires.
    std::function<void(Character * character)> expireFunction;
    /// Health modifier.
    int health;
    /// Stamina modifier.
    int stamina;
    /// Abilities modifier.
    std::map<Ability, int> abilities;
    /// Melee hit chance modifier.
    int meleeHit;
    /// Melee damage modifier.
    int meleeDamage;
    /// Ranged hit chance modifier.
    int rangedHit;
    /// Ranged damage modifier.
    int rangedDamage;

    /// @brief Constructor.
    Effect(Character * _affected,
           std::string _name,
           unsigned int _remainingTic,
           std::string _messageActivate,
           std::string _messageFade,
           std::function<void(Character * character)> _expireFunction,
           int _health,
           int _stamina,
           std::map<Ability, int> _abilities,
           int _meleeHit,
           int _meleeDamage,
           int _rangedHit,
           int _rangedDamage);

    /// @brief Update the cooldown of the effect.
    /// @return <b>True</b> if the effect is expired,<br>
    ///         <b>False</b> otherwise.
    bool update();

    /// @brief Operator used to order the effect based on the remaining time.
    bool operator<(const Effect & right) const;
};
