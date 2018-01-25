/// @file   toolType.cpp
/// @brief  Implement the toolType class.
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

#include "toolType.hpp"

std::string ToolType::toString() const
{
    if (value == Pickaxe) return "Pickaxe";
    if (value == WoodcutterAxe) return "WoodcutterAxe";
    if (value == Saw) return "Saw";
    if (value == PrecisionChisel) return "PrecisionChisel";
    if (value == Hammer) return "Hammer";
    if (value == PlaneChisel) return "PlaneChisel";
    if (value == Forge) return "Forge";
    if (value == Anvil) return "Anvil";
    if (value == BlacksmithHammer) return "BlacksmithHammer";
    if (value == Bellows) return "Bellows";
    if (value == Crucible) return "Crucible";
    if (value == Firelighter) return "Firelighter";
    return "None";
}
