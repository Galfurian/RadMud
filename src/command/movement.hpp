/// @file   movement.hpp
/// @author Enrico Fraccaroli
/// @date   Jan 07 2017
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

#include "argumentHandler.hpp"
#include "character.hpp"

/// Loads all the commands related to movement.
void LoadMovementCommands();

/// @defgroup MovementCommands Movement Commands.
/// @brief All the movement related commands.
/// @{

/// Execute character movement.
bool DoDirection(Character * character, Direction direction);

/// Allow the character to move between areas.
bool DoTravel(Character * character, ArgumentHandler & args);

/// Stop the current action of the character.
bool DoStop(Character * character, ArgumentHandler & args);

/// Make the player stand up.
bool DoStand(Character * character, ArgumentHandler & args);

/// Make the player crouch.
bool DoCrouch(Character * character, ArgumentHandler & args);

/// Make the player sit.
bool DoSit(Character * character, ArgumentHandler & args);

/// Make the player prone.
bool DoProne(Character * character, ArgumentHandler & args);

/// Make the player lie down and rest.
bool DoRest(Character * character, ArgumentHandler & args);

/// Make the player lie down and sleep.
bool DoSleep(Character * character, ArgumentHandler & args);

/// Make the player wake up.
bool DoWake(Character * character, ArgumentHandler & args);

///@}