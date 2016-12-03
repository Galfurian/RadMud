/// @file   communication.hpp
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

/// Loads all the commands used to communicate.
void LoadCommunicationCommands();

/// @defgroup ComInterfaces Commands List Interface.
/// @brief All the functions necessary to handle the commands that a player can execute.
/// @{

/// The character say something.
bool DoSay(Character * character, ArgumentHandler & args);

/// The character whisper something to someone, even in distance.
bool DoWhisper(Character * character, ArgumentHandler & args);

/// The character emote something.
bool DoEmote(Character * character, ArgumentHandler & args);

/// The character report a bug.
bool DoBug(Character * character, ArgumentHandler & args);

/// The character send an idea.
bool DoIdea(Character * character, ArgumentHandler & args);

/// The character report a typo.
bool DoTypo(Character * character, ArgumentHandler & args);

///@}
