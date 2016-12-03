/// @file   playerLoadFunctions.cpp
/// @brief  
/// @author Enrico Fraccaroli
/// @date   03/12/2016
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
#include "stopwatch.hpp"
#include "logger.hpp"
#include "mud.hpp"

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
    if (!this->loadPlayerLuaVariables(player))
    {
        Logger::log(LogLevel::Error, "Encountered an error during loading Player Lua Variables.");
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
    // Prepare the query.
    std::string query = "SELECT count(*) FROM Player WHERE name=\"" + name + "\";";
    // Execute the query.
    ResultSet * result = SQLiteDbms::instance().dbConnection.executeSelect(query.c_str());
    if (result != nullptr)
    {
        if (result->next())
        {
            if (result->getNextInteger() == 1)
            {
                outcome = true;
            }
        }
        result->release();
    }
    return outcome;
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
        this->showLastError();
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
        player->prompt = result->getNextString();
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
    std::string query = "SELECT item, position FROM ItemPlayer WHERE owner = \"" + player->name + "\";";
    // Execute the query.
    ResultSet * result = dbConnection.executeSelect(query.c_str());
    // Check the result.
    if (result == nullptr)
    {
        Logger::log(LogLevel::Error, "Query result is empty.");
        this->showLastError();
        return false;
    }
    // Loading status.
    bool status = true;
    while (result->next())
    {
        // The pointer to the object.
        auto item = Mud::instance().findItem(result->getNextInteger());
        EquipmentSlot slot(result->getNextUnsignedInteger());

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
    std::string query = "SELECT skill, value FROM Advancement WHERE player=\"" + player->name + "\";";
    // Execute the query.
    ResultSet * result = dbConnection.executeSelect(query.c_str());
    // Check the result.
    if (result == nullptr)
    {
        Logger::log(LogLevel::Error, "Query result is empty.");
        this->showLastError();
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

bool SQLiteDbms::loadPlayerLuaVariables(Player * player)
{
    // Prepare the query.
    std::string query = "SELECT name, value FROM PlayerVariable WHERE player=\"" + player->name + "\";";
    // Execute the query.
    ResultSet * result = dbConnection.executeSelect(query.c_str());
    // Check the result.
    if (result == nullptr)
    {
        Logger::log(LogLevel::Error, "Query result is empty.");
        this->showLastError();
        return false;
    }
    // Loading status.
    bool status = true;
    while (result->next())
    {
        std::string variableName = result->getNextString();
        std::string variableValue = result->getNextString();
        player->setLuaVariable(variableName, variableValue);
        Logger::log(LogLevel::Debug, variableName + " = " + variableValue + ";");
    }
    // release the resource.
    result->release();
    return status;
}
