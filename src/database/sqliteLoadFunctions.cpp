/// @file   sqliteLoadFunctions.cpp
/// @author Enrico Fraccaroli
/// @date   Aug 4 2016
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

#include "sqliteLoadFunctions.hpp"
#include "sqliteException.hpp"

#include "liquidContainerItem.hpp"
#include "characterUtilities.hpp"
#include "currencyModel.hpp"
#include "modelFactory.hpp"
#include "itemFactory.hpp"
#include "mobileModel.hpp"
#include "shopItem.hpp"
#include "mud.hpp"
#include "resourceModel.hpp"

bool LoadBadName(ResultSet *result)
{
	if (!Mud::instance().badNames.insert(result->getNextString()).second) {
		throw SQLiteException("Error during bad name loading.");
	}
	return true;
}

bool LoadBlockedIp(ResultSet *result)
{
	if (!Mud::instance().blockedIPs.insert(result->getNextString()).second) {
		throw SQLiteException("Error during blocked ips loading.");
	}
	return true;
}

bool LoadNews(ResultSet *result)
{
	if (!Mud::instance()
			 .mudNews
			 .insert(std::make_pair(result->getNextString(),
									result->getNextString()))
			 .second) {
		throw SQLiteException("Error during news loading.");
	}
	return true;
}

bool LoadItemContent(ResultSet *result)
{
	auto container = Mud::instance().findItem(result->getNextUnsignedInteger());
	if (container == nullptr) {
		throw SQLiteException("Can't find container item.");
	}
	auto contained = Mud::instance().findItem(result->getNextUnsignedInteger());
	if (contained == nullptr) {
		throw SQLiteException("Can't find contained item.");
	}
	container->putInside(contained, false);
	return true;
}

bool LoadItem(ResultSet *result)
{
	auto itemVnum = result->getNextUnsignedInteger();
	auto itemModel =
		Mud::instance().findItemModel(result->getNextUnsignedInteger());
	if (itemModel == nullptr) {
		throw SQLiteException("Item has wrong model (" + ToString(itemVnum) +
							  ")");
	}
	// Create the item.
	auto item = ItemFactory::newItem(itemModel->getType());
	item->vnum = itemVnum;
	item->model = itemModel;
	item->quantity = result->getNextUnsignedInteger();
	item->maker = result->getNextString();
	item->condition = result->getNextDouble();
	item->composition =
		Mud::instance().findMaterial(result->getNextUnsignedInteger());
	item->quality = ItemQuality(result->getNextUnsignedInteger());
	item->flags = result->getNextUnsignedInteger();
	// Check correctness of attributes.
	if (!item->check()) {
		delete (item);
		throw SQLiteException("Error during error checking.");
	}
	// Add the item to the map of items.
	if (!Mud::instance().addItem(item)) {
		delete (item);
		throw SQLiteException("Error during error checking.");
	}
	return true;
}

bool LoadSkill(ResultSet *result)
{
	// Create an empty Skill.
	auto skill = std::make_shared<Skill>();
	skill->vnum = result->getNextUnsignedInteger();
	skill->name = result->getNextString();
	skill->description = result->getNextString();
	skill->ability = Ability(result->getNextUnsignedInteger());
	skill->stage = result->getNextInteger();
	// Check the correctness.
	if (!skill->check()) {
		throw SQLiteException("Error during error checking.");
	}
	if (!Mud::instance().addSkill(skill)) {
		throw SQLiteException("Error during skill insertion.");
	}
	MudLog(LogLevel::Debug, "\t%s%s%s", Align(skill->name, align::left, 25),
		   Align(skill->ability.toString(), align::left, 15),
		   skill->description);
	return true;
}

bool LoadSkillPrerequisite(ResultSet *result)
{
	auto skillVnum = result->getNextUnsignedInteger();
	auto skill = Mud::instance().findSkill(skillVnum);
	if (skill == nullptr) {
		throw SQLiteException("Can't find the skill " + ToString(skillVnum));
	}
	auto requiredSkillVnum = result->getNextUnsignedInteger();
	auto requiredSkill = Mud::instance().findSkill(requiredSkillVnum);
	if (requiredSkill == nullptr) {
		throw SQLiteException("Can't find the skill " +
							  ToString(requiredSkillVnum) +
							  " required by the skill " + ToString(skillVnum));
	}
	skill->requiredSkill.emplace_back(requiredSkillVnum);
	requiredSkill->usedForSkill.emplace_back(skillVnum);
	MudLog(LogLevel::Debug, "\t%s requires %s",
		   Align(skill->name, align::left, 25),
		   Align(requiredSkill->name, align::left, 25));
	return true;
}

bool LoadSkillAbilityModifier(ResultSet *result)
{
	auto skillVnum = result->getNextUnsignedInteger();
	auto skill = Mud::instance().findSkill(skillVnum);
	if (skill == nullptr) {
		throw SQLiteException("Can't find the skill " + ToString(skillVnum));
	}
	auto abilityNumber = result->getNextUnsignedInteger();
	auto ability = Ability(abilityNumber);
	if (ability == Ability::None) {
		throw SQLiteException("Can't find the ability " +
							  ToString(abilityNumber));
	}
	auto modifier = result->getNextInteger();
	skill->modifierManager->setAbilityMod(ability, modifier);
	// Log it.
	MudLog(LogLevel::Debug, "\t%s%s%s", Align(skill->name, align::left, 25),
		   Align(ability.toString(), align::left, 35),
		   Align(modifier, align::left, 35));
	return true;
}

bool LoadSkillStatusModifier(ResultSet *result)
{
	auto skillVnum = result->getNextUnsignedInteger();
	auto skill = Mud::instance().findSkill(skillVnum);
	if (skill == nullptr) {
		throw SQLiteException("Can't find the skill " + ToString(skillVnum));
	}
	auto statusModifierNumber = result->getNextUnsignedInteger();
	auto statusModifier = StatusModifier(statusModifierNumber);
	if (statusModifier == StatusModifier::None) {
		throw SQLiteException("Can't find the status modifier " +
							  ToString(statusModifierNumber));
	}
	auto modifier = result->getNextInteger();
	skill->modifierManager->setStatusMod(statusModifier, modifier);
	// Log it.
	MudLog(LogLevel::Debug, "\t%s%s%s", Align(skill->name, align::left, 25),
		   Align(statusModifier.toString(), align::left, 35),
		   Align(modifier, align::left, 35));
	return true;
}

bool LoadSkillCombatModifier(ResultSet *result)
{
	auto skillVnum = result->getNextUnsignedInteger();
	auto skill = Mud::instance().findSkill(skillVnum);
	if (skill == nullptr) {
		throw SQLiteException("Can't find the skill " + ToString(skillVnum));
	}
	auto combatModifierNumber = result->getNextUnsignedInteger();
	auto combatModifier = CombatModifier(combatModifierNumber);
	if (combatModifier == CombatModifier::None) {
		throw SQLiteException("Can't find the combat modifier " +
							  ToString(combatModifierNumber));
	}
	auto modifier = result->getNextInteger();
	skill->modifierManager->setCombatMod(combatModifier, modifier);
	// Log it.
	MudLog(LogLevel::Debug, "\t%s%s%s", Align(skill->name, align::left, 25),
		   Align(combatModifier.toString(), align::left, 35),
		   Align(modifier, align::left, 35));
	return true;
}

bool LoadSkillKnowledge(ResultSet *result)
{
	auto skillVnum = result->getNextUnsignedInteger();
	auto skill = Mud::instance().findSkill(skillVnum);
	if (skill == nullptr) {
		throw SQLiteException("Can't find the skill " + ToString(skillVnum));
	}
	auto knowledgeNumber = result->getNextUnsignedInteger();
	auto knowledge = Knowledge(knowledgeNumber);
	if (knowledge == Ability::None) {
		throw SQLiteException("Can't find the knowledge " +
							  ToString(knowledgeNumber));
	}
	skill->modifierManager->setKnowledge(knowledge, 1);
	// Log it.
	MudLog(LogLevel::Debug, "\t%s%s", Align(skill->name, align::left, 25),
		   Align(knowledge.toString(), align::left, 35));
	return true;
}

bool LoadFaction(ResultSet *result)
{
	// Create an empty Faction.
	auto faction = new Faction();
	faction->vnum = result->getNextUnsignedInteger();
	faction->name = result->getNextString();
	faction->description = result->getNextString();
	auto currencyVnum = result->getNextUnsignedInteger();
	auto currencyModel = Mud::instance().findItemModel(currencyVnum);
	if (currencyModel == nullptr) {
		throw SQLiteException("Can't find the currency " +
							  ToString(currencyVnum));
	}
	if (currencyModel->getType() != ModelType::Currency) {
		throw SQLiteException("Model is not currency " +
							  ToString(currencyVnum));
	}
	faction->currency = currencyModel->to<CurrencyModel>();
	// Translate new_line.
	FindAndReplace(&faction->description, "%r", "\n");
	// Check the correctness.
	if (!faction->check()) {
		throw SQLiteException("Error during error checking.");
	}
	if (!Mud::instance().addFaction(faction)) {
		throw SQLiteException("Error during faction insertion.");
	}
	return true;
}

bool LoadModel(ResultSet *result)
{
	// Retrieve the vnum and the type of model.
	auto vnum = result->getNextUnsignedInteger();
	// Create a pointer to the new item model.
	auto itemModel =
		ModelFactory::newModel(ModelType(result->getNextUnsignedInteger()));
	if (itemModel == nullptr) {
		throw SQLiteException("Wrong type of model " + ToString(vnum));
	}
	// Set the values of the new model.
	itemModel->vnum = vnum;
	itemModel->name = result->getNextString();
	itemModel->article = result->getNextString();
	itemModel->shortdesc = result->getNextString();
	itemModel->keys = SplitString(result->getNextString(), " ");
	itemModel->description = result->getNextString();
	itemModel->modelFlags = result->getNextUnsignedInteger();
	itemModel->baseWeight = result->getNextDouble();
	itemModel->basePrice = result->getNextUnsignedInteger();
	itemModel->material = MaterialType(result->getNextUnsignedInteger());
	itemModel->tileSet = result->getNextInteger();
	itemModel->tileId = result->getNextInteger();
	if (!itemModel->setModel(result->getNextString())) {
		throw SQLiteException("Error when setting the model.");
	}
	// Translate new_line.
	FindAndReplace(&itemModel->description, "%r", "\n");
	// Check the correctness.
	if (!itemModel->check()) {
		throw SQLiteException("Error during error checking.");
	}
	if (!Mud::instance().addItemModel(itemModel)) {
		throw SQLiteException("Error during itemModel insertion.");
	}
	return true;
}

bool LoadRace(ResultSet *result)
{
	// Create an empty Race.
	auto race = new Race();
	race->vnum = result->getNextUnsignedInteger();
	race->article = result->getNextString();
	race->name = result->getNextString();
	race->description = result->getNextString();
	race->height = result->getNextDouble();
	race->player_allow = (result->getNextInteger() == 1);
	unsigned value;
	for (Ability it = Ability::Strength; it <= Ability::Intelligence; ++it) {
		value = result->getNextUnsignedInteger();
		// Set the base ability value of the race.
		race->abilities[it] = value;
	}
	auto corpse = std::make_shared<CorpseModel>();
	corpse->vnum = 0;
	corpse->name = "corpse";
	corpse->article = "a";
	corpse->shortdesc = "the corpse of " + race->getShortDescription();
	for (auto key : SplitString(race->name, " ")) {
		corpse->keys.emplace_back(key);
	}
	corpse->keys.emplace_back("corpse");
	corpse->modelFlags = 0;
	corpse->tileSet = race->tileSet;
	corpse->tileId = race->tileId;
	corpse->description = result->getNextString();
	// Retrieve and set the values.
	corpse->corpseRace = race;
	auto corpseCompositionVnum = result->getNextUnsignedInteger();
	auto corpseComposition =
		Mud::instance().findMaterial(corpseCompositionVnum);
	if (corpseComposition == nullptr) {
		throw SQLiteException("Cannot find the material " +
							  ToString(corpseCompositionVnum) +
							  " for a corpse.");
	}
	corpse->corpseComposition = corpseComposition;
	race->corpse = corpse;
	race->tileSet = result->getNextInteger();
	race->tileId = result->getNextInteger();
	// Translate new_line.
	FindAndReplace(&race->description, "\n", " ");
	// Check the correctness.
	if (!race->check()) {
		throw SQLiteException("Error during error checking.");
	}
	if (!Mud::instance().addRace(race)) {
		throw SQLiteException("Error during race insertion.");
	}
	return true;
}

bool LoadRaceBodyPart(ResultSet *result)
{
	auto raceVnum = result->getNextUnsignedInteger();
	auto race = Mud::instance().findRace(raceVnum);
	if (race == nullptr) {
		throw SQLiteException("Cannot find the race " + ToString(raceVnum));
	}
	auto bodyPartVnum = result->getNextUnsignedInteger();
	auto bodyPart = Mud::instance().findBodyPart(bodyPartVnum);
	if (bodyPart == nullptr) {
		throw SQLiteException("Cannot find the body part " +
							  ToString(bodyPartVnum));
	}
	// Add the body part to the race.
	race->bodyParts.emplace_back(bodyPart);
	// Log the body part.
	MudLog(LogLevel::Debug, "\t%s%s", Align(race->name, align::left, 25),
		   Align(bodyPart->name, align::left, 25));
	return true;
}

bool LoadRaceBaseSkill(ResultSet *result)
{
	auto raceVnum = result->getNextUnsignedInteger();
	auto race = Mud::instance().findRace(raceVnum);
	if (race == nullptr) {
		throw SQLiteException("Cannot find the race " + ToString(raceVnum));
	}
	auto skillVnum = result->getNextUnsignedInteger();
	auto skill = Mud::instance().findSkill(skillVnum);
	if (skill == nullptr) {
		throw SQLiteException("Cannot find the skill " + ToString(skillVnum));
	}
	auto skillLevel = result->getNextUnsignedInteger();
	// Set the base skill of the race.
	race->skills.emplace_back(std::make_shared<SkillData>(skill, skillLevel));
	// Log the skill.
	MudLog(LogLevel::Debug, "\t%s%s%s", Align(race->name, align::left, 25),
		   Align(skill->name, align::left, 25), skillLevel);
	return true;
}

bool LoadMobileModel(ResultSet *result)
{
	(void)result;
	// Create an empty Mobile model.
	auto mm = std::make_shared<MobileModel>();
	mm->vnum = result->getNextUnsignedInteger();
	mm->propnoun = result->getNextString();
	mm->keys = GetWords(result->getNextString());
	mm->shortdesc = result->getNextString();
	mm->staticdesc = result->getNextString();
	mm->description = result->getNextString();
	FindAndReplace(&mm->description, "%r", "\n");
	mm->race = Mud::instance().findRace(result->getNextUnsignedInteger());
	if (mm->race == nullptr) {
		throw SQLiteException("Cannot find the race for the mobile model.");
	}
	mm->faction = Mud::instance().findFaction(result->getNextUnsignedInteger());
	if (mm->faction == nullptr) {
		throw SQLiteException("Cannot find the faction for the mobile model.");
	}
	mm->gender = static_cast<GenderType>(result->getNextInteger());
	mm->weight = result->getNextDouble();
	mm->actions = GetWords(result->getNextString());
	mm->flags = result->getNextUnsignedInteger();
	mm->level = result->getNextUnsignedInteger();
	auto abilities_str = result->getNextString();
	if (!abilities_str.empty()) {
		if (!ParseAbilities(mm->abilities, abilities_str)) {
			throw SQLiteException("Wrong characteristics.");
		}
	} else {
		mm->abilities = mm->race->abilities;
	}
	mm->lua_script = result->getNextString();
	if (!Mud::instance()
			 .mudMobileModels.insert(std::make_pair(mm->vnum, mm))
			 .second) {
		throw SQLiteException("Failed to store mobile model.");
	}
	return true;
}

bool LoadMobileSpawn(ResultSet *result)
{
	// Retrive the values.
	auto vnum = result->getNextUnsignedInteger();
	auto modelVnum = result->getNextUnsignedInteger();
	auto roomVnum = result->getNextUnsignedInteger();
	// Retrive the data.
	auto mm = Mud::instance().findMobileModel(modelVnum);
	if (mm == nullptr) {
		throw SQLiteException("Cannot find mobile model " +
							  ToString(modelVnum) + ".");
	}
	auto room = Mud::instance().findRoom(roomVnum);
	if (room == nullptr) {
		throw SQLiteException("Cannot find the room " + ToString(roomVnum) +
							  ".");
	}
	if (mm->spawn(room, vnum) == nullptr) {
		throw SQLiteException("Cannot spawn mobile " + ToString(vnum) + ".");
	}
	return true;
}

bool LoadRoom(ResultSet *result)
{
	// Create an empty Room.
	auto room = new Room();
	// Initialize the Room.
	room->vnum = result->getNextUnsignedInteger();
	room->coord.x = result->getNextInteger();
	room->coord.y = result->getNextInteger();
	room->coord.z = result->getNextInteger();
	room->terrain =
		Mud::instance().findTerrain(result->getNextUnsignedInteger());
	room->name = result->getNextString();
	room->description = result->getNextString();
	room->flags = result->getNextUnsignedInteger();
	// Translate new_line.
	FindAndReplace(&room->description, "%r", "\n");
	// Check the correctness.
	if (!room->check()) {
		delete (room);
		throw SQLiteException("Error during error checking.");
	}
	if (!Mud::instance().addRoom(room)) {
		delete (room);
		throw SQLiteException("Error during room insertion.");
	}
	return true;
}

bool LoadRoomLiquid(ResultSet *result)
{
	auto roomVnum = result->getNextUnsignedInteger();
	auto room = Mud::instance().findRoom(roomVnum);
	if (room == nullptr) {
		throw SQLiteException("Cannot find the room: " + ToString(roomVnum));
	}
	auto liquidVnum = result->getNextUnsignedInteger();
	auto liquid = Mud::instance().findLiquid(liquidVnum);
	if (liquid == nullptr) {
		throw SQLiteException("Cannot find the liquid: " +
							  ToString(liquidVnum));
	}
	auto quantity = result->getNextInteger();
	if (quantity > 0) {
		room->liquidContent = std::make_pair(liquid, quantity);
	}
	return true;
}

bool LoadExit(ResultSet *result)
{
	// Create an empty exit.
	auto newExit = std::make_shared<Exit>();
	// Retrieve the values.
	auto sourceVnum = result->getNextUnsignedInteger();
	auto destinationVnum = result->getNextUnsignedInteger();
	auto directionValue = result->getNextUnsignedInteger();
	auto flagValue = result->getNextUnsignedInteger();
	// Check the correctness.
	newExit->source = Mud::instance().findRoom(sourceVnum);
	if (newExit->source == nullptr) {
		throw SQLiteException("Can't find source " + ToString(sourceVnum));
	}
	newExit->destination = Mud::instance().findRoom(destinationVnum);
	if (newExit->destination == nullptr) {
		throw SQLiteException("Can't find destination " +
							  ToString(destinationVnum));
	}
	Direction direction(directionValue);
	if (direction == Direction::None) {
		throw SQLiteException("Direction si not valid " +
							  ToString(directionValue));
	}
	newExit->direction = direction;
	newExit->flags = flagValue;
	if (!newExit->check()) {
		throw SQLiteException("Error during error checking.");
	}
	// Push this exit into the list of exits of the source room.
	newExit->source->addExit(newExit);
	return true;
}

bool LoadItemRoom(ResultSet *result)
{
	auto room = Mud::instance().findRoom(result->getNextUnsignedInteger());
	auto item = Mud::instance().findItem(result->getNextUnsignedInteger());
	// Check the correctness.
	if (room == nullptr) {
		throw SQLiteException("Can't find the room.");
	}
	if (item == nullptr) {
		throw SQLiteException("Can't find the item.");
	}
	// Load the item inside the room.
	if (HasFlag(item->flags, ItemFlag::Built)) {
		room->addBuilding(item, false);
	} else {
		room->addItem(item, false);
	}
	return true;
}

bool LoadArea(ResultSet *result)
{
	// Create an empty area.
	auto area = new Area();
	// Initialize the area.
	area->vnum = result->getNextUnsignedInteger();
	area->name = result->getNextString();
	area->builder = result->getNextString();
	area->width = result->getNextInteger();
	area->height = result->getNextInteger();
	area->elevation = result->getNextInteger();
	area->tileSet = result->getNextInteger();
	area->type = static_cast<AreaType>(result->getNextUnsignedInteger());
	area->status = static_cast<AreaStatus>(result->getNextUnsignedInteger());
	// Check the correctness.
	if (!area->check()) {
		throw SQLiteException("Error during error checking.");
	}
	if (!Mud::instance().addArea(area)) {
		throw SQLiteException("Error during area insertion.");
	}
	return true;
}

bool LoadAreaList(ResultSet *result)
{
	auto areaVnum = result->getNextUnsignedInteger();
	auto roomVnum = result->getNextUnsignedInteger();
	auto area = Mud::instance().findArea(areaVnum);
	auto room = Mud::instance().findRoom(roomVnum);
	// Check the correctness.
	if (area == nullptr) {
		throw SQLiteException("Can't find the area " + ToString(areaVnum));
	}
	if (room == nullptr) {
		throw SQLiteException("Can't find the room " + ToString(roomVnum));
	}
	// Load the room inside the area.
	area->addRoom(room);
	if (!room->check(true)) {
		throw SQLiteException("Wrong room data.");
	}
	return true;
}

bool LoadWriting(ResultSet *result)
{
	// Create an empty Writing.
	auto writing = new Writing();
	// Initialize the Writing.
	writing->vnum = result->getNextUnsignedInteger();
	writing->title = result->getNextString();
	writing->author = result->getNextString();
	writing->content = result->getNextString();
	// Fid the item on which the writing is attached.
	auto item = Mud::instance().findItem(writing->vnum);
	if (item == nullptr) {
		throw SQLiteException("Can't find the item " + ToString(writing->vnum));
	}
	if (!Mud::instance().addWriting(writing)) {
		throw SQLiteException("Error during writing insertion.");
	}
	return true;
}

bool LoadMaterial(ResultSet *result)
{
	// Create an empty Material.
	auto material = new Material();
	// Intialize the material.
	material->vnum = result->getNextUnsignedInteger();
	material->type = MaterialType(result->getNextUnsignedInteger());
	material->name = result->getNextString();
	material->article = result->getNextString();
	material->worth = result->getNextDouble();
	material->hardness = result->getNextDouble();
	material->lightness = result->getNextDouble();
	// Check the correctness.
	if (!material->check()) {
		throw SQLiteException("Error during error checking.");
	}
	if (!Mud::instance().addMaterial(material)) {
		throw SQLiteException("Error during material insertion.");
	}
	return true;
}

bool LoadProfession(ResultSet *result)
{
	// Create an empty Profession.
	auto professions = new Profession();
	// Initialize the profession.
	professions->vnum = result->getNextUnsignedInteger();
	professions->command = result->getNextString();
	professions->action = result->getNextString();
	professions->description = result->getNextString();
	professions->startMessage = result->getNextString();
	professions->finishMessage = result->getNextString();
	professions->successMessage = result->getNextString();
	professions->failureMessage = result->getNextString();
	professions->interruptMessage = result->getNextString();
	professions->notFoundMessage = result->getNextString();
	// Check the correctness.
	if (!professions->check()) {
		throw SQLiteException("Error during error checking.");
	}
	if (!Mud::instance().addProfession(professions)) {
		throw SQLiteException("Error during professions insertion.");
	}
	return true;
}

bool LoadProduction(ResultSet *result)
{
	// Create an empty Production.
	auto production = new Production();
	// Initialize the Production.
	production->vnum = result->getNextUnsignedInteger();
	production->name = result->getNextString();
	auto professionVnum = result->getNextUnsignedInteger();
	auto profession = Mud::instance().findProfession(professionVnum);
	if (profession == nullptr) {
		throw SQLiteException("Can't find the profession " +
							  ToString(professionVnum));
	}
	production->profession = profession;
	production->difficulty = result->getNextUnsignedInteger();
	production->time = result->getNextDouble();
	production->assisted = static_cast<bool>(result->getNextInteger());
	auto workbenchValue = result->getNextUnsignedInteger();
	production->workbench = ToolType(workbenchValue);
	if (!Mud::instance().addProduction(production)) {
		throw SQLiteException("Error during production insertion.");
	}
	profession->productions.emplace_back(production);
	// Log it.
	MudLog(LogLevel::Debug, "\t%s%s", Align(production->name, align::left, 25),
		   Align(profession->command, align::left, 35));
	return true;
}

bool LoadProductionTool(ResultSet *result)
{
	auto productionVnum = result->getNextUnsignedInteger();
	auto production = Mud::instance().findProduction(productionVnum);
	if (production == nullptr) {
		throw SQLiteException("Can't find the production " +
							  ToString(productionVnum));
	}
	auto toolTypeNumber = result->getNextUnsignedInteger();
	auto toolType = ToolType(toolTypeNumber);
	if (toolType == ToolType::None) {
		throw SQLiteException("Can't find the tool type " +
							  ToString(toolTypeNumber));
	}
	production->tools.emplace_back(toolType);
	// Log it.
	MudLog(LogLevel::Debug, "\t%s%s", Align(production->name, align::left, 25),
		   Align(toolType.toString(), align::left, 35));
	return true;
}

bool LoadProductionOutcome(ResultSet *result)
{
	auto productionVnum = result->getNextUnsignedInteger();
	auto production = Mud::instance().findProduction(productionVnum);
	if (production == nullptr) {
		throw SQLiteException("Can't find the production " +
							  ToString(productionVnum));
	}
	auto outcomeVnum = result->getNextUnsignedInteger();
	auto outcome = Mud::instance().findItemModel(outcomeVnum);
	if (outcome == nullptr) {
		throw SQLiteException("Can't find the outcome " +
							  ToString(outcomeVnum));
	}
	production->outcome = outcome;
	production->quantity = result->getNextUnsignedInteger();
	// Log it.
	MudLog(LogLevel::Debug, "\t%s%s%s",
		   Align(production->name, align::left, 25),
		   Align(outcome->name, align::left, 35),
		   Align(production->quantity, align::left, 35));
	return true;
}

bool LoadProductionIngredient(ResultSet *result)
{
	auto productionVnum = result->getNextUnsignedInteger();
	auto production = Mud::instance().findProduction(productionVnum);
	if (production == nullptr) {
		throw SQLiteException("Can't find the production " +
							  ToString(productionVnum));
	}
	auto ingredientNumber = result->getNextUnsignedInteger();
	auto ingredient = ResourceType(ingredientNumber);
	if (ingredient == ResourceType::None) {
		throw SQLiteException("Can't find the ingredient " +
							  ToString(ingredientNumber));
	}
	auto quantity = result->getNextUnsignedInteger();
	if (quantity == 0) {
		throw SQLiteException("Wrong quantity " + ToString(quantity));
	}
	production->ingredients.insert(std::make_pair(ingredient, quantity));
	// Log it.
	MudLog(LogLevel::Debug, "\t%s%s%s",
		   Align(production->name, align::left, 25),
		   Align(ingredient.toString(), align::left, 35),
		   Align(quantity, align::left, 35));
	return true;
}

bool LoadProductionKnowledge(ResultSet *result)
{
	auto productionVnum = result->getNextUnsignedInteger();
	auto production = Mud::instance().findProduction(productionVnum);
	if (production == nullptr) {
		throw SQLiteException("Can't find the production " +
							  ToString(productionVnum));
	}
	auto knowledgeNumber = result->getNextUnsignedInteger();
	auto knowledge = Knowledge(knowledgeNumber);
	if (knowledge == ResourceType::None) {
		throw SQLiteException("Can't find the ingredient " +
							  ToString(knowledgeNumber));
	}
	production->requiredKnowledge.emplace_back(knowledge);
	// Log it.
	MudLog(LogLevel::Debug, "\t%s%s", Align(production->name, align::left, 25),
		   Align(knowledge.toString(), align::left, 35));
	return true;
}

bool LoadLiquid(ResultSet *result)
{
	// Create an empty Liquid.
	auto liquid = new Liquid();
	// Load the liquid.
	liquid->vnum = result->getNextUnsignedInteger();
	liquid->type = LiquidType(result->getNextUnsignedInteger());
	liquid->name = result->getNextString();
	liquid->description = result->getNextString();
	liquid->flags = result->getNextUnsignedInteger();
	liquid->worth = result->getNextInteger();
	liquid->quench = result->getNextDouble();
	// Check the correctness.
	if (!liquid->check()) {
		throw SQLiteException("Error during error checking.");
	}
	if (!Mud::instance().addLiquid(liquid)) {
		throw SQLiteException("Error during liquid insertion.");
	}
	return true;
}

bool LoadItemLiquidContent(ResultSet *result)
{
	auto containerVnum = result->getNextUnsignedInteger();
	auto container = Mud::instance().findItem(containerVnum);
	if (container == nullptr) {
		throw SQLiteException("Can't find the container " +
							  ToString(containerVnum));
	}
	if (container->getType() != ModelType::LiquidContainer) {
		throw SQLiteException("Wrong container for liquids.");
	}
	auto liquidVnum = result->getNextUnsignedInteger();
	auto liquid = Mud::instance().findLiquid(liquidVnum);
	if (liquid == nullptr) {
		throw SQLiteException("Can't find the liquid " + ToString(liquidVnum));
	}
	auto quantity = result->getNextDouble();
	// Cast the item to liquid container.
	auto liquidContainer = dynamic_cast<LiquidContainerItem *>(container);
	// Pour in the liquid.
	liquidContainer->pourIn(liquid, quantity, false);
	return true;
}

bool LoadTravelPoint(ResultSet *result)
{
	auto sourceArea =
		Mud::instance().findArea(result->getNextUnsignedInteger());
	if (sourceArea == nullptr) {
		throw SQLiteException("Can't find the source area.");
	}
	auto sourceRoom = sourceArea->getRoom(result->getNextUnsignedInteger());
	if (sourceRoom == nullptr) {
		throw SQLiteException("Can't find the source room.");
	}
	auto targetArea =
		Mud::instance().findArea(result->getNextUnsignedInteger());
	if (targetArea == nullptr) {
		throw SQLiteException("Can't find the target area.");
	}
	auto targetRoom = targetArea->getRoom(result->getNextUnsignedInteger());
	if (targetRoom == nullptr) {
		throw SQLiteException("Can't find the target room.");
	}
	if (!Mud::instance().addTravelPoint(sourceRoom, targetRoom)) {
		throw SQLiteException("Error during TravelPoint insertion.");
	}
	if (!Mud::instance().addTravelPoint(targetRoom, sourceRoom)) {
		throw SQLiteException("Error during TravelPoint insertion.");
	}
	SetFlag(sourceRoom->flags, RoomFlag::TravelPoint);
	SetFlag(targetRoom->flags, RoomFlag::TravelPoint);
	return true;
}

bool LoadBuilding(ResultSet *result)
{
	// Create an empty Building.
	auto building = std::make_shared<Building>();
	// Initialize the Production.
	building->vnum = result->getNextUnsignedInteger();
	building->name = result->getNextString();
	building->difficulty = result->getNextUnsignedInteger();
	building->time = result->getNextUnsignedInteger();
	building->assisted = static_cast<bool>(result->getNextInteger());
	auto modelVnum = result->getNextUnsignedInteger();
	building->buildingModel = Mud::instance().findItemModel(modelVnum);
	if (building->buildingModel == nullptr) {
		throw SQLiteException("Can't find the building model: " +
							  ToString(modelVnum));
	}
	if (!Mud::instance().addBuilding(building)) {
		throw SQLiteException("Error during building insertion.");
	}
	MudLog(LogLevel::Debug, "\t%s%s", Align(building->vnum, align::left, 25),
		   Align(building->name, align::left, 35));
	return true;
}

bool LoadBuildingTool(ResultSet *result)
{
	auto buildingVnum = result->getNextUnsignedInteger();
	auto building = Mud::instance().findBuilding(buildingVnum);
	if (building == nullptr) {
		throw SQLiteException("Can't find the building: " +
							  ToString(buildingVnum));
	}
	auto toolId = result->getNextUnsignedInteger();
	ToolType tool(toolId);
	if (tool == ToolType::None) {
		throw SQLiteException("Can't find the type of tool : " +
							  ToString(toolId));
	}
	building->tools.emplace_back(tool);
	MudLog(LogLevel::Debug, "\t%s%s", Align(building->name, align::left, 25),
		   Align(tool.toString(), align::left, 35));
	return true;
}

bool LoadBuildingIngredient(ResultSet *result)
{
	auto buildingVnum = result->getNextUnsignedInteger();
	auto building = Mud::instance().findBuilding(buildingVnum);
	if (building == nullptr) {
		throw SQLiteException("Can't find the building: " +
							  ToString(buildingVnum));
	}
	auto ingredientId = result->getNextUnsignedInteger();
	ResourceType ingredient(ingredientId);
	if (ingredient == ResourceType::None) {
		throw SQLiteException("Can't find the type of resource: " +
							  ToString(ingredientId));
	}
	auto quantity = result->getNextUnsignedInteger();
	building->ingredients.insert(std::make_pair(ingredient, quantity));
	MudLog(LogLevel::Debug, "\t%s%s%s", Align(building->name, align::left, 25),
		   Align(ingredient.toString(), align::left, 35),
		   Align(quantity, align::left, 35));
	return true;
}

bool LoadBuildingKnowledge(ResultSet *result)
{
	auto buildingVnum = result->getNextUnsignedInteger();
	auto building = Mud::instance().findBuilding(buildingVnum);
	if (building == nullptr) {
		throw SQLiteException("Can't find the building: " +
							  ToString(buildingVnum));
	}
	auto knowledgeId = result->getNextUnsignedInteger();
	Knowledge knowledge(knowledgeId);
	if (knowledge == Knowledge::None) {
		throw SQLiteException("Can't find the knowledge: " +
							  ToString(knowledgeId));
	}
	building->requiredKnowledge.emplace_back(knowledge);
	MudLog(LogLevel::Debug, "\t[%s] %s%s",
		   Align(building->vnum, align::left, 4),
		   Align(building->name, align::left, 25),
		   Align(knowledge.toString(), align::left, 35));
	return true;
}

bool LoadShop(ResultSet *result)
{
	// Retrieve the item vnum.
	auto vnum = result->getNextUnsignedInteger();
	auto item = Mud::instance().findItem(vnum);
	if (item == nullptr)
		throw SQLiteException("Can't find the item " + ToString(vnum));
	if (item->getType() != ModelType::Shop)
		throw SQLiteException("Wrong type of item " + ToString(vnum));
	// Cast the item to shop.
	auto shop = dynamic_cast<ShopItem *>(item);
	shop->shopName = result->getNextString();
	shop->shopBuyTax = result->getNextUnsignedInteger();
	shop->shopSellTax = result->getNextUnsignedInteger();
	shop->balance = result->getNextUnsignedInteger();
	shop->shopKeeper =
		Mud::instance().findMobile(result->getNextUnsignedInteger());
	shop->openingHour = result->getNextUnsignedInteger();
	shop->closingHour = result->getNextUnsignedInteger();
	if (shop->shopKeeper != nullptr)
		shop->shopKeeper->managedItem = shop;
	return true;
}

bool LoadShopDefaultStock(ResultSet *result)
{
	auto shopVnum = result->getNextUnsignedInteger();
	auto shopItem = Mud::instance().findItem(shopVnum);
	if (shopItem == nullptr) {
		throw SQLiteException("Wrong shop (" + ToString(shopVnum) + ")");
	}
	if (shopItem->getType() != ModelType::Shop) {
		throw SQLiteException("Wrong type (" + shopItem->getType().toString() +
							  ")");
	}
	auto shop = dynamic_cast<ShopItem *>(shopItem);
	auto modelVnum = result->getNextUnsignedInteger();
	auto model = Mud::instance().findItemModel(modelVnum);
	if (model == nullptr) {
		throw SQLiteException("Wrong model (" + ToString(modelVnum) + ")");
	}
	auto compositionVnum = result->getNextUnsignedInteger();
	auto composition = Mud::instance().findMaterial(compositionVnum);
	if (composition == nullptr) {
		throw SQLiteException("Wrong composition (" +
							  ToString(compositionVnum) + ")");
	}
	auto quality = ItemQuality(result->getNextUnsignedInteger());
	auto quantity = result->getNextUnsignedInteger();
	shop->addDefaultStock(model, composition, quality, quantity);
	return true;
}

bool LoadCurrency(ResultSet *result)
{
	// Retrieve the item vnum.
	auto modelVnum = result->getNextUnsignedInteger();
	auto materialVnum = result->getNextUnsignedInteger();
	auto worth = result->getNextUnsignedInteger();
	auto model = Mud::instance().findItemModel(modelVnum);
	if (model == nullptr) {
		throw SQLiteException("Can't find the model " + ToString(modelVnum));
	}
	if (model->getType() != ModelType::Currency) {
		throw SQLiteException("Wrong type of model " + ToString(modelVnum));
	}
	auto material = Mud::instance().findMaterial(materialVnum);
	if (material == nullptr) {
		throw SQLiteException("Can't find the material " +
							  ToString(materialVnum));
	}
	auto currency = model->to<CurrencyModel>();
	if (!currency->addPrice(materialVnum, worth)) {
		throw SQLiteException("Can't add the price for " + ToString(modelVnum));
	}
	return true;
}

bool LoadTerrain(ResultSet *result)
{
	auto terrain = std::make_shared<Terrain>();
	terrain->vnum = result->getNextUnsignedInteger();
	terrain->name = result->getNextString();
	terrain->flags = result->getNextUnsignedInteger();
	terrain->generationFlags = result->getNextUnsignedInteger();
	terrain->space = result->getNextUnsignedInteger();
	terrain->symbol = result->getNextString();
	if (!Mud::instance().addTerrain(terrain)) {
		throw SQLiteException("Can't add the terrain " +
							  ToString(terrain->vnum) + " - " + terrain->name);
	}
	MudLog(LogLevel::Debug, "\t%s%s", Align(terrain->vnum, align::left, 25),
		   Align(terrain->name, align::left, 25));
	return true;
}

bool LoadTerrainLiquid(ResultSet *result)
{
	auto terrainVnum = result->getNextUnsignedInteger();
	auto terrain = Mud::instance().findTerrain(terrainVnum);
	if (terrain == nullptr) {
		throw SQLiteException("Can't find the terrain " +
							  ToString(terrainVnum));
	}
	auto liquidVnum = result->getNextUnsignedInteger();
	auto liquid = Mud::instance().findLiquid(liquidVnum);
	if (liquid == nullptr) {
		throw SQLiteException("Can't find the liquid " + ToString(liquidVnum));
	}
	auto quantity = result->getNextUnsignedInteger();
	terrain->liquidContent = std::make_pair(liquid, quantity);
	MudLog(LogLevel::Debug, "\t%s%s", Align(terrain->name, align::left, 25),
		   Align(liquid->name, align::left, 25));
	return true;
}

bool LoadTerrainLiquidSources(ResultSet *result)
{
	auto terrainVnum = result->getNextUnsignedInteger();
	auto terrain = Mud::instance().findTerrain(terrainVnum);
	if (terrain == nullptr) {
		throw SQLiteException("Can't find the terrain " +
							  ToString(terrainVnum));
	}
	auto liquidVnum = result->getNextUnsignedInteger();
	auto liquid = Mud::instance().findLiquid(liquidVnum);
	if (liquid == nullptr) {
		throw SQLiteException("Can't find the liquid " + ToString(liquidVnum));
	}
	auto probability = result->getNextUnsignedInteger();
	// Add the liquid source.
	terrain->addLiquidSource(liquid, probability);
	MudLog(LogLevel::Debug, "\t%s%s%s", Align(terrain->name, align::left, 25),
		   Align(liquid->name, align::left, 25),
		   Align(probability, align::left, 25));
	return true;
}

bool LoadBodyPart(ResultSet *result)
{
	std::shared_ptr<BodyPart> bodyPart = std::make_shared<BodyPart>();
	bodyPart->vnum = result->getNextUnsignedInteger();
	bodyPart->name = result->getNextString();
	bodyPart->description = result->getNextString();
	bodyPart->flags = result->getNextUnsignedInteger();
	if (!Mud::instance().addBodyPart(bodyPart)) {
		throw SQLiteException("Can't add the body part" + bodyPart->name);
	}
	return true;
}

bool LoadBodyPartResources(ResultSet *result)
{
	auto bodyPartVnum = result->getNextUnsignedInteger();
	auto bodyPart = Mud::instance().findBodyPart(bodyPartVnum);
	if (bodyPart == nullptr) {
		throw SQLiteException("Can't find the body part " +
							  ToString(bodyPartVnum));
	}
	auto modelVnum = result->getNextUnsignedInteger();
	auto model = Mud::instance().findItemModel(modelVnum);
	if (model == nullptr) {
		throw SQLiteException("Can't find the model " + ToString(modelVnum));
	}
	if (model->getType() != ModelType::Resource) {
		throw SQLiteException("The model is not a resource " +
							  ToString(modelVnum));
	}
	auto materialVnum = result->getNextUnsignedInteger();
	auto material = Mud::instance().findMaterial(materialVnum);
	if (material == nullptr) {
		throw SQLiteException("Can't find the material " +
							  ToString(materialVnum));
	}
	auto quantity = result->getNextInteger();
	auto difficulty = result->getNextInteger();
	BodyPart::BodyResource resource;
	resource.material = material;
	resource.resource = model->to<ResourceModel>();
	resource.quantity = quantity;
	resource.difficulty = difficulty;
	bodyPart->resources.emplace_back(std::move(resource));
	return true;
}

bool LoadBodyPartWeapon(ResultSet *result)
{
	auto bodyPartVnum = result->getNextUnsignedInteger();
	auto bodyPart = Mud::instance().findBodyPart(bodyPartVnum);
	if (bodyPart == nullptr) {
		throw SQLiteException("Can't add the body part" +
							  ToString(bodyPartVnum));
	}
	auto bodyWeapon = std::make_shared<BodyPart::BodyWeapon>();
	bodyWeapon->name = result->getNextString();
	bodyWeapon->article = result->getNextString();
	bodyWeapon->minDamage = result->getNextUnsignedInteger();
	bodyWeapon->maxDamage = result->getNextUnsignedInteger();
	bodyWeapon->range = result->getNextInteger();
	bodyPart->weapon = bodyWeapon;
	return true;
}

bool LoadModelBodyPart(ResultSet *result)
{
	auto modelVnum = result->getNextUnsignedInteger();
	auto model = Mud::instance().findItemModel(modelVnum);
	if (model == nullptr)
		throw SQLiteException("Can't find the model " + ToString(modelVnum));
	auto bodyPartVnum = result->getNextUnsignedInteger();
	auto bodyPart = Mud::instance().findBodyPart(bodyPartVnum);
	if (bodyPart == nullptr)
		throw SQLiteException("Can't find the body part " +
							  ToString(bodyPartVnum));
	model->bodyParts.emplace_back(bodyPart);
	return true;
}
