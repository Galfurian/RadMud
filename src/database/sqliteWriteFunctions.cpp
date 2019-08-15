/// @file   sqliteWriteFunctions.cpp
/// @author Enrico Fraccaroli
/// @date   feb 05 2017
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

#include "sqliteWriteFunctions.hpp"
#include "sqliteDbms.hpp"
#include "player.hpp"
#include "area.hpp"
#include "room.hpp"

bool SavePlayer(Player *player)
{
	std::vector<std::string> args;
	// Prepare the arguments of the query.
	args.push_back(player->name);
	args.push_back(player->password);
	args.push_back(ToString(player->race->vnum));
	args.push_back(ToString(player->getAbility(Ability::Strength, false)));
	args.push_back(ToString(player->getAbility(Ability::Agility, false)));
	args.push_back(ToString(player->getAbility(Ability::Perception, false)));
	args.push_back(ToString(player->getAbility(Ability::Constitution, false)));
	args.push_back(ToString(player->getAbility(Ability::Intelligence, false)));
	args.push_back(ToString(static_cast<int>(player->gender)));
	args.push_back(ToString(player->age));
	args.push_back(player->description);
	args.push_back(ToString(player->weight));
	args.push_back(ToString(player->faction->vnum));
	args.push_back(ToString(player->level));
	args.push_back(ToString(player->experience));
	args.push_back(ToString(player->room->vnum));
	args.push_back(player->prompt);
	args.push_back(ToString(player->flags));
	args.push_back(ToString(player->health));
	args.push_back(ToString(player->stamina));
	args.push_back(ToString(player->hunger));
	args.push_back(ToString(player->thirst));
	args.push_back(ToString(player->rent_room));
	// Start a transaction.
	if (!SQLiteDbms::instance().insertInto("Player", args, false, true)) {
		return false;
	}
	return true;
}

bool SavePlayerSkills(Player *player)
{
	for (const auto &skillData : player->skillManager.skills) {
		std::vector<std::string> args;
		args.push_back(player->name);
		args.push_back(ToString(skillData->skillVnum));
		args.push_back(ToString(skillData->skillLevel));
		if (!SQLiteDbms::instance().insertInto("PlayerSkill", args, false,
											   true)) {
			return false;
		}
	}
	return true;
}

bool SavePlayerLuaVariables(Player *player)
{
	// Prepare the arguments of the query for lua variables table.
	for (auto iterator : player->luaVariables) {
		std::vector<std::string> args;
		args.push_back(player->name);
		args.push_back(iterator.first);
		args.push_back(iterator.second);
		if (!SQLiteDbms::instance().insertInto("PlayerVariable", args, false,
											   true)) {
			return false;
		}
	}
	return true;
}

bool SaveItemPlayer(Player *player, Item *item,
					const unsigned int &bodyPartVnum)
{
	std::vector<std::string> args;
	args.emplace_back(player->name);
	args.emplace_back(ToString(item->vnum));
	args.emplace_back(ToString(bodyPartVnum));
	// Start a transaction.
	SQLiteDbms::instance().beginTransaction();
	if (!SQLiteDbms::instance().insertInto("ItemPlayer", args, false, true)) {
		SQLiteDbms::instance().rollbackTransection();
		return false;
	}
	SQLiteDbms::instance().endTransaction();
	return true;
}

bool SaveShopItem(ShopItem *item, const bool &transaction)
{
	// Prepare the vector used to insert into the database.
	std::vector<std::string> args;
	args.push_back(ToString(item->vnum));
	args.push_back(item->shopName);
	args.push_back(ToString(item->shopBuyTax));
	args.push_back(ToString(item->shopSellTax));
	args.push_back(ToString(item->balance));
	if (item->shopKeeper != nullptr) {
		args.push_back(ToString(item->shopKeeper->vnum));
	} else {
		args.push_back("");
	}
	args.push_back(ToString(item->openingHour));
	args.push_back(ToString(item->closingHour));
	if (!transaction) {
		return SQLiteDbms::instance().insertInto("Shop", args, false, true);
	}
	SQLiteDbms::instance().beginTransaction();
	if (!SQLiteDbms::instance().insertInto("Shop", args, false, true)) {
		SQLiteDbms::instance().rollbackTransection();
		return false;
	}
	SQLiteDbms::instance().endTransaction();
	return true;
}

bool SaveItem(Item *item, const bool &transaction)
{
	// Prepare the vector used to insert into the database.
	std::vector<std::string> args;
	args.push_back(ToString(item->vnum));
	args.push_back(ToString(item->model->vnum));
	args.push_back(ToString(item->quantity));
	args.push_back(item->maker);
	args.push_back(ToString(item->condition));
	args.push_back(ToString(item->composition->vnum));
	args.push_back(ToString(item->quality.toUInt()));
	args.push_back(ToString(item->flags));
	if (!transaction) {
		return SQLiteDbms::instance().insertInto("Item", args, false, true);
	}
	SQLiteDbms::instance().beginTransaction();
	if (!SQLiteDbms::instance().insertInto("Item", args, false, true)) {
		SQLiteDbms::instance().rollbackTransection();
		return false;
	}
	SQLiteDbms::instance().endTransaction();
	return true;
}

bool SaveArea(Area *area)
{
	std::vector<std::string> args;
	args.push_back(ToString(area->vnum));
	args.push_back(area->name);
	args.push_back(area->builder);
	args.push_back(ToString(area->width));
	args.push_back(ToString(area->height));
	args.push_back(ToString(area->elevation));
	args.push_back("0");
	args.push_back(ToString(static_cast<unsigned int>(area->type)));
	args.push_back(ToString(static_cast<unsigned int>(area->status)));
	// Start a transaction.
	SQLiteDbms::instance().beginTransaction();
	if (!SQLiteDbms::instance().insertInto("Area", args, false, true)) {
		MudLog(LogLevel::Error, "I was not able to save the area.");
		SQLiteDbms::instance().rollbackTransection();
		return false;
	}
	SQLiteDbms::instance().endTransaction();
	return true;
}

bool SaveRoom(Room *room)
{
	// Insert into table Room the newly created room.
	std::vector<std::string> args;
	args.push_back(ToString(room->vnum));
	args.push_back(ToString(room->coord.x));
	args.push_back(ToString(room->coord.y));
	args.push_back(ToString(room->coord.z));
	args.push_back(ToString(room->terrain->vnum));
	args.push_back(room->name);
	args.push_back(room->description);
	args.push_back(ToString(room->flags));
	// Start a transaction.
	SQLiteDbms::instance().beginTransaction();
	if (!SQLiteDbms::instance().insertInto("Room", args, false, true)) {
		MudLog(LogLevel::Error, "I was not able to save the room.");
		SQLiteDbms::instance().rollbackTransection();
		return false;
	}
	SQLiteDbms::instance().endTransaction();
	return true;
}

bool SaveAreaList(Area *area, Room *room)
{
	// Insert Room in AreaList.
	std::vector<std::string> args;
	args.push_back(ToString(area->vnum));
	args.push_back(ToString(room->vnum));
	// Start a transaction.
	SQLiteDbms::instance().beginTransaction();
	if (!SQLiteDbms::instance().insertInto("AreaList", args, false, true)) {
		MudLog(LogLevel::Error, "I was not able to save the area list.");
		SQLiteDbms::instance().rollbackTransection();
		return false;
	}
	SQLiteDbms::instance().endTransaction();
	return true;
}

bool SaveRoomExit(const std::shared_ptr<Exit> &roomExit)
{
	// Update the values on Database.
	std::vector<std::string> args;
	args.push_back(ToString(roomExit->source->vnum));
	args.push_back(ToString(roomExit->destination->vnum));
	args.push_back(ToString(roomExit->direction.toUInt()));
	args.push_back(ToString(roomExit->flags));
	// Start a transaction.
	SQLiteDbms::instance().beginTransaction();
	if (!SQLiteDbms::instance().insertInto("Exit", args, false, true)) {
		MudLog(LogLevel::Error, "I was not able to save the exit.");
		SQLiteDbms::instance().rollbackTransection();
		return false;
	}
	SQLiteDbms::instance().endTransaction();
	return true;
}