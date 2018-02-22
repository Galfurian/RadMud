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

namespace EffectFactory
{

Effect clearTargets(Character * actor,
                    const unsigned int & duration)
{
    Effect::Functionality functionality;
    functionality.expire = [](Character * character)
    {
        if (character != nullptr)
        {
            character->combatHandler.charactersInSight.clear();
        }
    };
    auto effect = Effect(actor,
                         "ClearTargets",
                         0,
                         duration,
                         functionality);
    return effect;
}

Effect disturbedAim(Character * actor,
                    const unsigned int & duration,
                    const int & negativeMagnitude)
{
    auto effect = Effect(actor,
                         "DisturbedAim",
                         0,
                         duration,
                         Effect::Functionality());
    effect.setCombatMod(CombatModifier::RangedWeaponHitRoll, negativeMagnitude);
    return effect;
}

Effect poisonDamage(Character * actor,
                    std::string const & name,
                    const unsigned int & delay,
                    const unsigned int & duration,
                    std::string const & messageActivate,
                    std::string const & messagePeriodic,
                    std::string const & messageExpire,
                    std::string const & messageDeath,
                    const unsigned int & damage)
{
    Effect::Functionality functionality;
    functionality.activate = [messageActivate](Character * character)
    {
        if (character != nullptr)
        {
            character->sendMsg(messageActivate + "\n\n");
        }
    };
    functionality.periodic =
        [damage, messageDeath, messagePeriodic](Character * character)
        {
            if (character != nullptr)
            {
                if (!character->remHealth(damage))
                {
                    character->sendMsg(messageDeath + "\n\n");
                    character->kill();
                }
                else
                {
                    character->sendMsg(messagePeriodic + "\n\n");
                }
            }
        };
    functionality.expire = [messageExpire](Character * character)
    {
        if (character != nullptr)
        {
            character->sendMsg(messageExpire + "\n\n");
        }
    };
    auto effect = Effect(actor,
                         name,
                         delay,
                         duration,
                         functionality);
    return effect;
}

}