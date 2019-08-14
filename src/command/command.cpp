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
#include "mud.hpp"
#include "combat.hpp"
#include "communication.hpp"
#include "crafting.hpp"
#include "general.hpp"
#include "manager.hpp"
#include "commandObject.hpp"
#include "commandGod.hpp"
#include "movement.hpp"

Command::Command() :
	handler(),
	name(),
	arguments(),
	help(),
	gods(),
	canUseInCombat(),
	typedCompletely()
{
	// Nothing to do.
}

Command::Command(const std::function<bool(Character *character,
										  ArgumentHandler &args)> &_handler,
				 const std::string &_name, const std::string &_arguments,
				 const std::string &_help, const bool &_gods,
				 const bool &_canUseInCombat, const bool &_typedCompletely) :
	handler(_handler),
	name(_name),
	arguments(_arguments),
	help(_help),
	gods(_gods),
	canUseInCombat(_canUseInCombat),
	typedCompletely(_typedCompletely)
{
	// Nothing to do.
}

bool Command::canUse(Character *character) const
{
	return (gods && HasFlag(character->flags, CharacterFlag::IsGod)) || (!gods);
}

void StopAction(Character *character)
{
	if ((character->getAction() != ActionType::Wait)) {
		if (character->getAction() != ActionType::Combat) {
			character->doCommand("stop");
		}
	}
}

void LoadCommands()
{
	// Beware the order is important.
	// Changing the order will not compromise the correct
	//  execution of the Mud, but...
	// If a player just types 'l' or 'lo' in order to 'look'
	//  the first command which get hit is the 'load' command.
	LoadMovementCommands();
	LoadGeneralCommands();
	LoadObjectCommands();
	LoadCommunicationCommands();
	LoadCraftingCommands();
	LoadCombatCommands();
	LoadManagerCommands();
	LoadGodCommands();
}
