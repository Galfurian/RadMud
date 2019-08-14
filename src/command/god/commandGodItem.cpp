/// @file   commandGodItem.cpp
/// @author Enrico Fraccaroli
/// @date   Jan 02 2017
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

#include "commandGodItem.hpp"
#include "mud.hpp"

bool DoItemCreate(Character *character, ArgumentHandler &args)
{
	// Stop any ongoing action.
	StopAction(character);
	// Get the number of arguments.
	auto argv(args.size());
	// If no argument is provided show the list of models.
	if (argv == 0) {
		character->sendMsg("What do you want to create?\n");
		DoModelList(character, args);
		return false;
	}
	// Get the quantity.
	auto quantity = args[0].getMultiplier();
	// Get the model.
	auto modeVnum = ToNumber<unsigned int>(args[0].getContent());
	auto itemModel = Mud::instance().findItemModel(modeVnum);
	if (itemModel == nullptr) {
		character->sendMsg("Cannot find model '%s'.\n", args[0].getContent());
		return false;
	}
	// If no more arguments are provided, show the materials for the model.
	if (argv == 1) {
		character->sendMsg("You can make %s out of:\n", itemModel->getName());
		for (auto const &it : Mud::instance().mudMaterials) {
			if (it.second->type == itemModel->material) {
				character->sendMsg("    [%s] %s\n",
								   Align(it.first, align::right, 4),
								   it.second->name);
			}
		}
		return false;
	}
	// Get the material.
	auto materialVnum = ToNumber<unsigned int>(args[1].getContent());
	auto material = Mud::instance().findMaterial(materialVnum);
	if (material == nullptr) {
		character->sendMsg("Cannot find material '%s'.\n",
						   args[1].getContent());
		return false;
	}
	// Get the quality.
	auto quality = ItemQuality(ItemQuality::Normal);
	if (argv >= 3) {
		quality = ItemQuality(ToNumber<unsigned int>(args[2].getContent()));
		if (quality == ItemQuality::None) {
			character->sendMsg("Not a valid quality.\n");
			return false;
		}
	}
	// Create the item.
	auto item = itemModel->createItem(character->getName(), material, false,
									  quality, quantity);
	if (item == nullptr) {
		character->sendMsg("Creation failed.\n");
		return false;
	}
	character->addInventoryItem(item);
	character->sendMsg(
		"You produce '%s' out of your apparently empty top hat.\n",
		item->getName(true));
	return true;
}

bool DoItemDestroy(Character *character, ArgumentHandler &args)
{
	if (args.size() != 1) {
		character->sendMsg("You must instert an item vnum.\n");
		return false;
	}
	auto itemVnum = ToNumber<unsigned int>(args[0].getContent());
	auto item = Mud::instance().findItem(itemVnum);
	if (item == nullptr) {
		character->sendMsg("Invalid vnum (%s).\n", ToString(itemVnum));
		return false;
	}
	if (!item->isEmpty()) {
		character->sendMsg("You cannot destroy a non-empty item.\n");
		return false;
	}
	character->sendMsg("You have destroyed the desired object.\n");
	MudUpdater::instance().addItemToDestroy(item);
	return true;
}

bool DoItemGet(Character *character, ArgumentHandler &args)
{
	if (args.size() != 1) {
		character->sendMsg("You must instert an item vnum.\n");
		return false;
	}
	auto itemVnum = ToNumber<unsigned int>(args[0].getContent());
	auto item = Mud::instance().findItem(itemVnum);
	if (item == nullptr) {
		character->sendMsg("Invalid vnum (%s).\n", ToString(itemVnum));
		return false;
	}
	// Check if the God player can carry the item.
	if (!character->canCarry(item, item->quantity)) {
		character->sendMsg("You can't carry anymore items.\n");
		return false;
	}
	if (item->room != nullptr) {
		character->sendMsg("The item was inside the room '%s' (%s)\n",
						   item->room->name, ToString(item->room->vnum));
		item->room->removeItem(item);
	} else if (item->owner != nullptr) {
		character->sendMsg("The item was possessed by '%s'\n",
						   item->owner->getName());
		if (!character->remEquipmentItem(item)) {
			if (!character->remInventoryItem(item)) {
				character->sendMsg("Cannot take the item from the owner!");
				return false;
			}
		}
	} else if (item->container != nullptr) {
		character->sendMsg("The item was inside the container '%s'\n",
						   item->container->getName(true));
		item->container->takeOut(item);
	} else {
		character->sendMsg("The item was nowhere!");
	}
	character->sendMsg("You materialize the desired object in your hands.\n");
	character->addInventoryItem(item);
	return true;
}

bool DoItemInfo(Character *character, ArgumentHandler &args)
{
	if (args.size() != 1) {
		character->sendMsg("You must insert the item vnum or the name"
						   " of the item inside the room.\n");
		return false;
	}
	auto itemVnum = ToNumber<unsigned int>(args[0].getContent());
	auto item = Mud::instance().findItem(itemVnum);
	if (item == nullptr) {
		item =
			character->findNearbyItem(args[0].getContent(), args[0].getIndex());
		if (item == nullptr) {
			character->sendMsg("Cannot find the target item.\n");
			return false;
		}
	}
	// Create a table.
	Table sheet;
	sheet.addHeader(item->getNameCapital());
	// Get the sheet.
	item->getSheet(sheet);
	// Show the seet to character.
	character->sendMsg(sheet.getTable());
	return true;
}

bool DoItemList(Character *character, ArgumentHandler &args)
{
	// Variables used to filter the listed items.
	std::string itemName;
	std::string typeName;
	unsigned int modelVnum = 0;
	for (size_t argIt = 0; argIt < args.size(); ++argIt) {
		if (args[argIt].getContent() == "--help") {
			std::string help = "Usage:\n";
			help += "    item_list [options]\n";
			help += "Options:\n";
			help += "    -n [string]     Search items with the name which";
			help += " contains the given string.\n";
			help += "    -t [type_name]  Search items of the given type.\n";
			help += "    -m [model_vnum] Search the items of the model";
			help += " having the given vnum.\n";
			character->sendMsg(help);
			return true;
		}
		if ((argIt + 1) < args.size()) {
			if (args[argIt].getContent() == "-n") {
				itemName = args[argIt + 1].getContent();
			}
			if (args[argIt].getContent() == "-t") {
				typeName = args[argIt + 1].getContent();
			}
			if (args[argIt].getContent() == "-m") {
				modelVnum =
					ToNumber<unsigned int>(args[argIt + 1].getContent());
			}
		}
	}
	Table table;
	table.addColumn("Vnum", align::right);
	table.addColumn("Name", align::left);
	table.addColumn("Type", align::left);
	table.addColumn("Model", align::left);
	table.addColumn("Location", align::left);
	for (auto iterator : Mud::instance().mudItems) {
		auto item = iterator.second;
		if (!itemName.empty()) {
			if (item->getName(false).find(itemName) == std::string::npos) {
				continue;
			}
		}
		if (!typeName.empty()) {
			if (!BeginWith(ToLower(item->getTypeName()), ToLower(typeName))) {
				continue;
			}
		}
		if (modelVnum != 0) {
			if (item->model->vnum != modelVnum)
				continue;
		}
		// Prepare the row.
		TableRow row;
		row.emplace_back(ToString(item->vnum));
		row.emplace_back(item->getNameCapital());
		row.emplace_back(item->getTypeName());
		row.emplace_back(ToString(item->model->vnum));
		if (item->owner != nullptr) {
			row.emplace_back(" Owner  : " + item->owner->getName());
		} else if (item->room != nullptr) {
			row.emplace_back(" Room   : " + ToString(item->room->vnum));
		} else if (item->container != nullptr) {
			row.emplace_back(" Inside : " + ToString(item->container->vnum));
		} else {
			row.emplace_back(" Is nowhere.");
		}
		// Add the row to the table.
		table.addRow(row);
	}
	character->sendMsg(table.getTable());
	return true;
}

bool DoModelInfo(Character *character, ArgumentHandler &args)
{
	if (args.size() != 1) {
		character->sendMsg("You must insert a model vnum.\n");
		return false;
	}
	auto modelVnum = ToNumber<unsigned int>(args[0].getContent());
	auto itemModel = Mud::instance().findItemModel(modelVnum);
	if (itemModel == nullptr) {
		character->sendMsg("Item model not found %s.\n", modelVnum);
		return false;
	}
	// Create a table.
	Table sheet;
	// Get the sheet.
	itemModel->getSheet(sheet);
	// Show the seet to character.
	character->sendMsg(sheet.getTable());
	return true;
}

bool DoModelList(Character *character, ArgumentHandler &args)
{
	// Variables used to filter the listed items.
	std::string modelName;
	std::string modelType;
	unsigned int modelVnum = 0;
	for (size_t argIt = 0; argIt < args.size(); ++argIt) {
		if (args[argIt].getContent() == "--help") {
			std::string help = "Usage:\n";
			help += "    model_list [options]\n";
			help += "Options:\n";
			help += "    -n [string]     Search models with the name which";
			help += " contains the given string.\n";
			help += "    -t [type_name]  Search models of the given type.\n";
			help += "    -m [model_vnum] Search models with the given vnum.\n";
			character->sendMsg(help);
			return true;
		}
		if ((argIt + 1) < args.size()) {
			if (args[argIt].getContent() == "-n") {
				modelName = args[argIt + 1].getContent();
			}
			if (args[argIt].getContent() == "-t") {
				modelType = args[argIt + 1].getContent();
			}
			if (args[argIt].getContent() == "-m") {
				modelVnum =
					ToNumber<unsigned int>(args[argIt + 1].getContent());
			}
		}
	}
	Table table;
	table.addColumn("VNUM", align::right);
	table.addColumn("NAME", align::left);
	table.addColumn("TYPE", align::left);
	table.addColumn("FLAGS", align::right);
	for (auto iterator : Mud::instance().mudItemModels) {
		auto itemModel = iterator.second;
		if (!modelName.empty()) {
			if (itemModel->name.find(modelName) == std::string::npos) {
				continue;
			}
		}
		if (!modelType.empty()) {
			if (!BeginWith(ToLower(itemModel->getTypeName()),
						   ToLower(modelType))) {
				continue;
			}
		}
		if (modelVnum != 0) {
			if (itemModel->vnum != modelVnum)
				continue;
		}
		// Prepare the row.
		TableRow row;
		row.emplace_back(ToString(itemModel->vnum));
		row.emplace_back(itemModel->name);
		row.emplace_back(itemModel->getTypeName());
		row.emplace_back(ToString(itemModel->modelFlags));
		// Add the row to the table.
		table.addRow(row);
	}
	character->sendMsg(table.getTable());
	return true;
}

bool DoWritingList(Character *character, ArgumentHandler & /*args*/)
{
	Table table;
	table.addColumn("VNUM", align::center);
	table.addColumn("AUTHOR", align::left);
	table.addColumn("TITLE", align::left);
	for (auto iterator : Mud::instance().mudWritings) {
		Writing *writing = iterator.second;
		// Prepare the row.
		TableRow row;
		row.emplace_back(ToString(writing->vnum));
		row.emplace_back(writing->author);
		row.emplace_back(writing->title);
		// Add the row to the table.
		table.addRow(row);
	}
	character->sendMsg(table.getTable());
	return true;
}
