/// @file   knowledge.cpp
/// @author Enrico Fraccaroli
/// @date   Jan 13 2017
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

#include "enumerators/knowledge.hpp"

std::string Knowledge::toString() const
{
    if (value == Knowledge::GatherHerbs)
        return "GatherHerbs";
    if (value == Knowledge::GatherPlant)
        return "GatherPlant";
    if (value == Knowledge::Butchery)
        return "Butchery";
    if (value == Knowledge::SkinAnimal)
        return "SkinAnimal";
    if (value == Knowledge::TanHide)
        return "TanHide";
    if (value == Knowledge::ReadBook)
        return "ReadBook";
    if (value == Knowledge::Climb)
        return "Climb";
    if (value == Knowledge::Run)
        return "Run";
    if (value == Knowledge::Dash)
        return "Dash";
    if (value == Knowledge::Woodcutting)
        return "Woodcutting";
    if (value == Knowledge::Carpentry)
        return "Carpentry";
    if (value == Knowledge::Woodcarving)
        return "Woodcarving";
    if (value == Knowledge::Mining)
        return "Mining";
    if (value == Knowledge::Smelting)
        return "Smelting";
    if (value == Knowledge::Blacksmithing)
        return "Blacksmithing";
    if (value == Knowledge::MetalWeaponCrafting)
        return "MetalWeaponCrafting";
    if (value == Knowledge::MetalArmorCrafting)
        return "MetalArmorCrafting";
    if (value == Knowledge::Scavenge)
        return "Scavenge";
    if (value == Knowledge::CraftSurvivalTool)
        return "CraftSurvivalTool";
    if (value == Knowledge::BasicArmorProficiency)
        return "BasicArmorProficiency";
    else return "None";
}
