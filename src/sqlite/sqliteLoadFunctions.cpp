/// @file   sqliteLoadFunctions.cpp
/// @author Enrico Fraccaroli
/// @date   Aug 4 2016
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

#include "sqliteLoadFunctions.hpp"
#include "sqliteException.hpp"

#include "liquidContainerItem.hpp"
#include "currencyModel.hpp"
#include "modelFactory.hpp"
#include "itemFactory.hpp"
#include "shopItem.hpp"
#include "logger.hpp"
#include "mud.hpp"

bool LoadBadName(ResultSet * result)
{
    while (result->next())
    {
        try
        {
            if (!Mud::instance().badNames.insert(
                result->getNextString()).second)
            {
                throw SQLiteException("Error during bad name loading.");
            }
        }
        catch (SQLiteException & e)
        {
            Logger::log(LogLevel::Error, std::string(e.what()));
            return false;
        }
    }
    return true;
}

bool LoadBlockedIp(ResultSet * result)
{
    while (result->next())
    {
        try
        {
            if (!Mud::instance().blockedIPs.insert(
                result->getNextString()).second)
            {
                throw SQLiteException("Error during blocked ips loading.");
            }
        }
        catch (SQLiteException & e)
        {
            Logger::log(LogLevel::Error, std::string(e.what()));
            return false;
        }
    }
    return true;
}

bool LoadNews(ResultSet * result)
{
    while (result->next())
    {
        try
        {
            if (!Mud::instance().mudNews.insert(
                std::make_pair(result->getNextString(),
                               result->getNextString())).second)
            {
                throw SQLiteException("Error during news loading.");
            }
        }
        catch (SQLiteException & e)
        {
            Logger::log(LogLevel::Error, std::string(e.what()));
            return false;
        }
    }
    return true;
}

bool LoadContent(ResultSet * result)
{
    while (result->next())
    {
        try
        {
            auto container = Mud::instance().findItem(result->getNextInteger());
            if (container == nullptr)
            {
                throw SQLiteException("Can't find container item.");
            }
            auto contained = Mud::instance().findItem(result->getNextInteger());
            if (contained == nullptr)
            {
                throw SQLiteException("Can't find contained item.");
            }
            container->putInside(contained, false);
        }
        catch (SQLiteException & e)
        {
            Logger::log(LogLevel::Error, std::string(e.what()));
            return false;
        }
    }
    return true;
}

bool LoadItem(ResultSet * result)
{
    while (result->next())
    {
        try
        {
            auto itemVnum = result->getNextInteger();
            auto itemModel = Mud::instance().findItemModel(
                result->getNextInteger());
            if (itemModel == nullptr)
            {
                throw SQLiteException(
                    "Item has wrong model (" + ToString(itemVnum) + ")");
            }
            // Create the item.
            auto item = ItemFactory::newItem(itemModel->getType());
            item->vnum = itemVnum;
            item->model = itemModel;
            item->quantity = result->getNextUnsignedInteger();
            item->maker = result->getNextString();
            item->price = result->getNextUnsignedInteger();
            item->weight = result->getNextDouble();
            item->condition = result->getNextDouble();
            item->maxCondition = result->getNextDouble();
            item->composition = Mud::instance().findMaterial(
                result->getNextInteger());
            item->quality = ItemQuality(result->getNextUnsignedInteger());
            item->flags = result->getNextUnsignedInteger();
            // Check correctness of attributes.
            if (!item->check())
            {
                delete (item);
                throw SQLiteException("Error during error checking.");
            }
            // Add the item to the map of items.
            if (!Mud::instance().addItem(item))
            {
                delete (item);
                throw SQLiteException("Error during error checking.");
            }
        }
        catch (SQLiteException & e)
        {
            Logger::log(LogLevel::Error, std::string(e.what()));
            return false;
        }
    }
    return true;
}

bool LoadSkill(ResultSet * result)
{
    while (result->next())
    {
        try
        {
            // Create an empty Skill.
            auto skill = std::make_shared<Skill>();
            skill->vnum = result->getNextInteger();
            skill->name = result->getNextString();
            skill->description = result->getNextString();
            skill->attribute = result->getNextInteger();
            skill->stage = result->getNextInteger();
            // Check the correctness.
            if (!skill->check())
            {
                throw SQLiteException("Error during error checking.");
            }
            if (!Mud::instance().addSkill(skill))
            {
                throw SQLiteException("Error during skill insertion.");
            }
        }
        catch (SQLiteException & e)
        {
            Logger::log(LogLevel::Error, std::string(e.what()));
            return false;
        }
    }
    return true;
}

bool LoadSkillRequirements(ResultSet * result)
{
    while (result->next())
    {
        try
        {
            auto skillVnum = result->getNextInteger();
            auto requiredSkillVnum = result->getNextInteger();
            auto requiredLevel = result->getNextInteger();

            auto skill = Mud::instance().findSkill(skillVnum);
            auto requiredSkill = Mud::instance().findSkill(requiredSkillVnum);
            if (skill == nullptr)
            {
                throw SQLiteException("Can't find the skill " +
                                      ToString(skillVnum));
            }
            if (requiredSkill == nullptr)
            {
                throw SQLiteException("Can't find the skill " +
                                      ToString(requiredSkillVnum) +
                                      " required by the skill " +
                                      ToString(skillVnum));
            }
            skill->requiredSkills.emplace_back(
                std::make_pair(requiredSkill,
                               requiredLevel));
        }
        catch (SQLiteException & e)
        {
            Logger::log(LogLevel::Error, std::string(e.what()));
            return false;
        }
    }
    return true;
}

bool LoadFaction(ResultSet * result)
{
    while (result->next())
    {
        try
        {
            // Create an empty Faction.
            auto faction = new Faction();
            faction->vnum = result->getNextInteger();
            faction->name = result->getNextString();
            faction->description = result->getNextString();
            auto currencyVnum = result->getNextInteger();
            auto currencyModel = Mud::instance().findItemModel(currencyVnum);
            if (currencyModel == nullptr)
            {
                throw SQLiteException(
                    "Can't find the currency " + ToString(currencyVnum));
            }
            if (currencyModel->getType() != ModelType::Currency)
            {
                throw SQLiteException(
                    "Model is not currency " + ToString(currencyVnum));
            }
            faction->currency = currencyModel->toCurrency();
            // Translate new_line.
            FindAndReplace(&faction->description, "%r", "\n");
            // Check the correctness.
            if (!faction->check())
            {
                throw SQLiteException("Error during error checking.");
            }
            if (!Mud::instance().addFaction(faction))
            {
                throw SQLiteException("Error during faction insertion.");
            }
        }
        catch (SQLiteException & e)
        {
            Logger::log(LogLevel::Error, std::string(e.what()));
            return false;
        }
    }
    return true;
}

bool LoadModel(ResultSet * result)
{
    while (result->next())
    {
        try
        {
            // Retrieve the vnum and the type of model.
            auto vnum = result->getNextInteger();
            // Create a pointer to the new item model.
            auto itemModel = ModelFactory::newModel(
                ModelType(result->getNextUnsignedInteger()));
            if (itemModel == nullptr)
            {
                throw SQLiteException("Wrong type of model " + ToString(vnum));
            }
            // Set the values of the new model.
            itemModel->vnum = vnum;
            itemModel->name = result->getNextString();
            itemModel->article = result->getNextString();
            itemModel->shortdesc = result->getNextString();
            itemModel->keys = SplitString(result->getNextString(), " ");
            itemModel->description = result->getNextString();
            itemModel->modelFlags = result->getNextUnsignedInteger();
            itemModel->baseWeight = result->getNextDouble();
            itemModel->basePrice = result->getNextUnsignedInteger();
            itemModel->condition = result->getNextDouble();
            itemModel->material = MaterialType(
                result->getNextUnsignedInteger());
            itemModel->tileSet = result->getNextInteger();
            itemModel->tileId = result->getNextInteger();
            if (!itemModel->setModel(result->getNextString()))
            {
                throw SQLiteException("Error when setting the model.");
            }
            // Translate new_line.
            FindAndReplace(&itemModel->description, "%r", "\n");
            // Check the correctness.
            if (!itemModel->check())
            {
                throw SQLiteException("Error during error checking.");
            }
            if (!Mud::instance().addItemModel(itemModel))
            {
                throw SQLiteException("Error during itemModel insertion.");
            }
        }
        catch (SQLiteException & e)
        {
            Logger::log(LogLevel::Error, std::string(e.what()));
            return false;
        }
    }
    return true;
}

bool LoadRace(ResultSet * result)
{
    while (result->next())
    {
        try
        {
            // Create an empty Race.
            auto race = new Race();
            race->vnum = result->getNextInteger();
            race->article = result->getNextString();
            race->name = result->getNextString();
            race->description = result->getNextString();
            race->material = Mud::instance().findMaterial(
                result->getNextInteger());
            race->setAbilities(result->getNextString());
            if (!race->setAvailableFactions(result->getNextString()))
            {
                throw SQLiteException("Error when setting race factions.");
            }
            race->player_allow = result->getNextInteger();
            race->tileSet = result->getNextInteger();
            race->tileId = result->getNextInteger();
            std::string corpseDescription = result->getNextString();
            race->naturalWeapon = result->getNextString();
            // Translate new_line.
            FindAndReplace(&race->description, "%r", "\n");
            // Intialize the corpse.
            race->initializeCorpse(corpseDescription);
            // Check the correctness.
            if (!race->check())
            {
                throw SQLiteException("Error during error checking.");
            }
            if (!Mud::instance().addRace(race))
            {
                throw SQLiteException("Error during race insertion.");
            }
        }
        catch (SQLiteException & e)
        {
            Logger::log(LogLevel::Error, std::string(e.what()));
            return false;
        }
    }
    return true;
}

bool LoadMobile(ResultSet * result)
{
    while (result->next())
    {
        try
        {
            // Create an empty Mobile.
            auto mobile = new Mobile();
            // Initialize the mobile.
            mobile->id = result->getNextString();
            mobile->respawnRoom = Mud::instance().findRoom(
                result->getNextInteger());
            mobile->room = mobile->respawnRoom;
            mobile->name = result->getNextString();
            mobile->keys = GetWords(result->getNextString());
            mobile->shortdesc = result->getNextString();
            mobile->staticdesc = result->getNextString();
            mobile->description = result->getNextString();
            mobile->race = Mud::instance().findRace(result->getNextInteger());
            mobile->faction = Mud::instance().findFaction(
                result->getNextInteger());
            mobile->gender = static_cast<GenderType>(result->getNextInteger());
            mobile->weight = result->getNextDouble();
            mobile->actions = GetWords(result->getNextString());
            mobile->flags = result->getNextUnsignedInteger();
            mobile->level = result->getNextUnsignedInteger();
            if (!mobile->setAbilities(result->getNextString()))
            {
                delete (mobile);
                throw SQLiteException("Wrong characteristics.");
            }
            mobile->lua_script = result->getNextString();

            mobile->setHealth(mobile->getMaxHealth(), true);
            mobile->setStamina(mobile->getMaxStamina(), true);

            // Translate new_line.
            FindAndReplace(&mobile->description, "%r", "\n");
            // Check the correctness.
            if (!mobile->check())
            {
                delete (mobile);
                throw SQLiteException("Error during error checking.");
            }
            if (!Mud::instance().addMobile(mobile))
            {
                delete (mobile);
                throw SQLiteException("Error during mobile insertion.");
            }
            // Load the script.
            mobile->loadScript(
                Mud::instance().getMudSystemDirectory() + "lua/" +
                mobile->lua_script);
            // Respawn it.
            mobile->respawn(true);
        }
        catch (SQLiteException & e)
        {
            Logger::log(LogLevel::Error, std::string(e.what()));
            return false;
        }
    }
    return true;
}

bool LoadRoom(ResultSet * result)
{
    while (result->next())
    {
        try
        {
            // Create an empty Room.
            auto room = new Room();
            // Initialize the Room.
            room->vnum = result->getNextInteger();
            room->coord.x = result->getNextInteger();
            room->coord.y = result->getNextInteger();
            room->coord.z = result->getNextInteger();
            room->terrain = Mud::instance().findTerrain(
                result->getNextUnsignedInteger());
            room->name = result->getNextString();
            room->description = result->getNextString();
            room->flags = result->getNextUnsignedInteger();
            // Translate new_line.
            FindAndReplace(&room->description, "%r", "\n");
            // Check the correctness.
            if (!room->check())
            {
                delete (room);
                throw SQLiteException("Error during error checking.");
            }
            if (!Mud::instance().addRoom(room))
            {
                delete (room);
                throw SQLiteException("Error during room insertion.");
            }
        }
        catch (SQLiteException & e)
        {
            Logger::log(LogLevel::Error, std::string(e.what()));
            return false;
        }
    }
    return true;
}

bool LoadExit(ResultSet * result)
{
    while (result->next())
    {
        try
        {
            // Create an empty exit.
            auto newExit = std::make_shared<Exit>();
            // retrive the values.
            auto sourceVnum = result->getNextInteger();
            auto destinationVnum = result->getNextInteger();
            auto directionValue = result->getNextUnsignedInteger();
            auto flagValue = result->getNextUnsignedInteger();
            // Check the correctness.
            newExit->source = Mud::instance().findRoom(sourceVnum);
            if (newExit->source == nullptr)
            {
                throw SQLiteException(
                    "Can't find source " + ToString(sourceVnum));
            }
            newExit->destination = Mud::instance().findRoom(destinationVnum);
            if (newExit->destination == nullptr)
            {
                throw SQLiteException(
                    "Can't find destination " + ToString(destinationVnum));
            }
            if (!Direction::isValid(directionValue))
            {
                throw SQLiteException(
                    "Direction si not valid " + ToString(directionValue));
            }
            newExit->direction = Direction(directionValue);
            newExit->flags = flagValue;
            if (!newExit->check())
            {
                throw SQLiteException("Error during error checking.");
            }
            // Push this exit into the list of exits of the source room.
            newExit->source->addExit(newExit);
        }
        catch (SQLiteException & e)
        {
            Logger::log(LogLevel::Error, std::string(e.what()));
            return false;
        }
    }
    return true;
}

bool LoadItemRoom(ResultSet * result)
{
    while (result->next())
    {
        try
        {
            auto room = Mud::instance().findRoom(result->getNextInteger());
            auto item = Mud::instance().findItem(result->getNextInteger());
            // Check the correctness.
            if (room == nullptr)
            {
                throw SQLiteException("Can't find the room.");
            }
            if (item == nullptr)
            {
                throw SQLiteException("Can't find the item.");
            }
            // Load the item inside the room.
            if (HasFlag(item->flags, ItemFlag::Built))
            {
                room->addBuilding(item, false);
            }
            else
            {
                room->addItem(item, false);
            }
        }
        catch (SQLiteException & e)
        {
            Logger::log(LogLevel::Error, std::string(e.what()));
            return false;
        }
    }
    return true;
}

bool LoadArea(ResultSet * result)
{
    while (result->next())
    {
        try
        {
            // Create an empty area.
            auto area = new Area();
            // Initialize the area.
            area->vnum = result->getNextInteger();
            area->name = result->getNextString();
            area->builder = result->getNextString();
            area->width = result->getNextInteger();
            area->height = result->getNextInteger();
            area->elevation = result->getNextInteger();
            area->tileSet = result->getNextInteger();
            area->type = static_cast<AreaType>(result->getNextUnsignedInteger());
            area->status = static_cast<AreaStatus>(result->getNextUnsignedInteger());
            // Check the correctness.
            if (!area->check())
            {
                throw SQLiteException("Error during error checking.");
            }
            if (!Mud::instance().addArea(area))
            {
                throw SQLiteException("Error during area insertion.");
            }
        }
        catch (SQLiteException & e)
        {
            Logger::log(LogLevel::Error, std::string(e.what()));
            return false;
        }
    }
    return true;
}

bool LoadAreaList(ResultSet * result)
{
    while (result->next())
    {
        try
        {
            int areaVnum = result->getNextInteger();
            int roomVnum = result->getNextInteger();
            auto area = Mud::instance().findArea(areaVnum);
            auto room = Mud::instance().findRoom(roomVnum);
            // Check the correctness.
            if (area == nullptr)
            {
                throw SQLiteException(
                    "Can't find the area " + ToString(areaVnum));
            }
            if (room == nullptr)
            {
                throw SQLiteException(
                    "Can't find the room " + ToString(roomVnum));
            }
            // Load the room inside the area.
            area->addRoom(room);
            if (!room->check(true))
            {
                throw SQLiteException("Wrong room data.");
            }
        }
        catch (SQLiteException & e)
        {
            Logger::log(LogLevel::Error, std::string(e.what()));
            return false;
        }
    }
    return true;
}

bool LoadWriting(ResultSet * result)
{
    while (result->next())
    {
        try
        {
            // Create an empty Writing.
            auto writing = new Writing();
            // Initialize the Writing.
            writing->vnum = result->getNextInteger();
            writing->title = result->getNextString();
            writing->author = result->getNextString();
            writing->content = result->getNextString();
            // Fid the item on which the writing is attached.
            auto item = Mud::instance().findItem(writing->vnum);
            if (item == nullptr)
            {
                throw SQLiteException(
                    "Can't find the item " + ToString(writing->vnum));
            }
            if (!Mud::instance().addWriting(writing))
            {
                throw SQLiteException("Error during writing insertion.");
            }
        }
        catch (SQLiteException & e)
        {
            Logger::log(LogLevel::Error, std::string(e.what()));
            return false;
        }
    }
    return true;
}

bool LoadContinent(ResultSet * result)
{
    while (result->next())
    {
        try
        {
            auto continent = new Continent();
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
                throw SQLiteException("Error during error checking.");
            }
            if (!Mud::instance().addContinent(continent))
            {
                throw SQLiteException("Error during continent insertion.");
            }
        }
        catch (SQLiteException & e)
        {
            Logger::log(LogLevel::Error, std::string(e.what()));
            return false;
        }
    }
    return true;
}

bool LoadMaterial(ResultSet * result)
{
    while (result->next())
    {
        try
        {
            // Create an empty Material.
            auto material = new Material();
            // Intialize the material.
            material->vnum = result->getNextInteger();
            material->type = MaterialType(result->getNextUnsignedInteger());
            material->name = result->getNextString();
            material->article = result->getNextString();
            material->worth = result->getNextUnsignedInteger();
            material->hardness = result->getNextUnsignedInteger();
            material->lightness = result->getNextUnsignedInteger();
            // Check the correctness.
            if (!material->check())
            {
                throw SQLiteException("Error during error checking.");
            }
            if (!Mud::instance().addMaterial(material))
            {
                throw SQLiteException("Error during material insertion.");
            }
        }
        catch (SQLiteException & e)
        {
            Logger::log(LogLevel::Error, std::string(e.what()));
            return false;
        }
    }
    return true;
}

bool LoadProfession(ResultSet * result)
{
    while (result->next())
    {
        try
        {
            // Create an empty Profession.
            auto professions = new Profession();
            // Initialize the profession.
            professions->vnum = result->getNextUnsignedInteger();
            professions->name = result->getNextString();
            professions->description = result->getNextString();
            professions->command = result->getNextString();
            professions->posture = CharacterPosture(
                result->getNextUnsignedInteger());
            professions->action = result->getNextString();
            professions->startMessage = result->getNextString();
            professions->finishMessage = result->getNextString();
            professions->successMessage = result->getNextString();
            professions->failureMessage = result->getNextString();
            professions->interruptMessage = result->getNextString();
            professions->notFoundMessage = result->getNextString();
            // Check the correctness.
            if (!professions->check())
            {
                throw SQLiteException("Error during error checking.");
            }
            if (!Mud::instance().addProfession(professions))
            {
                throw SQLiteException("Error during professions insertion.");
            }
        }
        catch (SQLiteException & e)
        {
            Logger::log(LogLevel::Error, std::string(e.what()));
            return false;
        }
    }
    return true;
}

bool LoadProduction(ResultSet * result)
{
    while (result->next())
    {
        try
        {
            // Checker flag.
            bool check = true;
            // Create an empty Production.
            auto production = new Production();
            // Initialize the Production.
            production->vnum = result->getNextInteger();
            production->name = result->getNextString();
            production->profession = Mud::instance().findProfession(
                result->getNextString());
            production->difficulty = result->getNextUnsignedInteger();
            production->time = result->getNextUnsignedInteger();
            production->assisted = result->getNextInteger();
            check &= production->setOutcome(result->getNextString());
            check &= production->setTool(result->getNextString());
            check &= production->setIngredient(result->getNextString());
            production->material = ResourceType(
                result->getNextUnsignedInteger());
            production->workbench = ToolType(result->getNextUnsignedInteger());
            // ////////////////////////////////////////////////////////////////
            // Check the correctness.
            if (!check)
            {
                throw SQLiteException(
                    "The production is incorrect " + production->name);
            }
            if (!production->check())
            {
                throw SQLiteException("Error during error checking.");
            }
            if (!Mud::instance().addProduction(production))
            {
                throw SQLiteException("Error during production insertion.");
            }
        }
        catch (SQLiteException & e)
        {
            Logger::log(LogLevel::Error, std::string(e.what()));
            return false;
        }
    }
    return true;
}

bool LoadLiquid(ResultSet * result)
{
    while (result->next())
    {
        try
        {
            // Create an empty Liquid.
            auto liquid = new Liquid();
            // Load the liquid.
            liquid->vnum = result->getNextInteger();
            liquid->type = LiquidType(result->getNextUnsignedInteger());
            liquid->name = result->getNextString();
            liquid->description = result->getNextString();
            liquid->worth = result->getNextInteger();
            liquid->quench = result->getNextDouble();
            // Check the correctness.
            if (!liquid->check())
            {
                throw SQLiteException("Error during error checking.");
            }
            if (!Mud::instance().addLiquid(liquid))
            {
                throw SQLiteException("Error during liquid insertion.");
            }
        }
        catch (SQLiteException & e)
        {
            Logger::log(LogLevel::Error, std::string(e.what()));
            return false;
        }
    }
    return true;
}

bool LoadContentLiq(ResultSet * result)
{
    while (result->next())
    {
        try
        {
            auto container = Mud::instance().findItem(result->getNextInteger());
            auto liquid = Mud::instance().findLiquid(result->getNextInteger());
            auto quantity = result->getNextDouble();
            if (container == nullptr)
            {
                throw SQLiteException("Can't find container item.");
            }
            if (container->getType() != ModelType::LiquidContainer)
            {
                throw SQLiteException("Wrong container for liquids.");
            }
            if (liquid == nullptr)
            {
                throw SQLiteException("Can't find liquid.");
            }
            // Cast the item to liquid container.
            auto liquidContainer = static_cast<LiquidContainerItem *>(container);
            // Pour in the liquid.
            liquidContainer->pourIn(liquid, quantity, false);
        }
        catch (SQLiteException & e)
        {
            Logger::log(LogLevel::Error, std::string(e.what()));
            return false;
        }
    }
    return true;
}

bool LoadTravelPoint(ResultSet * result)
{
    while (result->next())
    {
        try
        {
            auto sourceArea = Mud::instance().findArea(
                result->getNextInteger());
            if (sourceArea == nullptr)
            {
                throw SQLiteException("Can't find the source area.");
            }
            auto sourceRoom = sourceArea->getRoom(result->getNextInteger());
            if (sourceRoom == nullptr)
            {
                throw SQLiteException("Can't find the source room.");
            }
            auto targetArea = Mud::instance().findArea(
                result->getNextInteger());
            if (targetArea == nullptr)
            {
                throw SQLiteException("Can't find the target area.");
            }
            auto targetRoom = targetArea->getRoom(result->getNextInteger());
            if (targetRoom == nullptr)
            {
                throw SQLiteException("Can't find the target room.");
            }
            if (!Mud::instance().addTravelPoint(sourceRoom, targetRoom))
            {
                throw SQLiteException("Error during TravelPoint insertion.");
            }
            if (!Mud::instance().addTravelPoint(targetRoom, sourceRoom))
            {
                throw SQLiteException("Error during TravelPoint insertion.");
            }
            SetFlag(&sourceRoom->flags, RoomFlag::TravelPoint);
            SetFlag(&targetRoom->flags, RoomFlag::TravelPoint);
        }
        catch (SQLiteException & e)
        {
            Logger::log(LogLevel::Error, std::string(e.what()));
            return false;
        }
    }
    return true;
}

bool LoadBuilding(ResultSet * result)
{
    while (result->next())
    {
        try
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
            building.buildingModel = Mud::instance().findItemModel(
                result->getNextInteger());
            building.setIngredient(result->getNextString());
            building.unique = static_cast<bool>(result->getNextInteger());
            if (building.buildingModel == nullptr)
            {
                throw SQLiteException("Can't find the building itemModel.");
            }
            if (!building.check())
            {
                throw SQLiteException("Error during error checking.");
            }
            if (!Mud::instance().addBuilding(building))
            {
                throw SQLiteException("Error during building insertion.");
            }
        }
        catch (SQLiteException & e)
        {
            Logger::log(LogLevel::Error, std::string(e.what()));
            return false;
        }
    }
    return true;
}

bool LoadShop(ResultSet * result)
{
    while (result->next())
    {
        try
        {
            // Retrieve the item vnum.
            auto vnum = result->getNextInteger();
            auto item = Mud::instance().findItem(vnum);
            if (item == nullptr)
            {
                throw SQLiteException("Can't find the item " + ToString(vnum));
            }
            if (item->getType() != ModelType::Shop)
            {
                throw SQLiteException("Wrong type of item " + ToString(vnum));
            }
            // Cast the item to shop.
            auto shop = static_cast<ShopItem *>(item);
            shop->shopName = result->getNextString();
            shop->shopBuyTax = result->getNextUnsignedInteger();
            shop->shopSellTax = result->getNextUnsignedInteger();
            shop->balance = result->getNextUnsignedInteger();
            shop->shopKeeper = Mud::instance().findMobile(
                result->getNextString());
            shop->openingHour = result->getNextUnsignedInteger();
            shop->closingHour = result->getNextUnsignedInteger();
            if (shop->shopKeeper != nullptr)
            {
                shop->shopKeeper->managedItem = shop;
            }
        }
        catch (SQLiteException & e)
        {
            Logger::log(LogLevel::Error, std::string(e.what()));
            return false;
        }
    }
    return true;
}

bool LoadCurrency(ResultSet * result)
{
    while (result->next())
    {
        try
        {
            // Retrieve the item vnum.
            auto modelVnum = result->getNextInteger();
            auto materialVnum = result->getNextInteger();
            auto worth = result->getNextUnsignedInteger();

            auto model = Mud::instance().findItemModel(modelVnum);
            if (model == nullptr)
            {
                throw SQLiteException(
                    "Can't find the model " + ToString(modelVnum));
            }
            if (model->getType() != ModelType::Currency)
            {
                throw SQLiteException(
                    "Wrong type of model " + ToString(modelVnum));
            }
            auto material = Mud::instance().findMaterial(materialVnum);
            if (material == nullptr)
            {
                throw SQLiteException(
                    "Can't find the material " + ToString(materialVnum));
            }
            auto currency = model->toCurrency();
            if (!currency->addPrice(materialVnum, worth))
            {
                throw SQLiteException(
                    "Can't add the price for " + ToString(modelVnum));
            }
        }
        catch (SQLiteException & e)
        {
            Logger::log(LogLevel::Error, std::string(e.what()));
            return false;
        }
    }
    return true;
}

bool LoadTerrain(ResultSet * result)
{
    while (result->next())
    {
        try
        {
            std::shared_ptr<Terrain> terrain = std::make_shared<Terrain>();
            terrain->vnum = result->getNextUnsignedInteger();
            terrain->name = result->getNextString();
            terrain->flags = result->getNextUnsignedInteger();
            terrain->space = result->getNextUnsignedInteger();
            terrain->indoor = result->getNextUnsignedInteger();
            terrain->light = result->getNextUnsignedInteger();
            if (!Mud::instance().addTerrain(terrain))
            {
                throw SQLiteException(
                    "Can't add the terrain " + ToString(terrain->vnum) + " - " +
                    terrain->name);
            }
        }
        catch (SQLiteException & e)
        {
            Logger::log(LogLevel::Error, std::string(e.what()));
            return false;
        }
    }
    return true;
}

bool LoadBodyPart(ResultSet * result)
{
    while (result->next())
    {
        try
        {
            std::shared_ptr<BodyPart> bodyPart = std::make_shared<BodyPart>();
            bodyPart->vnum = result->getNextUnsignedInteger();
            bodyPart->name = result->getNextString();
            bodyPart->description = result->getNextString();
            bodyPart->flags = result->getNextUnsignedInteger();
            if (bodyPart->check())
            {
                Mud::instance().mudBodyParts.emplace_back(bodyPart);
            }
        }
        catch (SQLiteException & e)
        {
            Logger::log(LogLevel::Error, std::string(e.what()));
            return false;
        }
    }
    return true;
}

bool LoadRaceBodyPart(ResultSet * result)
{
    while (result->next())
    {
        try
        {
            auto race = Mud::instance().findRace(result->getNextInteger());
            auto bodyPart = Mud::instance().findBodyPart(
                result->getNextUnsignedInteger());
            assert(race != nullptr);
            assert(bodyPart != nullptr);
            race->bodyParts.emplace_back(bodyPart);
        }
        catch (SQLiteException & e)
        {
            Logger::log(LogLevel::Error, std::string(e.what()));
            return false;
        }
    }
    return true;
}

bool LoadModelBodyPart(ResultSet * result)
{
    while (result->next())
    {
        try
        {
            auto model = Mud::instance().findItemModel(
                result->getNextInteger());
            auto bodyPart = Mud::instance().findBodyPart(
                result->getNextUnsignedInteger());
            assert(model != nullptr);
            assert(bodyPart != nullptr);
            model->bodyParts.emplace_back(bodyPart);
        }
        catch (SQLiteException & e)
        {
            Logger::log(LogLevel::Error, std::string(e.what()));
            return false;
        }
    }
    return true;
}
