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

using namespace std;

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

bool SQLiteDbms::insertInto(
    std::string table,
    std::vector<std::string> args,
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

bool SQLiteDbms::updatePlayers()
{
    // Start a new transaction.
    dbConnection.beginTransaction();
    // Then update all the items.
    bool result = true;
    for (auto player : Mud::instance().mudPlayers)
    {
        if (player->isPlaying())
        {
            QueryList value, where;
            value.push_back(std::make_pair("name", player->name));
            value.push_back(std::make_pair("password", player->password));
            value.push_back(std::make_pair("race", ToString(player->race->vnum)));
            value.push_back(
                std::make_pair("str", ToString(player->getAbility(Ability::Strength, false))));
            value.push_back(
                std::make_pair("agi", ToString(player->getAbility(Ability::Agility, false))));
            value.push_back(
                std::make_pair("per", ToString(player->getAbility(Ability::Perception, false))));
            value.push_back(
                std::make_pair("con", ToString(player->getAbility(Ability::Constitution, false))));
            value.push_back(
                std::make_pair("int", ToString(player->getAbility(Ability::Intelligence, false))));
            value.push_back(std::make_pair("gender", ToString(static_cast<int>(player->gender))));
            value.push_back(std::make_pair("age", ToString(player->age)));
            value.push_back(std::make_pair("description", player->description));
            value.push_back(std::make_pair("weight", ToString(player->weight)));
            value.push_back(std::make_pair("faction", ToString(player->faction->vnum)));
            value.push_back(std::make_pair("level", ToString(player->level)));
            value.push_back(std::make_pair("experience", ToString(player->experience)));
            value.push_back(std::make_pair("position", ToString(player->room->vnum)));
            value.push_back(std::make_pair("prompt", player->prompt));
            value.push_back(std::make_pair("flag", ToString(player->flags)));
            value.push_back(std::make_pair("health", ToString(player->getHealth())));
            value.push_back(std::make_pair("stamina", ToString(player->getStamina())));
            value.push_back(std::make_pair("rent_room", ToString(player->rent_room)));
            value.push_back(std::make_pair("hunger", ToString(player->getHunger())));
            value.push_back(std::make_pair("thirst", ToString(player->getThirst())));
            // Where clause.
            where.push_back(std::make_pair("name", player->name));
            if (!SQLiteDbms::instance().updateInto("Player", value, where))
            {
                Logger::log(LogLevel::Error, "Error during Player's Information save.");
                this->showLastError();
                result = false;
            }
            for (auto iterator : player->skills)
            {
                vector<string> arguments;
                arguments.push_back(player->name);
                arguments.push_back(ToString(iterator.first));
                arguments.push_back(ToString(iterator.second));

                if (!SQLiteDbms::instance().insertInto("Advancement", arguments, false, true))
                {
                    Logger::log(LogLevel::Error, "Error during Player's Skills save.");
                    this->showLastError();
                    result = false;
                }
            }
        }
    }
    // Complete the transaction.
    dbConnection.endTransaction();
    return result;
}

bool SQLiteDbms::updateItems()
{
    // Start a transaction.
    dbConnection.beginTransaction();
    // First delete all from table.
    if (!dbConnection.executeQuery("DELETE FROM Item;"))
    {
        Logger::log(LogLevel::Error, "Can't truncate the Item table!");
        dbConnection.rollbackTransection();
        return false;
    }
    // Complete the transaction.
    dbConnection.endTransaction();

    // Start a new transaction.
    dbConnection.beginTransaction();
    // Then update all the items.
    bool result = true;
    for (auto it : Mud::instance().mudItems)
    {
        Item * item = it.second;
        // Prepare the vector used to insert into the database.
        std::vector<std::string> arguments;
        arguments.push_back(ToString(item->vnum));
        arguments.push_back(ToString(item->model->vnum));
        arguments.push_back(ToString(item->quantity));
        arguments.push_back(item->maker);
        arguments.push_back(ToString(item->getPrice()));
        arguments.push_back(ToString(item->getWeight()));
        arguments.push_back(ToString(item->condition));
        arguments.push_back(ToString(item->maxCondition));
        arguments.push_back(ToString(item->composition->vnum));
        arguments.push_back(ToString(item->quality.toUInt()));
        arguments.push_back(ToString(item->flags));
        // Execute the insert.
        if (!SQLiteDbms::instance().insertInto("Item", arguments))
        {
            Logger::log(LogLevel::Error, "Can't save the item %s!", item->getName());
            this->showLastError();
            result = false;
        }
    }
    if (!result)
    {
        dbConnection.rollbackTransection();
        return false;
    }
    // Complete the transaction.
    dbConnection.endTransaction();

    // Start a new transaction.
    dbConnection.beginTransaction();
    for (auto it : Mud::instance().mudItems)
    {
        Item * item = it.second;
        // Save the item's position.
        if (item->room != nullptr)
        {
            std::vector<std::string> arguments;
            arguments.push_back(ToString(item->room->vnum));
            arguments.push_back(ToString(item->vnum));
            if (!SQLiteDbms::instance().insertInto("ItemRoom", arguments))
            {
                Logger::log(LogLevel::Error, "Can't save the item inside the room!");
                this->showLastError();
                result = false;
                break;
            }
        }
        else if (item->owner != nullptr)
        {
            if (item->owner->isPlayer())
            {
                std::vector<std::string> arguments;
                arguments.push_back(item->owner->name);
                arguments.push_back(ToString(item->vnum));
                if (item->owner->hasEquipmentItem(item))
                {
                    arguments.push_back(EnumToString(item->getCurrentSlot()));
                }
                else
                {
                    arguments.push_back("0");
                }
                if (!SQLiteDbms::instance().insertInto("ItemPlayer", arguments, false, true))
                {
                    Logger::log(LogLevel::Error, "Can't save the item possesed by the Player!");
                    this->showLastError();
                    result = false;
                    break;
                }
            }
        }
        else if (item->container != nullptr)
        {
            if (item->container->model->getType() != ModelType::Corpse)
            {
                std::vector<std::string> arguments;
                arguments.push_back(ToString(item->container->vnum));
                arguments.push_back(ToString(item->vnum));
                if (!SQLiteDbms::instance().insertInto("ItemContent", arguments))
                {
                    Logger::log(LogLevel::Error, "Can't save the item inside the container!");
                    this->showLastError();
                    result = false;
                    break;
                }
            }
        }
        if (!item->content.empty())
        {
            for (auto iterator2 : item->content)
            {
                std::vector<std::string> arguments;
                // Prepare the query arguments.
                arguments.push_back(ToString(item->vnum));
                arguments.push_back(ToString(iterator2->vnum));
                if (!SQLiteDbms::instance().insertInto("ItemContent", arguments))
                {
                    Logger::log(LogLevel::Error, "Can't save the contained item!");
                    this->showLastError();
                    result = false;
                    break;
                }
            }
        }
        if (item->contentLiq.first != nullptr)
        {
            std::vector<std::string> arguments;
            // Prepare the query arguments.
            arguments.push_back(ToString(item->vnum));
            arguments.push_back(ToString(item->contentLiq.first->vnum));
            arguments.push_back(ToString(item->contentLiq.second));
            if (!SQLiteDbms::instance().insertInto("ItemContentLiq", arguments, false, true))
            {
                Logger::log(LogLevel::Error, "Can't save the contained liquid!");
                this->showLastError();
                result = false;
                break;
            }
        }
    }
    if (!result)
    {
        dbConnection.rollbackTransection();
        return false;
    }
    // Complete the transaction.
    dbConnection.endTransaction();
    return result;
}

bool SQLiteDbms::updateRooms()
{
    // Start a new transaction.
    dbConnection.beginTransaction();
    // Then update all the items.
    bool result = true;
    for (auto it : Mud::instance().mudRooms)
    {
        Room * room = it.second;
        QueryList value;
        value.push_back(std::make_pair("x", ToString(room->coord.x)));
        value.push_back(std::make_pair("y", ToString(room->coord.y)));
        value.push_back(std::make_pair("z", ToString(room->coord.z)));
        value.push_back(std::make_pair("terrain", room->terrain));
        value.push_back(std::make_pair("name", room->name));
        value.push_back(std::make_pair("description", room->description));
        value.push_back(std::make_pair("flag", ToString(room->flags)));
        QueryList where;
        where.push_back(std::make_pair("vnum", ToString(room->vnum)));
        if (!SQLiteDbms::instance().updateInto("Room", value, where))
        {
            Logger::log(LogLevel::Error, "Can't save the room!");
            this->showLastError();
            result = false;
        }
    }
    // Complete the transaction.
    dbConnection.endTransaction();
    return result;
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
