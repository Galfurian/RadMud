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
}

bool SQLiteDbms::openDatabase()
{
    if (!dbConnection.openConnection(kDatabaseName, kSystemDir))
    {
        LogError("Error code :" + ToString(dbConnection.getLastErrorCode()));
        LogError("Last error :" + dbConnection.getLastErrorMsg());
        return false;
    }
    return true;
}

bool SQLiteDbms::closeDatabase()
{
    if (!dbConnection.closeConnection())
    {
        LogError("Error code :" + ToString(dbConnection.getLastErrorCode()));
        LogError("Last error :" + dbConnection.getLastErrorMsg());
        return false;
    }
    return true;
}

bool SQLiteDbms::loadTables()
{
    for (auto iterator : tableLoaders)
    {
        LogMessage(kMDat, "        Loading Table: " + iterator.first + ".");
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
            LogError("Encountered an error during loading table: " + iterator.first);
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
    LogMessage(kMDat, "Loading player " + player->getName() + ".");
    stopwatch.start();

    ///////////////////////////////////////////////////////////////////////////////
    query = "SELECT * FROM Player WHERE name = \"" + player->name + "\";";
    // Execute the query.
    result = dbConnection.executeSelect(query.c_str());
    // Check the result.
    if (result == nullptr)
    {
        LogError("Result of query is empty.");
        return false;
    }
    // Call the rows parsing function.
    if (!LoadPlayerInformation(result, player))
    {
        LogError("Encountered an error during loading Player Information.");
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
        LogError("Result of query is empty.");
        return false;
    }
    // Call the rows parsing function.
    if (!LoadPlayerItems(result, player))
    {
        LogError("Encountered an error during loading Player Items.");
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
        LogError("Result of query is empty.");
        return false;
    }
    // Call the rows parsing function.
    if (!LoadPlayerSkill(result, player))
    {
        LogError("Encountered an error during loading Player Skills.");
        result->release();
        return false;
    }
    // release the resource.
    result->release();

    ///////////////////////////////////////////////////////////////////////////////
    // Check the loaded player.
    if (!player->check())
    {
        LogError("Error during error checking.");
        return false;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Log the elapsed time.
    LogMessage(kMDat, "Elapsed Time (" + ToString(stopwatch.elapsed()) + " ms).");

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
        LogError("No result from the query.");
        return false;
    }

    player->name = result->getNextString();
    player->password = result->getNextString();
    player->race = Mud::getInstance().findRace(result->getNextInteger());
    player->strength = result->getNextInteger();
    player->agility = result->getNextInteger();
    player->perception = result->getNextInteger();
    player->constitution = result->getNextInteger();
    player->intelligence = result->getNextInteger();
    player->sex = result->getNextInteger();
    player->age = result->getNextInteger();
    player->description = result->getNextString();
    player->weight = result->getNextInteger();
    player->faction = Mud::getInstance().findFaction(result->getNextInteger());
    player->level = result->getNextInteger();
    player->experience = result->getNextInteger();
    player->room = Mud::getInstance().findRoom(result->getNextInteger());
    player->prompt = player->prompt_save = result->getNextString();
    player->flags = result->getNextInteger();
    player->health = result->getNextInteger();
    player->stamina = result->getNextInteger();
    player->hunger = result->getNextInteger();
    player->thirst = result->getNextInteger();
    player->rent_room = result->getNextInteger();

    if (player->room == nullptr)
    {
        player->room = Mud::getInstance().findRoom(player->rent_room);
        if (player->room == nullptr)
        {
            LogError("No room has been set.");
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
        Item * item = Mud::getInstance().findItem(result->getNextInteger());
        EquipmentSlot slot = static_cast<EquipmentSlot>(result->getNextInteger());

        if (item == nullptr)
        {
            LogError("Item not found!");
            return false;
        }

        if (item->room != nullptr)
        {
            LogError("The item is no more available.");
            return false;
        }
        if (item->owner != nullptr)
        {
            if (item->owner != character)
            {
                LogError("The item is no more available.");
                return false;
            }
        }

        switch (slot)
        {
            case EquipmentSlot::Head:
            case EquipmentSlot::Torso:
            case EquipmentSlot::Back:
            case EquipmentSlot::Legs:
            case EquipmentSlot::Feet:
            case EquipmentSlot::RightHand:
            case EquipmentSlot::LeftHand:
                // Change the slot of the item.
                item->setCurrentSlot(slot);
                // Add the item to the equipment.
                character->addEquipmentItem(item);
                break;
            case EquipmentSlot::None:
                // Add the item to the inventory.
                character->addInventoryItem(item);
                break;
            default:
                LogError("[LoadPlayerItems] Wrong equipment_slot value.");
                return false;
                break;
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
            Skill * skill = Mud::getInstance().findSkill(result->getNextInteger());
            unsigned int value = result->getNextInteger();

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
        LogError("    " + std::string(e.what()));
        return false;
    }
    catch (std::bad_alloc & e)
    {
        LogError("    " + std::string(e.what()));
        return false;
    }

    return true;
}

bool LoadBadName(ResultSet * result)
{
    while (result->next())
    {
        if (!Mud::getInstance().badNames.insert(result->getNextString()).second)
        {
            LogError("Error during bad name loading.");
            return false;
        }
    }
    return true;
}

bool LoadBlockedIp(ResultSet * result)
{
    while (result->next())
    {
        if (!Mud::getInstance().blockedIPs.insert(result->getNextString()).second)
        {
            LogError("Error during blocked ips loading.");
            return false;
        }
    }
    return true;
}

bool LoadNews(ResultSet * result)
{
    while (result->next())
    {
        if (!Mud::getInstance().mudNews.insert(std::make_pair(result->getNextString(), result->getNextString())).second)
        {
            LogError("Error during news loading.");
            return false;
        }
    }
    return true;
}

bool LoadContent(ResultSet * result)
{
    while (result->next())
    {
        Item * container = Mud::getInstance().findItem(result->getNextInteger());
        Item * contained = Mud::getInstance().findItem(result->getNextInteger());
        if (container == nullptr)
        {
            LogError("Can't find container item.");
            return false;
        }
        if (contained == nullptr)
        {
            LogError("Can't find contained item.");
            return false;
        }
        container->content.push_back(contained);
        contained->container = container;
        if (!contained->check(true))
        {
            LogError("Error during error checking.");
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
        item->model = Mud::getInstance().findModel(result->getNextInteger());
        item->maker = result->getNextString();
        item->condition = result->getNextInteger();
        item->composition = Mud::getInstance().findMaterial(result->getNextInteger());
        item->quality = (ItemQuality) result->getNextInteger();
        item->flags = result->getNextInteger();

        if (!item->check())
        {
            LogError("Error during error checking.");
            delete (item);
            return false;
        }

        // Add the item to the map of items.
        if (!Mud::getInstance().addItem(item))
        {
            LogError("Error during item insertion.");
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
            LogError("Error during error checking.");
            return false;
        }
        if (!Mud::getInstance().addSkill(skill))
        {
            LogError("Error during skill insertion.");
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
            LogError("Error during error checking.");
        }
        if (!Mud::getInstance().addFaction(faction))
        {
            LogError("Error during faction insertion.");
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
        model.flags = result->getNextInteger();
        model.weight = result->getNextInteger();
        model.price = result->getNextInteger();
        model.condition = result->getNextInteger();
        model.decay = result->getNextInteger();
        model.material = (MaterialType) result->getNextInteger();
        model.tileSet = result->getNextInteger();
        model.tileId = result->getNextInteger();
        model.functions = GetIntVect(result->getNextString());
        // Translate new_line.
        FindAndReplace(model.description, "%r", "\n");
        // Check the correctness.
        if (!model.check())
        {
            LogError("Error during error checking.");
            return false;
        }
        if (!Mud::getInstance().addModel(model))
        {
            LogError("Error during model insertion.");
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
        race.material = Mud::getInstance().findMaterial(result->getNextInteger());
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
            LogError("Error during error checking.");
            return false;
        }
        if (!Mud::getInstance().addRace(race))
        {
            LogError("Error during race insertion.");
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
        mobile->respawnRoom = Mud::getInstance().findRoom(result->getNextInteger());
        mobile->room = mobile->respawnRoom;
        mobile->name = result->getNextString();
        mobile->keys = GetWords(result->getNextString());
        mobile->shortdesc = result->getNextString();
        mobile->staticdesc = result->getNextString();
        mobile->description = result->getNextString();
        mobile->race = Mud::getInstance().findRace(result->getNextInteger());
        mobile->faction = Mud::getInstance().findFaction(result->getNextInteger());
        mobile->sex = result->getNextInteger();
        mobile->weight = result->getNextInteger();
        mobile->actions = GetWords(result->getNextString());
        mobile->flags = result->getNextInteger();
        mobile->level = result->getNextInteger();
        if (!mobile->setCharacteristic(result->getNextString()))
        {
            LogError("Wrong characteristics.");
            delete (mobile);
            return false;
        }
        mobile->lua_script = result->getNextString();

        mobile->health = mobile->getMaxHealth();
        mobile->stamina = mobile->getMaxStamina();

        // Translate new_line.
        FindAndReplace(mobile->description, "%r", "\n");
        // Check the correctness.
        if (!mobile->check())
        {
            LogError("Error during error checking.");
            delete (mobile);
            return false;
        }
        if (!Mud::getInstance().addMobile(mobile))
        {
            LogError("Error during mobile insertion.");
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
        room->flags = result->getNextInteger();
        // Translate new_line.
        FindAndReplace(room->description, "%r", "\n");
        // Check the correctness.
        if (!room->check())
        {
            LogError("Error during error checking.");
            delete (room);
            return false;
        }
        if (!Mud::getInstance().addRoom(room))
        {
            LogError("Error during room insertion.");
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
        Exit * exit = new Exit();
        // Retrieve the rooms vnum.
        exit->source = Mud::getInstance().findRoom(result->getNextInteger());
        exit->destination = Mud::getInstance().findRoom(result->getNextInteger());
        exit->direction = static_cast<Direction>(result->getNextInteger());
        exit->flags = result->getNextInteger();

        // Check the correctness.
        if (exit->source == nullptr)
        {
            LogError("Can't find the source room.");
            delete (exit);
            return false;
        }
        if (exit->destination == nullptr)
        {
            LogError("Can't find the destination room.");
            delete (exit);
            return false;
        }
        if (!exit->check())
        {
            LogError("Error during error checking.");
            delete (exit);
            return false;
        }
        // Push this exit into the list of exits of the source room.
        exit->source->exits.push_back(exit);
    }
    return true;
}

bool LoadItemRoom(ResultSet * result)
{
    while (result->next())
    {
        Room * room = Mud::getInstance().findRoom(result->getNextInteger());
        Item * item = Mud::getInstance().findItem(result->getNextInteger());

        // Check the correctness.
        if (room == nullptr)
        {
            LogError("Can't find the room.");
            return false;
        }
        if (item == nullptr)
        {
            LogError("Can't find the item.");
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
            LogError("Error during error checking.");
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
        // Check the correctness.
        if (!area->check())
        {
            LogError("Error during error checking.");
            return false;
        }
        if (!Mud::getInstance().addArea(area))
        {
            LogError("Error during area insertion.");
            return false;
        }
    }
    return true;
}

bool LoadAreaList(ResultSet * result)
{
    while (result->next())
    {
        Area * area = Mud::getInstance().findArea(result->getNextInteger());
        Room * room = Mud::getInstance().findRoom(result->getNextInteger());
        // Check the correctness.
        if (area == nullptr)
        {
            LogError("Can't find the area.");
            return false;
        }
        if (room == nullptr)
        {
            LogError("Can't find the room.");
            return false;
        }
        // Load the room inside the area.
        area->addRoom(room);
        if (!room->check(true))
        {
            LogError("Wrong room data.");
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
        Item * item = Mud::getInstance().findItem(writing->vnum);
        if (item == nullptr)
        {
            LogError("Can't find the item :" + ToString(writing->vnum));
            return false;
        }
        if (!Mud::getInstance().addWriting(writing))
        {
            LogError("Error during writing insertion.");
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
            LogError("Error during error checking.");
            return false;
        }
        if (!Mud::getInstance().addContinent(continent))
        {
            LogError("Error during continent insertion.");
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
            LogError("Error during error checking.");
            return false;
        }
        if (!Mud::getInstance().addMaterial(material))
        {
            LogError("Error during material insertion.");
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
            LogError("Error during error checking.");
            return false;
        }
        if (!Mud::getInstance().addProfession(professions))
        {
            LogError("Error during professions insertion.");
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
        production.profession = Mud::getInstance().findProfession(result->getNextString());
        production.difficulty = result->getNextInteger();
        production.time = result->getNextInteger();
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
            LogError("The production is incorrect " + production.name);
            return false;
        }
        if (!production.check())
        {
            LogError("Error during error checking.");
            return false;
        }
        if (!Mud::getInstance().addProduction(production))
        {
            LogError("Error during production insertion.");
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
            LogError("Error during error checking.");
            return false;
        }
        if (!Mud::getInstance().addLiquid(liquid))
        {
            LogError("Error during liquid insertion.");
            return false;
        }
    }
    return true;
}

bool LoadContentLiq(ResultSet * result)
{
    while (result->next())
    {
        Item * container = Mud::getInstance().findItem(result->getNextInteger());
        Liquid * liquid = Mud::getInstance().findLiquid(result->getNextInteger());
        int quantity = result->getNextInteger();
        bool check = true;
        if (container == nullptr)
        {
            LogError("Can't find container item.");
            check = false;
        }
        if (liquid == nullptr)
        {
            LogError("Can't find liquid.");
            check = false;
        }
        if (quantity <= 0)
        {
            LogError("Liquid content quantity misplaced.");
            check = false;
        }
        if (!check)
        {
            LogError("Error during error checking.");
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
        Area * sourceArea = Mud::getInstance().findArea(result->getNextInteger());
        Room * sourceRoom = sourceArea->getRoom(result->getNextInteger());
        Area * targetArea = Mud::getInstance().findArea(result->getNextInteger());
        Room * targetRoom = targetArea->getRoom(result->getNextInteger());
        bool check = true;
        if (sourceArea == nullptr)
        {
            LogError("Can't find the source area.");
            check = false;
        }
        if (sourceRoom == nullptr)
        {
            LogError("Can't find the source room.");
            check = false;
        }
        if (targetArea == nullptr)
        {
            LogError("Can't find the target area.");
            check = false;
        }
        if (targetRoom == nullptr)
        {
            LogError("Can't find the target room.");
            check = false;
        }
        if (!check)
        {
            LogError("Error during error checking.");
            return false;
        }
        if (!Mud::getInstance().addTravelPoint(sourceRoom, targetRoom))
        {
            LogError("Error during TravelPoint insertion.");
            return false;
        }
        if (!Mud::getInstance().addTravelPoint(targetRoom, sourceRoom))
        {
            LogError("Error during TravelPoint insertion.");
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
        building.difficulty = result->getNextInteger();
        building.time = result->getNextInteger();
        building.assisted = result->getNextInteger();
        std::vector<int> tools = GetIntVect(result->getNextString());
        building.buildingModel = Mud::getInstance().findModel(result->getNextInteger());
        std::vector<int> ingredients = GetIntVect(result->getNextString());
        building.unique = static_cast<bool>(result->getNextInteger());

        // Checker flag.
        bool check = true;
        // ////////////////////////////////////////////////////////////////
        if (building.buildingModel == nullptr)
        {
            LogError("Can't find the building model.");
            check = false;
            break;
        }

        // ////////////////////////////////////////////////////////////////
        if (tools.empty())
        {
            LogError("No tool set.");
            check = false;
        }
        for (auto it : tools)
        {
            ToolType toolType = static_cast<ToolType>(it);
            if (toolType == ToolType::NoType)
            {
                LogError("Can't find the Tool :" + ToString(it));
                check = false;
                break;
            }
            building.tools.insert(toolType);
        }

        // ////////////////////////////////////////////////////////////////
        if ((ingredients.size() % 2) != 0)
        {
            LogError("Ingredients are not even.");
            check = false;
        }
        for (std::vector<int>::iterator it = ingredients.begin(); it != ingredients.end(); it++)
        {
            ResourceType ingredient = static_cast<ResourceType>(*it);
            if (ingredient == ResourceType::NoType)
            {
                LogError("Can't find the Ingredient :" + ToString(*it));
                check = false;
                break;
            }
            ++it;
            if (it == ingredients.end())
            {
                LogError("Can't find the quantity of the Ingredient :" + GetResourceTypeName(ingredient));
                check = false;
                break;
            }
            building.ingredients[ingredient] = *it;
        }
        // Check the correctness.
        if (!check)
        {
            LogError("The building is incorrect " + building.name);
            return false;
        }
        if (!building.check())
        {
            LogError("Error during error checking.");
            return false;
        }
        if (!Mud::getInstance().addBuilding(building))
        {
            LogError("Error during building insertion.");
            return false;
        }
    }
    return true;
}
