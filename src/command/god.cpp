/// @file   god.cpp
/// @brief  Implements the methods used by <b>gods</b>.
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

#include "god.hpp"

#include "mud.hpp"
#include "aStar.hpp"

void LoadGodCommands()
{
    // ////////////////////////////////////////////////////////////////////////
    Mud::instance().addCommand(std::make_shared<Command>(
        DoShutdown, "mud_shutdown", "",
        "Shut the MUD down.",
        true, true, true));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoMudSave, "mud_save", "",
        "Save the MUD.",
        true, true, true));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoGoTo, "goto", "(room vnum)",
        "Go to another room.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoFindPath, "findpath", "(room vnum)",
        "Finds the path to the given room.",
        true, true, true));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoHurt, "hurt", "(character)",
        "Hurt the desired character.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoInvisibility, "invisibility", "",
        "Became invisible.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoVisible, "visible", "",
        "Return visible.",
        true, true, false));
    // ////////////////////////////////////////////////////////////////////////
    Mud::instance().addCommand(std::make_shared<Command>(
        DoGodInfo, "char_info", "(character)",
        "Get information about a character.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoAggroList, "char_aggro", "(character)",
        "Provides the list of opponents of the given target.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoTransfer, "char_transfer", "(character) [where]",
        "Transfer another character here, or to another room.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoFeast, "char_feast", "(character)",
        "Restores completely the health and the stamina of the target.",
        true, true, false));
    // ////////////////////////////////////////////////////////////////////////
    Mud::instance().addCommand(std::make_shared<Command>(
        DoPlayerList, "player_list", "",
        "List all the players.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoSetFlag, "player_set_flag", "(player) (flag)",
        "Sets the given flag to the player.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoClearFlag, "player_rem_flag", "(player) (flag)",
        "Remove the given flag from the player.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoModSkill, "player_mod_skill", "(player) (skill) (modifier)",
        "Modify the value of the player skill.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoModAttr, "player_mod_attribute", "(player) (attribute) (modifier)",
        "Modify the value of the player attribute.",
        true, true, false));
    // ////////////////////////////////////////////////////////////////////////
    Mud::instance().addCommand(std::make_shared<Command>(
        DoModelInfo, "model_info", "(model vnum)",
        "List all the information about a model.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoModelList, "model_list", "Type --help for more information.",
        "List all the models.",
        true, true, false));
    // ////////////////////////////////////////////////////////////////////////
    Mud::instance().addCommand(std::make_shared<Command>(
        DoItemCreate, "item_create", "(model vnum) (material vnum) [quality]",
        "Create a new item, if not set the quality will be Normal.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoItemDestroy, "item_destroy", "(item vnum)",
        "Destroy the desired object.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoItemGet, "item_get", "(item vnum)",
        "Materialize the desired object.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoItemInfo, "item_information", "(item vnum)",
        "Show information about an item.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoItemList, "item_list", "Type --help for more information.",
        "List all the items.",
        true, true, false));
    // ////////////////////////////////////////////////////////////////////////
    Mud::instance().addCommand(std::make_shared<Command>(
        DoAreaInfo, "area_information", "(area vnum)",
        "Show the information about a specific area.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoAreaList, "area_list", "",
        "List all the areas.",
        true, true, false));
    // ////////////////////////////////////////////////////////////////////////
    Mud::instance().addCommand(std::make_shared<Command>(
        DoRoomInfo, "room_information", "(room vnum)",
        "Show the information about a specific room.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoRoomList, "room_list", "",
        "List all the rooms.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoRoomCreate, "room_create", "(direction)",
        "Create a room in the given direction.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoRoomDelete, "room_delete", "(direction)",
        "Delete the room in the given direction.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoRoomEdit, "room_edit", "(option) (value)",
        "Change room values.",
        true, true, false));
    // ////////////////////////////////////////////////////////////////////////
    Mud::instance().addCommand(std::make_shared<Command>(
        DoMobileKill, "mob_kill", "(mobile name)",
        "Kill the desired mobile, in the same room.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoMobileList, "mob_list", "(mobile name)",
        "List all the mobiles.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoMobileReload, "mob_reload", "(mobile name)",
        "Reload the lua script for the target mobile, in the same room.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoMobileTrigger, "mob_trigger", "(mobile name) (event name)",
        "Trigger a specific event of the given mobile.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoMobileLog, "mob_log", "(mobile name)",
        "Given a mobile id, it returns the corresponding mobile log.",
        true, true, false));
    // ////////////////////////////////////////////////////////////////////////
    Mud::instance().addCommand(std::make_shared<Command>(
        DoMaterialInfo, "mat_information", "(material name)",
        "Show the information about a specific material.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoMaterialList, "mat_list", "",
        "List all the materials.",
        true, true, false));
    // ////////////////////////////////////////////////////////////////////////
    Mud::instance().addCommand(std::make_shared<Command>(
        DoLiquidInfo, "liquid_information", "(liquid name)",
        "Show the information about a specific liquid.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoLiquidList, "liquid_list", "",
        "List all the liquids.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoLiquidCreate, "liquid_create", "(container)(liquid vnum) (quantity)",
        "Materialize some liquid sinde a container.",
        true, true, false));
    // ////////////////////////////////////////////////////////////////////////
    Mud::instance().addCommand(std::make_shared<Command>(
        DoBuildingInfo, "building_information", "(building vnum)",
        "Provides information about a building.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoBuildingList, "building_list", "",
        "List all the buildings.",
        true, true, false));
    // ////////////////////////////////////////////////////////////////////////
    Mud::instance().addCommand(std::make_shared<Command>(
        DoProductionInfo, "production_information", "(production vnum)",
        "Provide all the information regarding the given production.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoProductionList, "production_list", "",
        "Get the list of all the productions.",
        true, true, false));
    // ////////////////////////////////////////////////////////////////////////
    Mud::instance().addCommand(std::make_shared<Command>(
        DoProfessionInfo, "profession_information", "(profession command)",
        "Provide all the information regarding the given profession.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoProfessionList, "profession_list", "",
        "Get the list of all the professions.",
        true, true, false));
    // ////////////////////////////////////////////////////////////////////////
    Mud::instance().addCommand(std::make_shared<Command>(
        DoFactionInfo, "faction_information", "(faction vnum)",
        "Provide all the information regarding the given faction.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoFactionList, "faction_list", "",
        "List all the factions.",
        true, true, false));
    // ////////////////////////////////////////////////////////////////////////
    Mud::instance().addCommand(std::make_shared<Command>(
        DoRaceList, "race_list", "",
        "List all the races.",
        true, true, false));
    // ////////////////////////////////////////////////////////////////////////
    Mud::instance().addCommand(std::make_shared<Command>(
        DoSkillList, "skill_list", "",
        "List all the skills.",
        true, true, false));
    // ////////////////////////////////////////////////////////////////////////
    Mud::instance().addCommand(std::make_shared<Command>(
        DoWritingList, "writing_list", "",
        "List all the writings.",
        true, true, false));
    // ////////////////////////////////////////////////////////////////////////
    Mud::instance().addCommand(std::make_shared<Command>(
        DoContinentList, "continent_list", "",
        "List all the continents.",
        true, true, false));
    // ////////////////////////////////////////////////////////////////////////
}

bool DoShutdown(Character * character, ArgumentHandler &)
{
    // Send message to all the players.
    Mud::instance().broadcastMsg(0, character->getNameCapital() +
                                    " has shut down the game!");
    Mud::instance().shutDownSignal();
    return true;
}

bool DoMudSave(Character * character, ArgumentHandler &)
{
    if (!Mud::instance().saveMud())
    {
        character->sendMsg("Something gone wrong during the saving process.\n");
        return false;
    }
    // Send message to all the players.
    Mud::instance().broadcastMsg(0, character->getNameCapital() +
                                    " is writing the history...");
    character->sendMsg("Ok.\n");
    return true;
}

bool DoGoTo(Character * character, ArgumentHandler & args)
{
    if (args.size() != 1)
    {
        character->sendMsg("You have to provide a room vnum.");
        return false;
    }
    auto roomVnum = ToNumber<int>(args[0].getContent());
    auto destination = Mud::instance().findRoom(roomVnum);
    if (destination == nullptr)
    {
        character->sendMsg("The room %s doesn't exists.\n", roomVnum);
        return false;
    }
    // Stop any action the character is executing.
    StopAction(character);
    // Move player.
    character->moveTo(
        destination,
        character->getNameCapital() + " disappears in a puff of smoke!\n",
        character->getNameCapital() + " appears in a puff of smoke!\n",
        "You go to room " + destination->name + ".\n");
    return true;
}

bool DoFindPath(Character * character, ArgumentHandler & args)
{
    if (args.size() != 1)
    {
        character->sendMsg("You have to provide a room vnum.\n\n");
        return false;
    }
    auto roomVnum = ToNumber<int>(args[0].getContent());
    auto room = character->room->area->getRoom(roomVnum);
    if (room == nullptr)
    {
        character->sendMsg("The room %s doesn't exists.\n", roomVnum);
        return false;
    }
    AStar<Room *> aStar;
    std::vector<Room *> path;
    auto checkFunction = [&character](Room * from, Room * to)
    {
        // Get the direction.
        auto direction = Area::getDirection(from->coord, to->coord);
        // Get the exit;
        auto destExit = from->findExit(direction);
        // If the direction is upstairs, check if there is a stair.
        if (direction == Direction::Up)
        {
            if (!HasFlag(destExit->flags, ExitFlag::Stairs)) return false;
        }
        // Check if the destination is correct.
        if (destExit->destination == nullptr) return false;
        // Check if the destination is bocked by a door.
        auto door = destExit->destination->findDoor();
        if (door != nullptr)
        {
            if (HasFlag(door->flags, ItemFlag::Closed)) return false;
        }
        // Check if the destination has a floor.
        auto destDown = destExit->destination->findExit(Direction::Down);
        if (destDown != nullptr)
        {
            if (!HasFlag(destDown->flags, ExitFlag::Stairs)) return false;
        }
        // Check if the destination is forbidden for mobiles.
        return !(character->isMobile() &&
                 HasFlag(destExit->flags, ExitFlag::NoMob));
    };
    if (!aStar.findPath(character->room, room, path, checkFunction))
    {
        character->sendMsg("There is no path to that room.\n\n");
        return false;
    }
    character->sendMsg("You have to go:\n");
    Coordinates previous = character->room->coord;
    for (auto node : path)
    {
        auto direction = Area::getDirection(previous, node->coord);
        previous = node->coord;
        character->sendMsg("    %s\n", direction.toString());
    }
    character->sendMsg("\n");
    return true;
}

bool DoTransfer(Character * character, ArgumentHandler & args)
{
    if ((args.size() != 1) && (args.size() != 2))
    {
        character->sendMsg("You have to select at least a target and"
                               " optionally a destination.\n");
        return false;
    }
    Character * target = Mud::instance().findPlayer(args[0].getContent());
    if (target == nullptr)
    {
        target = Mud::instance().findMobile(args[0].getContent());
        if (target == nullptr)
        {
            character->sendMsg("Can't find the target character.\n");
            return false;
        }
    }
    if (target->isMobile())
    {
        if (!target->toMobile()->isAlive())
        {
            character->sendMsg("You cannot transfer a dead mobile.\n");
            return false;
        }
    }
    auto destination = character->room;
    if (args.size() == 2)
    {
        auto roomVnum = ToNumber<int>(args[1].getContent());
        destination = character->room->area->getRoom(roomVnum);
        if (destination == nullptr)
        {
            character->sendMsg("Can't find the room %s setting this room.\n",
                               ToString(roomVnum));
            return false;
        }
    }
    // Stop any action the character is executing.
    StopAction(target);
    // Prepare messages.
    // Move player.
    target->moveTo(
        destination,
        target->getNameCapital() + " is yanked away by unseen forces!",
        target->getNameCapital() + " appears breathlessly!",
        "\n" + character->getNameCapital() +
        " transfers you to another room!\n");
    character->sendMsg("You transfer %s to room %s.\n", target->getName(),
                       destination->name);
    return true;
}

bool DoFeast(Character * character, ArgumentHandler & args)
{
    if (args.size() != 1)
    {
        character->sendMsg("You must insert a valide target.\n");
        return false;
    }
    auto target = character->room->findCharacter(args[0].getContent(),
                                                 args[0].getIndex());
    if (target == nullptr)
    {
        character->sendMsg("You must provide a valide target.\n");
        return false;
    }
    target->setHealth(target->getMaxHealth(), true);
    target->setStamina(target->getMaxStamina(), true);
    target->setThirst(100);
    target->setHunger(100);
    target->sendMsg(
        "%sA banquet with any kind of delicacy appears from nowhere!%s%s\n",
        Formatter::magenta(), Formatter::reset(),
        ((target != character) ? "\n" : ""));
    return true;
}

bool DoSetFlag(Character * character, ArgumentHandler & args)
{
    if (args.size() != 2)
    {
        character->sendMsg("You must provide a target and a flag.");
        return false;
    }
    auto target = Mud::instance().findPlayer(args[0].getContent());
    if (target == nullptr)
    {
        character->sendMsg("You can't find the player '%s'.\n",
                           args[0].getContent());
        return false;
    }
    auto flag = static_cast<CharacterFlag>(ToNumber<int>(args[1].getContent()));
    if (flag == CharacterFlag::None)
    {
        character->sendMsg("You must insert a valid flag.\n");
        return false;
    }
    // Check if the character already has the flag.
    if (HasFlag(target->flags, flag))
    {
        character->sendMsg("Flag already set.\n");
        return false;
    }
    // Set the flag.
    SetFlag(&target->flags, flag);
    // Send confirmation to the player.
    character->sendMsg("You set the flag '%s' for %s\n",
                       GetCharacterFlagName(flag), target->getName());
    return true;
}

bool DoClearFlag(Character * character, ArgumentHandler & args)
{
    if (args.size() != 2)
    {
        character->sendMsg("You must provide a target and a flag.");
        return false;
    }
    auto target = Mud::instance().findPlayer(args[0].getContent());
    if (target == nullptr)
    {
        character->sendMsg("You can't find the player '%s'.\n",
                           args[0].getContent());
        return false;
    }
    auto flag = static_cast<CharacterFlag>(ToNumber<int>(args[1].getContent()));
    if (flag == CharacterFlag::None)
    {
        character->sendMsg("You must insert a valid flag.\n");
        return false;
    }
    // Check if the character already has the flag.
    if (HasFlag(target->flags, flag))
    {
        character->sendMsg("Flag not set.\n");
        return false;
    }
    // Set the flag.
    ClearFlag(&target->flags, flag);
    // Send confirmation to the player.
    character->sendMsg("You clear the flag '%s' for %s\n",
                       GetCharacterFlagName(flag), target->getName());
    return true;
}

// ////////////////////////////////////////////////////////////////////////////
// Model related commands.
// ////////////////////////////////////////////////////////////////////////////

bool DoModelInfo(Character * character, ArgumentHandler & args)
{
    if (args.size() != 1)
    {
        character->sendMsg("You must insert a model vnum.\n");
        return false;
    }
    auto modelVnum = ToNumber<int>(args[0].getContent());
    auto itemModel = Mud::instance().findItemModel(modelVnum);
    if (itemModel == nullptr)
    {
        character->sendMsg("Item model not found %s.\n", modelVnum);
        return false;
    }
    // Create a table.
    Table sheet;
    // Get the sheet.
    itemModel->getSheet(sheet);
    // Show the seet to character.
    character->sendMsg(sheet.getTable());
    return true;
}

bool DoModelList(Character * character, ArgumentHandler & args)
{
    // Variables used to filter the listed items.
    std::string modelName;
    std::string modelType;
    int modelVnum = -1;
    for (size_t argIt = 0; argIt < args.size(); ++argIt)
    {
        if (args[argIt].getContent() == "--help")
        {
            std::string help = "Usage:\n";
            help += "    model_list [options]\n";
            help += "Options:\n";
            help += "    -n [string]     Search models with the name which";
            help += " contains the given string.\n";
            help += "    -t [type_name]  Search models of the given type.\n";
            help += "    -m [model_vnum] Search models with the given vnum.\n";
            character->sendMsg(help);
            return true;
        }
        if ((argIt + 1) < args.size())
        {
            if (args[argIt].getContent() == "-n")
            {
                modelName = args[argIt + 1].getContent();
            }
            if (args[argIt].getContent() == "-t")
            {
                modelType = args[argIt + 1].getContent();
            }
            if (args[argIt].getContent() == "-m")
            {
                modelVnum = ToNumber<int>(args[argIt + 1].getContent());
            }
        }
    }
    Table table;
    table.addColumn("VNUM", StringAlign::Right);
    table.addColumn("NAME", StringAlign::Left);
    table.addColumn("TYPE", StringAlign::Left);
    table.addColumn("SLOT", StringAlign::Left);
    table.addColumn("FLAGS", StringAlign::Right);
    for (auto iterator : Mud::instance().mudItemModels)
    {
        ItemModel * itemModel = iterator.second;
        if (!modelName.empty())
        {
            if (itemModel->name.find(modelName) == std::string::npos)
            {
                continue;
            }
        }
        if (!modelType.empty())
        {
            if (!BeginWith(ToLower(itemModel->getTypeName()),
                           ToLower(modelType)))
            {
                continue;
            }
        }
        if (modelVnum != -1)
        {
            if (itemModel->vnum != modelVnum) continue;
        }
        // Prepare the row.
        TableRow row;
        row.push_back(ToString(itemModel->vnum));
        row.push_back(itemModel->name);
        row.push_back(itemModel->getTypeName());
        row.push_back(itemModel->slot.toString());
        row.push_back(ToString(itemModel->modelFlags));
        // Add the row to the table.
        table.addRow(row);
    }
    character->sendMsg(table.getTable());
    return true;
}

// ////////////////////////////////////////////////////////////////////////////
// Item related commands.
// ////////////////////////////////////////////////////////////////////////////

bool DoItemCreate(Character * character, ArgumentHandler & args)
{
    // Prevent mobiles to execute this command.
    NoMobile(character);
    // Stop any ongoing action.
    StopAction(character);
    // Check the number of arguments.
    if ((args.size() != 2) && (args.size() != 3))
    {
        character->sendMsg("What do you want to create?\n");
        return false;
    }
    // Get the quantity.
    auto quantity = args[0].getMultiplier();
    // Get the model.
    auto modeVnum = ToNumber<int>(args[0].getContent());
    auto itemModel = Mud::instance().findItemModel(modeVnum);
    if (itemModel == nullptr)
    {
        character->sendMsg("Cannot find model '%s'.\n", args[0].getContent());
        return false;
    }
    // Get the material.
    auto materialVnum = ToNumber<int>(args[1].getContent());
    auto material = Mud::instance().findMaterial(materialVnum);
    if (material == nullptr)
    {
        character->sendMsg("Cannot find material '%s'.\n",
                           args[1].getContent());
        return false;
    }
    // Get the quality.
    auto quality = ItemQuality(ItemQuality::Normal);
    if (args.size() == 3)
    {
        auto itemQualityValue = ToNumber<unsigned int>(args[2].getContent());
        if (!ItemQuality::isValid(itemQualityValue))
        {
            character->sendMsg("Not a valid quality.\n");
            return false;
        }
        quality = ItemQuality(itemQualityValue);
    }
    // Create the item.
    auto item = itemModel->createItem(character->getName(), material, false,
                                      quality, quantity);
    if (item == nullptr)
    {
        character->sendMsg("Creation failed.\n");
        return false;
    }
    character->addInventoryItem(item);
    character->sendMsg(
        "You produce '%s' out of your apparently empty top hat.\n",
        item->getName(true));
    return true;
}

bool DoItemDestroy(Character * character, ArgumentHandler & args)
{
    if (args.size() != 1)
    {
        character->sendMsg("You must instert an item vnum.\n");
        return false;
    }
    auto item = Mud::instance().findItem(ToNumber<int>(args[0].getContent()));
    if (item == nullptr)
    {
        character->sendMsg("Invalid vnum.\n");
        return false;
    }
    if (!item->isEmpty())
    {
        character->sendMsg("You cannot destroy a non-empty item.\n");
        return false;
    }
    character->sendMsg("You have destroyed the desired object.\n");
    MudUpdater::instance().addItemToDestroy(item);
    return true;
}

bool DoItemGet(Character * character, ArgumentHandler & args)
{
    if (args.size() != 1)
    {
        character->sendMsg("You must instert an item vnum.\n");
        return false;
    }
    auto itemVnum = ToNumber<int>(args[0].getContent());
    auto item = Mud::instance().findItem(itemVnum);
    if (item == nullptr)
    {
        character->sendMsg("Invalid vnum (%s).\n", ToString(itemVnum));
        return false;
    }
    // Check if the God player can carry the item.
    if (!character->canCarry(item, item->quantity))
    {
        character->sendMsg("You can't carry anymore items.\n");
        return false;
    }
    if (item->room != nullptr)
    {
        character->sendMsg("The item was inside the room '%s' (%s)\n",
                           item->room->name, ToString(item->room->vnum));
        item->room->removeItem(item);
    }
    else if (item->owner != nullptr)
    {
        character->sendMsg("The item was possessed by '%s'\n",
                           item->owner->getName());
        if (!character->remEquipmentItem(item))
        {
            if (!character->remInventoryItem(item))
            {
                character->sendMsg("Cannot take the item from the owner!");
                return false;
            }
        }
    }
    else if (item->container != nullptr)
    {
        character->sendMsg("The item was inside the container '%s'\n",
                           item->container->getName(true));
        item->container->takeOut(item);
    }
    else
    {
        character->sendMsg("The item was nowhere!");
    }
    character->sendMsg("You materialize the desired object in your hands.\n");
    character->addInventoryItem(item);
    return true;
}

bool DoItemInfo(Character * character, ArgumentHandler & args)
{
    if (args.size() != 1)
    {
        character->sendMsg("You must insert the item vnum or the name"
                               " of the item inside the room.\n");
        return false;
    }
    auto item = Mud::instance().findItem(ToNumber<int>(args[0].getContent()));
    if (item == nullptr)
    {
        item = character->findNearbyItem(args[0].getContent(),
                                         args[0].getIndex());
        if (item == nullptr)
        {

            character->sendMsg("Cannot find the target"
                                   " item (provide vnum or item name).\n");
            return false;
        }
    }
    // Create a table.
    Table sheet(item->getNameCapital());
    // Get the sheet.
    item->getSheet(sheet);
    // Show the seet to character.
    character->sendMsg(sheet.getTable());
    return true;
}

bool DoItemList(Character * character, ArgumentHandler & args)
{
    // Variables used to filter the listed items.
    std::string itemName;
    std::string typeName;
    int modelVnum = -1;
    for (size_t argIt = 0; argIt < args.size(); ++argIt)
    {
        if (args[argIt].getContent() == "--help")
        {
            std::string help = "Usage:\n";
            help += "    item_list [options]\n";
            help += "Options:\n";
            help += "    -n [string]     Search items with the name which";
            help += " contains the given string.\n";
            help += "    -t [type_name]  Search items of the given type.\n";
            help += "    -m [model_vnum] Search the items of the model";
            help += " having the given vnum.\n";
            character->sendMsg(help);
            return true;
        }
        if ((argIt + 1) < args.size())
        {
            if (args[argIt].getContent() == "-n")
            {
                itemName = args[argIt + 1].getContent();
            }
            if (args[argIt].getContent() == "-t")
            {
                typeName = args[argIt + 1].getContent();
            }
            if (args[argIt].getContent() == "-m")
            {
                modelVnum = ToNumber<int>(args[argIt + 1].getContent());
            }
        }
    }
    Table table;
    table.addColumn("Vnum", StringAlign::Right);
    table.addColumn("Name", StringAlign::Left);
    table.addColumn("Type", StringAlign::Left);
    table.addColumn("Model", StringAlign::Left);
    table.addColumn("Location", StringAlign::Left);
    for (auto iterator : Mud::instance().mudItems)
    {
        auto item = iterator.second;
        if (!itemName.empty())
        {
            if (item->getName(false).find(itemName) == std::string::npos)
            {
                continue;
            }
        }
        if (!typeName.empty())
        {
            if (!BeginWith(ToLower(item->getTypeName()), ToLower(typeName)))
            {
                continue;
            }
        }
        if (modelVnum != -1)
        {
            if (item->model->vnum != modelVnum) continue;
        }
        // Prepare the row.
        TableRow row;
        row.push_back(ToString(item->vnum));
        row.push_back(item->getNameCapital());
        row.push_back(item->getTypeName());
        row.push_back(ToString(item->model->vnum));
        if (item->owner != nullptr)
        {
            row.push_back(" Owner  : " + item->owner->getName());
        }
        else if (item->room != nullptr)
        {
            row.push_back(" Room   : " + ToString(item->room->vnum));
        }
        else if (item->container != nullptr)
        {
            row.push_back(" Inside : " + ToString(item->container->vnum));
        }
        else
        {
            row.push_back(" Is nowhere.");
        }
        // Add the row to the table.
        table.addRow(row);
    }
    character->sendMsg(table.getTable());
    return true;
}

// ////////////////////////////////////////////////////////////////////////////
// Room related commands.
// ////////////////////////////////////////////////////////////////////////////

bool DoRoomCreate(Character * character, ArgumentHandler & args)
{
    // Stop any action the character is executing.
    StopAction(character);

    // Check the number of arguments.
    if (args.size() != 1)
    {
        character->sendMsg("Usage: [direction]\n");
        return false;
    }
    auto currentRoom = character->room;
    auto currentArea = currentRoom->area;
    if (currentArea == nullptr)
    {
        character->sendMsg("Your room's area has not been defined!\n");
        return false;
    }
    // Check if it's a direction.
    auto direction = Mud::instance().findDirection(args[0].getContent(), false);
    if (direction == Direction::None)
    {
        character->sendMsg("You must insert a valid direction!\n");
        return false;
    }
    // Get the coordinate modifier.
    auto targetCoord = currentRoom->coord + direction.getCoordinates();
    if (!currentArea->inBoundaries(targetCoord))
    {
        character->sendMsg("Its outside the boundaries.\n");
        return false;
    }
    // Find the room.
    auto targetRoom = currentArea->getRoom(targetCoord);
    if (targetRoom)
    {
        character->sendMsg("There is already a room.\n");
        return false;
    }
    if (!CreateRoom(targetCoord, currentRoom))
    {
        character->sendMsg("Sorry but you couldn't create the room.\n");
        return false;
    }
    character->sendMsg("Room create (%s)\n", targetCoord.toString());
    return true;
}

bool DoRoomDelete(Character * character, ArgumentHandler & args)
{
    // Stop any action the character is executing.
    StopAction(character);
    // Check the number of arguments.
    if (args.size() != 1)
    {
        character->sendMsg("Usage: [direction]\n");
        return false;
    }
    auto currentArea = character->room->area;
    if (currentArea == nullptr)
    {
        character->sendMsg("Your room's area has not been defined!\n");
        return false;
    }
    // Check if it's a direction.
    auto direction = Mud::instance().findDirection(args[0].getContent(), false);
    if (direction == Direction::None)
    {
        character->sendMsg("You must insert a valid direction!\n");
        return false;
    }
    // Get the coordinate modifier.
    auto coord = character->room->coord + direction.getCoordinates();
    if (!currentArea->inBoundaries(coord))
    {
        character->sendMsg("Outside the boundaries.\n");
        return false;
    }
    // Find the room.
    auto targetRoom = currentArea->getRoom(coord);
    if (targetRoom == nullptr)
    {
        character->sendMsg("Sorry but in that direction there is no room.\n");
        return false;
    }
    // Remove the room from the Database.
    if (!targetRoom->removeOnDB())
    {
        character->sendMsg("You couldn't delete the selected room.\n");
        character->sendMsg("There are items or characters in that room.\n");
        return false;
    }
    // Remove the room from the list of rooms.
    if (!Mud::instance().remRoom(targetRoom))
    {
        character->sendMsg("You cannot remove the room.\n");
        return false;
    }
    // Delete completely the room.
    delete (targetRoom);
    character->sendMsg("You have destroyed the room at coordinates :\n");
    character->sendMsg("%s\n", coord.toString());
    return true;
}

bool DoRoomEdit(Character * character, ArgumentHandler & args)
{
    if (args.empty())
    {
        character->sendMsg("What do you want to edit?\n");
        return false;
    }
    if (args[0].getContent() == "des")
    {
        auto input = args.substr(1);
        if (input.empty())
        {
            character->sendMsg("You can't set an empty description.\n");
            return false;
        }
        QueryList value = {std::make_pair("description", input)};
        QueryList where = {
            std::make_pair("vnum", ToString(character->room->vnum))};
        if (!SQLiteDbms::instance().updateInto("Room", value, where))
        {
            character->sendMsg("Command gone wrong.\n");
            return false;
        }
        character->room->description = input;
        character->sendMsg("Room description modified.\n");
        return true;
    }
    else if (args[0].getContent() == "nam")
    {
        auto input = args.substr(1);
        if (input.empty())
        {
            character->sendMsg("You can't set an empty name.\n");
            return false;
        }
        QueryList value = {std::make_pair("name", input)};
        QueryList where = {
            std::make_pair("vnum", ToString(character->room->vnum))};
        if (!SQLiteDbms::instance().updateInto("Room", value, where))
        {
            character->sendMsg("Command gone wrong.\n");
            return false;
        }
        character->room->name = input;
        character->sendMsg("Room name modified.\n");
        return true;
    }
    else if (args[0].getContent() == "sflag")
    {
        auto input = args.substr(1);
        if (input.empty())
        {
            character->sendMsg("Wrong flag.\n");
        }
        else
        {
            if (input == "R")
            {
                SetFlag(&character->room->flags, RoomFlag::Rent);
                return true;
            }
            else if (input == "P")
            {
                SetFlag(&character->room->flags, RoomFlag::Peaceful);
                return true;
            }
            character->sendMsg("Not a valid flag.\n");
        }
    }
    else if (args[0].getContent() == "cflag")
    {
        auto input = args.substr(1);
        if (input.empty())
        {
            character->sendMsg("Wrong flag.\n");
        }
        else
        {
            if (input == "R")
            {
                ClearFlag(&character->room->flags, RoomFlag::Rent);
                return true;
            }
            else if (input == "P")
            {
                ClearFlag(&character->room->flags, RoomFlag::Peaceful);
                return true;
            }
            character->sendMsg("Not a valid flag.\n");
        }
    }
    else
    {
        std::string msg;
        msg += "Usage:                           \n";
        msg += "|  Argument  |  Text            |\n";
        msg += "| ---------- | ---------------- |\n";
        msg += "|  nam       |  [name]          |\n";
        msg += "|  ter       |  [terrain]       |\n";
        msg += "|  des       |  [description]   |\n";
        msg += "|  sflag     |  [Set Flag]      |\n";
        msg += "|            |   R -> kRent     |\n";
        msg += "|            |   P -> kPeaceful |\n";
        msg += "|  cflag     |  [Clear Flag]    |\n";
        msg += "|            |   R -> kRent     |\n";
        msg += "|            |   P -> kPeaceful |\n";
        msg += "| ---------- | ---------------- |\n";
        character->sendMsg(msg);
    }
    return false;
}

// ////////////////////////////////////////////////////////////////////////////
// Mobile related commands.
// ////////////////////////////////////////////////////////////////////////////

bool DoMobileKill(Character * character, ArgumentHandler & args)
{
    if (args.size() != 1)
    {
        character->sendMsg("You must provide a target mobile.\n");
        return false;
    }
    auto mobile = character->room->findMobile(args[0].getContent(),
                                              args[0].getIndex());
    if (mobile == nullptr)
    {
        character->sendMsg("Mobile not found.\n");
        return false;
    }
    // Kill the mob.
    mobile->kill();
    // Notify the death.
    character->sendMsg("You snap your fingers.\n");
    character->room->sendToAll("%s fall to the ground dead.",
                               {}, mobile->getNameCapital());
    return true;
}

bool DoMobileReload(Character * character, ArgumentHandler & args)
{
    if (args.size() != 1)
    {
        character->sendMsg("You must provide a target mobile.\n");
        return false;
    }
    auto mobile = character->room->findMobile(args[0].getContent(),
                                              args[0].getIndex());
    if (mobile == nullptr)
    {
        character->sendMsg("Mobile not found.\n");
        return false;
    }
    // Reload the mob.
    mobile->reloadLua();
    // Notify.
    character->sendMsg("Target(%s) Script(%s)\n",
                       mobile->getName(), mobile->lua_script);
    return true;
}

bool DoMobileTrigger(Character * character, ArgumentHandler & args)
{
    if (args.size() != 1)
    {
        character->sendMsg("You must provide a target mobile.\n");
    }
    else
    {
        auto mobile = character->room->findMobile(args[0].getContent(),
                                                  args[0].getIndex());
        if (mobile == nullptr)
        {
            character->sendMsg("Mobile not found.\n");
        }
        else
        {
            mobile->triggerEventMain();
            character->sendMsg("%s::triggerEventMain()\n",
                               mobile->getNameCapital());
            return true;
        }
    }
    return false;
}

bool DoMobileLog(Character * character, ArgumentHandler & args)
{
    if (args.size() != 1)
    {
        character->sendMsg("You must provide a mobile id.\n");
        return false;
    }
    auto mobile = Mud::instance().findMobile(args[0].getContent());
    if (mobile == nullptr)
    {
        character->sendMsg("Mobile not found.\n");
        return false;
    }
    // Notify.
    character->sendMsg("Target(%s)\n", mobile->getName());
    character->sendMsg("Log:\n%s\n", mobile->message_buffer);
    return true;
}

bool DoHurt(Character * character, ArgumentHandler & args)
{
    if (args.size() != 1)
    {
        character->sendMsg("Who do you want to hurt?\n");
        return false;
    }
    auto target = character->room->findCharacter(args[0].getContent(),
                                                 args[0].getIndex());
    if (target == nullptr)
    {
        character->sendMsg("Target not found.\n");
        return false;
    }
    // Set health to 1.
    target->setHealth(1);
    // Notify.
    character->sendMsg("You point your finger, %s cry in pain.\n",
                       target->getName());
    target->sendMsg("%s points the finger towards you, you cry in pain.\n",
                    character->getNameCapital());
    // Send the message inside the room.
    target->room->sendToAll(
        "%s points the finger towards %s, %s cries in pain.\n",
        {character, target},
        character->getNameCapital(),
        target->getName(),
        target->getSubjectPronoun());
    return true;
}

bool DoInvisibility(Character * character, ArgumentHandler & /*args*/)
{
    // Check if already invisibile.
    if (HasFlag(character->flags, CharacterFlag::Invisible))
    {
        character->sendMsg("Don't worry, you are already invisible.\n");
        return false;
    }
    // Set the character invisible.
    SetFlag(&character->flags, CharacterFlag::Invisible);
    character->sendMsg("You are invisible now.\n");
    return true;
}

bool DoVisible(Character * character, ArgumentHandler & /*args*/)
{
    // Check if already visible.
    if (!HasFlag(character->flags, CharacterFlag::Invisible))
    {
        character->sendMsg("You are not invisible.\n");
        return false;
    }
    // Set the character visible.
    ClearFlag(&character->flags, CharacterFlag::Invisible);
    character->sendMsg("You are no more invisible.\n");
    return true;
}

bool DoModSkill(Character * character, ArgumentHandler & args)
{
    if (args.size() != 3)
    {
        character->sendMsg("Usage: [target] [#skill] [+/-VALUE]\n");
        return false;
    }
    auto target = character->room->findPlayer(args[0].getContent(),
                                              args[0].getIndex());
    if (target == nullptr)
    {
        character->sendMsg("Target not found.\n");
        return false;
    }
    auto skill = Mud::instance().findSkill(ToNumber<int>(args[1].getContent()));
    if (skill == nullptr)
    {
        character->sendMsg("Cannot find the desired skill.\n");
        return false;
    }
    int modifier = ToNumber<int>(args[2].getContent());
    if ((modifier == 0) || (modifier < -100) || (modifier > 100))
    {
        character->sendMsg("You must insert a valid modifier.\n");
        return false;
    }
    auto modified = static_cast<int>(target->skills[skill->vnum]) + modifier;
    if (modified <= 0)
    {
        character->sendMsg("You cannot reduce the skill level <= 0.\n");
        return false;
    }
    if (modified >= 100)
    {
        character->sendMsg("You cannot increase the skill to 100 or above.\n");
        return false;
    }
    // Change the skill value.
    target->skills[skill->vnum] = static_cast<unsigned int>(modified);
    // Notify.
    character->sendMsg("You have successfully %s by %s the \"%s\" skill,"
                           "the new level is %s.\n",
                       ((modifier > 0) ? "increased " : "decreased"),
                       ToString(modifier),
                       skill->name,
                       ToString(target->skills[skill->vnum]));
    return true;
}

bool DoModAttr(Character * character, ArgumentHandler & args)
{
    // Stop any action the character is executing.
    StopAction(character);
    // Check the number of arguments.
    if (args.size() != 3)
    {
        character->sendMsg("Usage: [target] [attribute] [+/-VALUE]\n");
        return false;
    }
    auto target = character->room->findCharacter(args[0].getContent(),
                                                 args[0].getIndex());
    if (target == nullptr)
    {
        character->sendMsg("Target not found.\n");
        return false;
    }
    auto modifier = ToNumber<int>(args[2].getContent());
    if (modifier == 0)
    {
        character->sendMsg("You must insert a valid value.\n");
        return false;
    }
    Ability ability;
    if (args[1].getContent() == "str")
    {
        ability = Ability::Strength;
    }
    else if (args[1].getContent() == "agi")
    {
        ability = Ability::Agility;
    }
    else if (args[1].getContent() == "per")
    {
        ability = Ability::Perception;
    }
    else if (args[1].getContent() == "con")
    {
        ability = Ability::Constitution;
    }
    else if (args[1].getContent() == "int")
    {
        ability = Ability::Intelligence;
    }
    else
    {
        character->sendMsg("Bad attribute name, accepted :"
                               "(str, agi, per, con, int).\n");
        return false;
    }
    // Get the resulting ability value.
    auto result = static_cast<int>(target->getAbility(ability)) + modifier;
    if (result < 0)
    {
        character->sendMsg("Attribute cannot go below 0.");
        return false;
    }
    else if (!target->setAbility(ability, static_cast<unsigned int>(result)))
    {
        character->sendMsg("Attribute cannot go above 60.");
        return false;
    }
    character->sendMsg("You have successfully %s by %s the %s of the target.",
                       std::string((modifier > 0) ? "increased" : "decreased"),
                       ToString(modifier),
                       ability.toString());
    return true;
}

bool DoLiquidCreate(Character * character, ArgumentHandler & args)
{
    if (args.size() != 3)
    {
        character->sendMsg("Usage: [Item] [Liquid] [Quantity].\n");
        return false;
    }
    auto item = character->findNearbyItem(args[0].getContent(),
                                          args[0].getIndex());
    if (item == nullptr)
    {
        character->sendMsg("Can't find the desire item.\n");
        return false;
    }
    if (item->model->getType() != ModelType::LiquidContainer)
    {
        character->sendMsg("The item is not a container of liquids.\n");
        return false;
    }
    auto liquidVnum = ToNumber<int>(args[1].getContent());
    auto liquid = Mud::instance().findLiquid(liquidVnum);
    if (liquid == nullptr)
    {
        character->sendMsg("Can't find the desire liquid %s.\n", liquidVnum);
        return false;
    }
    auto quantity = ToNumber<int>(args[2].getContent());
    if ((quantity <= 0) || (quantity >= 100))
    {
        character->sendMsg("Accepted quantity of liquids (from 1 to 99).\n");
        return false;
    }
    if (!item->pourIn(liquid, static_cast<unsigned int>(quantity)))
    {
        character->sendMsg("Item can't contain that quantity of liquid.\n");
        return false;
    }
    character->sendMsg("You materialise %s units of %s inside %s.\n",
                       ToString(quantity),
                       liquid->getName(),
                       item->getName(true));
    return true;
}

// ////////////////////////////////////////////////////////////////////////////
// Building related commands.
// ////////////////////////////////////////////////////////////////////////////

bool DoBuildingList(Character * character, ArgumentHandler & /*args*/)
{
    Table table;
    table.addColumn("VNUM", StringAlign::Center);
    table.addColumn("NAME", StringAlign::Center);
    table.addColumn("DIFFICULTY", StringAlign::Left);
    table.addColumn("TIME", StringAlign::Center);
    table.addColumn("UNIQUE", StringAlign::Center);
    for (auto iterator : Mud::instance().mudBuildings)
    {
        auto building = &(iterator.second);
        // Prepare the row.
        TableRow row;
        row.push_back(ToString(building->vnum));
        row.push_back(building->name);
        row.push_back(ToString(building->difficulty));
        row.push_back(ToString(building->time));
        row.push_back(ToString(building->unique));
        // Add the row to the table.
        table.addRow(row);
    }
    character->sendMsg(table.getTable());
    return true;
}

bool DoBuildingInfo(Character * character, ArgumentHandler & args)
{
    if (args.size() != 1)
    {
        character->sendMsg("You must provide a building vnum.\n");
        return false;
    }
    auto buildingVnum = ToNumber<int>(args[0].getContent());
    auto building = Mud::instance().findBuilding(buildingVnum);
    if (building == nullptr)
    {
        character->sendMsg("Can't find the desire building %s.\n",
                           buildingVnum);
        return false;
    }
    std::string msg;
    msg += "Vnum        : " + ToString(building->vnum) + "\n";
    msg += "Name        : " + building->name + "\n";
    msg += "Difficulty  : " + ToString(building->difficulty) + "\n";
    msg += "Time        : " + ToString(building->time) + "\n";
    msg += "Assisted    : " + ToString(building->assisted) + "\n";
    msg += "Tools       :\n";
    for (auto iterator : building->tools)
    {
        msg += "                  " + GetToolTypeName(iterator) + "\n";
    }
    msg += "Building    : " + building->buildingModel->name + "\n";
    msg += "Ingredients :\n";
    for (auto iterator : building->ingredients)
    {
        msg += "    " + iterator.first.toString();
        msg += "(" + ToString(iterator.second) + ")\n";
    }
    msg += "Unique      : " + ToString(building->unique) + "\n";
    character->sendMsg(msg);
    return true;
}

// ////////////////////////////////////////////////////////////////////////////
// INFORMATION
// ////////////////////////////////////////////////////////////////////////////

bool DoGodInfo(Character * character, ArgumentHandler & args)
{
    if (args.size() != 1)
    {
        character->sendMsg("You must insert a valide player name.\n");
        return false;
    }
    Character * target = Mud::instance().findPlayer(args[0].getContent());
    if (target == nullptr)
    {
        target = Mud::instance().findMobile(args[0].getContent());
        if (target == nullptr)
        {
            std::string msgFound;
            for (auto it : Mud::instance().mudMobiles)
            {
                if (BeginWith(it.first, args[0].getContent()))
                {
                    if (msgFound.empty())
                    {
                        msgFound += "Maybe you mean:\n";
                    }
                    msgFound += "    " + it.first + "\n";
                }
            }
            character->sendMsg("Mobile not found.\n" + msgFound);
            return false;
        }
    }
    // Create a table.
    Table sheet;
    // Get the sheet.
    target->getSheet(sheet);
    // Show the seet to character.
    character->sendMsg(sheet.getTable());
    return true;
}

bool DoAreaInfo(Character * character, ArgumentHandler & args)
{
    if (args.size() != 1)
    {
        character->sendMsg("You must instert an area vnum.\n");
        return false;
    }
    auto area = Mud::instance().findArea(ToNumber<int>(args[0].getContent()));
    if (area == nullptr)
    {
        character->sendMsg("The selected area does not exist.");
        return false;
    }
    std::string msg;
    msg += "Area Informations:\n";
    msg += " Vnum      :" + ToString(area->vnum) + "\n";
    msg += " Name      :" + area->name + "\n";
    msg += " Builder   :" + area->builder + "\n";
    msg += " Width     :" + ToString(area->width) + "\n";
    msg += " Height    :" + ToString(area->height) + "\n";
    msg += " Elevation :" + ToString(area->elevation) + "\n";
    msg += " N. Rooms  :" + ToString(area->map.size()) + "\n";
    character->sendMsg(msg);
    return true;
}

bool DoRoomInfo(Character * character, ArgumentHandler & args)
{
    Room * room;
    if (args.empty())
    {
        room = character->room;
    }
    else if (args.size() == 1)
    {
        room = Mud::instance().findRoom(ToNumber<int>(args[0].getContent()));
        if (room == nullptr)
        {
            character->sendMsg("Can't find the desired room.\n");
            return false;
        }
    }
    else
    {
        character->sendMsg("CanYu can only provide nothing or a room vnum.\n");
        return false;
    }
    std::string msg;
    msg += "Room Informations:\n";
    msg += " Vnum        :" + ToString(room->vnum) + "\n";
    msg += " Area        :";
    msg += ((room->area) ? ToString(room->area->vnum) : "NULL") + "\n";
    msg += " X           :" + ToString(room->coord.x) + "\n";
    msg += " Y           :" + ToString(room->coord.y) + "\n";
    msg += " Z           :" + ToString(room->coord.z) + "\n";
    msg += " Name        :" + room->name + "\n";
    msg += " Description :" + room->description + "\n";
    msg += " Terrain     :" + room->terrain->name + "\n";
    msg += " Flags       :" + GetRoomFlagString(room->flags) + "\n";
    character->sendMsg(msg);
    return true;
}

bool DoMaterialInfo(Character * character, ArgumentHandler & args)
{
    if (args.size() != 1)
    {
        character->sendMsg("You must insert a valide material vnum.\n");
        return false;
    }
    auto materialVnum = ToNumber<int>(args[0].getContent());
    auto material = Mud::instance().findMaterial(materialVnum);
    if (material == nullptr)
    {
        character->sendMsg("Can't find the desire material %s.\n",
                           materialVnum);
        return false;
    }
    std::string msg;
    msg += "Vnum      : " + ToString(material->vnum) + "\n";
    msg += "Type      : " + material->type.toString() + "\n";
    msg += "Name      : " + material->name + "\n";
    msg += "Worth     : " + ToString(material->worth) + "\n";
    msg += "Hardness  : " + ToString(material->hardness) + "\n";
    msg += "Lightness : " + ToString(material->lightness) + "\n";
    character->sendMsg(msg);
    return true;
}

bool DoLiquidInfo(Character * character, ArgumentHandler & args)
{
    if (args.size() != 1)
    {
        character->sendMsg("You must provide a liquid vnum.\n");
        return false;
    }
    auto liquidVnum = ToNumber<int>(args[0].getContent());
    auto liquid = Mud::instance().findLiquid(liquidVnum);
    if (liquid == nullptr)
    {
        character->sendMsg("Can't find the desire liquid %s.\n", liquidVnum);
        return false;
    }
    std::string msg;
    msg += "Vnum  : " + ToString(liquid->vnum) + "\n";
    msg += "Name  : " + liquid->getNameCapital() + "\n";
    msg += "Worth : " + ToString(liquid->worth) + "\n";
    character->sendMsg(msg);
    return true;
}

bool DoProductionInfo(Character * character, ArgumentHandler & args)
{
    if (args.size() != 1)
    {
        character->sendMsg("You must provide a production vnum.\n");
        return false;
    }
    auto productionVnum = ToNumber<int>(args[0].getContent());
    auto production = Mud::instance().findProduction(productionVnum);
    if (production == nullptr)
    {
        character->sendMsg("Can't find the desire production %s.\n",
                           productionVnum);
        return false;
    }
    std::string msg;
    msg += "Vnum        : " + ToString(production->vnum) + "\n";
    msg += "Name        : " + production->name + "\n";
    msg += "Profession  : " + production->profession->command + "\n";
    msg += "Difficulty  : " + ToString(production->difficulty) + "\n";
    msg += "Time        : " + ToString(production->time) + "\n";
    msg += "Assisted    : " + ToString(production->assisted) + "\n";
    msg += "Outcome     : " + production->outcome->name + "*";
    msg += ToString(production->quantity) + "\n";
    msg += "Tools       :\n";
    for (auto iterator : production->tools)
    {
        msg += "                  " + GetToolTypeName(iterator) + "\n";
    }
    msg += "Ingredients :\n";
    for (auto iterator : production->ingredients)
    {
        msg += "    " + iterator.first.toString();
        msg += "(" + ToString(iterator.second) + ")\n";
    }
    msg += "Workbench   :" + GetToolTypeName(production->workbench) + "\n";
    character->sendMsg(msg);
    return true;
}

bool DoProfessionInfo(Character * character, ArgumentHandler & args)
{
    if (args.size() != 1)
    {
        character->sendMsg("You must provide a profession name.\n");
        return false;
    }
    auto profession = Mud::instance().findProfession(args[0].getContent());
    if (profession == nullptr)
    {
        character->sendMsg("Can't find the desire profession.\n");
        return false;
    }
    std::string msg;
    msg += "Name          : " + profession->name + "\n";
    msg += "Description   : " + profession->description + "\n";
    msg += "Command       : " + profession->command + "\n";
    msg += "Posture       : " + profession->posture.toString() + "\n";
    msg += "Action        : " + profession->action + "\n";
    msg += "    Start     : " + profession->startMessage + "\n";
    msg += "    Finish    : " + profession->finishMessage + "\n";
    msg += "    Success   : " + profession->successMessage + "\n";
    msg += "    Failure   : " + profession->failureMessage + "\n";
    msg += "    Interrupt : " + profession->interruptMessage + "\n";
    msg += "    Not Found : " + profession->notFoundMessage + "\n";
    character->sendMsg(msg);
    return true;
}

bool DoFactionInfo(Character * character, ArgumentHandler & args)
{
    if (args.size() != 1)
    {
        character->sendMsg("You must insert a valide faction vnum.\n");
        return false;
    }
    auto factionVnum = ToNumber<int>(args[0].getContent());
    auto faction = Mud::instance().findFaction(factionVnum);
    if (faction == nullptr)
    {
        character->sendMsg("Faction not found.\n");
        return false;
    }
    // Create a table.
    Table sheet;
    // Get the sheet.
    faction->getSheet(sheet);
    // Show the seet to character.
    character->sendMsg(sheet.getTable());
    return true;
}

// ////////////////////////////////////////////////////////////////////////////
// LIST
// ////////////////////////////////////////////////////////////////////////////

bool DoAggroList(Character * character, ArgumentHandler & args)
{
    if (args.size() != 1)
    {
        character->sendMsg("You must insert a valide character name.\n");
        return false;
    }
    Character * targer = Mud::instance().findPlayer(args[0].getContent());
    if (targer == nullptr)
    {
        targer = Mud::instance().findMobile(args[0].getContent());
        if (targer == nullptr)
        {
            character->sendMsg("Character not found.\n");
            return false;
        }
    }
    for (auto aggressor : targer->combatHandler)
    {
        if (aggressor->aggressor != nullptr)
        {
            character->sendMsg("%s\t%s",
                               ToString(aggressor->aggression),
                               aggressor->aggressor->getNameCapital());
        }
    }
    return true;
}

bool DoMobileList(Character * character, ArgumentHandler & /*args*/)
{
    Table table;
    table.addColumn("ALIVE", StringAlign::Center);
    table.addColumn("ID", StringAlign::Left);
    table.addColumn("NAME", StringAlign::Left);
    table.addColumn("LOCATION", StringAlign::Right);
    for (auto iterator : Mud::instance().mudMobiles)
    {
        auto mobile = iterator.second;
        // Prepare the row.
        TableRow row;
        row.push_back((mobile->isAlive()) ? "Yes" : "No");
        row.push_back(mobile->id);
        row.push_back(mobile->getName());
        if (mobile->room != nullptr)
        {
            row.push_back("Room : " + ToString(mobile->room->vnum));
        }
        else
        {
            row.push_back("Is nowhere.");
        }
        // Add the row to the table.
        table.addRow(row);
    }
    character->sendMsg(table.getTable());
    return true;
}

bool DoPlayerList(Character * character, ArgumentHandler & /*args*/)
{
    Table table;
    table.addColumn("NAME", StringAlign::Left);
    table.addColumn("ROOM", StringAlign::Left);
    for (auto iterator : Mud::instance().mudPlayers)
    {
        // If the player is not playing, continue.
        if (!iterator->isPlaying())
        {
            continue;
        }
        // Prepare the row.
        TableRow row;
        row.push_back(iterator->name);
        if (iterator->room != nullptr)
        {
            row.push_back(iterator->room->name);
        }
        else
        {
            row.push_back("Nowhere!");
        }
        // Add the row to the table.
        table.addRow(row);
    }
    character->sendMsg(table.getTable());
    return true;
}

bool DoAreaList(Character * character, ArgumentHandler & /*args*/)
{
    Table table;
    table.addColumn("VNUM", StringAlign::Center);
    table.addColumn("NAME", StringAlign::Left);
    table.addColumn("BUILDER", StringAlign::Left);
    table.addColumn("ROOMS", StringAlign::Center);
    for (auto iterator : Mud::instance().mudAreas)
    {
        Area * area = iterator.second;
        // Prepare the row.
        TableRow row;
        row.push_back(ToString(area->vnum));
        row.push_back(area->name);
        row.push_back(area->builder);
        row.push_back(ToString(area->map.size()));
        // Add the row to the table.
        table.addRow(row);
    }
    character->sendMsg(table.getTable());
    return true;
}

bool DoRoomList(Character * character, ArgumentHandler & /*args*/)
{
    Table table;
    table.addColumn("VNUM", StringAlign::Center);
    table.addColumn("AREA", StringAlign::Left);
    table.addColumn("COORD", StringAlign::Center);
    table.addColumn("TERRAIN", StringAlign::Center);
    table.addColumn("NAME", StringAlign::Left);
    for (auto iterator : Mud::instance().mudRooms)
    {
        Room * room = iterator.second;
        // Prepare the row.
        TableRow row;
        row.push_back(ToString(room->vnum));
        if (room->area != nullptr)
        {
            row.push_back(room->area->name);
        }
        else
        {
            row.push_back("None");
        }
        row.push_back(
            ToString(room->coord.x) + ' ' + ToString(room->coord.y) + ' '
            + ToString(room->coord.z));
        row.push_back(room->terrain->name);
        row.push_back(room->name);
        // Add the row to the table.
        table.addRow(row);
    }
    character->sendMsg(table.getTable());
    return true;
}

bool DoRaceList(Character * character, ArgumentHandler & /*args*/)
{
    Table table;
    table.addColumn("VNUM", StringAlign::Center);
    table.addColumn("NAME", StringAlign::Left);
    table.addColumn("ALLOWED", StringAlign::Left);
    table.addColumn("STRENGTH", StringAlign::Right);
    table.addColumn("AGILITY", StringAlign::Right);
    table.addColumn("PERCEPTION", StringAlign::Right);
    table.addColumn("CONSTITUTION", StringAlign::Right);
    table.addColumn("INTELLIGENCE", StringAlign::Right);
    for (auto iterator : Mud::instance().mudRaces)
    {
        Race * race = iterator.second;
        // Prepare the row.
        TableRow row;
        row.push_back(ToString(race->vnum));
        row.push_back(race->name);
        row.push_back(BoolToString(race->player_allow));
        row.push_back(ToString(race->getAbility(Ability::Strength)));
        row.push_back(ToString(race->getAbility(Ability::Agility)));
        row.push_back(ToString(race->getAbility(Ability::Perception)));
        row.push_back(ToString(race->getAbility(Ability::Constitution)));
        row.push_back(ToString(race->getAbility(Ability::Intelligence)));
        // Add the row to the table.
        table.addRow(row);
    }
    character->sendMsg(table.getTable());
    return true;
}

bool DoFactionList(Character * character, ArgumentHandler & /*args*/)
{
    Table table;
    table.addColumn("VNUM", StringAlign::Center);
    table.addColumn("NAME", StringAlign::Left);
    for (auto iterator : Mud::instance().mudFactions)
    {
        // Prepare the row.
        TableRow row;
        row.push_back(ToString(iterator.second->vnum));
        row.push_back(iterator.second->name);
        // Add the row to the table.
        table.addRow(row);
    }
    character->sendMsg(table.getTable());
    return true;
}

bool DoSkillList(Character * character, ArgumentHandler & /*args*/)
{
    Table table;
    table.addColumn("VNUM", StringAlign::Center);
    table.addColumn("NAME", StringAlign::Left);
    table.addColumn("ATTRIBUTE", StringAlign::Left);
    for (auto iterator : Mud::instance().mudSkills)
    {
        // Prepare the row.
        TableRow row;
        row.push_back(ToString(iterator.second->vnum));
        row.push_back(iterator.second->name);
        row.push_back(GetAttributeName(iterator.second->attribute, true));
        // Add the row to the table.
        table.addRow(row);
    }
    character->sendMsg(table.getTable());
    return true;
}

bool DoWritingList(Character * character, ArgumentHandler & /*args*/)
{
    Table table;
    table.addColumn("VNUM", StringAlign::Center);
    table.addColumn("AUTHOR", StringAlign::Left);
    table.addColumn("TITLE", StringAlign::Left);
    for (auto iterator : Mud::instance().mudWritings)
    {
        Writing * writing = iterator.second;
        // Prepare the row.
        TableRow row;
        row.push_back(ToString(writing->vnum));
        row.push_back(writing->author);
        row.push_back(writing->title);
        // Add the row to the table.
        table.addRow(row);
    }
    character->sendMsg(table.getTable());
    return true;
}

bool DoContinentList(Character * /*character*/, ArgumentHandler & /*args*/)
{
    return true;
}

bool DoMaterialList(Character * character, ArgumentHandler & /*args*/)
{
    Table table;
    table.addColumn("VNUM", StringAlign::Right);
    table.addColumn("TYPE", StringAlign::Center);
    table.addColumn("NAME", StringAlign::Left);
    table.addColumn("WORTH", StringAlign::Right);
    table.addColumn("HARDNESS", StringAlign::Right);
    table.addColumn("LIGHTNESS", StringAlign::Right);
    for (auto iterator : Mud::instance().mudMaterials)
    {
        // Prepare the row.
        TableRow row;
        row.push_back(ToString(iterator.second->vnum));
        row.push_back(iterator.second->type.toString());
        row.push_back(iterator.second->name);
        row.push_back(ToString(iterator.second->worth));
        row.push_back(ToString(iterator.second->hardness));
        row.push_back(ToString(iterator.second->lightness));
        // Add the row to the table.
        table.addRow(row);
    }
    character->sendMsg(table.getTable());
    return true;
}

bool DoProfessionList(Character * character, ArgumentHandler & /*args*/)
{
    Table table;
    table.addColumn("NAME", StringAlign::Center);
    table.addColumn("COMMAND", StringAlign::Center);
    table.addColumn("POSTURE", StringAlign::Center);
    table.addColumn("ACTION", StringAlign::Center);
    for (auto iterator : Mud::instance().mudProfessions)
    {
        // Prepare the row.
        TableRow row;
        row.push_back(iterator.second->name);
        row.push_back(iterator.second->command);
        row.push_back(iterator.second->posture.toString());
        row.push_back(iterator.second->action);
        // Add the row to the table.
        table.addRow(row);
    }
    character->sendMsg(table.getTable());
    return true;
}

bool DoProductionList(Character * character, ArgumentHandler & /*args*/)
{
    Table table;
    table.addColumn("VNUM", StringAlign::Center);
    table.addColumn("NAME", StringAlign::Center);
    table.addColumn("PROFESSION", StringAlign::Center);
    table.addColumn("DIFFICULTY", StringAlign::Left);
    for (auto iterator : Mud::instance().mudProductions)
    {
        // Prepare the row.
        TableRow row;
        row.push_back(ToString(iterator.second->vnum));
        row.push_back(iterator.second->name);
        row.push_back(iterator.second->profession->command);
        row.push_back(ToString(iterator.second->difficulty));
        // Add the row to the table.
        table.addRow(row);
    }
    character->sendMsg(table.getTable());
    return true;
}

bool DoLiquidList(Character * character, ArgumentHandler & /*args*/)
{
    Table table;
    table.addColumn("VNUM", StringAlign::Center);
    table.addColumn("NAME", StringAlign::Right);
    table.addColumn("WORTH", StringAlign::Right);
    for (auto iterator : Mud::instance().mudLiquids)
    {
        // Prepare the row.
        TableRow row;
        row.push_back(ToString(iterator.second->vnum));
        row.push_back(iterator.second->getNameCapital());
        row.push_back(ToString(iterator.second->worth));
        // Add the row to the table.
        table.addRow(row);
    }
    character->sendMsg(table.getTable());
    return true;
}
