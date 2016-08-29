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

#include "sqliteLoadFunctions.hpp"

#include "../mud.hpp"
#include "../constants.hpp"
#include "../utilities/logger.hpp"
#include "../utilities/stopwatch.hpp"

#include <sstream>
#include <chrono>

SQLiteDbms::SQLiteDbms()
{
    loaders.push_back(TableLoader("BadName", LoadBadName));
    loaders.push_back(TableLoader("BlockedIp", LoadBlockedIp));
    loaders.push_back(TableLoader("News", LoadNews));
    loaders.push_back(TableLoader("Material", LoadMaterial));
    loaders.push_back(TableLoader("Skill", LoadSkill));
    loaders.push_back(TableLoader("Continent", LoadContinent));
    loaders.push_back(TableLoader("Area", LoadArea));
    loaders.push_back(TableLoader("Room", LoadRoom));
    loaders.push_back(TableLoader("Exit", LoadExit));
    loaders.push_back(TableLoader("AreaList", LoadAreaList));
    loaders.push_back(TableLoader("TravelPoint", LoadTravelPoint));
    loaders.push_back(TableLoader("Model", LoadModel));
    loaders.push_back(TableLoader("Faction", LoadFaction));
    loaders.push_back(TableLoader("Race", LoadRace));
    loaders.push_back(TableLoader("Liquid", LoadLiquid));
    loaders.push_back(TableLoader("Item", LoadItem));
    loaders.push_back(TableLoader("ItemContent", LoadContent));
    loaders.push_back(TableLoader("ItemContentLiq", LoadContentLiq));
    loaders.push_back(TableLoader("ItemRoom", LoadItemRoom));
    loaders.push_back(TableLoader("Writings", LoadWriting));
    loaders.push_back(TableLoader("Profession", LoadProfession));
    loaders.push_back(TableLoader("Production", LoadProduction));
    loaders.push_back(TableLoader("Mobile", LoadMobile));
    loaders.push_back(TableLoader("Building", LoadBuilding));
    loaders.push_back(TableLoader("Shop", LoadShop));
    loaders.push_back(TableLoader("Currency", LoadCurrency));
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
        Logger::log(LogLevel::Debug, "    Loading Table: " + iterator.table + ".");
        // Execute the query.
        ResultSet * result = dbConnection.executeSelect(iterator.getQuery().c_str());
        // Check the result.
        if (result == nullptr)
        {
            return false;
        }
        // Call the rows parsing function.
        if (!iterator.loadFunction(result))
        {
            // Log an error.
            Logger::log(LogLevel::Error, "Error when loading table: " + iterator.table);
            this->showLastError();
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

bool SQLiteDbms::searchPlayer(const std::string & name)
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

bool SQLiteDbms::insertInto(
    std::string table,
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

bool SQLiteDbms::deleteFrom(std::string table, QueryList where)
{
    std::stringstream stream;
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
    std::stringstream stream;
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
                Logger::log(LogLevel::Error, "Can't save the player '%s'.", player->getName());
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
            Logger::log(LogLevel::Error, "Can't save the item '%s'.", it.second->getName());
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
            Logger::log(LogLevel::Error, "Can't save the room '%s'.", it.second->name);
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
    Logger::log(LogLevel::Error, "Error code :" + ToString(dbConnection.getLastErrorCode()));
    Logger::log(LogLevel::Error, "Last error :" + dbConnection.getLastErrorMsg());
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
            player->inventory.push_back_item(item);
            // Set the owner of the item.
            item->owner = player;
        }
        else
        {
            // Change the slot of the item.
            item->setCurrentSlot(slot);
            // Add the item to the inventory.
            player->equipment.push_back_item(item);
            // Set the owner of the item.
            item->owner = player;
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
