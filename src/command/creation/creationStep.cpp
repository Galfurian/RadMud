/// @file   creationStep.cpp
/// @author Enrico Fraccaroli
/// @date   Nov 14, 2016
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

#include "creationStep.hpp"
#include "character.hpp"
#include "formatter.hpp"
#include "player.hpp"

CreationStep::CreationStep()
{
    // Nothing to do.
}

CreationStep::~CreationStep()
{
    // Nothing to do.
}


void CreationStep::printChices(Character * character)
{
    auto player = character->toPlayer();
    std::string preview = std::string();
    preview += Formatter::clearScreen();
    preview += "# ------------ Character Creation ------------ #\n";

    // NAME
    preview += "# Name         :";
    if (!player->name.empty())
    {
        preview += player->name + Formatter::reset();
    }
    preview += "\n";

    // PASSWORD
    preview += "# Passowrd     :";
    if (!player->password.empty())
    {
        for (unsigned int i = 0; i < player->password.size(); i++)
            preview += "*";
    }
    preview += "\n";

    // RACE
    preview += "# Race         :";
    if (player->race != nullptr)
    {
        preview += player->race->name + Formatter::reset();
    }
    preview += "\n";

    // STRENGTH
    preview += "# Strength     :";
    if (player->getAbility(Ability::Strength, false) > 0)
    {
        preview += ToString(player->getAbility(Ability::Strength, false)) + Formatter::reset();
    }
    preview += "\n";

    // AGILITY
    preview += "# Agility      :";
    if (player->getAbility(Ability::Agility, false) > 0)
    {
        preview += ToString(player->getAbility(Ability::Agility, false)) + Formatter::reset();
    }
    preview += "\n";

    // PERCEPTION
    preview += "# Perception   :";
    if (player->getAbility(Ability::Perception, false) > 0)
    {
        preview += ToString(player->getAbility(Ability::Perception, false)) + Formatter::reset();
    }
    preview += "\n";

    // CONSTITUTION
    preview += "# Constitution :";
    if (player->getAbility(Ability::Constitution, false) > 0)
    {
        preview += ToString(player->getAbility(Ability::Constitution, false)) + Formatter::reset();
    }
    preview += "\n";

    // INTELLIGENCE
    preview += "# Intelligence :";
    if (player->getAbility(Ability::Intelligence, false) > 0)
    {
        preview += ToString(player->getAbility(Ability::Intelligence, false)) + Formatter::reset();
    }
    preview += "\n";

    // GENDER
    preview += "# Gender       :" + GetGenderTypeName(player->gender) + "\n";

    // AGE
    preview += "# Age          :";
    if (player->age > 0)
    {
        preview += ToString(player->age) + Formatter::reset();
    }
    preview += "\n";

    // DESCRIPTION
    preview += "# Description  :";
    if (!player->description.empty())
    {
        preview += player->description + Formatter::reset();
    }
    preview += "\n";

    // WEIGHT
    preview += "# Weight       :";
    if (player->weight > 0)
    {
        preview += ToString(player->weight) + Formatter::reset();
    }
    preview += "\n";
    preview += "# -------------------------------------------- #\n";
    player->sendMsg(preview);
}
