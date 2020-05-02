/// @file   itemUtils.hpp
/// @brief  Define utility function for items.
/// @author Enrico Fraccaroli
/// @date   02 01 2018
/// @copyright
/// Copyright (c) 2018 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
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

#include "item/item.hpp"
#include "enumerators/toolType.hpp"
#include "enumerators/resourceType.hpp"

#include <functional>

namespace ItemUtils
{
using FindPredicate = std::function<bool(Item *)>;

/// @brief Search for the item inside the given container.
/// @param container The container.
/// @param key       The key of the item to search.
/// @param number    Position of the item we want to look for.
/// @return The item, if it's in the character's inventory.
Item *FindItemIn(std::vector<Item *> const &container, std::string const &key, unsigned int number,
				 unsigned int *number_ptr = nullptr, const FindPredicate &predicate = nullptr);

/// @brief Properly check if the given item can be used as tool.
/// @param item         The item to check.
/// @param exceptions   The exceptions.
/// @param toolType     The type of tool.
/// @return If the item can be used as tool.
bool IsValidTool(Item *item, ItemVector const &exceptions, ToolType const &toolType);

// Create a function which checks if the given item is of the required type.
bool IsValidResource(Item *item, const ResourceType &type);

} // namespace ItemUtils