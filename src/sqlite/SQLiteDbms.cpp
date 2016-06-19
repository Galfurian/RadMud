/// @file   SQLiteDbms.cpp
/// @brief  It's manage action to Database.
/// @author Enrico Fraccaroli
/// @date   23 Agosto 2014
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

#include "SQLiteDbms.hpp"

#include <sstream>
#include <chrono>
#include "../constants.hpp"
#include "../mud.hpp"
#include "../utils.hpp"
#include "../logger.hpp"

using namespace std;

SQLiteDbms::SQLiteDbms()
{
    tableLoaders.push_back(std::make_pair("BadName", LoadBadName));
    tableLoaders.push_back(std::make_pair("BlockedIp", LoadBlockedIp));
    tableLoaders.push_back(std::make_pair("News", LoadNews));
    tableLoaders.push_back(std::make_pair("Material", LoadMaterial));
    tableLoaders.push_back(std::make_pair("Skill", LoadSkill));
    tableLoaders.push_back(std::make_pair("Race", LoadRace));
    tableLoaders.push_back(std::make_pair("Faction", LoadFaction));
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
    for (auto iterator : tableLoaders)
    {
        Logger::log(LogLevel::Debug, "    Loading Table: " + iterator.first + ".");
        // Execute the query.
        ResultSet * result = dbConnection.executeSelect(("SELECT * FROM " + iterator.first + ";").c_str());
        // Check the result.
        if (result == nullptr)
        {
            return false;
        }
        // Call the rows parsing function.
        if (!iterator.second(result))
        {
            // Log an error.
            Logger::log(LogLevel::Error, "Encountered an error during loading table: " + iterator.first);
            // release the resource.
            result->release();
            return false;
        }
        // release the resource.
        result->release();
    }
    return true;
}

bool SQLiteDbms::loadPlayer(Player * player)
{
    Stopwatch<std::chrono::milliseconds> stopwatch("LoadPlayer");
    std::string query;
    ResultSet * result;
    Logger::log(LogLevel::Debug, "Loading player " + player->getName() + ".");
    stopwatch.start();

    ///////////////////////////////////////////////////////////////////////////////
    query = "SELECT * FROM Player WHERE name = \"" + player->name + "\";";
    // Execute the query.
    result = dbConnection.executeSelect(query.c_str());
    // Check the result.
    if (result == nullptr)
    {
        Logger::log(LogLevel::Error, "Result of query is empty.");
        return false;
    }
    // Call the rows parsing function.
    if (!LoadPlayerInformation(result, player))
    {
        Logger::log(LogLevel::Error, "Encountered an error during loading Player Information.");
        result->release();
        return false;
    }
    // release the resource.
    result->release();

    ///////////////////////////////////////////////////////////////////////////////
    query = "SELECT item, position FROM ItemPlayer WHERE owner = \"" + player->name + "\";";
    // Execute the query.
    result = dbConnection.executeSelect(query.c_str());
    // Check the result.
    if (result == nullptr)
    {
        Logger::log(LogLevel::Error, "Result of query is empty.");
        return false;
    }
    // Call the rows parsing function.
    if (!LoadPlayerItems(result, player))
    {
        Logger::log(LogLevel::Error, "Encountered an error during loading Player Items.");
        result->release();
        return false;
    }
    // release the resource.
    result->release();

    ///////////////////////////////////////////////////////////////////////////////
    query = "SELECT skill,value FROM Advancement WHERE player=\"" + player->name + "\";";
    // Execute the query.
    result = dbConnection.executeSelect(query.c_str());
    // Check the result.
    if (result == nullptr)
    {
        Logger::log(LogLevel::Error, "Result of query is empty.");
        return false;
    }
    // Call the rows parsing function.
    if (!LoadPlayerSkill(result, player))
    {
        Logger::log(LogLevel::Error, "Encountered an error during loading Player Skills.");
        result->release();
        return false;
    }
    // release the resource.
    result->release();

    ///////////////////////////////////////////////////////////////////////////////
    // Check the loaded player.
    if (!player->check())
    {
        Logger::log(LogLevel::Error, "Error during error checking.");
        return false;
    }

    ///////////////////////////////////////////////////////////////////////////////
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

bool SQLiteDbms::insertInto(std::string table, std::vector<std::string> args, bool orIgnore, bool orReplace)
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

bool LoadPlayerInformation(ResultSet * result, Player * player)
{
    if (!result->next())
    {
        Logger::log(LogLevel::Error, "No result from the query.");
        return false;
    }

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
            return false;
        }
    }

    return true;
}

bool LoadPlayerItems(ResultSet * result, Character * character)
{
    while (result->next())
    {
        // The pointer to the object.
        Item * item = Mud::instance().findItem(result->getNextInteger());
        EquipmentSlot slot = static_cast<EquipmentSlot>(result->getNextInteger());

        if (item == nullptr)
        {
            Logger::log(LogLevel::Error, "Item not found!");
            return false;
        }

        if (item->room != nullptr)
        {
            Logger::log(LogLevel::Error, "The item is no more available.");
            return false;
        }
        if (item->owner != nullptr)
        {
            if (item->owner != character)
            {
                Logger::log(LogLevel::Error, "The item is no more available.");
                return false;
            }
        }
        if (slot == EquipmentSlot::None)
        {
            // Add the item to the inventory.
            character->addInventoryItem(item);
        }
        else
        {
            // Change the slot of the item.
            item->setCurrentSlot(slot);
            // Add the item to the equipment.
            character->addEquipmentItem(item);
        }
    }
    return true;
}

bool LoadPlayerSkill(ResultSet * result, Player * player)
{
    try
    {
        while (result->next())
        {
            Skill * skill = Mud::instance().findSkill(result->getNextInteger());
            unsigned int value = result->getNextUnsignedInteger();

            if (skill == nullptr)
            {
                throw runtime_error("Wrong skill id.");
            }
            // Prevent saved values below 0.
            if (value <= 0)
            {
                value = 1;
            }

            player->skills[skill->vnum] = value;
        }
    }
    catch (std::runtime_error & e)
    {
        Logger::log(LogLevel::Error, "    " + std::string(e.what()));
        return false;
    }
    catch (std::bad_alloc & e)
    {
        Logger::log(LogLevel::Error, "    " + std::string(e.what()));
        return false;
    }

    return true;
}

bool LoadBadName(ResultSet * result)
{
    while (result->next())
    {
        if (!Mud::instance().badNames.insert(result->getNextString()).second)
        {
            Logger::log(LogLevel::Error, "Error during bad name loading.");
            return false;
        }
    }
    return true;
}

bool LoadBlockedIp(ResultSet * result)
{
    while (result->next())
    {
        if (!Mud::instance().blockedIPs.insert(result->getNextString()).second)
        {
            Logger::log(LogLevel::Error, "Error during blocked ips loading.");
            return false;
        }
    }
    return true;
}

bool LoadNews(ResultSet * result)
{
    while (result->next())
    {
        if (!Mud::instance().mudNews.insert(std::make_pair(result->getNextString(), result->getNextString())).second)
        {
            Logger::log(LogLevel::Error, "Error during news loading.");
            return false;
        }
    }
    return true;
}

bool LoadContent(ResultSet * result)
{
    while (result->next())
    {
        Item * container = Mud::instance().findItem(result->getNextInteger());
        Item * contained = Mud::instance().findItem(result->getNextInteger());
        if (container == nullptr)
        {
            Logger::log(LogLevel::Error, "Can't find container item.");
            return false;
        }
        if (contained == nullptr)
        {
            Logger::log(LogLevel::Error, "Can't find contained item.");
            return false;
        }
        container->content.push_back(contained);
        contained->container = container;
        if (!contained->check(true))
        {
            Logger::log(LogLevel::Error, "Error during error checking.");
            return false;
        }
    }
    return true;
}

bool LoadItem(ResultSet * result)
{
    while (result->next())
    {
        // Create a new item.
        Item * item = new Item();
        // Initialize the item.
        item->vnum = result->getNextInteger();
        item->model = Mud::instance().findModel(result->getNextInteger());
        item->maker = result->getNextString();
        item->condition = result->getNextInteger();
        item->composition = Mud::instance().findMaterial(result->getNextInteger());
        item->quality = (ItemQuality) result->getNextInteger();
        item->flags = result->getNextUnsignedInteger();

        if (!item->check())
        {
            Logger::log(LogLevel::Error, "Error during error checking.");
            delete (item);
            return false;
        }

        // Add the item to the map of items.
        if (!Mud::instance().addItem(item))
        {
            Logger::log(LogLevel::Error, "Error during item insertion.");
            delete (item);
            return false;
        }
    }
    return true;
}

bool LoadSkill(ResultSet * result)
{
    while (result->next())
    {
        // Create an empty Skill.
        Skill skill;
        skill.vnum = result->getNextInteger();
        skill.name = result->getNextString();
        skill.description = result->getNextString();
        skill.attribute = result->getNextInteger();
        // Check the correctness.
        if (!skill.check())
        {
            Logger::log(LogLevel::Error, "Error during error checking.");
            return false;
        }
        if (!Mud::instance().addSkill(skill))
        {
            Logger::log(LogLevel::Error, "Error during skill insertion.");
            return false;
        }
    }
    return true;
}

bool LoadFaction(ResultSet * result)
{
    while (result->next())
    {
        // Create an empty Faction.
        Faction faction;
        faction.vnum = result->getNextInteger();
        faction.name = result->getNextString();
        faction.description = result->getNextString();
        // Translate new_line.
        FindAndReplace(faction.description, "%r", "\n");
        // Check the correctness.
        if (!faction.check())
        {
            Logger::log(LogLevel::Error, "Error during error checking.");
        }
        if (!Mud::instance().addFaction(faction))
        {
            Logger::log(LogLevel::Error, "Error during faction insertion.");
            return false;
        }
    }
    return true;
}

bool LoadModel(ResultSet * result)
{
    while (result->next())
    {
        // Create an empty model.
        Model model;
        model.vnum = result->getNextInteger();
        model.name = result->getNextString();
        model.article = result->getNextString();
        model.shortdesc = result->getNextString();
        model.keys = GetWords(result->getNextString());
        model.description = result->getNextString();
        model.type = (ModelType) result->getNextInteger();
        model.slot = (EquipmentSlot) result->getNextInteger();
        model.flags = result->getNextUnsignedInteger();
        model.weight = result->getNextUnsignedInteger();
        model.price = result->getNextInteger();
        model.condition = result->getNextInteger();
        model.decay = result->getNextInteger();
        model.material = (MaterialType) result->getNextInteger();
        model.tileSet = result->getNextInteger();
        model.tileId = result->getNextInteger();
        model.setFunctions(result->getNextString());
        // Translate new_line.
        FindAndReplace(model.description, "%r", "\n");
        // Check the correctness.
        if (!model.check())
        {
            Logger::log(LogLevel::Error, "Error during error checking.");
            return false;
        }
        if (!Mud::instance().addModel(model))
        {
            Logger::log(LogLevel::Error, "Error during model insertion.");
            return false;
        }
    }
    return true;
}

bool LoadRace(ResultSet * result)
{
    while (result->next())
    {
        // Create an empty Race.
        Race race;
        race.vnum = result->getNextInteger();
        race.name = result->getNextString();
        race.description = result->getNextString();
        race.material = Mud::instance().findMaterial(result->getNextInteger());
        race.setCharacteristic(result->getNextString());
        race.available_faction = GetIntVect(result->getNextString());
        race.player_allow = result->getNextInteger();
        race.tileSet = result->getNextInteger();
        race.tileId = result->getNextInteger();
        race.corpseDescription = result->getNextString();
        // Translate new_line.
        FindAndReplace(race.description, "%r", "\n");
        // Check the correctness.
        if (!race.check())
        {
            Logger::log(LogLevel::Error, "Error during error checking.");
            return false;
        }
        if (!Mud::instance().addRace(race))
        {
            Logger::log(LogLevel::Error, "Error during race insertion.");
            return false;
        }
    }
    return true;
}

bool LoadMobile(ResultSet * result)
{
    while (result->next())
    {
        // Create an empty Mobile.
        Mobile * mobile = new Mobile();
        // Initialize the mobile.
        mobile->id = result->getNextString();
        mobile->respawnRoom = Mud::instance().findRoom(result->getNextInteger());
        mobile->room = mobile->respawnRoom;
        mobile->name = result->getNextString();
        mobile->keys = GetWords(result->getNextString());
        mobile->shortdesc = result->getNextString();
        mobile->staticdesc = result->getNextString();
        mobile->description = result->getNextString();
        mobile->race = Mud::instance().findRace(result->getNextInteger());
        mobile->faction = Mud::instance().findFaction(result->getNextInteger());
        mobile->gender = static_cast<GenderType>(result->getNextInteger());
        mobile->weight = result->getNextUnsignedInteger();
        mobile->actions = GetWords(result->getNextString());
        mobile->flags = result->getNextUnsignedInteger();
        mobile->level = result->getNextUnsignedInteger();
        if (!mobile->setAbilities(result->getNextString()))
        {
            Logger::log(LogLevel::Error, "Wrong characteristics.");
            delete (mobile);
            return false;
        }
        mobile->lua_script = result->getNextString();

        mobile->setHealth(mobile->getMaxHealth(), true);
        mobile->setStamina(mobile->getMaxStamina(), true);

        // Translate new_line.
        FindAndReplace(mobile->description, "%r", "\n");
        // Check the correctness.
        if (!mobile->check())
        {
            Logger::log(LogLevel::Error, "Error during error checking.");
            delete (mobile);
            return false;
        }
        if (!Mud::instance().addMobile(mobile))
        {
            Logger::log(LogLevel::Error, "Error during mobile insertion.");
            delete (mobile);
            return false;
        }

        // Set the respawn time.
        mobile->nextRespawn = std::chrono::system_clock::now() + std::chrono::seconds(5 * mobile->level);
        // Set the next action time.
        mobile->nextActionCooldown = std::chrono::system_clock::now() + std::chrono::seconds(10 * mobile->level);
        // Load its script.
        mobile->loadScript(kSystemDir + "lua/" + mobile->lua_script);
    }
    return true;
}

bool LoadRoom(ResultSet * result)
{
    while (result->next())
    {
        // Create an empty Room.
        Room * room = new Room();
        // Initialize the Room.
        room->vnum = result->getNextInteger();
        room->coord.x = result->getNextInteger();
        room->coord.y = result->getNextInteger();
        room->coord.z = result->getNextInteger();
        room->terrain = result->getNextString();
        room->name = result->getNextString();
        room->description = result->getNextString();
        room->flags = result->getNextUnsignedInteger();
        // Translate new_line.
        FindAndReplace(room->description, "%r", "\n");
        // Check the correctness.
        if (!room->check())
        {
            Logger::log(LogLevel::Error, "Error during error checking.");
            delete (room);
            return false;
        }
        if (!Mud::instance().addRoom(room))
        {
            Logger::log(LogLevel::Error, "Error during room insertion.");
            delete (room);
            return false;
        }
    }
    return true;
}

bool LoadExit(ResultSet * result)
{
    while (result->next())
    {
        // Create an empty exit.
        std::shared_ptr<Exit> newExit = std::make_shared<Exit>();
        // Retrieve the rooms vnum.
        newExit->source = Mud::instance().findRoom(result->getNextInteger());
        newExit->destination = Mud::instance().findRoom(result->getNextInteger());
        newExit->direction = static_cast<Direction>(result->getNextInteger());
        newExit->flags = result->getNextUnsignedInteger();

        // Check the correctness.
        if (newExit->source == nullptr)
        {
            Logger::log(LogLevel::Error, "Can't find the source room.");
            return false;
        }
        if (newExit->destination == nullptr)
        {
            Logger::log(LogLevel::Error, "Can't find the destination room.");
            return false;
        }
        if (!newExit->check())
        {
            Logger::log(LogLevel::Error, "Error during error checking.");
            return false;
        }
        // Push this exit into the list of exits of the source room.
        newExit->source->addExit(newExit);
    }
    return true;
}

bool LoadItemRoom(ResultSet * result)
{
    while (result->next())
    {
        Room * room = Mud::instance().findRoom(result->getNextInteger());
        Item * item = Mud::instance().findItem(result->getNextInteger());

        // Check the correctness.
        if (room == nullptr)
        {
            Logger::log(LogLevel::Error, "Can't find the room.");
            return false;
        }
        if (item == nullptr)
        {
            Logger::log(LogLevel::Error, "Can't find the item.");
            return false;
        }
        // Load the item inside the room.
        if (HasFlag(item->flags, ItemFlag::Built))
        {
            room->addBuilding(item);
        }
        else
        {
            room->addItem(item);
        }
        if (!item->check(true))
        {
            Logger::log(LogLevel::Error, "Error during error checking.");
            return false;
        }
    }
    return true;
}

bool LoadArea(ResultSet * result)
{
    while (result->next())
    {
        // Create an empty area.
        Area * area = new Area();
        // Initialize the area.
        area->vnum = result->getNextInteger();
        area->name = result->getNextString();
        area->builder = result->getNextString();
        area->continent = result->getNextString();
        area->width = result->getNextInteger();
        area->height = result->getNextInteger();
        area->elevation = result->getNextInteger();
        area->tileSet = result->getNextInteger();
        area->type = static_cast<AreaType>(result->getNextUnsignedInteger());
        area->status = static_cast<AreaStatus>(result->getNextUnsignedInteger());
        // Check the correctness.
        if (!area->check())
        {
            Logger::log(LogLevel::Error, "Error during error checking.");
            return false;
        }
        if (!Mud::instance().addArea(area))
        {
            Logger::log(LogLevel::Error, "Error during area insertion.");
            return false;
        }
    }
    return true;
}

bool LoadAreaList(ResultSet * result)
{
    while (result->next())
    {
        int areaVnum = result->getNextInteger();
        int roomVnum = result->getNextInteger();
        Area * area = Mud::instance().findArea(areaVnum);
        Room * room = Mud::instance().findRoom(roomVnum);
        // Check the correctness.
        if (area == nullptr)
        {
            Logger::log(LogLevel::Error, "Can't find the area (%s).", ToString(areaVnum));
            return false;
        }
        if (room == nullptr)
        {
            Logger::log(LogLevel::Error, "Can't find the room (%s).", ToString(roomVnum));
            return false;
        }
        // Load the room inside the area.
        area->addRoom(room);
        if (!room->check(true))
        {
            Logger::log(LogLevel::Error, "Wrong room data.");
            return false;
        }
    }
    return true;
}

bool LoadWriting(ResultSet * result)
{
    while (result->next())
    {
        // Create an empty Writing.
        Writing * writing = new Writing();
        // Initialize the Writing.
        writing->vnum = result->getNextInteger();
        writing->title = result->getNextString();
        writing->author = result->getNextString();
        writing->content = result->getNextString();
        // Fid the item on which the writing is attached.
        Item * item = Mud::instance().findItem(writing->vnum);
        if (item == nullptr)
        {
            Logger::log(LogLevel::Error, "Can't find the item :" + ToString(writing->vnum));
            return false;
        }
        if (!Mud::instance().addWriting(writing))
        {
            Logger::log(LogLevel::Error, "Error during writing insertion.");
            return false;
        }
    }
    return true;
}

bool LoadContinent(ResultSet * result)
{
    while (result->next())
    {
        Continent * continent = new Continent();
        // Initialize the continent.
        continent->vnum = result->getNextInteger();
        continent->name = result->getNextString();
        continent->builder = result->getNextString();
        continent->width = result->getNextInteger();
        continent->height = result->getNextInteger();
        continent->txtMap = result->getNextString();
        // Add the continent to the map.
        continent->init();
        // Check the correctness.
        if (!continent->check())
        {
            Logger::log(LogLevel::Error, "Error during error checking.");
            return false;
        }
        if (!Mud::instance().addContinent(continent))
        {
            Logger::log(LogLevel::Error, "Error during continent insertion.");
            return false;
        }
    }
    return true;
}

bool LoadMaterial(ResultSet * result)
{
    while (result->next())
    {
        // Create an empty Material.
        Material material;
        // Intialize the material.
        material.vnum = result->getNextInteger();
        material.type = (MaterialType) result->getNextInteger();
        material.name = result->getNextString();
        material.article = result->getNextString();
        material.worth = result->getNextInteger();
        material.hardness = result->getNextInteger();
        material.lightness = result->getNextInteger();
        // Check the correctness.
        if (!material.check())
        {
            Logger::log(LogLevel::Error, "Error during error checking.");
            return false;
        }
        if (!Mud::instance().addMaterial(material))
        {
            Logger::log(LogLevel::Error, "Error during material insertion.");
            return false;
        }
    }
    return true;
}

bool LoadProfession(ResultSet * result)
{
    while (result->next())
    {
        // Create an empty Profession.
        Profession professions;
        // Initialize the profession.
        professions.vnum = result->getNextUnsignedInteger();
        professions.name = result->getNextString();
        professions.description = result->getNextString();
        professions.command = result->getNextString();
        professions.posture = (CharacterPosture) result->getNextInteger();
        professions.action = result->getNextString();
        professions.startMessage = result->getNextString();
        professions.finishMessage = result->getNextString();
        professions.successMessage = result->getNextString();
        professions.failureMessage = result->getNextString();
        professions.interruptMessage = result->getNextString();
        professions.notFoundMessage = result->getNextString();
        // Check the correctness.
        if (!professions.check())
        {
            Logger::log(LogLevel::Error, "Error during error checking.");
            return false;
        }
        if (!Mud::instance().addProfession(professions))
        {
            Logger::log(LogLevel::Error, "Error during professions insertion.");
            return false;
        }
    }
    return true;
}

bool LoadProduction(ResultSet * result)
{
    while (result->next())
    {
        // Checker flag.
        bool check = true;

        // Create an empty Production.
        Production production;
        // Initialize the Production.
        production.vnum = result->getNextInteger();
        production.name = result->getNextString();
        production.profession = Mud::instance().findProfession(result->getNextString());
        production.difficulty = result->getNextUnsignedInteger();
        production.time = result->getNextUnsignedInteger();
        production.assisted = result->getNextInteger();
        check &= production.setOutcome(result->getNextString());
        check &= production.setTool(result->getNextString());
        check &= production.setIngredient(result->getNextString());
        production.material = static_cast<ResourceType>(result->getNextInteger());
        production.workbench = static_cast<ToolType>(result->getNextInteger());

        // ////////////////////////////////////////////////////////////////
        // Check the correctness.
        if (!check)
        {
            Logger::log(LogLevel::Error, "The production is incorrect " + production.name);
            return false;
        }
        if (!production.check())
        {
            Logger::log(LogLevel::Error, "Error during error checking.");
            return false;
        }
        if (!Mud::instance().addProduction(production))
        {
            Logger::log(LogLevel::Error, "Error during production insertion.");
            return false;
        }
    }
    return true;
}

bool LoadLiquid(ResultSet * result)
{
    while (result->next())
    {
        // Create an empty Liquid.
        Liquid liquid;
        // Load the liquid.
        liquid.vnum = result->getNextInteger();
        liquid.name = result->getNextString();
        liquid.worth = result->getNextInteger();
        // Check the correctness.
        if (!liquid.check())
        {
            Logger::log(LogLevel::Error, "Error during error checking.");
            return false;
        }
        if (!Mud::instance().addLiquid(liquid))
        {
            Logger::log(LogLevel::Error, "Error during liquid insertion.");
            return false;
        }
    }
    return true;
}

bool LoadContentLiq(ResultSet * result)
{
    while (result->next())
    {
        Item * container = Mud::instance().findItem(result->getNextInteger());
        Liquid * liquid = Mud::instance().findLiquid(result->getNextInteger());
        unsigned int quantity = result->getNextUnsignedInteger();
        bool check = true;
        if (container == nullptr)
        {
            Logger::log(LogLevel::Error, "Can't find container item.");
            check = false;
        }
        if (liquid == nullptr)
        {
            Logger::log(LogLevel::Error, "Can't find liquid.");
            check = false;
        }
        if (quantity == 0)
        {
            Logger::log(LogLevel::Error, "Liquid content quantity misplaced.");
            check = false;
        }
        if (!check)
        {
            Logger::log(LogLevel::Error, "Error during error checking.");
            return false;
        }
        container->contentLiq.first = liquid;
        container->contentLiq.second = quantity;
    }
    return true;
}

bool LoadTravelPoint(ResultSet * result)
{
    while (result->next())
    {
        Area * sourceArea = Mud::instance().findArea(result->getNextInteger());
        Room * sourceRoom = sourceArea->getRoom(result->getNextInteger());
        Area * targetArea = Mud::instance().findArea(result->getNextInteger());
        Room * targetRoom = targetArea->getRoom(result->getNextInteger());
        bool check = true;
        if (sourceArea == nullptr)
        {
            Logger::log(LogLevel::Error, "Can't find the source area.");
            check = false;
        }
        if (sourceRoom == nullptr)
        {
            Logger::log(LogLevel::Error, "Can't find the source room.");
            check = false;
        }
        if (targetArea == nullptr)
        {
            Logger::log(LogLevel::Error, "Can't find the target area.");
            check = false;
        }
        if (targetRoom == nullptr)
        {
            Logger::log(LogLevel::Error, "Can't find the target room.");
            check = false;
        }
        if (!check)
        {
            Logger::log(LogLevel::Error, "Error during error checking.");
            return false;
        }
        if (!Mud::instance().addTravelPoint(sourceRoom, targetRoom))
        {
            Logger::log(LogLevel::Error, "Error during TravelPoint insertion.");
            return false;
        }
        if (!Mud::instance().addTravelPoint(targetRoom, sourceRoom))
        {
            Logger::log(LogLevel::Error, "Error during TravelPoint insertion.");
            return false;
        }
        SetFlag(sourceRoom->flags, RoomFlag::TravelPoint);
        SetFlag(targetRoom->flags, RoomFlag::TravelPoint);
    }
    return true;
}

bool LoadBuilding(ResultSet * result)
{
    while (result->next())
    {
        // Create an empty Building.
        Building building;
        // Initialize the Production.

        building.vnum = result->getNextInteger();
        building.name = result->getNextString();
        building.difficulty = result->getNextUnsignedInteger();
        building.time = result->getNextUnsignedInteger();
        building.assisted = result->getNextInteger();
        building.setTool(result->getNextString());
        building.buildingModel = Mud::instance().findModel(result->getNextInteger());
        building.setIngredient(result->getNextString());
        building.unique = static_cast<bool>(result->getNextInteger());

        if (building.buildingModel == nullptr)
        {
            Logger::log(LogLevel::Error, "Can't find the building model.");
            break;
        }
        if (!building.check())
        {
            Logger::log(LogLevel::Error, "Error during error checking.");
            return false;
        }
        if (!Mud::instance().addBuilding(building))
        {
            Logger::log(LogLevel::Error, "Error during building insertion.");
            return false;
        }
    }
    return true;
}
