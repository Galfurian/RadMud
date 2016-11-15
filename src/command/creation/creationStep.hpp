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

#include "inputHandler.hpp"

/// Player names must consist of characters from this list.
#define  VALID_CHARACTERS_NAME "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_-"
/// Player descriptions must consist of characters from this list.
#define  VALID_CHARACTERS_DESC "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789 ,.\n"

class CreationStep :
    public InputHandler
{
public:
    /// @brief Constructor.
    CreationStep();

    /// @brief Destructor.
    virtual ~CreationStep();

    /// @brief Print the values inserted until now.
    /// @param character The player whose creating a new character.
    void printChices(Character * character);

    /// @brief Print the advancement in the character creation.
    /// @param character The player whose creating a new character.
    /// @param error     An optional message used only during error handling.
    virtual void advance(Character * character, const std::string & error = std::string()) = 0;

    /// @brief Reset the informations inserted in the previous state.
    /// @param character The player whose creating a new character.
    virtual void rollBack(Character * character) = 0;
};
