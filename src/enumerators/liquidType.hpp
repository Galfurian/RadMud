/// @file   liquidType.hpp
/// @author Enrico Fraccaroli
/// @date   gen 07 2017
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

#include <string>

extern "C"
{
#include "lua.h"
}

/// @brief Used to determine the type of the model.
class LiquidType
{
public:
    /// The possible types of model.
    enum Enum
    {
        None,    ///< [0] No type of liquid.
        Normal,  ///< [1] Normal liquid.
        Alcohol, ///< [2] The liquid is alcohol.
        Poison,  ///< [3] The liquid is poison.
        Blood,   ///< [4] The liquid is blood.
    };

    /// @brief Constructor.
    LiquidType();

    /// @brief Constructor.
    LiquidType(const unsigned int & _liquidType);

    /// @brief Constructor.
    LiquidType(const Enum & _liquidType);

    /// @brief Constructor.
    LiquidType(const std::string & _liquidType);

    /// @brief Check is the given number is a valid type of model.
    static bool isValid(const unsigned int & _liquidType);

    /// @brief Check is the given string is a valid type of model.
    static bool isValid(const std::string & _liquidType);

    /// @brief Returns the type of model as string.
    std::string toString() const;

    /// @brief Returns the type of model as number.
    unsigned int toUInt() const;

    /// @brief Equality operator w.r.t. a type of model object.
    bool operator==(const LiquidType & rhs) const;

    /// @brief Inequality operator w.r.t. a type of model object.
    bool operator!=(const LiquidType & rhs) const;

    /// @brief Function used to register inside the lua environment the class.
    /// @param L The lua environment.
    static void luaRegister(lua_State * L);

private:
    /// Internal type of liquid.
    Enum liquidType;
};
