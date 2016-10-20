/// @file   general.hpp
/// @author Enrico Fraccaroli
/// @date   Aug 23 2016
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

#include "argumentHandler.hpp"
#include "character.hpp"

/// Loads all the general commands.
void LoadGeneralCommands();

/// @defgroup ComInterfaces Commands List Interface.
/// @brief All the functions necessary to handle the commands that a player can execute.
/// @{

/// Execute character movement.
void DoDirection(Character * character, Direction direction);

/// Allow the character to mote the continental map.
void DoTravel(Character * character, ArgumentHandler & args);

/// Disconnect the player from the Mud.
void DoQuit(Character * character, ArgumentHandler & args);

/// Shows a list of all the online players.
void DoWho(Character * character, ArgumentHandler & args);

/// Allow to modify the character informations.
void DoSet(Character * character, ArgumentHandler & args);

/// Stop the current action of the character.
void DoStop(Character * character, ArgumentHandler & args);

/// The character look at: the current room, the near rooms, an object, a player, a character, and so on.
void DoLook(Character * character, ArgumentHandler & args);

/// Provide a complete help on the mud.
void DoHelp(Character * character, ArgumentHandler & args);

/// Allow character to modify his prompt.
void DoPrompt(Character * character, ArgumentHandler & args);

/// Report the current day phase.
void DoTime(Character * character, ArgumentHandler & args);

/// Make the player stand up.
void DoStand(Character * character, ArgumentHandler & args);

/// Make the player crouch.
void DoCrouch(Character * character, ArgumentHandler & args);

/// Make the player sit.
void DoSit(Character * character, ArgumentHandler & args);

/// Make the player prone.
void DoProne(Character * character, ArgumentHandler & args);

/// Make the player lie down and rest.
void DoRest(Character * character, ArgumentHandler & args);

/// Show player statistics.
void DoStatistics(Character * character, ArgumentHandler & args);

/// Allow player to rent and disconnect.
void DoRent(Character * character, ArgumentHandler & args);

/// Show player skills.
void DoSkills(Character * character, ArgumentHandler & args);

/// Show server informations and statistics.
void DoServer(Character * character, ArgumentHandler & args);

///@}
