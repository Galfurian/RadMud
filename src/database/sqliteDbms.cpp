/// @file   sqliteDbms.cpp
/// @brief  It's manage action to Database.
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

#include "sqliteDbms.hpp"

#include "sqliteLoadFunctions.hpp"
#include "logger.hpp"
#include "mud.hpp"
#include "sqliteException.hpp"

SQLiteDbms::SQLiteDbms() : dbConnection(), loaders()
{
	loaders.emplace_back(TableLoader("BadName", LoadBadName));
	loaders.emplace_back(TableLoader("BlockedIp", LoadBlockedIp));
	loaders.emplace_back(TableLoader("News", LoadNews));
	loaders.emplace_back(TableLoader("Terrain", LoadTerrain));
	loaders.emplace_back(TableLoader("Material", LoadMaterial));
	loaders.emplace_back(TableLoader("Area", LoadArea));
	loaders.emplace_back(TableLoader("Room", LoadRoom));
	loaders.emplace_back(TableLoader("Exit", LoadExit));
	loaders.emplace_back(TableLoader("AreaList", LoadAreaList));
	loaders.emplace_back(TableLoader("TravelPoint", LoadTravelPoint));
	loaders.emplace_back(TableLoader("Skill", LoadSkill));
	loaders.emplace_back(
		TableLoader("SkillPrerequisite", LoadSkillPrerequisite));
	loaders.emplace_back(
		TableLoader("SkillAbilityModifier", LoadSkillAbilityModifier));
	loaders.emplace_back(
		TableLoader("SkillStatusModifier", LoadSkillStatusModifier));
	loaders.emplace_back(
		TableLoader("SkillCombatModifier", LoadSkillCombatModifier));
	loaders.emplace_back(TableLoader("SkillKnowledge", LoadSkillKnowledge));
	loaders.emplace_back(TableLoader("BodyPart", LoadBodyPart));
	loaders.emplace_back(TableLoader("Model", LoadModel));
	loaders.emplace_back(TableLoader("Faction", LoadFaction));
	loaders.emplace_back(TableLoader("Race", LoadRace));
	loaders.emplace_back(TableLoader("RaceBaseSkill", LoadRaceBaseSkill));
	loaders.emplace_back(TableLoader("RaceBodyPart", LoadRaceBodyPart));
	loaders.emplace_back(TableLoader("Liquid", LoadLiquid));
	loaders.emplace_back(TableLoader("Item", LoadItem));
	loaders.emplace_back(TableLoader("ItemContent", LoadItemContent));
	loaders.emplace_back(
		TableLoader("ItemLiquidContent", LoadItemLiquidContent));
	loaders.emplace_back(TableLoader("ItemRoom", LoadItemRoom));
	loaders.emplace_back(TableLoader("Writings", LoadWriting));
	loaders.emplace_back(TableLoader("Profession", LoadProfession));
	loaders.emplace_back(TableLoader("Production", LoadProduction));
	loaders.emplace_back(TableLoader("ProductionTool", LoadProductionTool));
	loaders.emplace_back(
		TableLoader("ProductionOutcome", LoadProductionOutcome));
	loaders.emplace_back(
		TableLoader("ProductionIngredient", LoadProductionIngredient));
	loaders.emplace_back(
		TableLoader("ProductionKnowledge", LoadProductionKnowledge));
	loaders.emplace_back(TableLoader("Currency", LoadCurrency));
	loaders.emplace_back(TableLoader("MobileModel", LoadMobileModel));
	loaders.emplace_back(TableLoader("MobileSpawn", LoadMobileSpawn));
	loaders.emplace_back(
		TableLoader("BodyPartResources", LoadBodyPartResources));
	loaders.emplace_back(TableLoader("BodyPartWeapon", LoadBodyPartWeapon));
	loaders.emplace_back(TableLoader("TerrainLiquid", LoadTerrainLiquid));
	loaders.emplace_back(
		TableLoader("TerrainLiquidSources", LoadTerrainLiquidSources));
	loaders.emplace_back(TableLoader("RoomLiquid", LoadRoomLiquid));
	loaders.emplace_back(TableLoader("Building", LoadBuilding));
	loaders.emplace_back(
		TableLoader("BuildingIngredient", LoadBuildingIngredient));
	loaders.emplace_back(
		TableLoader("BuildingKnowledge", LoadBuildingKnowledge));
	loaders.emplace_back(TableLoader("BuildingTool", LoadBuildingTool));
	loaders.emplace_back(TableLoader("Shop", LoadShop));
	loaders.emplace_back(TableLoader("ShopDefaultStock", LoadShopDefaultStock));
}

SQLiteDbms::~SQLiteDbms()
{
	// Nothing to do.
}

SQLiteDbms &SQLiteDbms::instance()
{
	// Since it's a static variable, if the class has already been created,
	// It won't be created again. And it **is** thread-safe in C++11.
	static SQLiteDbms instance;
	// Return a reference to our instance.
	return instance;
}

bool SQLiteDbms::openDatabase()
{
	if (!dbConnection.openConnection(Mud::instance().getMudDatabaseName(),
									 Mud::instance().getMudSystemDirectory(),
									 true)) {
		this->showLastError();
		return false;
	}
	return true;
}

bool SQLiteDbms::closeDatabase()
{
	if (!dbConnection.closeConnection()) {
		this->showLastError();
		return false;
	}
	return true;
}

bool SQLiteDbms::loadTables()
{
	// Status variable for loading operation.
	bool status = true;
	for (auto iterator : loaders) {
		Logger::log(LogLevel::Debug,
					"    Loading Table: " + iterator.table + ".");
		// Execute the query.
		auto result = dbConnection.executeSelect(iterator.getQuery().c_str());
		// Check the result.
		if (result == nullptr) {
			return false;
		}
		try {
			// Iterate through the rows.
			while (result->next()) {
				// Call the row parsing function.
				iterator.loadFunction(result);
			}
		} catch (SQLiteException &e) {
			Logger::log(LogLevel::Error, std::string(e.what()));
			// Release the resource.
			result->release();
			status = false;
			break;
		}
		// Release the resource.
		result->release();
	}
	return status;
}

bool SQLiteDbms::insertInto(std::string const &table,
							std::vector<std::string> const &args, bool orIgnore,
							bool orReplace)
{
	std::stringstream stream;
	stream << "INSERT";
	if (orIgnore) {
		stream << " OR IGNORE";
	} else if (orReplace) {
		stream << " OR REPLACE";
	}
	stream << " INTO `" << table << "`" << std::endl;
	stream << "VALUES(" << std::endl;
	for (auto it = args.begin(); it != args.end(); ++it) {
		auto value = (*it);
		if ((it + 1) == args.end()) {
			stream << "\"" << value << "\");";
		} else {
			stream << "\"" << value << "\", ";
		}
	}
	return (dbConnection.executeQuery(stream.str().c_str()) != 0);
}

bool SQLiteDbms::deleteFrom(std::string const &table, QueryList const &where)
{
	std::stringstream stream;
	stream << "DELETE FROM " << table << std::endl;
	stream << "WHERE" << std::endl;
	for (auto it = where.begin(); it != where.end(); ++it) {
		auto clause = (*it);
		if ((it + 1) == where.end()) {
			stream << "    " << clause.first << " = \"" << clause.second
				   << "\";" << std::endl;
		} else {
			stream << "    " << clause.first << " = \"" << clause.second
				   << "\" AND" << std::endl;
		}
	}
	return (dbConnection.executeQuery(stream.str().c_str()) != 0);
}

bool SQLiteDbms::updateInto(std::string const &table, QueryList const &value,
							QueryList const &where)
{
	std::stringstream stream;
	stream << "UPDATE " << table << std::endl;
	stream << "SET" << std::endl;
	for (auto it = value.begin(); it != value.end(); ++it) {
		auto clause = (*it);
		if ((it + 1) == value.end()) {
			stream << "    " << clause.first << " = \"" << clause.second << "\""
				   << std::endl;
		} else {
			stream << "    " << clause.first << " = \"" << clause.second
				   << "\"," << std::endl;
		}
	}
	stream << "WHERE" << std::endl;
	for (auto it = where.begin(); it != where.end(); ++it) {
		auto clause = (*it);
		if ((it + 1) == where.end()) {
			stream << "    " << clause.first << " = \"" << clause.second
				   << "\";" << std::endl;
		} else {
			stream << "    " << clause.first << " = \"" << clause.second
				   << "\" AND" << std::endl;
		}
	}
	return (dbConnection.executeQuery(stream.str().c_str()) != 0);
}

bool SQLiteDbms::updatePlayers()
{
	// Start a new transaction.
	dbConnection.beginTransaction();
	for (auto player : Mud::instance().mudPlayers) {
		if (player->isPlaying()) {
			if (!player->updateOnDB()) {
				Logger::log(LogLevel::Error, "Can't save the player '%s'.",
							player->getName());
				this->showLastError();
			}
		}
	}
	// Complete the transaction.
	dbConnection.endTransaction();
	return true;
}

bool SQLiteDbms::updateItems()
{
	// Start a transaction.
	//    dbConnection.beginTransaction();
	for (auto it : Mud::instance().mudItems) {
		if (!it.second->updateOnDB()) {
			Logger::log(LogLevel::Error, "Can't save the item '%s'.",
						it.second->getName());
			this->showLastError();
		}
	}
	// Complete the transaction.
	//    dbConnection.endTransaction();
	return true;
}

bool SQLiteDbms::updateRooms()
{
	// Start a new transaction.
	//    dbConnection.beginTransaction();
	for (auto it : Mud::instance().mudRooms) {
		if (!it.second->updateOnDB()) {
			Logger::log(LogLevel::Error, "Can't save the room '%s'.",
						it.second->name);
			this->showLastError();
		}
	}
	// Complete the transaction.
	//    dbConnection.endTransaction();
	return true;
}

bool SQLiteDbms::updateInMemoryDatabase()
{
	return dbConnection.updateInMemoryDatabase();
}

void SQLiteDbms::beginTransaction()
{
	dbConnection.beginTransaction();
}

void SQLiteDbms::rollbackTransection()
{
	dbConnection.rollbackTransection();
}

void SQLiteDbms::endTransaction()
{
	dbConnection.endTransaction();
}

void SQLiteDbms::showLastError() const
{
	Logger::log(LogLevel::Error,
				"Error code :" + ToString(dbConnection.getLastErrorCode()));
	Logger::log(LogLevel::Error,
				"Last error :" + dbConnection.getLastErrorMsg());
}
