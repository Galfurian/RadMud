/// @file   sqliteDbms.cpp
/// @brief  It's manage action to Database.
/// @author Enrico Fraccaroli
/// @date   Aug 23 2014
/// @copyright
/// Copyright (c) 2014, 2015, 2016 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
/// Permission to use, copy, modify, and distribute this software for any
/// purpose with or without fee is hereby granted, provided that the above
/// copyright notice and this permission notice appear in all copies.
///
/// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
/// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
/// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
/// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
/// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
/// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
/// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

#include "sqliteDbms.hpp"

#include "../mud.hpp"
#include "../constants.hpp"
#include "../utilities/logger.hpp"
#include "../utilities/stopwatch.hpp"

#include <sstream>
#include <chrono>

using namespace std;

SQLiteDbms::SQLiteDbms()
{
    tableLoaders.push_back(std::make_pair("BadName", LoadBadName));
    tableLoaders.push_back(std::make_pair("BlockedIp", LoadBlockedIp));
    tableLoaders.push_back(std::make_pair("News", LoadNews));
    tableLoaders.push_back(std::make_pair("Material", LoadMaterial));
    tableLoaders.push_back(std::make_pair("Skill", LoadSkill));
    tableLoaders.push_back(std::make_pair("Faction", LoadFaction));
    tableLoaders.push_back(std::make_pair("Race", LoadRace));
    tableLoaders.push_back(std::make_pair("Continent", LoadContinent));
    tableLoaders.push_back(std::make_pair("Area", LoadArea));
    tableLoaders.push_back(std::make_pair("Room", LoadRoom));
    tableLoaders.push_back(std::make_pair("Exit", LoadExit));
    tableLoaders.push_back(std::make_pair("AreaList", LoadAreaList));
    tableLoaders.push_back(std::make_pair("TravelPoint", LoadTravelPoint));
    tableLoaders.push_back(std::make_pair("Model", LoadModel));
    tableLoaders.push_back(std::make_pair("Liquid", LoadLiquid));
    tableLoaders.push_back(std::make_pair("Item", LoadItem));
    tableLoaders.push_back(std::make_pair("ItemContent", LoadContent));
    tableLoaders.push_back(std::make_pair("ItemContentLiq", LoadContentLiq));
    tableLoaders.push_back(std::make_pair("ItemRoom", LoadItemRoom));
    tableLoaders.push_back(std::make_pair("Writings", LoadWriting));
    tableLoaders.push_back(std::make_pair("Profession", LoadProfession));
    tableLoaders.push_back(std::make_pair("Production", LoadProduction));
    tableLoaders.push_back(std::make_pair("Mobile", LoadMobile));
    tableLoaders.push_back(std::make_pair("Building", LoadBuilding));
    tableLoaders.push_back(std::make_pair("ItemShop", LoadItemShop));
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
    if (!dbConnection.openConnection(kDatabaseName, kSystemDir))
    {
        Logger::log(LogLevel::Error, "Error code :" + ToString(dbConnection.getLastErrorCode()));
        Logger::log(LogLevel::Error, "Last error :" + dbConnection.getLastErrorMsg());
        return false;
    }
    return true;
}

bool SQLiteDbms::closeDatabase()
{
    if (!dbConnection.closeConnection())
    {
        Logger::log(LogLevel::Error, "Error code :" + ToString(dbConnection.getLastErrorCode()));
        Logger::log(LogLevel::Error, "Last error :" + dbConnection.getLastErrorMsg());
        return false;
    }
    return true;
}

bool SQLiteDbms::loadTables()
{
    // Status variable for loading operation.
    bool status = true;
    for (auto iterator : tableLoaders)
    {
        Logger::log(LogLevel::Debug, "    Loading Table: " + iterator.first + ".");
        // Prepare the query.
        std::string query = "SELECT * FROM " + iterator.first + ";";
        // Execute the query.
        ResultSet * result = dbConnection.executeSelect(query.c_str());
        // Check the result.
        if (result == nullptr)
        {
            return false;
        }
        // Call the rows parsing function.
        if (!iterator.second(result))
        {
            // Log an error.
            Logger::log(
                LogLevel::Error,
                "Encountered an error during loading table: " + iterator.first);
            status = false;
        }
        // release the resource.
        result->release();
        if (status == false)
        {
            break;
        }
    }
    return status;
}

bool SQLiteDbms::loadPlayer(Player * player)
{
    Logger::log(LogLevel::Debug, "Loading player " + player->getName() + ".");
    Stopwatch<std::chrono::milliseconds> stopwatch("LoadPlayer");
    stopwatch.start();
    if (!this->loadPlayerInformation(player))
    {
        Logger::log(LogLevel::Error, "Encountered an error during loading Player Information.");
        return false;
    }
    if (!this->loadPlayerItems(player))
    {
        Logger::log(LogLevel::Error, "Encountered an error during loading Player Items.");
        return false;
    }
    if (!this->loadPlayerSkill(player))
    {
        Logger::log(LogLevel::Error, "Encountered an error during loading Player Skills.");
        return false;
    }
    // Check the loaded player.
    if (!player->check())
    {
        Logger::log(LogLevel::Error, "Error during error checking.");
        return false;
    }
    // Log the elapsed time.
    Logger::log(LogLevel::Debug, "Elapsed Time (" + ToString(stopwatch.elapsed()) + " ms).");
    return true;
}

bool SQLiteDbms::searchPlayer(string name)
{
    bool outcome = false;
    ResultSet * result = dbConnection.executeSelect(
        ("SELECT count(*) FROM Player WHERE name=\"" + name + "\";").c_str());
    if (result)
    {
        if (result->next())
        {
            if (result->getNextInteger() == 1)
            {
                outcome = true;
            }
        }
    }
    result->release();
    return outcome;
}

bool SQLiteDbms::insertInto(std::string table, std::vector<std::string> args,
bool orIgnore,
bool orReplace)
{
    stringstream stream;
    stream << "INSERT";
    if (orIgnore)
    {
        stream << " OR IGNORE";
    }
    else if (orReplace)
    {
        stream << " OR REPLACE";
    }
    stream << " INTO " << table << std::endl;
    stream << "VALUES(" << std::endl;
    for (unsigned int it = 0; it < args.size(); it++)
    {
        std::string value = args.at(it);
        if (it == (args.size() - 1))
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

bool SQLiteDbms::deleteFrom(string table, QueryList where)
{
    stringstream stream;
    stream << "DELETE FROM " << table << std::endl;
    stream << "WHERE" << std::endl;
    for (unsigned int it = 0; it < where.size(); ++it)
    {
        std::pair<std::string, std::string> clause = where.at(it);
        if (it == (where.size() - 1))
        {
            stream << "    " << clause.first << " = \"" << clause.second << "\";" << std::endl;
        }
        else
        {
            stream << "    " << clause.first << " = \"" << clause.second << "\" AND" << std::endl;
        }
    }
    return (dbConnection.executeQuery(stream.str().c_str()) != 0);
}

bool SQLiteDbms::updateInto(std::string table, QueryList value, QueryList where)
{
    stringstream stream;
    stream << "UPDATE " << table << std::endl;
    stream << "SET" << std::endl;
    for (unsigned int it = 0; it < value.size(); ++it)
    {
        std::pair<std::string, std::string> clause = value.at(it);
        if (it == (value.size() - 1))
        {
            stream << "    " << clause.first << " = \"" << clause.second << "\"" << std::endl;
        }
        else
        {
            stream << "    " << clause.first << " = \"" << clause.second << "\"," << std::endl;
        }
    }
    stream << "WHERE" << std::endl;
    for (unsigned int it = 0; it < where.size(); ++it)
    {
        std::pair<std::string, std::string> clause = where.at(it);
        if (it == (where.size() - 1))
        {
            stream << "    " << clause.first << " = \"" << clause.second << "\";" << std::endl;
        }
        else
        {
            stream << "    " << clause.first << " = \"" << clause.second << "\" AND" << std::endl;
        }
    }
    return (dbConnection.executeQuery(stream.str().c_str()) != 0);
}

ResultSet * SQLiteDbms::executeSelect(std::string table, QueryList where)
{
    stringstream stream;
    stream << "SELECT * FROM " << table << std::endl;
    stream << "WHERE" << std::endl;
    for (unsigned int it = 0; it < where.size(); ++it)
    {
        std::pair<std::string, std::string> clause = where.at(it);
        if (it == (where.size() - 1))
        {
            stream << "    " << clause.first << " = \"" << clause.second << "\";" << std::endl;
        }
        else
        {
            stream << "    " << clause.first << " = \"" << clause.second << "\" AND" << std::endl;
        }
    }
    return dbConnection.executeSelect(stream.str().c_str());
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

bool SQLiteDbms::loadPlayerInformation(Player * player)
{
    // Prepare the query.
    std::string query = "SELECT * FROM Player WHERE name = \"" + player->name + "\";";
    // Execute the query.
    ResultSet * result = dbConnection.executeSelect(query.c_str());
    // Check the result.
    if (result == nullptr)
    {
        Logger::log(LogLevel::Error, "Query result is empty.");
        return false;
    }
    // Loading status.
    bool status = true;
    if (result->next())
    {
        player->name = result->getNextString();
        player->password = result->getNextString();
        player->race = Mud::instance().findRace(result->getNextInteger());
        player->setAbility(Ability::Strength, result->getNextUnsignedInteger());
        player->setAbility(Ability::Agility, result->getNextUnsignedInteger());
        player->setAbility(Ability::Perception, result->getNextUnsignedInteger());
        player->setAbility(Ability::Constitution, result->getNextUnsignedInteger());
        player->setAbility(Ability::Intelligence, result->getNextUnsignedInteger());
        player->gender = static_cast<GenderType>(result->getNextInteger());
        player->age = result->getNextInteger();
        player->description = result->getNextString();
        player->weight = result->getNextUnsignedInteger();
        player->faction = Mud::instance().findFaction(result->getNextInteger());
        player->level = result->getNextUnsignedInteger();
        player->experience = result->getNextInteger();
        player->room = Mud::instance().findRoom(result->getNextInteger());
        player->prompt = player->prompt_save = result->getNextString();
        player->flags = result->getNextUnsignedInteger();
        player->setHealth(result->getNextUnsignedInteger(), true);
        player->setStamina(result->getNextUnsignedInteger(), true);
        player->setHunger(result->getNextInteger());
        player->setThirst(result->getNextInteger());
        player->rent_room = result->getNextInteger();
        if (player->room == nullptr)
        {
            player->room = Mud::instance().findRoom(player->rent_room);
            if (player->room == nullptr)
            {
                Logger::log(LogLevel::Error, "No room has been set.");
                status = false;
            }
        }
    }
    // release the resource.
    result->release();
    return status;
}

bool SQLiteDbms::loadPlayerItems(Player * player)
{
    // Prepare the query.
    std::string query = "SELECT item, position FROM ItemPlayer WHERE owner = \"" + player->name
        + "\";";
    // Execute the query.
    ResultSet * result = dbConnection.executeSelect(query.c_str());
    // Check the result.
    if (result == nullptr)
    {
        Logger::log(LogLevel::Error, "Query result is empty.");
        return false;
    }
    // Loading status.
    bool status = true;
    while (result->next())
    {
        // The pointer to the object.
        Item * item = Mud::instance().findItem(result->getNextInteger());
        EquipmentSlot slot = static_cast<EquipmentSlot>(result->getNextInteger());

        if (item == nullptr)
        {
            Logger::log(LogLevel::Error, "Item not found!");
            status = false;
            break;
        }
        if (item->room != nullptr)
        {
            Logger::log(LogLevel::Error, "The item is no more available.");
            status = false;
            break;
        }
        if (item->owner != nullptr)
        {
            if (item->owner != player)
            {
                Logger::log(LogLevel::Error, "The item is no more available.");
                status = false;
                break;
            }
        }
        if (slot == EquipmentSlot::None)
        {
            // Add the item to the inventory.
            player->addInventoryItem(item);
        }
        else
        {
            // Change the slot of the item.
            item->setCurrentSlot(slot);
            // Add the item to the equipment.
            player->addEquipmentItem(item);
        }
    }
    // release the resource.
    result->release();
    return status;
}

bool SQLiteDbms::loadPlayerSkill(Player * player)
{
    // Prepare the query.
    std::string query = "SELECT skill, value FROM Advancement WHERE player=\"" + player->name
        + "\";";
    // Execute the query.
    ResultSet * result = dbConnection.executeSelect(query.c_str());
    // Check the result.
    if (result == nullptr)
    {
        Logger::log(LogLevel::Error, "Query result is empty.");
        return false;
    }
    // Loading status.
    bool status = true;
    while (result->next())
    {
        Skill * skill = Mud::instance().findSkill(result->getNextInteger());
        unsigned int value = result->getNextUnsignedInteger();
        if (skill == nullptr)
        {
            Logger::log(LogLevel::Error, "Wrong skill id.");
            status = false;
            break;
        }
        player->skills[skill->vnum] = value;
    }
    // release the resource.
    result->release();
    return status;
}
