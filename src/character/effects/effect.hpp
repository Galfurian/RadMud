/// @file   effect.hpp
/// @brief  Define the effect class.
/// @author Enrico Fraccaroli
/// @date   May 2 2016
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

#include "defines.hpp"
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
    /// Hit chance modifier.
    int hit;
    /// Damage modifier.
    int damage;

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
           int _hit,
           int _damage);

    /// @brief Update the cooldown of the effect.
    /// @return <b>True</b> if the effect is expired,<br>
    ///         <b>False</b> otherwise.
    bool update();

    /// @brief Operator used to order the effect based on the remaining time.
    bool operator<(const Effect & right) const;
};
