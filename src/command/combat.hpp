/// @file   combat.cpp
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
#include "../character/character.hpp"

/// Loads all the combat commands.
void LoadCombatCommands();

/// @defgroup ComInterfaces Commands List Interface.
/// @brief All the functions necessary to handle the commands that a player can execute.
/// @{

/// Engage in combat the desired target.
void DoKill(Character * character, ArgumentHandler & args);

/// Try to flee from combat.
void DoFlee(Character * character, ArgumentHandler & args);

/// Provides information about the surrounding area.
void DoScout(Character * character, ArgumentHandler & args);

/// Allows to load a magazine with projectiles.
void DoLoad(Character * character, ArgumentHandler & args);

/// Allows to unload a magazine with projectiles.
void DoUnload(Character * character, ArgumentHandler & args);

/// Allows to reload a firearm.
void DoReload(Character * character, ArgumentHandler & args);

/// Allows to aim at a target.
void DoAim(Character * character, ArgumentHandler & args);

/// Allows to shot with an equipped ranged weapon to an aimed target.
void DoFire(Character * character, ArgumentHandler & args);

///@}
