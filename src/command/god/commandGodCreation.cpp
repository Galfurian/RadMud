/// @file   commandGodCreation.cpp
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

#include "commandGodCreation.hpp"
#include "toolModel.hpp"
#include "mud.hpp"

bool DoMaterialInfo(Character *character, ArgumentHandler &args)
{
	if (args.size() != 1) {
		character->sendMsg("You must insert a valide material vnum.\n");
		return false;
	}
	auto materialVnum = ToNumber<unsigned int>(args[0].getContent());
	auto material = Mud::instance().findMaterial(materialVnum);
	if (material == nullptr) {
		character->sendMsg("Can't find the desire material %s.\n",
						   materialVnum);
		return false;
	}
	std::string msg;
	msg += "Vnum      : " + ToString(material->vnum) + "\n";
	msg += "Type      : " + material->type.toString() + "\n";
	msg += "Name      : " + material->name + "\n";
	msg += "Worth     : " + ToString(material->worth) + "\n";
	msg += "Hardness  : " + ToString(material->hardness) + "\n";
	msg += "Lightness : " + ToString(material->lightness) + "\n";
	character->sendMsg(msg);
	return true;
}

bool DoMaterialList(Character *character, ArgumentHandler & /*args*/)
{
	Table table;
	table.addColumn("VNUM", align::right);
	table.addColumn("TYPE", align::center);
	table.addColumn("NAME", align::left);
	table.addColumn("WORTH", align::right);
	table.addColumn("HARDNESS", align::right);
	table.addColumn("LIGHTNESS", align::right);
	for (auto iterator : Mud::instance().mudMaterials) {
		// Prepare the row.
		TableRow row;
		row.emplace_back(ToString(iterator.second->vnum));
		row.emplace_back(iterator.second->type.toString());
		row.emplace_back(iterator.second->name);
		row.emplace_back(ToString(iterator.second->worth));
		row.emplace_back(ToString(iterator.second->hardness));
		row.emplace_back(ToString(iterator.second->lightness));
		// Add the row to the table.
		table.addRow(row);
	}
	character->sendMsg(table.getTable());
	return true;
}

bool DoBuildingInfo(Character *character, ArgumentHandler &args)
{
	if (args.size() != 1) {
		character->sendMsg("You must provide a building vnum.\n");
		return false;
	}
	auto buildingVnum = ToNumber<unsigned int>(args[0].getContent());
	auto building = Mud::instance().findBuilding(buildingVnum);
	if (building == nullptr) {
		character->sendMsg("Can't find the desire building %s.\n",
						   buildingVnum);
		return false;
	}
	std::string msg;
	msg += "Vnum        : " + ToString(building->vnum) + "\n";
	msg += "Name        : " + building->name + "\n";
	msg += "Difficulty  : " + ToString(building->difficulty) + "\n";
	msg += "Time        : " + ToString(building->time) + "\n";
	msg += "Assisted    : " + ToString(building->assisted) + "\n";
	msg += "Tools       :\n";
	for (auto const &iterator : building->tools) {
		msg += "                  " + iterator.toString() + "\n";
	}
	msg += "Building    : " + building->buildingModel->name + "\n";
	msg += "Ingredients :\n";
	for (auto const &iterator : building->ingredients) {
		msg += "    " + iterator.first.toString();
		msg += "(" + ToString(iterator.second) + ")\n";
	}
	character->sendMsg(msg);
	return true;
}

bool DoBuildingList(Character *character, ArgumentHandler & /*args*/)
{
	Table table;
	table.addColumn("VNUM", align::center);
	table.addColumn("NAME", align::center);
	table.addColumn("DIFFICULTY", align::left);
	table.addColumn("TIME", align::center);
	for (auto it : Mud::instance().mudBuildings) {
		// Prepare the row.
		TableRow row;
		row.emplace_back(ToString(it.second->vnum));
		row.emplace_back(it.second->name);
		row.emplace_back(ToString(it.second->difficulty));
		row.emplace_back(ToString(it.second->time));
		// Add the row to the table.
		table.addRow(row);
	}
	character->sendMsg(table.getTable());
	return true;
}

bool DoProfessionInfo(Character *character, ArgumentHandler &args)
{
	if (args.size() != 1) {
		character->sendMsg("You must provide a profession name.\n");
		return false;
	}
	auto profession = Mud::instance().findProfession(args[0].getContent());
	if (profession == nullptr) {
		character->sendMsg("Can't find the desire profession.\n");
		return false;
	}
	std::string msg;
	msg += "Description   : " + profession->description + "\n";
	msg += "Command       : " + profession->command + "\n";
	msg += "Action        : " + profession->action + "\n";
	msg += "    Start     : " + profession->startMessage + "\n";
	msg += "    Finish    : " + profession->finishMessage + "\n";
	msg += "    Success   : " + profession->successMessage + "\n";
	msg += "    Failure   : " + profession->failureMessage + "\n";
	msg += "    Interrupt : " + profession->interruptMessage + "\n";
	msg += "    Not Found : " + profession->notFoundMessage + "\n";
	character->sendMsg(msg);
	return true;
}

bool DoProfessionList(Character *character, ArgumentHandler & /*args*/)
{
	Table table;
	table.addColumn("COMMAND", align::center);
	table.addColumn("ACTION", align::center);
	for (auto iterator : Mud::instance().mudProfessions) {
		// Prepare the row.
		TableRow row;
		row.emplace_back(iterator.second->command);
		row.emplace_back(iterator.second->action);
		// Add the row to the table.
		table.addRow(row);
	}
	character->sendMsg(table.getTable());
	return true;
}

bool DoProductionInfo(Character *character, ArgumentHandler &args)
{
	if (args.size() != 1) {
		character->sendMsg("You must provide a production vnum.\n");
		return false;
	}
	auto productionVnum = ToNumber<unsigned int>(args[0].getContent());
	auto production = Mud::instance().findProduction(productionVnum);
	if (production == nullptr) {
		character->sendMsg("Can't find the desire production %s.\n",
						   productionVnum);
		return false;
	}
	std::string msg;
	msg += "Vnum        : " + ToString(production->vnum) + "\n";
	msg += "Name        : " + production->name + "\n";
	msg += "Profession  : " + production->profession->command + "\n";
	msg += "Difficulty  : " + ToString(production->difficulty) + "\n";
	msg += "Time        : " + ToString(production->time) + "\n";
	msg += "Assisted    : " + ToString(production->assisted) + "\n";
	msg += "Outcome     : " + production->outcome->name + "*";
	msg += ToString(production->quantity) + "\n";
	msg += "Tools       :\n";
	for (auto const &iterator : production->tools) {
		msg += "                  " + iterator.toString() + "\n";
	}
	msg += "Ingredients :\n";
	for (auto const &iterator : production->ingredients) {
		msg += "    " + iterator.first.toString();
		msg += "(" + ToString(iterator.second) + ")\n";
	}
	msg += "Workbench   :" + production->workbench.toString() + "\n";
	character->sendMsg(msg);
	return true;
}

bool DoProductionList(Character *character, ArgumentHandler & /*args*/)
{
	Table table;
	table.addColumn("VNUM", align::center);
	table.addColumn("NAME", align::center);
	table.addColumn("PROFESSION", align::center);
	table.addColumn("DIFFICULTY", align::left);
	for (auto iterator : Mud::instance().mudProductions) {
		// Prepare the row.
		TableRow row;
		row.emplace_back(ToString(iterator.second->vnum));
		row.emplace_back(iterator.second->name);
		row.emplace_back(iterator.second->profession->command);
		row.emplace_back(ToString(iterator.second->difficulty));
		// Add the row to the table.
		table.addRow(row);
	}
	character->sendMsg(table.getTable());
	return true;
}

bool DoBodyPartList(Character *character, ArgumentHandler &)
{
	Table table;
	table.addColumn("VNUM", align::center);
	table.addColumn("NAME", align::left);
	for (auto it : Mud::instance().mudBodyParts) {
		// Prepare the row.
		TableRow row;
		row.emplace_back(ToString(it.first));
		row.emplace_back(it.second->name);
		// Add the row to the table.
		table.addRow(row);
	}
	character->sendMsg(table.getTable());
	return true;
}

bool DoBodyPartInfo(Character *character, ArgumentHandler &args)
{
	if (args.size() != 1) {
		character->sendMsg("You must insert a valid body part vnum.\n");
		return false;
	}
	auto vnum = ToNumber<unsigned int>(args[0].getContent());
	auto bodyPart = Mud::instance().findBodyPart(vnum);
	if (bodyPart == nullptr) {
		character->sendMsg("Can't find the desire body part %s.\n", vnum);
		return false;
	}
	Table table;
	bodyPart->getSheet(table);
	character->sendMsg(table.getTable());
	return true;
}
