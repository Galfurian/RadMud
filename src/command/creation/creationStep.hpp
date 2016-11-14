/// @file   creationStep.hpp
/// @author Enrico Fraccaroli
/// @date   Nov 14, 2016
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

#include "character.hpp"
#include "command.hpp"

/// Player names must consist of characters from this list.
#define  VALID_CHARACTERS_NAME "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_-"
/// Player descriptions must consist of characters from this list.
#define  VALID_CHARACTERS_DESC "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789 ,.\n"

class CreationStep
{
public:
    static void printChices(Character * character);

    static void process(Character * character, ArgumentHandler & args) = delete;

    static void advance(Character * character, const std::string & error = std::string()) = delete;

    static void rollBack(Character * character) = delete;
};

/// @defgroup ProcessStates Player state processing.
/// @brief All the functions necessary to process players commands, from creation to gameplay.
/// @{

/// Check player name.
void ProcessPlayerName(Character * character, ArgumentHandler & args);

/// Check if the player password is correct.
void ProcessPlayerPassword(Character * character, ArgumentHandler & args);

/// Step 1  - Choose the Name.
void ProcessNewName(Character * character, ArgumentHandler & args);

/// Step 2  - Choose the Password.
void ProcessNewPassword(Character * character, ArgumentHandler & args);

/// Step 3  - Confirm the Password.
void ProcessNewPasswordConfirm(Character * character, ArgumentHandler & args);

/// Step 4  - Short story of the mud world.
void ProcessNewStory(Character * character, ArgumentHandler & args);

/// Step 5  - Choose the Race.
void ProcessNewRace(Character * character, ArgumentHandler & args);

/// Step 6  - Choose the Attributes.
void ProcessNewAttributes(Character * character, ArgumentHandler & args);

/// Step 7  - Choose the Gender.
void ProcessNewGender(Character * character, ArgumentHandler & args);

/// Step 9  - Choose the description (optional).
void ProcessNewDescription(Character * character, ArgumentHandler & args);

/// Step 10 - Choose the Weight.
void ProcessNewWeight(Character * character, ArgumentHandler & args);

/// Step 11 - Confirm the character.
void ProcessNewConfirm(Character * character, ArgumentHandler & args);
///@}
