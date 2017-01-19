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

#include "knowledge.hpp"

Knowledge::Knowledge() :
    value(None)
{
    // Nothing to do.
}

Knowledge::Knowledge(const unsigned int & _value) :
    value()
{
    if (_value == static_cast<unsigned int>(GatherHerbs))
        value = GatherHerbs;
    else if (_value == static_cast<unsigned int>(GatherPlant))
        value = GatherPlant;
    else if (_value == static_cast<unsigned int>(ButcherAnimal))
        value = ButcherAnimal;
    else if (_value == static_cast<unsigned int>(SkinAnimal))
        value = SkinAnimal;
    else if (_value == static_cast<unsigned int>(TanHide))
        value = TanHide;
    else if (_value == static_cast<unsigned int>(ReadBook))
        value = ReadBook;
    else if (_value == static_cast<unsigned int>(Climb))
        value = Climb;
    else if (_value == static_cast<unsigned int>(Run))
        value = Run;
    else if (_value == static_cast<unsigned int>(Dash))
        value = Dash;
    else if (_value == static_cast<unsigned int>(Woodcutting))
        value = Woodcutting;
    else if (_value == static_cast<unsigned int>(Carpentry))
        value = Carpentry;
    else if (_value == static_cast<unsigned int>(Woodcarving))
        value = Woodcarving;
    else if (_value == static_cast<unsigned int>(Mining))
        value = Mining;
    else if (_value == static_cast<unsigned int>(Smelting))
        value = Smelting;
    else if (_value == static_cast<unsigned int>(Blacksmithing))
        value = Blacksmithing;
    else if (_value == static_cast<unsigned int>(MetalWeaponCrafting))
        value = MetalWeaponCrafting;
    else if (_value == static_cast<unsigned int>(MetalArmorCrafting))
        value = MetalArmorCrafting;
    else if (_value == static_cast<unsigned int>(Scavenge))
        value = Scavenge;
    else if (_value == static_cast<unsigned int>(CraftSurvivalTool))
        value = CraftSurvivalTool;
    else if (_value == static_cast<unsigned int>(BasicArmorProficiency))
        value = BasicArmorProficiency;
    else value = None;
}

Knowledge::Knowledge(const Enum & _value) :
    value(_value)
{
    // Nothing to do.
}

bool Knowledge::isValid(const unsigned int & _value)
{
    return (_value >= 1) && (_value <= 8);
}

std::string Knowledge::toString() const
{
    if (value == Knowledge::GatherHerbs)
        return "GatherHerbs";
    if (value == Knowledge::GatherPlant)
        return "GatherPlant";
    if (value == Knowledge::ButcherAnimal)
        return "ButcherAnimal";
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

unsigned int Knowledge::toUInt() const
{
    return static_cast<unsigned int>(value);
}
