/// @file   commandObjectManagement.hpp
/// @author Enrico Fraccaroli
/// @date   29/12/2016
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

#include "input/argument.hpp"
#include "character/character.hpp"

/// @addtogroup ComInterfaces
/// @{

/// @brief Allow the character to pick up an object, or take an object from
///         another one.
bool DoTake(Character *character, ArgumentHandler &args);

/// Allow the character to drop an object.
bool DoDrop(Character *character, ArgumentHandler &args);

/// Put an object inside a container.
bool DoPut(Character *character, ArgumentHandler &args);

/// Allow the character to give an object to another character.
bool DoGive(Character *character, ArgumentHandler &args);

///@}