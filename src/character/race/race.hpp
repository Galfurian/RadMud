/// @file   race.hpp
/// @brief  Define the race variables.
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

#include <map>
#include <string>
#include <vector>
#include <memory>

#include "skillManager.hpp"
#include "corpseModel.hpp"
#include "ability.hpp"
#include "faction.hpp"
#include "skill.hpp"

class Material;

/// @brief Holds details about a race.
class Race {
public:
	/// The race virtual number.
	unsigned int vnum;
	/// The article for the race.
	std::string article;
	/// The name of the race.
	std::string name;
	/// The description of the race.
	std::string description;
	/// The base height of the race.
	double height;
	/// Base race abilities.
	std::map<Ability, unsigned int> abilities;
	/// The flag that indicates if the race is selectable by the player during
	///  character creation.
	bool player_allow;
	/// TileSet of the icon.
	int tileSet;
	/// TileId of the icon.
	int tileId;
	/// Corpse model.
	std::shared_ptr<CorpseModel> corpse;
	/// The list of body parts.
	std::vector<std::shared_ptr<BodyPart> > bodyParts;
	/// Base skills.
	std::vector<std::shared_ptr<SkillData> > skills;

	/// @brief Constructor.
	Race();

	/// @brief Disable Copy Construct.
	Race(Race const &) = delete;

	/// @brief Disable Move construct.
	Race(Race &&) = delete;

	/// @brief Disable Copy assign.
	Race &operator=(Race const &) = delete;

	/// @brief Disable Move assign.
	Race &operator=(Race &&) = delete;

	/// @brief Destructor.
	~Race();

	/// @brief Check the correctness of the race.
	/// @return <b>True</b> if the race has correct values,<br>
	///         <b>False</b> otherwise.
	bool check();

	/// @brief Return the short description of this race.
	/// @param capital If true the return description starts with a capital.
	/// @return The short description.
	std::string getShortDescription(bool capital = false);

	/// @brief Provides the value of the given ability.
	/// @param ability The ability to retrieve.
	/// @return The overall ability value.
	unsigned int getAbility(const Ability &ability) const;

	/// @brief Returns the tile of the race.
	/// @return The string which contains the code of the race's tile.
	std::string getTile();

	/// @brief Fills the provided table with the faction informations.
	/// @param sheet The table that has to be filled.
	void getSheet(Table &sheet) const;
};
