/// @file   combat.cpp
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

/// Loads all the combat commands.
void LoadCombatCommands();

/// @defgroup ComInterfaces Commands List Interface.
/// @brief All the functions necessary to handle the commands that a player can execute.
/// @{

/// Engage in combat the desired target.
bool DoKill(Character * character, ArgumentHandler & args);

/// Try to flee from combat.
bool DoFlee(Character * character, ArgumentHandler & args);

/// Provides information about the surrounding area.
bool DoScout(Character * character, ArgumentHandler & args);

/// Allows to load a magazine with projectiles.
bool DoLoad(Character * character, ArgumentHandler & args);

/// Allows to unload a magazine with projectiles.
bool DoUnload(Character * character, ArgumentHandler & args);

/// Allows to reload a firearm.
bool DoReload(Character * character, ArgumentHandler & args);

/// Allows to aim at a target.
bool DoAim(Character * character, ArgumentHandler & args);

/// Allows to shot with an equipped ranged weapon to an aimed target.
bool DoFire(Character * character, ArgumentHandler & args);

///@}
