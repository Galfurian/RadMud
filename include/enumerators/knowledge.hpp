/// @file   knowledge.hpp
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

#pragma once

#include <cstdint>
#include <string>

/// @brief The list of notions a character can learn.
enum class Knowledge : uint8_t {
    None,
    GatherHerbs           = 3,  /// < Gather Herbs
    GatherPlant           = 4,  /// < Gather Plant
    Butchery              = 5,  /// < Butcher Animal
    SkinAnimal            = 6,  /// < Skin Animal
    TanHide               = 7,  /// < Tan Hide
    ReadBook              = 8,  /// < Read Book
    Climb                 = 10, /// < Climb
    Run                   = 11, /// < Run
    Dash                  = 12, /// < Dash
    Woodcutting           = 15, /// < Woodcutting
    Carpentry             = 16, /// < Carpentry
    Woodcarving           = 17, /// < Woodcarving
    Mining                = 20, /// < Mining
    Smelting              = 21, /// < Smelting
    Blacksmithing         = 22, /// < Blacksmithing
    MetalWeaponCrafting   = 23, /// < Metal Weapon Crafting
    MetalArmorCrafting    = 24, /// < Metal Armor Crafting
    CraftSurvivalTool     = 30, /// < Craft Survival Tool
    Scavenge              = 35, /// < Scavenge
    BasicArmorProficiency = 40  /// < Basic Armor Proficiency
};

/// @brief Returns the knowledge as string.
std::string knowledge_to_string(Knowledge knowledge);

/// @brief Returns the knowledge from string.
Knowledge string_to_knowledge(const std::string &value);
