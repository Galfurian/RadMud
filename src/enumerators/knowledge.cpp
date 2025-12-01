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

std::string knowledge_to_string(Knowledge knowledge)
{
    switch (knowledge) {
    case Knowledge::GatherHerbs:
        return "GatherHerbs";
    case Knowledge::GatherPlant:
        return "GatherPlant";
    case Knowledge::Butchery:
        return "Butchery";
    case Knowledge::SkinAnimal:
        return "SkinAnimal";
    case Knowledge::TanHide:
        return "TanHide";
    case Knowledge::ReadBook:
        return "ReadBook";
    case Knowledge::Climb:
        return "Climb";
    case Knowledge::Run:
        return "Run";
    case Knowledge::Dash:
        return "Dash";
    case Knowledge::Woodcutting:
        return "Woodcutting";
    case Knowledge::Carpentry:
        return "Carpentry";
    case Knowledge::Woodcarving:
        return "Woodcarving";
    case Knowledge::Mining:
        return "Mining";
    case Knowledge::Smelting:
        return "Smelting";
    case Knowledge::Blacksmithing:
        return "Blacksmithing";
    case Knowledge::MetalWeaponCrafting:
        return "MetalWeaponCrafting";
    case Knowledge::MetalArmorCrafting:
        return "MetalArmorCrafting";
    case Knowledge::CraftSurvivalTool:
        return "CraftSurvivalTool";
    case Knowledge::Scavenge:
        return "Scavenge";
    case Knowledge::BasicArmorProficiency:
        return "BasicArmorProficiency";
    case Knowledge::None:
    default:
        return "None";
    }
}

Knowledge knowledge_from_string(const std::string &value)
{
    if (value == "GatherHerbs")
        return Knowledge::GatherHerbs;
    else if (value == "GatherPlant")
        return Knowledge::GatherPlant;
    else if (value == "Butchery")
        return Knowledge::Butchery;
    else if (value == "SkinAnimal")
        return Knowledge::SkinAnimal;
    else if (value == "TanHide")
        return Knowledge::TanHide;
    else if (value == "ReadBook")
        return Knowledge::ReadBook;
    else if (value == "Climb")
        return Knowledge::Climb;
    else if (value == "Run")
        return Knowledge::Run;
    else if (value == "Dash")
        return Knowledge::Dash;
    else if (value == "Woodcutting")
        return Knowledge::Woodcutting;
    else if (value == "Carpentry")
        return Knowledge::Carpentry;
    else if (value == "Woodcarving")
        return Knowledge::Woodcarving;
    else if (value == "Mining")
        return Knowledge::Mining;
    else if (value == "Smelting")
        return Knowledge::Smelting;
    else if (value == "Blacksmithing")
        return Knowledge::Blacksmithing;
    else if (value == "MetalWeaponCrafting")
        return Knowledge::MetalWeaponCrafting;
    else if (value == "MetalArmorCrafting")
        return Knowledge::MetalArmorCrafting;
    else if (value == "CraftSurvivalTool")
        return Knowledge::CraftSurvivalTool;
    else if (value == "Scavenge")
        return Knowledge::Scavenge;
    else if (value == "BasicArmorProficiency")
        return Knowledge::BasicArmorProficiency;
    else
        return Knowledge::None;
}
