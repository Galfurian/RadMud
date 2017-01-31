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

SQLiteDbms::SQLiteDbms() :
    dbConnection(),
    loaders()
{
    loaders.push_back(TableLoader("BadName", LoadBadName));
    loaders.push_back(TableLoader("BlockedIp", LoadBlockedIp));
    loaders.push_back(TableLoader("News", LoadNews));
    loaders.push_back(TableLoader("Terrain", LoadTerrain));
    loaders.push_back(TableLoader("Material", LoadMaterial));
    loaders.push_back(TableLoader("Area", LoadArea));
    loaders.push_back(TableLoader("Room", LoadRoom));
    loaders.push_back(TableLoader("Exit", LoadExit));
    loaders.push_back(TableLoader("AreaList", LoadAreaList));
    loaders.push_back(TableLoader("TravelPoint", LoadTravelPoint));
    loaders.push_back(TableLoader("BodyPart", LoadBodyPart));
    loaders.push_back(TableLoader("Model", LoadModel));
    loaders.push_back(TableLoader("Faction", LoadFaction));
    loaders.push_back(TableLoader("Race", LoadRace));
    loaders.push_back(TableLoader("RaceBodyPart", LoadRaceBodyPart));
    loaders.push_back(TableLoader("RaceCorpse", LoadRaceCorpse));
    loaders.push_back(TableLoader("Liquid", LoadLiquid));
    loaders.push_back(TableLoader("Item", LoadItem));
    loaders.push_back(TableLoader("ItemContent", LoadContent));
    loaders.push_back(TableLoader("ItemContentLiq", LoadContentLiq));
    loaders.push_back(TableLoader("ItemRoom", LoadItemRoom));
    loaders.push_back(TableLoader("Writings", LoadWriting));
    loaders.push_back(TableLoader("Profession", LoadProfession));
    loaders.push_back(TableLoader("Production", LoadProduction));
    loaders.push_back(TableLoader("ProductionTool", LoadProductionTool));
    loaders.push_back(TableLoader("ProductionOutcome", LoadProductionOutcome));
    loaders.push_back(TableLoader("ProductionIngredient",
                                  LoadProductionIngredient));
    loaders.push_back(TableLoader("ProductionKnowledge",
                                  LoadProductionKnowledge));
    loaders.push_back(TableLoader("Building", LoadBuilding));
    loaders.push_back(TableLoader("Shop", LoadShop));
    loaders.push_back(TableLoader("Currency", LoadCurrency));
    loaders.push_back(TableLoader("ModelBodyPart", LoadModelBodyPart));
    loaders.push_back(TableLoader("Mobile", LoadMobile));
    loaders.push_back(TableLoader("Skill", LoadSkill));
    loaders.push_back(TableLoader("SkillPrerequisite", LoadSkillPrerequisite));
    loaders.push_back(TableLoader("SkillAbilityModifier",
                                  LoadSkillAbilityModifier));
    loaders.push_back(TableLoader("SkillStatusModifier",
                                  LoadSkillStatusModifier));
    loaders.push_back(TableLoader("SkillCombatModifier",
                                  LoadSkillCombatModifier));
    loaders.push_back(TableLoader("SkillKnowledge",
                                  LoadSkillKnowledge));
    loaders.push_back(TableLoader("BodyPartResources", LoadBodyPartResources));
    loaders.push_back(TableLoader("BodyPartWeapon", LoadBodyPartWeapon));
    loaders.push_back(TableLoader("RaceBaseSkill", LoadRaceBaseSkill));
    loaders.push_back(TableLoader("RaceBaseAbility", LoadRaceBaseAbility));
    loaders.push_back(TableLoader("HeightMap", LoadHeightMap));
    loaders.push_back(TableLoader("HeightMapThreshold",
                                  LoadHeightMapThreshold));
    loaders.push_back(TableLoader("TerrainLiquid",
                                  LoadTerrainLiquid));
    loaders.push_back(TableLoader("TerrainLiquidSources",
                                  LoadTerrainLiquidSources));
}

SQLiteDbms::~SQLiteDbms()
{
    // Nothing to do.
}

SQLiteDbms & SQLiteDbms::instance()
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
                                     true))
    {
        this->showLastError();
        return false;
    }
    return true;
}

bool SQLiteDbms::closeDatabase()
{
    if (!dbConnection.closeConnection())
    {
        this->showLastError();
        return false;
    }
    return true;
}

bool SQLiteDbms::loadTables()
{
    // Status variable for loading operation.
    bool status = true;
    for (auto iterator : loaders)
    {
        Logger::log(LogLevel::Debug,
                    "    Loading Table: " + iterator.table + ".");
        // Execute the query.
        auto result = dbConnection.executeSelect(iterator.getQuery().c_str());
        // Check the result.
        if (result == nullptr)
        {
            return false;
        }
        // Call the rows parsing function.
        if (!iterator.loadFunction(result))
        {
            // Log an error.
            Logger::log(LogLevel::Error,
                        "Error when loading table: " + iterator.table);
            status = false;
        }
        // release the resource.
        result->release();
        if (!status) break;
    }
    return status;
}

bool SQLiteDbms::insertInto(std::string table,
                            std::vector<std::string> args,
                            bool orIgnore,
                            bool orReplace)
{
    std::stringstream stream;
    stream << "INSERT";
    if (orIgnore)
    {
        stream << " OR IGNORE";
    }
    else if (orReplace)
    {
        stream << " OR REPLACE";
    }
    stream << " INTO `" << table << "`" << std::endl;
    stream << "VALUES(" << std::endl;
    for (auto it = args.begin(); it != args.end(); ++it)
    {
        auto value = (*it);
        if ((it + 1) == args.end())
        {
            stream << "\"" << value << "\");";
        }
        else
        {
            stream << "\"" << value << "\", ";
        }
    }
    return (dbConnection.executeQuery(stream.str().c_str()) != 0);
}

bool SQLiteDbms::deleteFrom(std::string table, QueryList where)
{
    std::stringstream stream;
    stream << "DELETE FROM " << table << std::endl;
    stream << "WHERE" << std::endl;
    for (auto it = where.begin(); it != where.end(); ++it)
    {
        auto clause = (*it);
        if ((it + 1) == where.end())
        {
            stream << "    " << clause.first
                   << " = \"" << clause.second << "\";" << std::endl;
        }
        else
        {
            stream << "    " << clause.first
                   << " = \"" << clause.second << "\" AND" << std::endl;
        }
    }
    return (dbConnection.executeQuery(stream.str().c_str()) != 0);
}

bool SQLiteDbms::updateInto(std::string table, QueryList value, QueryList where)
{
    std::stringstream stream;
    stream << "UPDATE " << table << std::endl;
    stream << "SET" << std::endl;
    for (auto it = value.begin(); it != value.end(); ++it)
    {
        auto clause = (*it);
        if ((it + 1) == value.end())
        {
            stream << "    " << clause.first
                   << " = \"" << clause.second << "\"" << std::endl;
        }
        else
        {
            stream << "    " << clause.first
                   << " = \"" << clause.second << "\"," << std::endl;
        }
    }
    stream << "WHERE" << std::endl;
    for (auto it = where.begin(); it != where.end(); ++it)
    {
        auto clause = (*it);
        if ((it + 1) == where.end())
        {
            stream << "    " << clause.first
                   << " = \"" << clause.second << "\";" << std::endl;
        }
        else
        {
            stream << "    " << clause.first
                   << " = \"" << clause.second << "\" AND" << std::endl;
        }
    }
    return (dbConnection.executeQuery(stream.str().c_str()) != 0);
}

bool SQLiteDbms::updatePlayers()
{
    // Start a new transaction.
    dbConnection.beginTransaction();
    for (auto player : Mud::instance().mudPlayers)
    {
        if (player->isPlaying())
        {
            if (!player->updateOnDB())
            {
                Logger::log(LogLevel::Error,
                            "Can't save the player '%s'.", player->getName());
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
    dbConnection.beginTransaction();
    for (auto it : Mud::instance().mudItems)
    {
        if (!it.second->updateOnDB())
        {
            Logger::log(LogLevel::Error,
                        "Can't save the item '%s'.", it.second->getName());
            this->showLastError();
        }
    }
    // Complete the transaction.
    dbConnection.endTransaction();
    return true;
}

bool SQLiteDbms::updateRooms()
{
    // Start a new transaction.
    dbConnection.beginTransaction();
    for (auto it : Mud::instance().mudRooms)
    {
        if (!it.second->updateOnDB())
        {
            Logger::log(LogLevel::Error,
                        "Can't save the room '%s'.", it.second->name);
            this->showLastError();
        }
    }
    // Complete the transaction.
    dbConnection.endTransaction();
    return true;
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
