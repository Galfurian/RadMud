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
    {
        Command command;
        command.gods = true;
        command.name = "shutdown";
        command.help = "Shut the MUD down.";
        command.arguments = "NONE";
        command.hndl = DoShutdown;
        command.typedCompletely = true;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.gods = true;
        command.name = "mudsave";
        command.help = "Save the MUD.";
        command.arguments = "NONE";
        command.hndl = DoMudSave;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.gods = true;
        command.name = "goto";
        command.help = "Go to another room.";
        command.arguments = "(Room.vnum)";
        command.hndl = DoGoTo;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.gods = true;
        command.name = "findpath";
        command.help = "Finds the path to the given room.";
        command.arguments = "(Room.vnum)";
        command.hndl = DoFindPath;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.gods = true;
        command.name = "hurt";
        command.help = "Hurt the desired target.";
        command.arguments = "(Target)";
        command.hndl = DoHurt;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.gods = true;
        command.name = "invisibility";
        command.help = "Became invisible.";
        command.arguments = "NONE";
        command.hndl = DoInvisibility;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.gods = true;
        command.name = "visible";
        command.help = "Return visible.";
        command.arguments = "NONE";
        command.hndl = DoVisible;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.gods = true;
        command.name = "ginfo";
        command.help = "Get information about a character.";
        command.arguments = "(target)";
        command.hndl = DoGodInfo;
        Mud::instance().addCommand(command);
    }

    // Interaction with Players.
    {
        Command command;
        command.gods = true;
        command.name = "aggrolist";
        command.help = "Provides the list of opponents of the given target.";
        command.arguments = "(Target)";
        command.hndl = DoAggroList;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.gods = true;
        command.name = "transfer";
        command.help = "Transfer another character here, or to another room.";
        command.arguments = "(Target) [Where]";
        command.hndl = DoTransfer;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.gods = true;
        command.name = "feast";
        command.help = "Restores completely the health and the stamina of the target.";
        command.arguments = "(Target)";
        command.hndl = DoFeast;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.gods = true;
        command.name = "sflag";
        command.help = "Sets a flag for a character.";
        command.arguments = "(Target) (Flag)";
        command.hndl = DoSetFlag;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.gods = true;
        command.name = "cflag";
        command.help = "Remove a flag from a character.";
        command.arguments = "(Target) (Flag)";
        command.hndl = DoClearFlag;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.gods = true;
        command.name = "modskill";
        command.help = "Modify the value of the player skill.";
        command.arguments = "(Target) (Skill) (Value)";
        command.hndl = DoModSkill;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.gods = true;
        command.name = "modattribute";
        command.help = "Modify the value of the player attribute.";
        command.arguments = "(Target) (Attribute) (Value)";
        command.hndl = DoModAttr;
        Mud::instance().addCommand(command);
    }

    // ////////////////////////////////////////////////////////////////////////
    // Model related commands.
    // ////////////////////////////////////////////////////////////////////////
    Mud::instance().addCommand(
        Command(true, "model_info",
                "List all the information about a model.",
                "(Model.vnum)",
                DoModelInfo, true, false));
    Mud::instance().addCommand(
        Command(true, "model_list",
                "List all the models.",
                "Type --help for more information.",
                DoModelList, true, false));

    // ////////////////////////////////////////////////////////////////////////
    // Item related commands.
    // ////////////////////////////////////////////////////////////////////////
    Mud::instance().addCommand(
        Command(true, "item_create",
                "Create a new item, if not set the quality will be Normal.",
                "(Model.vnum)(Material.vnum)[Quality]",
                DoItemCreate, true, false));
    Mud::instance().addCommand(
        Command(true, "item_destroy",
                "Destroy the desired object.",
                "(Item.vnum)",
                DoItemDestroy, true, false));
    Mud::instance().addCommand(
        Command(true, "item_get",
                "Materialize the desired object.",
                "(Item.vnum)",
                DoItemGet, true, false));
    Mud::instance().addCommand(
        Command(true, "item_information",
                "Show information about an item.",
                "(Item.vnum)",
                DoItemInfo, true, false));
    Mud::instance().addCommand(
        Command(true, "item_list",
                "List all the items.",
                "Type --help for more information.",
                DoItemList, true, false));

    // ////////////////////////////////////////////////////////////////////////

    // Areas.
    {
        Command command;
        command.gods = true;
        command.name = "iarea";
        command.help = "Show the informations about a specific area.";
        command.arguments = "(Area.vnum)";
        command.hndl = DoAreaInfo;
        Mud::instance().addCommand(command);
    }

    // Rooms.
    {
        Command command;
        command.gods = true;
        command.name = "iroom";
        command.help = "Show the informations about a specific room.";
        command.arguments = "(Room.vnum)";
        command.hndl = DoRoomInfo;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.gods = true;
        command.name = "croom";
        command.help = "Create a room in the given direction.";
        command.arguments = "(Direction)";
        command.hndl = DoRoomCreate;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.gods = true;
        command.name = "droom";
        command.help = "Delete the room in the given direction.";
        command.arguments = "(Direction)";
        command.hndl = DoRoomDelete;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.gods = true;
        command.name = "eroom";
        command.help = "Change room values.";
        command.arguments = "(Option) (Value)";
        command.hndl = DoRoomEdit;
        Mud::instance().addCommand(command);
    }

    // Mobiles.
    {
        Command command;
        command.gods = true;
        command.name = "mob_kill";
        command.help = "Kill the desired mobile, in the same room.";
        command.arguments = "(Mobile.name)";
        command.hndl = DoMobileKill;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.gods = true;
        command.name = "mob_reload";
        command.help = "Reload the lua script for the target mobile, in the same room.";
        command.arguments = "(Mobile.name)";
        command.hndl = DoMobileReload;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.gods = true;
        command.name = "mob_trigger";
        command.help = "Trigger the main behaviour of a mobile.";
        command.arguments = "(Mobile.name)";
        command.hndl = DoMobileTrigger;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.gods = true;
        command.name = "mob_log";
        command.help = "Given a mobile id, it returns the curresponding mobile log.";
        command.arguments = "(Mobile.id)";
        command.hndl = DoMobileLog;
        Mud::instance().addCommand(command);
    }

    // Materials.
    {
        Command command;
        command.gods = true;
        command.name = "imaterial";
        command.help = "Show the informations about a specific material.";
        command.arguments = "(Material.vnum)";
        command.hndl = DoMaterialInfo;
        Mud::instance().addCommand(command);
    }

    // Liquids.
    {
        Command command;
        command.gods = true;
        command.name = "iliquid";
        command.help = "Show the informations about a specific liquid.";
        command.arguments = "(Liquid.vnum)";
        command.hndl = DoLiquidInfo;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.gods = true;
        command.name = "cliquid";
        command.help = "Materialize some liquid sinde a container.";
        command.arguments = "(Container.name)(Liquid.vnum)(Quantity)";
        command.hndl = DoLiquidCreate;
        Mud::instance().addCommand(command);
    }

    // Productions.
    {
        Command command;
        command.gods = true;
        command.name = "iproduction";
        command.help = "Provide all the information regarding the given production.";
        command.arguments = "(Production.vnum)";
        command.hndl = DoProductionInfo;
        Mud::instance().addCommand(command);
    }

    // Profession.
    {
        Command command;
        command.gods = true;
        command.name = "iprofession";
        command.help = "Provide all the information regarding the given profession.";
        command.arguments = "(Profession.command)";
        command.hndl = DoProfessionInfo;
        Mud::instance().addCommand(command);
    }

    {
        Command command;
        command.gods = true;
        command.name = "ifaction";
        command.help = "Shows the infos about a faction.";
        command.arguments = "(vnum)";
        command.hndl = DoFactionInfo;
        Mud::instance().addCommand(command);
    }

    //
    // LISTS
    //
    {
        Command command;
        command.gods = true;
        command.name = "lmobile";
        command.help = "List all the mobiles.";
        command.arguments = "NONE";
        command.hndl = DoMobileList;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.gods = true;
        command.name = "lplayer";
        command.help = "List all the players.";
        command.arguments = "NONE";
        command.hndl = DoPlayerList;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.gods = true;
        command.name = "larea";
        command.help = "List all the areas.";
        command.arguments = "NONE";
        command.hndl = DoAreaList;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.gods = true;
        command.name = "lroom";
        command.help = "List all the rooms.";
        command.arguments = "NONE";
        command.hndl = DoRoomList;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.gods = true;
        command.name = "lrace";
        command.help = "List all the races.";
        command.arguments = "NONE";
        command.hndl = DoRaceList;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.gods = true;
        command.name = "lfaction";
        command.help = "List all the factions.";
        command.arguments = "NONE";
        command.hndl = DoFactionList;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.gods = true;
        command.name = "lskill";
        command.help = "List all the skills.";
        command.arguments = "NONE";
        command.hndl = DoSkillList;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.gods = true;
        command.name = "lwriting";
        command.help = "List all the writings.";
        command.arguments = "NONE";
        command.hndl = DoWritingList;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.gods = true;
        command.name = "lcorpse";
        command.help = "List all the corpses.";
        command.arguments = "NONE";
        command.hndl = DoCorpseList;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.gods = true;
        command.name = "lcontinent";
        command.help = "List all the continents.";
        command.arguments = "NONE";
        command.hndl = DoContinentList;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.gods = true;
        command.name = "lmaterial";
        command.help = "List all the materials.";
        command.arguments = "NONE";
        command.hndl = DoMaterialList;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.gods = true;
        command.name = "lprofession";
        command.help = "Get the list of all the professions.";
        command.arguments = "NONE";
        command.hndl = DoProfessionList;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.gods = true;
        command.name = "lproduction";
        command.help = "Get the list of all the productions.";
        command.arguments = "NONE";
        command.hndl = DoProductionList;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.gods = true;
        command.name = "lliquid";
        command.help = "List all the liquids.";
        command.arguments = "NONE";
        command.hndl = DoLiquidList;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.gods = true;
        command.name = "lbuild";
        command.help = "List all the buildings.";
        command.arguments = "NONE";
        command.hndl = DoBuildingList;
        Mud::instance().addCommand(command);
    }
}

bool DoShutdown(Character * character, ArgumentHandler &)
{
    // Send message to all the players.
    Mud::instance().broadcastMsg(0, character->getNameCapital() + " has shut down the game!");
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
    Mud::instance().broadcastMsg(0, character->getNameCapital() + " is writing the history...");
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
    auto destination = Mud::instance().findRoom(ToNumber<int>(args[0].getContent()));
    if (destination == nullptr)
    {
        character->sendMsg("That room doesen't exists.\n");
        return false;
    }
    // Stop any action the character is executing.
    StopAction(character);
    // Prepare messages.
    auto msgDepart = character->getNameCapital() + " disappears in a puff of smoke!\n";
    auto msgArrive = character->getNameCapital() + " appears in a puff of smoke!\n";
    auto msgChar = "You go to room " + destination->name + ".\n";
    // Move player.
    character->moveTo(destination, msgDepart, msgArrive, msgChar);
    return true;
}

bool DoFindPath(Character * character, ArgumentHandler & args)
{
    if (args.size() != 1)
    {
        character->sendMsg("You have to provide a room vnum.\n\n");
    }
    else
    {
        auto room = character->room->area->getRoom(ToNumber<int>(args[0].getContent()));
        if (room != nullptr)
        {
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
                return !(character->isMobile() && HasFlag(destExit->flags, ExitFlag::NoMob));
            };
            if (aStar.findPath(character->room, room, path, checkFunction))
            {
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
            else
            {
                character->sendMsg("There is no path to that room.\n\n");
            }
        }
        else
        {
            character->sendMsg("There is no room with that VNUM.\n\n");
        }
    }
    return false;
}

bool DoTransfer(Character * character, ArgumentHandler & args)
{
    if ((args.size() != 1) && (args.size() != 2))
    {
        character->sendMsg("You have to select at least a target and optionally a destination.\n");
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
        destination = character->room->area->getRoom(ToNumber<int>(args[1].getContent()));
        if (destination == nullptr)
        {
            character->sendMsg("Can't find the target room, setting the current room.\n");
            return false;
        }
    }
    // Stop any action the character is executing.
    StopAction(target);
    // Prepare messages.
    auto msgDepart = target->getNameCapital() + " is yanked away by unseen forces!";
    auto msgArrive = target->getNameCapital() + " appears breathlessly!";
    auto msgChar = "\n" + character->getNameCapital() + " transfers you to another room!\n";
    // Move player.
    target->moveTo(destination, msgDepart, msgArrive, msgChar);
    character->sendMsg("You transfer %s to room %s.\n", target->getName(), destination->name);
    return true;
}

bool DoFeast(Character * character, ArgumentHandler & args)
{
    if (args.size() != 1)
    {
        character->sendMsg("You must insert a valide target.\n");
        return false;
    }
    auto target = character->room->findCharacter(args[0].getContent(), args[0].getIndex(), {});
    if (target == nullptr)
    {
        character->sendMsg("You must provide a valide target.\n");
        return false;
    }
    target->setHealth(target->getMaxHealth(), true);
    target->setStamina(target->getMaxStamina(), true);
    target->setThirst(100);
    target->setHunger(100);
    target->sendMsg("%sA banquet with any kind of delicacy appears from nowhere!%s%s\n",
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
        character->sendMsg("You can't find the player '" + args[0].getContent() + "'.\n");
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
    character->sendMsg("You set the flag '%s' for %s\n", GetCharacterFlagName(flag), target->getName());
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
        character->sendMsg("You can't find the player '" + args[0].getContent() + "'.\n");
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
    character->sendMsg("You clear the flag '%s' for %s\n", GetCharacterFlagName(flag), target->getName());
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
    auto itemModel = Mud::instance().findItemModel(ToNumber<int>(args[0].getContent()));
    if (itemModel == nullptr)
    {
        character->sendMsg("Item model not found.\n");
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
            character->sendMsg("Usage:\n");
            character->sendMsg("    model_list [options]\n");
            character->sendMsg("Options:\n");
            character->sendMsg("    -n [string]     Search models with the name which contains the given string.\n");
            character->sendMsg("    -t [type_name]  Search models of the given type.\n");
            character->sendMsg("    -m [model_vnum] Search models with the given vnum.\n");
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
            if (itemModel->name.find(modelName) == std::string::npos) continue;
        }
        if (!modelType.empty())
        {
            if (!BeginWith(ToLower(itemModel->getTypeName()), ToLower(modelType))) continue;
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
    auto itemModel = Mud::instance().findItemModel(ToNumber<int>(args[0].getContent()));
    auto material = Mud::instance().findMaterial(ToNumber<int>(args[1].getContent()));
    auto quality = ItemQuality(ItemQuality::Normal);
    if (itemModel == nullptr)
    {
        character->sendMsg("Cannot find model '%s'.\n", args[0].getContent());
        return false;
    }
    if (material == nullptr)
    {
        character->sendMsg("Cannot find material '%s'.\n", args[1].getContent());
        return false;
    }
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
    auto item = itemModel->createItem(character->getName(), material, false, quality, quantity);
    if (item == nullptr)
    {
        character->sendMsg("Creation failed.\n");
        return false;
    }
    character->addInventoryItem(item);
    character->sendMsg("You produce '%s' out of your apparently empty top hat.\n", item->getName(true));
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
        character->sendMsg("You cannot destroy an item which has item inside.\n");
        return false;
    }
    item->removeFromMud();
    item->removeOnDB();
    delete (item);
    character->sendMsg("You have destroyed the desired object.\n");
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
        character->sendMsg("The item was inside the room '%s' (%s)\n", item->room->name, ToString(item->room->vnum));
        item->room->removeItem(item);
    }
    else if (item->owner != nullptr)
    {
        character->sendMsg("The item was possessed by '%s'\n", item->owner->getName());
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
        character->sendMsg("The item was inside the container '%s'\n", item->container->getName(true));
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
        character->sendMsg("You must instert the item vnum or the name of the item inside the room.\n");
        return false;
    }
    auto item = Mud::instance().findItem(ToNumber<int>(args[0].getContent()));
    if (item == nullptr)
    {
        item = character->findNearbyItem(args[0].getContent(), args[0].getIndex());
        if (item == nullptr)
        {

            character->sendMsg("Cannot find the target item (provide vnum or item name).\n");
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
            character->sendMsg("Usage:\n");
            character->sendMsg("    item_list [options]\n");
            character->sendMsg("Options:\n");
            character->sendMsg("    -n [string]     Search items with the name which contains the given string.\n");
            character->sendMsg("    -t [type_name]  Search items of the given type.\n");
            character->sendMsg("    -m [model_vnum] Search the items of the model having the given vnum.\n");
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
            if (item->getName(false).find(itemName) == std::string::npos) continue;
        }
        if (!typeName.empty())
        {
            if (!BeginWith(ToLower(item->getTypeName()), ToLower(typeName))) continue;
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
        character->sendMsg("Sorry but in that direction you will go outside the boundaries.\n");
        return false;
    }
    // Find the room.
    auto targetRoom = currentArea->getRoom(targetCoord);
    if (targetRoom)
    {
        character->sendMsg("Sorry but in that direction there is already a room.\n");
        return false;
    }
    if (!CreateRoom(targetCoord, currentRoom))
    {
        character->sendMsg("Sorry but you couldn't create the room.\n");
        return false;
    }
    character->sendMsg("You have created a room at: %s\n", targetCoord.toString());
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
        character->sendMsg("Sorry but in that direction you will go outside the boundaries.\n");
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
        character->sendMsg("Sorry but you couldn't delete the selected room.\n");
        character->sendMsg("Probably there are items or characters in that room.\n");
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
    character->sendMsg("[%s;%s;%s]\n", ToString(coord.x), ToString(coord.y), ToString(coord.z));
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
        QueryList where = {std::make_pair("vnum", ToString(character->room->vnum))};
        if (!SQLiteDbms::instance().updateInto("Room", value, where))
        {
            character->sendMsg("Command gone wrong.\n");
            return false;
        }
        character->room->description = input;
        character->sendMsg("Room description modified.\n");
        return true;
    }
    else if (args[0].getContent() == "ter")
    {
        auto input = args.substr(1);
        if (input.empty())
        {
            character->sendMsg("You can't set an empty terrain.\n");
            return false;
        }
        QueryList value = {std::make_pair("terrain", input)};
        QueryList where = {std::make_pair("vnum", ToString(character->room->vnum))};
        if (!SQLiteDbms::instance().updateInto("Room", value, where))
        {
            character->sendMsg("Command gone wrong.\n");
            return false;
        }
        character->room->terrain = input;
        character->sendMsg("Room terrain modified.\n");
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
        QueryList where = {std::make_pair("vnum", ToString(character->room->vnum))};
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
    auto mobile = character->room->findMobile(args[0].getContent(), args[0].getIndex(), {});
    if (mobile == nullptr)
    {
        character->sendMsg("Mobile not found.\n");
        return false;
    }
    // Kill the mob.
    mobile->kill();
    // Notify the death.
    character->sendMsg("You snap your fingers.\n");
    character->room->sendToAll("%s fall to the ground dead.", {}, mobile->getNameCapital());
    return true;
}

bool DoMobileReload(Character * character, ArgumentHandler & args)
{
    if (args.size() != 1)
    {
        character->sendMsg("You must provide a target mobile.\n");
        return false;
    }
    auto mobile = character->room->findMobile(args[0].getContent(), args[0].getIndex(), {});
    if (mobile == nullptr)
    {
        character->sendMsg("Mobile not found.\n");
        return false;
    }
    // Reload the mob.
    mobile->reloadLua();
    // Notify.
    character->sendMsg("Target(%s) Script(%s)\n", mobile->getName(), mobile->lua_script);
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
        auto mobile = character->room->findMobile(args[0].getContent(), args[0].getIndex(), {});
        if (mobile == nullptr)
        {
            character->sendMsg("Mobile not found.\n");
        }
        else
        {
            mobile->triggerEventMain();
            character->sendMsg("%s::triggerEventMain()\n", mobile->getNameCapital());
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
    auto target = character->room->findCharacter(args[0].getContent(), args[0].getIndex(), {});
    if (target == nullptr)
    {
        character->sendMsg("Target not found.\n");
        return false;
    }
    // Set health to 1.
    target->setHealth(1);
    // Notify.
    character->sendMsg("You point your finger, %s cry in pain.\n", target->getName());
    target->sendMsg("%s points the finger towards you, you cry in pain.\n", character->getNameCapital());
    // Send the message inside the room.
    target->room->sendToAll("%s points the finger towards %s, %s cries in pain.\n",
                            {character, target},
                            character->getNameCapital(), target->getName(), target->getSubjectPronoun());
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
    auto target = character->room->findPlayer(args[0].getContent(), args[0].getIndex(), {});
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
        character->sendMsg("You cannot reduce the skill level to 0 or below.\n");
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
    character->sendMsg("You have successfully %s by %s the \"%s\" skill, the new level is %s.\n",
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
    auto target = character->room->findCharacter(args[0].getContent(), args[0].getIndex());
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
        character->sendMsg("Bad attribute name, accepted : (str, agi, per, con, int).\n");
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
    auto item = character->findNearbyItem(args[0].getContent(), args[0].getIndex());
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
    auto liquid = Mud::instance().findLiquid(ToNumber<int>(args[1].getContent()));
    if (liquid == nullptr)
    {
        character->sendMsg("Can't find the desire liquid.\n");
        return false;
    }
    auto quantity = ToNumber<int>(args[2].getContent());
    if ((quantity <= 0) || (quantity >= 100))
    {
        character->sendMsg("You must insert a valid quantity of liquids between 1 and 99.\n");
        return false;
    }
    if (!item->pourIn(liquid, static_cast<unsigned int>(quantity)))
    {
        character->sendMsg("The selected item can't contain that quantity of liquid.\n");
        return false;
    }
    character->sendMsg("You materialise %s units of %s inside %s.\n",
                       ToString(quantity),
                       liquid->getName(),
                       item->getName(true));
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
    msg += " Continent :" + area->continent + "\n";
    msg += " Width     :" + ToString(area->width) + "\n";
    msg += " Height    :" + ToString(area->height) + "\n";
    msg += " Elevation :" + ToString(area->elevation) + "\n";
    msg += " N. Rooms  :" + ToString(area->areaMap.size()) + "\n";
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
    msg += " Area        :" + ((room->area != nullptr) ? ToString(room->area->vnum) : "NULL") + "\n";
    msg += " X           :" + ToString(room->coord.x) + "\n";
    msg += " Y           :" + ToString(room->coord.y) + "\n";
    msg += " Z           :" + ToString(room->coord.z) + "\n";
    msg += " Name        :" + room->name + "\n";
    msg += " Description :" + room->description + "\n";
    msg += " Terrain     :" + room->terrain + "\n";
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
    auto material = Mud::instance().findMaterial(ToNumber<int>(args[0].getContent()));
    if (material == nullptr)
    {
        character->sendMsg("Can't find the desire material.\n");
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
    auto liquid = Mud::instance().findLiquid(ToNumber<int>(args[0].getContent()));
    if (liquid == nullptr)
    {
        character->sendMsg("Can't find the desire liquid.\n");
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
    Production * production = Mud::instance().findProduction(ToNumber<int>(args[0].getContent()));
    if (production == nullptr)
    {
        character->sendMsg("Can't find the desire production.\n");
        return false;
    }
    std::string msg;
    msg += "Vnum        : " + ToString(production->vnum) + "\n";
    msg += "Name        : " + production->name + "\n";
    msg += "Profession  : " + production->profession->command + "\n";
    msg += "Difficulty  : " + ToString(production->difficulty) + "\n";
    msg += "Time        : " + ToString(production->time) + "\n";
    msg += "Assisted    : " + ToString(production->assisted) + "\n";
    msg += "Outcome     : " + production->outcome->name + "*" + ToString(production->quantity) + "\n";
    msg += "Tools       :\n";
    for (auto iterator : production->tools)
    {
        msg += "                  " + GetToolTypeName(iterator) + "\n";
    }
    msg += "Ingredients :\n";
    for (auto iterator : production->ingredients)
    {
        msg += "                  " + iterator.first.toString() + "(" + ToString(iterator.second) + ")\n";
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
    Profession * profession = Mud::instance().findProfession(args[0].getContent());
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
    auto * faction = Mud::instance().findFaction(ToNumber<int>(args[0].getContent()));
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
            character->sendMsg("%s\t%s", ToString(aggressor->aggression), aggressor->aggressor->getNameCapital());
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
    table.addColumn("CONTINENT", StringAlign::Center);
    table.addColumn("ROOMS", StringAlign::Center);
    for (auto iterator : Mud::instance().mudAreas)
    {
        Area * area = iterator.second;
        // Prepare the row.
        TableRow row;
        row.push_back(ToString(area->vnum));
        row.push_back(area->name);
        row.push_back(area->builder);
        row.push_back(area->continent);
        row.push_back(ToString(area->areaMap.size()));
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
        row.push_back(room->terrain);
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

bool DoCorpseList(Character * character, ArgumentHandler & /*args*/)
{
    Table table;
    table.addColumn("VNUM", StringAlign::Right);
    table.addColumn("NAME", StringAlign::Left);
    table.addColumn("LOCATION", StringAlign::Left);
    for (auto iterator : Mud::instance().mudCorpses)
    {
        auto corpse = iterator.second;
        // Prepare the row.
        TableRow row;
        row.push_back(ToString(corpse->vnum));
        row.push_back(corpse->getNameCapital());
        if (corpse->owner != nullptr)
        {
            row.push_back(" Owner  : " + corpse->owner->getName());
        }
        else if (corpse->room != nullptr)
        {
            row.push_back(" Room   : " + ToString(corpse->room->vnum));
        }
        else if (corpse->container != nullptr)
        {
            row.push_back(" Inside : " + ToString(corpse->container->vnum));
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
