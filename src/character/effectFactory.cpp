/// @file   effectFactory.cpp
/// @brief  Implements a factory of effects.
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

#include "effectFactory.hpp"
#include "character.hpp"

Effect EffectFactory::clearTargets(Character * actor, const unsigned int & duration)
{
    return Effect(actor,
                  "ClearTargets",
                  duration,
                  "",
                  "",
                  [](Character * character)
                  {
                      if (character != nullptr) character->combatHandler.charactersInSight.clear();
                  },
                  0,
                  0,
                  std::map<Ability, int>(),
                  0,
                  0,
                  0,
                  0);
}

Effect EffectFactory::disturbedAim(Character * actor, const unsigned int & duration, const int & magnitude)
{
    return Effect(actor,
                  "DisturbedAim",
                  duration,
                  "",
                  "",
                  nullptr,
                  0,
                  0,
                  std::map<Ability, int>(),
                  0,
                  0,
                  magnitude,
                  0);
}