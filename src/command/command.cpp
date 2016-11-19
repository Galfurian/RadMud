/// @file   command.cpp
/// @brief  Implement the general methods needed to handle commands.
/// @author Enrico Fraccaroli
/// @date   Sep 8 2015
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

// Basic Include.
#include "command.hpp"
#include "processInitialization.hpp"
#include "mud.hpp"

#include "combat.hpp"
#include "communication.hpp"
#include "crafting.hpp"
#include "general.hpp"
#include "god.hpp"
#include "manager.hpp"
#include "object.hpp"

Command::Command() :
    gods(),
    name(),
    help(),
    arguments(),
    hndl(),
    canUseInCombat()
{
    // Nothing to do.
}

Command::Command(bool _gods,
                 std::string _name,
                 std::string _help,
                 std::string _arguments,
                 std::function<void(Character * character, ArgumentHandler & args)> _hndl,
                 bool _canUseInCombat) :
    gods(_gods),
    name(_name),
    help(_help),
    arguments(_arguments),
    hndl(_hndl),
    canUseInCombat(_canUseInCombat)
{
    // Nothing to do.
}

Command & Command::setGods(const bool & _gods)
{
    gods = _gods;
    return *this;
}

Command & Command::setName(const std::string & _name)
{
    name = _name;
    return *this;
}

Command & Command::setHelp(const std::string & _help)
{
    help = _help;
    return *this;
}

Command & Command::setArgs(const std::string & _arguments)
{
    arguments = _arguments;
    return *this;
}

Command & Command::setHndl(const std::function<void(Character * character, ArgumentHandler & args)> & _hndl)
{
    hndl = _hndl;
    return *this;
}

Command & Command::setCanUseInCombat(const bool & _canUseInCombat)
{
    canUseInCombat = _canUseInCombat;
    return *this;
}

bool Command::canUse(Character * character) const
{
    return (gods && HasFlag(character->flags, CharacterFlag::IsGod)) || (!gods);
}

void NoMobile(Character * character)
{
    if (character->isMobile())
    {
        throw std::runtime_error("Npcs are not allowed to execute this command.\n");
    }
}

void StopAction(Character * character)
{
    if ((character->getAction()->getType() != ActionType::Wait))
    {
        if (character->getAction()->getType() != ActionType::Combat)
        {
            character->doCommand("stop");
        }
    }
}

void LoadCommands()
{
    // Beware the order is important.
    // Changing the order will not compromise the correct execution of the Mud, but...
    // If a player just types 'l' or 'lo' in order to 'look' the first commant which get hit is the 'load' command.
    LoadGeneralCommands();
    LoadObjectCommands();
    LoadCommunicationCommands();
    LoadCraftingCommands();
    LoadCombatCommands();
    LoadManagerCommands();
    LoadGodCommands();
}
