/// @file   commandGodItem.hpp
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

/// @addtogroup ComInterfaces
/// @{

/// Generate a new item from the vnum of a model.
bool DoItemCreate(Character *character, ArgumentHandler &args);

/// Destroy an item.
bool DoItemDestroy(Character *character, ArgumentHandler &args);

/// Materialize an item from everywhere.
bool DoItemGet(Character *character, ArgumentHandler &args);

/// Get information about an item.
bool DoItemInfo(Character *character, ArgumentHandler &args);

/// List all the items in the Mud.
bool DoItemList(Character *character, ArgumentHandler &args);

/// Show all the information fo the desired model.
bool DoModelInfo(Character *character, ArgumentHandler &args);

/// List all the model used to define items.
bool DoModelList(Character *character, ArgumentHandler &args);

/// List all the writings.
bool DoWritingList(Character *character, ArgumentHandler &args);

/// @}
