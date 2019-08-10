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

#include "baseEnumerator.hpp"

/// @brief Used to determine the type of the tools.
class ToolType :
    public BaseEnumerator
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
        Firelighter = 40,
        CookingFire = 41
    };

    /// @brief Constructor.
    ToolType() :
        BaseEnumerator()
    {
        // Nothing to do.
    }

    /// @brief Constructor from unsigned int.
    explicit ToolType(const unsigned int & _value) :
        BaseEnumerator(_value)
    {
        // Nothing to do.
    }

    /// @brief Returns the type of model as string.
    std::string toString() const override;
};
