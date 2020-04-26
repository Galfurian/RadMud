/// @file   buildAction.hpp
/// @brief  Definition of the class for a build action.
/// @author Enrico Fraccaroli
/// @date   Jul 14 2016
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

#include "generalAction.hpp"
#include "item/itemVector.hpp"

class Building;
class Item;

/// @brief An action executed by characters during building.
class BuildAction : public GeneralAction {
private:
	/// The schematics associated with the action.
	std::shared_ptr<Building> schematics;
	/// The item, target of the action.
	Item *building;
	/// The tool used by the actor for the action.
	ItemVector tools;
	/// The ingredients used by the actor for the action.
	std::vector<std::pair<Item *, unsigned int> > ingredients;

public:
	/// @brief Constructor.
	/// @param _actor       The actor who is doing the action.
	/// @param _schematics  A pointer to the schematic of the building.
	/// @param _building    A pointer to the item which has to be built.
	/// @param _tools       The list of used tools.
	/// @param _ingredients The list of used ingredients.
	BuildAction(
		Character *_actor, const std::shared_ptr<Building> &_schematics,
		Item *_building, ItemVector const &_tools,
		std::vector<std::pair<Item *, unsigned int> > const &_ingredients);

	/// @brief Destructor.
	virtual ~BuildAction();

	bool check(std::string &error) const override;

	ActionType getType() const override;

	std::string getDescription() const override;

	bool start() override;

	std::string stop() override;

	ActionStatus perform() override;

	unsigned int getCooldown() override;

	/// @brief Returns the stamina required to execute the action.
	/// @param character The actor.
	/// @return The required stamina.
	static unsigned int getConsumedStamina(Character *character);
};
