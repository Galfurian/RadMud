/// @file   effectFactory.hpp
/// @brief  Define a factory of effects.
/// @author Enrico Fraccaroli
/// @date   Oct 22 2016
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

///TODO:
/// Try to think about 'building' the effects by using the Builder paradigm.
/// The idea that came to my mind was:
///     You define inside the Database an Effect table. Inside that table
///     you can specify through a chained list of keyword which sub-effect
///     should be activated.
/// Eg:
///     Effect_Name        | Effects                                   | Duration
///     excruciating_death | death_when_expire; periodic_sick_messages | 10

#pragma once

#include "effect.hpp"

/// @brief Factory which allows to create predefined effects.
namespace EffectFactory
{

/// @brief Generates an effect which clears the target of a character.
/// @param actor    The character affected by the effect.
/// @param duration THe duration in TIC of the effect.
/// @return The created effect.
Effect clearTargets(Character * actor,
                    const unsigned int & duration);

/// @brief Generates an effect which clears the target of a character.
/// @details
/// If a character which is aiming to a target moves, then it suffers
///  a penality to hit.
/// @param actor             The character affected by the effect.
/// @param duration          The duration in TIC of the effect.
/// @param negativeMagnitude The magnitude of the disturbance.
/// @return The created effect.
Effect disturbedAim(Character * actor,
                    const unsigned int & duration,
                    const int & negativeMagnitude);

/// @brief Reduce health over time due to poisoning.
/// @param actor    The character affected by the effect.
/// @param duration The duration in TIC of the effect.
/// @param damage   The magnitude of the damage.
/// @return The created effect.
Effect poisonDamage(Character * actor,
                    std::string const & name,
                    const unsigned int & delay,
                    const unsigned int & duration,
                    std::string const & messageActivate,
                    std::string const & messagePeriodic,
                    std::string const & messageExpire,
                    std::string const & messageDeath,
                    const unsigned int & damage);

}
