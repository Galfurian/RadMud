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

#include "ability.hpp"
#include "combatModifier.hpp"
#include "statusModifier.hpp"
#include "knowledge.hpp"
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
    /// The map of abilities modifiers.
    std::map<Ability, int> effectAbilityModifier;
    /// The map of combat modifiers.
    std::map<CombatModifier, int> effectCombatModifier;
    /// The map of status modifiers.
    std::map<StatusModifier, int> effectStatusModifier;
    /// The map of knowledge.
    std::map<Knowledge, int> effectKnowledge;

    /// @brief Constructor.
    Effect(Character * _affected,
           std::string _name,
           unsigned int _remainingTic,
           std::string _messageActivate,
           std::string _messageExpire,
           std::function<void(Character * character)> _expireFunction);

    /// @brief Update the cooldown of the effect.
    /// @return <b>True</b> if the effect is expired,<br>
    ///         <b>False</b> otherwise.
    bool update();

    /// @brief Operator used to order the effect based on the remaining time.
    bool operator<(const Effect & right) const
    {
        return remainingTic < right.remainingTic;
    }

    /// @brief Equality operator between the names of two effects.
    bool operator==(const Effect & right) const
    {
        return name == right.name;
    }
};

/// @brief Addition-Assignment operator for two Ability Modifier maps.
std::map<Ability, int> & operator+=(
    std::map<Ability, int> & left,
    const std::map<Ability, int> & right);

/// @brief Subtraction-Assignment operator for two Ability Modifier maps.
std::map<Ability, int> & operator-=(
    std::map<Ability, int> & left,
    const std::map<Ability, int> & right);

/// @brief Addition-Assignment operator for two CombatModifier maps.
std::map<CombatModifier, int> & operator+=(
    std::map<CombatModifier, int> & left,
    const std::map<CombatModifier, int> & right);

/// @brief Subtraction-Assignment operator for two CombatModifier maps.
std::map<CombatModifier, int> & operator-=(
    std::map<CombatModifier, int> & left,
    const std::map<CombatModifier, int> & right);

/// @brief Addition-Assignment operator for two StatusModifier maps.
std::map<StatusModifier, int> & operator+=(
    std::map<StatusModifier, int> & left,
    const std::map<StatusModifier, int> & right);

/// @brief Subtraction-Assignment operator for two StatusModifier maps.
std::map<StatusModifier, int> & operator-=(
    std::map<StatusModifier, int> & left,
    const std::map<StatusModifier, int> & right);

/// @brief Addition-Assignment operator for two Knowledge maps.
std::map<Knowledge, int> & operator+=(
    std::map<Knowledge, int> & left,
    const std::map<Knowledge, int> & right);

/// @brief Subtraction-Assignment operator for two Knowledge maps.
std::map<Knowledge, int> & operator-=(
    std::map<Knowledge, int> & left,
    const std::map<Knowledge, int> & right);