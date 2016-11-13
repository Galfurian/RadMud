/// @file   general.hpp
/// @author Enrico Fraccaroli
/// @date   Aug 23 2016
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
