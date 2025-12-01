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

#include "enumerators/liquidType.hpp"


std::string liquid_type_to_string(LiquidType type)
{
    switch (type) {
    case LiquidType::Normal: return "Normal";
    case LiquidType::Alcohol: return "Alcohol";
    case LiquidType::Poison: return "Poison";
    case LiquidType::Blood: return "Blood";
    case LiquidType::Lava: return "Lava";
    case LiquidType::None:
    default:
        return "None";
    }
}

LiquidType liquid_type_from_string(const std::string & value)
{
    if (value == "Normal") return LiquidType::Normal;
    else if (value == "Alcohol") return LiquidType::Alcohol;
    else if (value == "Poison") return LiquidType::Poison;
    else if (value == "Blood") return LiquidType::Blood;
    else if (value == "Lava") return LiquidType::Lava;
    else return LiquidType::None;
}
