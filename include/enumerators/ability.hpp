/// @file   ability.hpp
/// @brief  Define the ability enum.
/// @author Enrico Fraccaroli
/// @date   Nov 19 2016
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

#include <cstdint>
#include <string>

/// @brief The list of character's abilities.
enum class Ability : uint8_t {
    None,
    Strength,
    Agility,
    Perception,
    Constitution,
    Intelligence
};

/// @brief Returns the ability as string.
std::string ability_to_string(Ability ability);

/// @brief Returns the ability from string.
Ability ability_from_string(const std::string &value);

/// @brief Returns the abbreviation of the ability name.
std::string get_ability_abbreviation(Ability ability);

/// @brief Returns the description of the ability.
std::string get_ability_description(Ability ability);

/// @brief Return the modifier of the given ability.
/// @param value The total ability value.
/// @return The ability modifier.
unsigned int get_ability_modifier(unsigned int value);
