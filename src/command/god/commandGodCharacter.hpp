/// @file   commandGodCharacter.hpp
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

/// Transfer a character from room to room.
bool DoTransfer(Character * character, ArgumentHandler & args);

/// Hurt the desired target.
bool DoHurt(Character * character, ArgumentHandler & args);

/// Get information about a character.
bool DoGodInfo(Character * character, ArgumentHandler & args);

/// Provides the list of opponents of the given target.
bool DoAggroList(Character * character, ArgumentHandler & args);

/// Regain completely health and stamina.
bool DoFeast(Character * character, ArgumentHandler & args);

/// Became invisible.
bool DoInvisible(Character * character, ArgumentHandler & args);

/// Return visible.
bool DoVisible(Character * character, ArgumentHandler & args);

/// List all the players.
bool DoPlayerList(Character * character, ArgumentHandler & args);

/// Set a flag to a character.
bool DoPlayerSetFlag(Character * character, ArgumentHandler & args);

/// Remove a flag from the character.
bool DoPlayerClearFlag(Character * character, ArgumentHandler & args);

/// Modify the value of the player skill.
bool DoPlayerModSkill(Character * character, ArgumentHandler & args);

/// Modify the value of the player attribute.
bool DoPlayerModAttr(Character * character, ArgumentHandler & args);

