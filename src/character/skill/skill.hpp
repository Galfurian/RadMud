/// @file   skill.hpp
/// @brief  Define the skill variables.
/// @author Enrico Fraccaroli
/// @date   Aug 23 2014
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

#include <string>
#include <vector>
#include <memory>
#include <map>

#include "ability.hpp"
#include "combatModifier.hpp"
#include "statusModifier.hpp"
#include "knowledge.hpp"
#include "modifierManager.hpp"

class Character;

/// @brief Holds details about a skill.
class Skill {
public:
	/// The skill virtual number.
	unsigned int vnum;
	/// The name of the skill.
	std::string name;
	/// The description of the skill.
	std::string description;
	/// The main ability of the skill.
	Ability ability;
	/// The overall stage of the skill.
	int stage;
	/// The list of required skills.
	std::vector<unsigned int> requiredSkill;
	/// The list of required skills.
	std::vector<unsigned int> usedForSkill;
	/// The internal modifier manager.
	std::shared_ptr<ModifierManager> modifierManager;

	/// @brief Constructor.
	Skill();

	/// @brief Disable Copy Construct.
	Skill(Skill const &) = delete;

	/// @brief Disable Move construct.
	Skill(Skill &&) = delete;

	/// @brief Disable Copy assign.
	Skill &operator=(Skill const &) = delete;

	/// @brief Disable Move assign.
	Skill &operator=(Skill &&) = delete;

	/// @brief Destructor.
	~Skill();

	/// @brief Check the correctness of the skill.
	/// @return <b>True</b> if it has correct values,<br>
	///         <b>False</b> otherwise.
	bool check();
};
