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

#include "baseEnumerator.hpp"

/// @brief The list of notions a character can learn.
class Knowledge : public BaseEnumerator {
public:
	/// The possible notions a character can learn.
	enum Enum {
		None,
		GatherHerbs = 3, ///< Gather Herbs
		GatherPlant = 4, ///< Gather Plant
		Butchery = 5, ///< Butcher Animal
		SkinAnimal = 6, ///< Skin Animal
		TanHide = 7, ///< Tan Hide
		ReadBook = 8, ///< Read Book
		Climb = 10, ///< Climb
		Run = 11, ///< Run
		Dash = 12, ///< Dash
		Woodcutting = 15, ///< Woodcutting
		Carpentry = 16, ///< Carpentry
		Woodcarving = 17, ///< Woodcarving
		Mining = 20, ///< Mining
		Smelting = 21, ///< Smelting
		Blacksmithing = 22, ///< Blacksmithing
		MetalWeaponCrafting = 23, ///< Metal Weapon Crafting
		MetalArmorCrafting = 24, ///< Metal Armor Crafting
		CraftSurvivalTool = 30, ///< Craft Survival Tool
		Cooking = 31, ///< Cooking skills.
		Scavenge = 35, ///< Scavenging skils.
		BasicArmorProficiency = 40 ///< Basic Armor Proficiency
	};

	/// @brief Constructor.
	Knowledge() : BaseEnumerator()
	{
		// Nothing to do.
	}

	/// @brief Constructor from unsigned int.
	explicit Knowledge(const unsigned int &_value) : BaseEnumerator(_value)
	{
		// Nothing to do.
	}

	/// @brief Constructor from enum.
	Knowledge(const Enum &_value) : BaseEnumerator(_value)
	{
		// Nothing to do.
	}

	/// @brief Returns the enumerator as string.
	std::string toString() const override;
};
