/// @file   liquidType.cpp
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

#include "liquidType.hpp"
#include "luaBridge.hpp"
#include "utils.hpp"

LiquidType::LiquidType() :
    liquidType()
{
    // Nothing to do.
}

LiquidType::LiquidType(const unsigned int & _liquidType) :
    liquidType()
{
    if (_liquidType == static_cast<unsigned int>(Normal))
        liquidType = Normal;
    else if (_liquidType == static_cast<unsigned int>(Alcohol))
        liquidType = Alcohol;
    else if (_liquidType == static_cast<unsigned int>(Poison))
        liquidType = Poison;
    else if (_liquidType == static_cast<unsigned int>(Blood))
        liquidType = Blood;
    else if (_liquidType == static_cast<unsigned int>(Lava))
        liquidType = Lava;
    else liquidType = None;
}

LiquidType::LiquidType(const Enum & _liquidType) :
    liquidType(_liquidType)
{
    // Nothing to do.
}

LiquidType::LiquidType(const std::string & _liquidType) :
    liquidType()
{
    if (ToLower(_liquidType) == "normal") liquidType = Normal;
    else if (ToLower(_liquidType) == "alcohol") liquidType = Alcohol;
    else if (ToLower(_liquidType) == "poison") liquidType = Poison;
    else if (ToLower(_liquidType) == "blood") liquidType = Blood;
    else if (ToLower(_liquidType) == "lava") liquidType = Lava;
    else liquidType = None;
}

std::string LiquidType::toString() const
{
    if (liquidType == Normal) return "Normal";
    if (liquidType == Alcohol) return "Alcohol";
    if (liquidType == Poison) return "Poison";
    if (liquidType == Blood) return "Blood";
    if (liquidType == Lava) return "Lava";
    return "None";
}

unsigned int LiquidType::toUInt() const
{
    return static_cast<unsigned int>(liquidType);
}

bool LiquidType::operator==(const LiquidType & rhs) const
{
    return liquidType == rhs.liquidType;
}

bool LiquidType::operator!=(const LiquidType & rhs) const
{
    return liquidType != rhs.liquidType;
}
