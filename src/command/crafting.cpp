/// @file   crafting.cpp
/// @brief  Implements the methods used by the player in order to <b>craft items</b>.
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

#include "crafting.hpp"

#include "mud.hpp"
#include "buildAction.hpp"
#include "craftAction.hpp"
#include "characterUtilities.hpp"
#include "formatter.hpp"

void LoadCraftingCommands()
{
	Mud::instance().addCommand(std::make_shared<Command>(
		DoBuild, "build", "(item)", "Build something.", false, false, false));
	Mud::instance().addCommand(std::make_shared<Command>(
		DoDeconstruct, "deconstruct", "(building)", "Deconstruct a building.",
		false, false, false));
	Mud::instance().addCommand(std::make_shared<Command>(
		DoRead, "read", "(item)", "Read an inscription from an item.", false,
		false, false));
}

bool DoProfession(Character *character, Profession *profession,
				  ArgumentHandler &args)
{
	// Check if the character is sleeping.
	if (character->posture == CharacterPosture::Sleep) {
		character->sendMsg("Not while you're sleeping.\n");
		return false;
	}
	// Stop any action the character is executing.
	StopAction(character);
	// If the argument list is empty show what the character can craft.
	if (args.empty()) {
		character->sendMsg("You know how to %s:\n", profession->command);
		Table buildingTable;
		buildingTable.addDivider();
		buildingTable.addHeader(profession->action);
		buildingTable.addDivider();
		buildingTable.addColumn("Product", align::left);
		buildingTable.addColumn("Difficulty", align::right);
		for (auto production : profession->productions) {
			if (!HasRequiredKnowledge(character,
									  production->requiredKnowledge)) {
				continue;
			}
			buildingTable.addRow({ production->getNameCapital(),
								   std::to_string(production->difficulty) });
		}
		character->sendMsg(buildingTable.getTable() + "\n");
		return true;
	}
	if (args.size() != 1) {
		character->sendMsg("What do you want to produce?\n");
		return false;
	}
	// Search the production.
	auto production = profession->findProduction(args[0].getContent());
	if (production == nullptr) {
		character->sendMsg("%s '%s'.\n", profession->notFoundMessage,
						   args[0].getContent());
		return false;
	}
	if (production->profession != profession) {
		character->sendMsg("%s '%s'.\n", profession->notFoundMessage,
						   args[0].getContent());
		return false;
	}
	if (!HasRequiredKnowledge(character, production->requiredKnowledge)) {
		character->sendMsg("%s '%s'.\n", profession->notFoundMessage,
						   args[0].getContent());
		return false;
	}
	// Check if the actor has enough stamina to execute the action.
	if (CraftAction::getConsumedStamina(character) > character->stamina) {
		character->sendMsg("You are too tired right now.\n");
		return false;
	}
	// Search the needed tools.
	ItemVector usedTools;
	// Prepare a structure to set the options of the search.
	SearchOptionsCharacter searchOptions;
	searchOptions.searchInRoom = true;
	searchOptions.searchInEquipment = true;
	searchOptions.searchInInventory = true;
	if (!FindNearbyTools(character, production->tools, usedTools,
						 searchOptions)) {
		character->sendMsg("You don't have the right tools.\n");
		return false;
	}
	// Search the needed ingredients.
	std::vector<std::pair<Item *, unsigned int> > usedIngredients;
	// Do not search the ingredients inside the equipment.
	searchOptions.searchInRoom = true;
	searchOptions.searchInEquipment = false;
	searchOptions.searchInInventory = true;
	ResourceType missing(ResourceType::None);
	// Search the resources nearby.
	if (!FindNearbyResouces(character, production->ingredients, usedIngredients,
							searchOptions, missing)) {
		character->sendMsg("You don't have enough material (%s).\n",
						   missing.toString());
		return false;
	}
	// Search the needed workbench.
	if (production->workbench != ToolType::None) {
		// Just check inside the room.
		searchOptions.searchInRoom = true;
		searchOptions.searchInEquipment = false;
		searchOptions.searchInInventory = false;
		auto workbench = FindNearbyTool(character, production->workbench,
										ItemVector(), searchOptions);
		if (workbench == nullptr) {
			character->sendMsg("The proper workbench is not present.\n");
			return false;
		}
	}
	// Prepare the action.
	auto craftAction = std::make_shared<CraftAction>(
		character, production, usedTools, usedIngredients);
	// Check the new action.
	std::string error;
	if (craftAction->check(error)) {
		// Set the new action.
		character->pushAction(craftAction);
		// Send the messages.
		character->sendMsg("%s %s.\n", profession->startMessage,
						   Formatter::yellow() +
							   production->outcome->getName() +
							   Formatter::reset());
		character->room->sendToAll("%s has started %s something...\n",
								   { character }, character->getNameCapital(),
								   production->profession->action);
		return true;
	}
	character->sendMsg("%s\n", error);
	return false;
}

bool DoBuild(Character *character, ArgumentHandler &args)
{
	// Check if the character is sleeping.
	if (character->posture == CharacterPosture::Sleep) {
		character->sendMsg("Not while you're sleeping.\n");
		return false;
	}
	// Stop any action the character is executing.
	StopAction(character);
	// -------------------------------------------------------------------------
	// If the argument list is empty show what the character can build.
	if (args.empty()) {
		character->sendMsg("You know how to build:\n");
		Table buildingTable;
		buildingTable.addDivider();
		buildingTable.addHeader("Buildings");
		buildingTable.addDivider();
		buildingTable.addColumn("Building", align::left);
		buildingTable.addColumn("Difficulty", align::right);
		for (auto const &it : Mud::instance().mudBuildings) {
			auto building = it.second;
			if (!HasRequiredKnowledge(character, building->requiredKnowledge)) {
				continue;
			}
			buildingTable.addRow({ building->getNameCapital(),
								   std::to_string(building->difficulty) });
		}
		buildingTable.addDivider();
		character->sendMsg(buildingTable.getTable() + "\n");
		return true;
	}
	if (args.size() != 1) {
		character->sendMsg("What do you want to build?\n");
		return false;
	}
	auto schematics = Mud::instance().findBuilding(args[0].getContent());
	if (schematics == nullptr) {
		character->sendMsg("You don't know how to build '%s'.\n",
						   args[0].getContent());
		return false;
	}
	if (!HasRequiredKnowledge(character, schematics->requiredKnowledge)) {
		character->sendMsg("You don't know how to build '%s'.\n",
						   args[0].getContent());
		return false;
	}
	// Check if the actor has enough stamina to execute the action.
	if (BuildAction::getConsumedStamina(character) > character->stamina) {
		character->sendMsg("You are too tired right now.\n");
		return false;
	}
	// Search the needed tools.
	ItemVector usedTools;
	if (!FindNearbyTools(character, schematics->tools, usedTools,
						 SearchOptionsCharacter::allOptions())) {
		character->sendMsg("You don't have the right tools.\n");
		return false;
	}
	// Search the needed ingredients.
	std::vector<std::pair<Item *, unsigned int> > usedIngredients;
	// Do not search the ingredients inside the equipment.
	// Keep the previous search options and search the building.
	auto building = FindNearbyBuilding(character, schematics->buildingModel,
									   SearchOptionsCharacter(true, true));
	if (building == nullptr) {
		character->sendMsg("You don't have the item to build.\n");
		return false;
	}
	ResourceType missing(ResourceType::None);
	// Search the resources nearby.
	if (!FindNearbyResouces(character, schematics->ingredients, usedIngredients,
							SearchOptionsCharacter(true, true), missing)) {
		character->sendMsg("You don't have enough material (%s).\n",
						   missing.toString());
		return false;
	}
	// Prepare the action.
	auto buildAction = std::make_shared<BuildAction>(
		character, schematics, building, usedTools, usedIngredients);
	// Check the new action.
	std::string error;
	if (buildAction->check(error)) {
		// Set the new action.
		character->pushAction(buildAction);
		character->sendMsg("You start building %s.\n",
						   Formatter::yellow() +
							   schematics->buildingModel->getName() +
							   Formatter::reset());
		// Send the message inside the room.
		character->room->sendToAll("%s has started building something...\n",
								   { character }, character->getNameCapital());
		return true;
	}
	character->sendMsg("%s\n", error);
	return false;
}

bool DoDeconstruct(Character *character, ArgumentHandler &args)
{
	// Check if the character is sleeping.
	if (character->posture == CharacterPosture::Sleep) {
		character->sendMsg("Not while you're sleeping.\n");
		return false;
	}
	// Stop any action the character is executing.
	StopAction(character);
	if (args.size() != 1) {
		character->sendMsg("What do you want to deconstruct, sai?\n");
		return false;
	}
	auto item =
		character->findNearbyItem(args[0].getContent(), args[0].getIndex());
	if (item == nullptr) {
		character->sendMsg("You can't find want you to deconstruct.\n");
		return false;
	}
	// Check if the actor has enough stamina to execute the action.
	if (BuildAction::getConsumedStamina(character) > character->stamina) {
		character->sendMsg("You are too tired right now.\n");
		return false;
	}
	std::string error;
	if (item->canDeconstruct(error)) {
		character->sendMsg("You deconstruct %s.\n", item->getName(true));
		// Reset item flags.
		ClearFlag(item->flags, ItemFlag::Built);
		return true;
	}
	character->sendMsg(error + "\n");
	return false;
}

bool DoRead(Character *character, ArgumentHandler &args)
{
	// Check if the character is sleeping.
	if (character->posture == CharacterPosture::Sleep) {
		character->sendMsg("Not while you're sleeping.\n");
		return false;
	}
	// Stop any action the character is executing.
	StopAction(character);
	if (args.size() != 1) {
		character->sendMsg("What do you want to read today, sai?\n");
		return false;
	}
	auto item =
		character->findNearbyItem(args[0].getContent(), args[0].getIndex());
	if (item == nullptr) {
		character->sendMsg("You can't find want you to read.\n");
		return false;
	}
	auto writing = Mud::instance().findWriting(item->vnum);
	if (writing == nullptr) {
		character->sendMsg("There is nothing written on %s.\n",
						   item->getName(true));
		return false;
	}
	character->sendMsg("You start reading %s...\n", item->getName(true));
	if (!writing->title.empty()) {
		character->sendMsg("The title is '%s'.\n", writing->title);
	}
	character->sendMsg(writing->content + "\n");
	if (!writing->author.empty()) {
		character->sendMsg("The author appears to be '%s'.\n", writing->author);
	}
	return true;
}
