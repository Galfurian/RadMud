/// @file   toolType.hpp
/// @brief  Define the toolType class.
/// @author Enrico Fraccaroli
/// @date   Nov 23 2016
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

#include <string>

extern "C"
{
#include "lua.h"
}

/// @brief Used to determine the type of the tools.
class ToolType
{
public:
    /// The possible types of tools.
    enum Enum
    {
        None = 0,
        Pickaxe = 1,
        WoodcutterAxe = 10,
        Saw = 11,
        PrecisionChisel = 12,
        Hammer = 20,
        PlaneChisel = 21,
        Forge = 30,
        Anvil = 31,
        BlacksmithHammer = 32,
        Bellows = 33,
        Crucible = 34,
        Firelighter = 40
    };

    /// @brief Constructor from uint.
    ToolType();

    /// @brief Constructor from uint.
    ToolType(const unsigned int & _toolType);

    /// @brief Constructor from enum.
    ToolType(const Enum & _toolType);

    /// @brief Constructor from string.
    ToolType(const std::string & _toolType);

    /// @brief Check is the given number is a valid type of model.
    static bool isValid(const unsigned int & _toolType);

    /// @brief Check is the given string is a valid type of model.
    static bool isValid(const std::string & _toolType);

    /// @brief Returns the type of model as string.
    std::string toString() const;

    /// @brief Returns the type of model as number.
    unsigned int toUInt() const;

    /// @brief Equality operator w.r.t. a type of model object.
    bool operator==(const ToolType & rhs) const;

    /// @brief Equality operator w.r.t. a type of model enum.
    bool operator==(const ToolType::Enum & rhs) const;

    /// @brief Inequality operator w.r.t. a type of model enum.
    bool operator!=(const ToolType::Enum & rhs) const;

    /// @brief Equality operator w.r.t. a type of model object.
    bool operator<(const ToolType & rhs) const;

    /// @brief Function used to register inside the lua environment the class.
    /// @param L The lua environment.
    static void luaRegister(lua_State * L);

private:
    /// Internal type of model.
    Enum toolType;
};
