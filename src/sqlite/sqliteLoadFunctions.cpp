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

#include "currencyModel.hpp"
#include "modelFactory.hpp"
#include "itemFactory.hpp"
#include "sqliteDbms.hpp"
#include "shopItem.hpp"
#include "logger.hpp"
#include "mud.hpp"

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
        if (!Mud::instance().mudNews.insert(
            std::make_pair(result->getNextString(), result->getNextString())).second)
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
        auto container = Mud::instance().findItem(result->getNextInteger());
        auto contained = Mud::instance().findItem(result->getNextInteger());
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
        container->putInside(contained, false);
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
        // Retrieve the values.
        auto itemVnum = result->getNextInteger();
        auto itemModelVnum = result->getNextInteger();
        auto itemQuantity = result->getNextUnsignedInteger();
        auto itemMaker = result->getNextString();
        auto itemPrice = result->getNextUnsignedInteger();
        auto itemWeight = result->getNextDouble();
        auto itemCondition = result->getNextUnsignedInteger();
        auto itemMaxCondition = result->getNextUnsignedInteger();
        auto itemCompositionVnum = result->getNextInteger();
        auto itemQualityValue = result->getNextUnsignedInteger();
        auto itemFlags = result->getNextUnsignedInteger();

        // Retrieve the model vnum.
        auto itemModel = Mud::instance().findItemModel(itemModelVnum);
        if (itemModel == nullptr)
        {
            Logger::log(LogLevel::Error, "Item has wrong model (%s)", ToString(itemModelVnum));
            return false;
        }
        // Check the dynamic attributes.
        auto itemComposition = Mud::instance().findMaterial(itemCompositionVnum);
        if (itemComposition == nullptr)
        {
            Logger::log(
                LogLevel::Error,
                "Item has wrong material (%s)",
                ToString(itemCompositionVnum));
            return false;
        }
        if (!ItemQuality::isValid(itemQualityValue))
        {
            Logger::log(LogLevel::Error, "Item has wrong quality (%s)", ToString(itemQualityValue));
            return false;
        }
        // Set the item values.
        auto item = ItemFactory::newItem(itemModel->getType());
        item->vnum = itemVnum;
        item->model = itemModel;
        item->quantity = itemQuantity;
        item->price = itemPrice;
        item->weight = itemWeight;
        item->condition = itemCondition;
        item->maxCondition = itemMaxCondition;
        item->maker = itemMaker;
        item->composition = itemComposition;
        item->quality = ItemQuality(itemQualityValue);
        item->flags = itemFlags;
        // Check correctness of attributes.
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
        auto skill = new Skill();
        skill->vnum = result->getNextInteger();
        skill->name = result->getNextString();
        skill->description = result->getNextString();
        skill->attribute = result->getNextInteger();
        // Check the correctness.
        if (!skill->check())
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
        auto vnum = result->getNextInteger();
        auto name = result->getNextString();
        auto description = result->getNextString();
        auto currencyVnum = result->getNextInteger();
        auto currencyModel = Mud::instance().findItemModel(currencyVnum);
        if (currencyModel == nullptr)
        {
            Logger::log(LogLevel::Error, "Can't find the currency %s", ToString(currencyVnum));
            return false;
        }
        if (currencyModel->getType() != ModelType::Currency)
        {
            Logger::log(LogLevel::Error, "Model is not currency %s", ToString(currencyVnum));
            return false;
        }
        auto currency = currencyModel->toCurrency();
        // Create an empty Faction.
        auto faction = new Faction();
        faction->vnum = vnum;
        faction->name = name;
        faction->description = description;
        faction->currency = currency;
        // Translate new_line.
        FindAndReplace(&faction->description, "%r", "\n");
        // Check the correctness.
        if (!faction->check())
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
        // Retrieve the vnum and the type of model.
        auto vnum = result->getNextInteger();
        auto type = ModelType(result->getNextUnsignedInteger());
        // Create a pointer to the new item model.
        auto itemModel = ModelFactory::newModel(type);
        if (itemModel == nullptr)
        {
            Logger::log(LogLevel::Error, "Wrong type of model %s.", ToString(vnum));
            return false;
        }
        // Set the values of the new model.
        itemModel->vnum = vnum;
        itemModel->name = result->getNextString();
        itemModel->article = result->getNextString();
        itemModel->shortdesc = result->getNextString();
        itemModel->keys = SplitString(result->getNextString(), " ");
        itemModel->description = result->getNextString();
        itemModel->slot = EquipmentSlot(result->getNextUnsignedInteger());
        itemModel->modelFlags = result->getNextUnsignedInteger();
        itemModel->baseWeight = result->getNextDouble();
        itemModel->basePrice = result->getNextUnsignedInteger();
        itemModel->condition = result->getNextUnsignedInteger();
        itemModel->decay = result->getNextUnsignedInteger();
        itemModel->material = static_cast<MaterialType>(result->getNextInteger());
        itemModel->tileSet = result->getNextInteger();
        itemModel->tileId = result->getNextInteger();
        if (!itemModel->setModel(result->getNextString()))
        {
            Logger::log(LogLevel::Error, "Error when setting the model.");
            return false;
        }
        //if (!itemModel->setFunctions(result->getNextString()))
        //{
        //    Logger::log(LogLevel::Error, "Wrong number of function arguments %s.", ToString(vnum));
        //    return false;
        //}
        // Translate new_line.
        FindAndReplace(&itemModel->description, "%r", "\n");
        // Check the correctness.
        if (!itemModel->check())
        {
            Logger::log(LogLevel::Error, "Error during error checking.");
            return false;
        }
        if (!Mud::instance().addItemModel(itemModel))
        {
            Logger::log(LogLevel::Error, "Error during itemModel insertion.");
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
        auto race = new Race();
        race->vnum = result->getNextInteger();
        race->article = result->getNextString();
        race->name = result->getNextString();
        race->description = result->getNextString();
        race->material = Mud::instance().findMaterial(result->getNextInteger());
        race->setAbilities(result->getNextString());
        if (!race->setAvailableFactions(result->getNextString()))
        {
            Logger::log(LogLevel::Error, "Error when setting race factions.");
            return false;
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
        auto mobile = new Mobile();
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
        FindAndReplace(&mobile->description, "%r", "\n");
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
        // Load the script.
        mobile->loadScript(Mud::instance().getMudSystemDirectory() + "lua/" + mobile->lua_script);
        // Respawn it.
        mobile->respawn(true);
    }
    return true;
}

bool LoadRoom(ResultSet * result)
{
    while (result->next())
    {
        // Create an empty Room.
        auto room = new Room();
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
        FindAndReplace(&room->description, "%r", "\n");
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
            Logger::log(LogLevel::Error, "Can't find source (%s).", ToString(sourceVnum));
            return false;
        }
        newExit->destination = Mud::instance().findRoom(destinationVnum);
        if (newExit->destination == nullptr)
        {
            Logger::log(LogLevel::Error, "Can't find destination (%s).", ToString(destinationVnum));
            return false;
        }
        if (!Direction::isValid(directionValue))
        {
            Logger::log(LogLevel::Error, "Direction si not valid (%s).", ToString(directionValue));
            return false;
        }
        newExit->direction = Direction(directionValue);
        newExit->flags = flagValue;
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
        auto room = Mud::instance().findRoom(result->getNextInteger());
        auto item = Mud::instance().findItem(result->getNextInteger());
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
            room->addBuilding(item, false);
        }
        else
        {
            room->addItem(item, false);
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
        auto area = new Area();
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
        auto area = Mud::instance().findArea(areaVnum);
        auto room = Mud::instance().findRoom(roomVnum);
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
        auto material = new Material();
        // Intialize the material.
        material->vnum = result->getNextInteger();
        material->type = static_cast<MaterialType>(result->getNextInteger());
        material->name = result->getNextString();
        material->article = result->getNextString();
        material->worth = result->getNextUnsignedInteger();
        material->hardness = result->getNextUnsignedInteger();
        material->lightness = result->getNextUnsignedInteger();
        // Check the correctness.
        if (!material->check())
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
        auto professions = new Profession();
        // Initialize the profession.
        professions->vnum = result->getNextUnsignedInteger();
        professions->name = result->getNextString();
        professions->description = result->getNextString();
        professions->command = result->getNextString();
        professions->posture = CharacterPosture(result->getNextUnsignedInteger());
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
        auto production = new Production();
        // Initialize the Production.
        production->vnum = result->getNextInteger();
        production->name = result->getNextString();
        production->profession = Mud::instance().findProfession(result->getNextString());
        production->difficulty = result->getNextUnsignedInteger();
        production->time = result->getNextUnsignedInteger();
        production->assisted = result->getNextInteger();
        check &= production->setOutcome(result->getNextString());
        check &= production->setTool(result->getNextString());
        check &= production->setIngredient(result->getNextString());
        production->material = static_cast<ResourceType>(result->getNextInteger());
        production->workbench = ToolType(result->getNextUnsignedInteger());
        // ////////////////////////////////////////////////////////////////
        // Check the correctness.
        if (!check)
        {
            Logger::log(LogLevel::Error, "The production is incorrect " + production->name);
            return false;
        }
        if (!production->check())
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
        auto liquid = new Liquid();
        // Load the liquid.
        liquid->vnum = result->getNextInteger();
        liquid->name = result->getNextString();
        liquid->worth = result->getNextInteger();
        // Check the correctness.
        if (!liquid->check())
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
        auto container = Mud::instance().findItem(result->getNextInteger());
        auto liquid = Mud::instance().findLiquid(result->getNextInteger());
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
        container->pourIn(liquid, quantity, false);
    }
    return true;
}

bool LoadTravelPoint(ResultSet * result)
{
    while (result->next())
    {
        auto sourceArea = Mud::instance().findArea(result->getNextInteger());
        auto sourceRoom = sourceArea->getRoom(result->getNextInteger());
        auto targetArea = Mud::instance().findArea(result->getNextInteger());
        auto targetRoom = targetArea->getRoom(result->getNextInteger());
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
        SetFlag(&sourceRoom->flags, RoomFlag::TravelPoint);
        SetFlag(&targetRoom->flags, RoomFlag::TravelPoint);
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
        building.buildingModel = Mud::instance().findItemModel(result->getNextInteger());
        building.setIngredient(result->getNextString());
        building.unique = static_cast<bool>(result->getNextInteger());
        if (building.buildingModel == nullptr)
        {
            Logger::log(LogLevel::Error, "Can't find the building itemModel.");
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

bool LoadShop(ResultSet * result)
{
    while (result->next())
    {
        // Retrieve the item vnum.
        auto vnum = result->getNextInteger();
        auto item = Mud::instance().findItem(vnum);
        auto name = result->getNextString();
        auto buy = result->getNextUnsignedInteger();
        auto sell = result->getNextUnsignedInteger();
        auto balance = result->getNextUnsignedInteger();
        auto shopKeeper = Mud::instance().findMobile(result->getNextString());
        if (item == nullptr)
        {
            Logger::log(LogLevel::Error, "Can't find the item (%s).", ToString(vnum));
            return false;
        }
        if (item->getType() != ModelType::Shop)
        {
            Logger::log(LogLevel::Error, "Wrong type of item (%s).", ToString(vnum));
            return false;
        }
        auto shop = item->toShopItem();
        shop->shopName = name;
        shop->shopBuyTax = buy;
        shop->shopSellTax = sell;
        shop->balance = balance;
        shop->shopKeeper = shopKeeper;
        if (shopKeeper != nullptr)
        {
            shopKeeper->managedItem = shop;
        }
    }
    return true;
}

bool LoadCurrency(ResultSet * result)
{
    while (result->next())
    {
        // Retrieve the item vnum.
        auto modelVnum = result->getNextInteger();
        auto materialVnum = result->getNextInteger();
        auto worth = result->getNextUnsignedInteger();

        auto model = Mud::instance().findItemModel(modelVnum);
        if (model == nullptr)
        {
            Logger::log(LogLevel::Error, "Can't find the model (%s).", ToString(modelVnum));
            return false;
        }
        if (model->getType() != ModelType::Currency)
        {
            Logger::log(LogLevel::Error, "Wrong type of model (%s).", ToString(modelVnum));
            return false;
        }
        auto material = Mud::instance().findMaterial(materialVnum);
        if (material == nullptr)
        {
            Logger::log(LogLevel::Error, "Can't find the material (%s).", ToString(materialVnum));
            return false;
        }
        auto currency = model->toCurrency();
        if (!currency->addPrice(materialVnum, worth))
        {
            Logger::log(LogLevel::Error, "Can't add the price for (%s).", ToString(modelVnum));
            return false;
        }
    }
    return true;
}
