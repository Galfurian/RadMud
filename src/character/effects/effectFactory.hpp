/// @file   effectFactory.hpp
/// @brief  Define a factory of effects.
/// @author Enrico Fraccaroli
/// @date   Oct 22 2016
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
class EffectFactory
{
public:
    /// @brief Generates an effect which clears the target of a character.
    static Effect clearTargets(Character * actor, const int & lastingTime);
};
