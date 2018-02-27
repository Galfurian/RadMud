/// @file   commandGodMud.hpp
/// @author Enrico Fraccaroli
/// @date   Jan 02 2017
/// @copyright
/// Copyright (c) 2017 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
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

#include "commandGod.hpp"

/// @defgroup ComInterfaces Commands List Interface.
/// @brief All the functions necessary to handle the commands that a player
///         can execute.
/// @{

/// Shutdown the Mud.
bool DoShutdown(Character * character, ArgumentHandler & args);

/// Save the Mud.
bool DoMudSave(Character * character, ArgumentHandler & args);

/// Go to the desired room.
bool DoGoTo(Character * character, ArgumentHandler & args);

/// Shows the information about a faction.
bool DoFactionInfo(Character * character, ArgumentHandler & args);

/// List all the factions.
bool DoFactionList(Character * character, ArgumentHandler & args);

/// Shows the information about a race.
bool DoRaceInfo(Character * character, ArgumentHandler & args);

/// List all the races.
bool DoRaceList(Character * character, ArgumentHandler & args);

/// List all the skills.
bool DoSkillList(Character * character, ArgumentHandler & args);

/// Generate a new map.
bool DoGenerateMap(Character * character, ArgumentHandler & args);

/// Shows a generated map.
bool DoShowGenerateMap(Character * character, ArgumentHandler & args);

/// Deletes a generated map.
bool DoDeleteGenerateMap(Character * character, ArgumentHandler & args);

/// Builds a generated map.
bool DoBuildGenerateMap(Character * character, ArgumentHandler & args);

///@}