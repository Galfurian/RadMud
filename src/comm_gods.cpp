/// @file   comm_gods.cpp
/// @brief  Implements the methods used by <b>gods</b>.
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

// Basic Include.
#include <istream>

#include "commands.hpp"
#include "constants.hpp"
#include "defines.hpp"
#include "mud.hpp"
// Other Include.
#include "utilities/table.hpp"

using namespace std;

void DoShutdown(Character * character, std::istream & sArgs)
{
    // Check no more input.
    NoMore(character, sArgs);
    // Send message to all the players.
    Mud::getInstance().broadcastMsg(0, character->getNameCapital() + " has shut down the game!");
    Mud::getInstance().shutDown();
}

void DoGoTo(Character * character, std::istream & sArgs)
{
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    if (arguments.size() != 1)
    {
        character->sendMsg("You have to provide a room vnum.");
        return;
    }
    Room * destination = Mud::getInstance().findRoom(ToInt(arguments[0].first));
    if (destination == nullptr)
    {
        character->sendMsg("That room doesen't exists.\n");
        return;
    }

    // Check if the player it's already doing something.
    StopAction(character);
    // Prepare messages.
    std::string msgDepart = character->getNameCapital() + " disappears in a puff of smoke!\n";
    std::string msgArrive = character->getNameCapital() + " appears in a puff of smoke!\n";
    std::string msgChar = "You go to room " + destination->name + ".\n";
    // Move player.
    character->moveTo(destination, msgDepart, msgArrive, msgChar);
}

void DoTransfer(Character * character, std::istream & sArgs)
{
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    if ((arguments.size() != 1) && (arguments.size() != 2))
    {
        character->sendMsg("You have to select at least a target and optionally a destination.\n");
        return;
    }
    Character * target = nullptr;
    if (arguments.size() >= 1)
    {
        target = Mud::getInstance().findPlayer(arguments[0].first);
        if (target == nullptr)
        {
            target = Mud::getInstance().findMobile(arguments[0].first);
            if (target == nullptr)
            {
                character->sendMsg("Can't find the target character.\n");
                return;
            }
        }
    }
    Room * destination = character->room;
    if (arguments.size() == 2)
    {
        destination = character->room->area->getRoom(ToInt(arguments[1].first));
        if (destination == nullptr)
        {
            character->sendMsg("Can't find the target room, setting the current room.\n");
            return;
        }
    }
    // Check if the player it's already doing something.
    StopAction(target);
    // Prepare messages.
    std::string msgDepart = target->getNameCapital() + " is yanked away by unseen forces!";
    std::string msgArrive = target->getNameCapital() + " appears breathlessly!";
    std::string msgChar = "\n" + character->getNameCapital() + " transfers you to another room!\n";
    // Move player.
    target->moveTo(destination, msgDepart, msgArrive, msgChar);
    character->sendMsg("You transfer " + target->getName() + " to room " + destination->name + ".\n");
}

void DoSetFlag(Character * character, std::istream & sArgs)
{
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    if (arguments.size() != 2)
    {
        character->sendMsg("You must provide a target and a flag.");
        return;
    }
    Player * target = Mud::getInstance().findPlayer(arguments[0].first);
    if (target == nullptr)
    {
        character->sendMsg("You can't find the player '" + arguments[0].first + "'.\n");
        return;
    }
    CharacterFlag flag = static_cast<CharacterFlag>(ToInt(arguments[1].first));
    if (flag == CharacterFlag::None)
    {
        character->sendMsg("You must insert a valid flag.\n");
        return;
    }
    // Check if the character already has the flag.
    if (HasFlag(target->flags, flag))
    {
        character->sendMsg("Flag already set.\n");
        return;
    }
    // Set the flag.
    SetFlag(target->flags, flag);
    // Send confirmation to the player.
    character->sendMsg("You set the flag '" + GetCharacterFlagName(flag) + "' for " + target->getName() + "\n");
}

void DoClearFlag(Character * character, std::istream & sArgs)
{
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    if (arguments.size() != 2)
    {
        character->sendMsg("You must provide a target and a flag.");
        return;
    }
    Player * target = Mud::getInstance().findPlayer(arguments[0].first);
    if (target == nullptr)
    {
        character->sendMsg("You can't find the player '" + arguments[0].first + "'.\n");
        return;
    }
    CharacterFlag flag = static_cast<CharacterFlag>(ToInt(arguments[1].first));
    if (flag == CharacterFlag::None)
    {
        character->sendMsg("You must insert a valid flag.\n");
        return;
    }
    // Check if the character already has the flag.
    if (HasFlag(target->flags, flag))
    {
        character->sendMsg("Flag not set.\n");
        return;
    }
    // Set the flag.
    ClearFlag(target->flags, flag);
    // Send confirmation to the player.
    character->sendMsg("You clear the flag '" + GetCharacterFlagName(flag) + "' for " + target->getName() + "\n");
}

void DoModelInfo(Character * character, std::istream & sArgs)
{
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    if (arguments.size() != 1)
    {
        character->sendMsg("You must insert a model vnum.\n");
        return;
    }
    Model * model = Mud::getInstance().findModel(ToInt(arguments[0].first));
    if (model == nullptr)
    {
        character->sendMsg("Model not found.\n");
        return;
    }

    std::string msg;
    msg += Telnet::yellow() + "Model Vnum      " + Telnet::reset() + ": " + ToString(model->vnum) + "\n";
    msg += Telnet::yellow() + "Name            " + Telnet::reset() + ": " + model->name + "\n";
    msg += Telnet::yellow() + "Article         " + Telnet::reset() + ": " + model->article + "\n";
    msg += Telnet::yellow() + "Short Descr.    " + Telnet::reset() + ": " + model->shortdesc + "\n";
    msg += Telnet::yellow() + "Keys            " + Telnet::reset() + ": {";
    for (auto it : model->keys)
    {
        msg += " " + it;
    }
    msg += "}\n";
    msg += Telnet::yellow() + "Description     " + Telnet::reset() + ": " + model->description + "\n";
    msg += Telnet::yellow() + "Type            " + Telnet::reset() + ": " + GetModelTypeName(model->type) + "\n";
    msg += Telnet::yellow() + "Slot            " + Telnet::reset() + ": " + GetEquipmentSlotName(model->slot) + "\n";
    msg += Telnet::yellow() + "Flags           " + Telnet::reset() + ": " + GetModelFlagString(model->flags) + "\n";
    msg += Telnet::yellow() + "Weight          " + Telnet::reset() + ": " + ToString(model->weight) + " " + mud_measure
        + ".\n";
    msg += Telnet::yellow() + "Price           " + Telnet::reset() + ": " + ToString(model->price) + " " + mud_currency
        + ".\n";
    msg += Telnet::yellow() + "Condition       " + Telnet::reset() + ": " + ToString(model->condition) + "\n";
    msg += Telnet::yellow() + "Decay           " + Telnet::reset() + ": " + ToString(model->decay) + "\n";
    msg += Telnet::yellow() + "Material        " + Telnet::reset() + ": " + GetMaterialTypeName(model->material) + "\n";
    msg += Telnet::yellow() + "Tile Set        " + Telnet::reset() + ": " + ToString(model->tileSet) + "\n";
    msg += Telnet::yellow() + "Tile Id         " + Telnet::reset() + ": " + ToString(model->tileId) + "\n";
    msg += Telnet::yellow() + "Functions       " + Telnet::reset() + ": {";
    for (auto it : model->functions)
    {
        msg += " " + it;
    }
    msg += "}\n";

    switch (model->type)
    {
        case ModelType::NoType:
        {
            break;
        }
        case ModelType::Corpse:
        {
            break;
        }
        case ModelType::Weapon:
        {
            WeaponFunc func = model->getWeaponFunc();
            msg += Telnet::brown() + "Type            " + Telnet::reset() + ": " + GetWeaponTypeName(func.type) + "\n";
            msg += Telnet::brown() + "Minimum Damage  " + Telnet::reset() + ": " + ToString(func.minDamage) + "\n";
            msg += Telnet::brown() + "Maximum Damage  " + Telnet::reset() + ": " + ToString(func.maxDamage) + "\n";
            msg += Telnet::brown() + "Range           " + Telnet::reset() + ": " + ToString(func.range) + "\n";
            break;
        }
        case ModelType::Armor:
        {
            ArmorFunc func = model->getArmorFunc();
            msg += Telnet::brown() + "Size            " + Telnet::reset() + ": " + GetArmorSizeName(func.size) + "\n";
            msg += Telnet::brown() + "Damage Absorb. " + Telnet::reset() + ": " + ToString(func.damageAbs) + "\n";
            msg += Telnet::brown() + "Allowed Anatom. " + Telnet::reset() + ": " + ToString(func.allowedAnatomy) + "\n";
            break;
        }
        case ModelType::Shield:
        {
            ShieldFunc func = model->getShieldFunc();
            msg += Telnet::brown() + "Size            " + Telnet::reset() + ": " + GetShieldSizeName(func.size) + "\n";
            msg += Telnet::brown() + "Parry Chance    " + Telnet::reset() + ": " + ToString(func.parryChance) + "\n";
            break;
        }
        case ModelType::Projectile:
        {
            ProjectileFunc func = model->getProjectileFunc();
            msg += Telnet::brown() + "Damage Bonus    " + Telnet::reset() + ": " + ToString(func.damageBonus) + "\n";
            msg += Telnet::brown() + "Range Bonus     " + Telnet::reset() + ": " + ToString(func.rangeBonus) + "\n";
            break;
        }
        case ModelType::Container:
        {
            ContainerFunc func = model->getContainerFunc();
            msg += Telnet::brown() + "Max Weight      " + Telnet::reset() + ": " + ToString(func.maxWeight) + "\n";
            msg += Telnet::brown() + "Flags           " + Telnet::reset() + ": {" + GetContainerFlagString(func.flags)
                + "}\n";
            msg += Telnet::brown() + "Key Vnum        " + Telnet::reset() + ": " + ToString(func.keyVnum) + "\n";
            msg += Telnet::brown() + "Lockpicking Lv. " + Telnet::reset() + ": " + ToString(func.difficulty) + "\n";
            break;
        }
        case ModelType::LiqContainer:
        {
            LiqContainerFunc func = model->getLiqContainerFunc();
            msg += Telnet::brown() + "Max Weight      " + Telnet::reset() + ": " + ToString(func.maxWeight) + "\n";
            msg += Telnet::brown() + "Flags           " + Telnet::reset() + ": {"
                + GetLiqContainerFlagString(func.flags) + "}\n";
            break;
        }
        case ModelType::Tool:
        {
            ToolFunc func = model->getToolFunc();
            msg += Telnet::brown() + "Type            " + Telnet::reset() + ": " + GetToolTypeName(func.type) + "\n";
            break;
        }
        case ModelType::Node:
        {
            NodeFunc func = model->getNodeFunc();
            msg += Telnet::brown() + "Type            " + Telnet::reset() + ": " + GetNodeTypeName(func.type) + "\n";
            break;
        }
        case ModelType::Resource:
        {
            ResourceFunc func = model->getResourceFunc();
            msg += Telnet::brown() + "Type            " + Telnet::reset() + ": " + GetResourceTypeName(func.type)
                + "\n";
            break;
        }
        case ModelType::Seed:
        {
            SeedFunc func = model->getSeedFunc();
            msg += Telnet::brown() + "Type            " + Telnet::reset() + ": " + GetSeedTypeName(func.type) + "\n";
            break;
        }
        case ModelType::Key:
        {
            break;
        }
        case ModelType::Furniture:
        {
            break;
        }
        case ModelType::Food:
        {
            FoodFunc func = model->getFoodFunc();
            msg += Telnet::brown() + "Feeding         " + Telnet::reset() + ": " + ToString(func.hours) + "\n";
            msg += Telnet::brown() + "Flags           " + Telnet::reset() + ": {" + GetFoodFlagString(func.flags)
                + "}\n";
            break;
        }
        case ModelType::Light:
        {
            LightFunc func = model->getLightFunc();
            msg += Telnet::brown() + "Autonomy        " + Telnet::reset() + ": " + ToString(func.maxHours) + "\n";
            msg += Telnet::brown() + "Rechargeable    " + Telnet::reset() + ": " + ToString(func.policy) + "\n";
            break;
        }
        case ModelType::Vehicle:
        {
            break;
        }
        case ModelType::Pen:
        {
            break;
        }
        case ModelType::Book:
        {
            BookFunc func = model->getBookFunc();
            msg += Telnet::brown() + "Capacity        " + Telnet::reset() + ": " + ToString(func.maxParchments) + "\n";
            break;
        }
        case ModelType::Rope:
        {
            RopeFunc func = model->getRopeFunc();
            msg += Telnet::brown() + "Difficulty      " + Telnet::reset() + ": " + ToString(func.difficulty) + "\n";
            msg += Telnet::brown() + "Type            " + Telnet::reset() + ": " + ToString(func.type) + "\n";
            break;
        }
        case ModelType::Trash:
        {
            break;
        }
        case ModelType::Mechanism:
        {
            MechanismFunc func = model->getMechanismFunc();
            msg += Telnet::brown() + "Type            " + Telnet::reset() + ": " + GetMechanismTypeName(func.type)
                + "\n";
            switch (func.type)
            {
                case MechanismType::Door:
                case MechanismType::Lock:
                    msg += Telnet::brown() + "Key             " + Telnet::reset() + ": " + ToString(func.key) + "\n";
                    msg += Telnet::brown() + "Difficulty      " + Telnet::reset() + ": " + ToString(func.difficulty)
                        + "\n";
                    break;
                case MechanismType::Picklock:
                    msg += Telnet::brown() + "Efficency       " + Telnet::reset() + ": " + ToString(func.efficency)
                        + "\n";
                    break;
                case MechanismType::Lever:
                    msg += Telnet::brown() + "Command         " + Telnet::reset() + ": " + ToString(func.command)
                        + "\n";
                    msg += Telnet::brown() + "Target          " + Telnet::reset() + ": " + ToString(func.target) + "\n";
                    break;
                case MechanismType::None:
                    msg += Telnet::brown() + "None" + Telnet::reset() + "\n";
            }
            break;
        }
        case ModelType::Currency:
        {
            break;
        }
    }
    character->sendMsg(msg);
}

void DoItemCreate(Character * character, std::istream & sArgs)
{
    // Prevent mobiles to execute this command.
    NoMobile(character);
    // Stop any ongoing action.
    StopAction(character);
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    // Check the number of arguments.
    if ((arguments.size() != 2) && (arguments.size() != 3))
    {
        character->sendMsg("What do you want to create?\n");
        return; // Skip the rest of the function.
    }
    Model * model = Mud::getInstance().findModel(ToInt(arguments[0].first));
    Material * material = Mud::getInstance().findMaterial(ToInt(arguments[1].first));
    ItemQuality quality = ItemQuality::Normal;
    if (model == nullptr)
    {
        character->sendMsg("Cannot find the model '" + arguments[0].first + "'.\n");
        return;
    }
    if (material == nullptr)
    {
        character->sendMsg("Cannot find the material '" + arguments[1].first + ".\n");
        return;
    }
    if (arguments.size() == 3)
    {
        quality = static_cast<ItemQuality>(ToInt(arguments[2].first));
        if (quality == ItemQuality::None)
        {
            character->sendMsg("Not a valid quality '" + arguments[2].first + "'.\n");
            return;
        }
    }
    // Create the item.
    Item * item = model->createItem(character->getName(), material, quality);
    if (item == nullptr)
    {
        character->sendMsg("Creation failed.\n");
        return;
    }
    character->addInventoryItem(item);
    character->sendMsg(
        "You produce " + Telnet::yellow() + item->getName() + Telnet::reset()
            + " out of your apparently empty top hat.\n");
}

void DoItemGet(Character * character, std::istream & sArgs)
{
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    if (arguments.size() != 1)
    {
        character->sendMsg("You must instert an item vnum.\n");
        return;
    }
    Item * item = Mud::getInstance().findItem(ToInt(arguments[0].first));
    if (item == nullptr)
    {
        character->sendMsg("Invalid vnum.\n");
        return;
    }
    // Check if the God player can carry the item.
    if (!character->canCarry(item))
    {
        character->sendMsg("You can't carry anymore items.\n");
        return;
    }
    if (item->room != nullptr)
    {
        character->sendMsg("The item was in the room :" + item->room->name + "\n");
        FindErase(item->room->items, item);
        item->room = nullptr;

        // Remove the item from the table ItemRoom.
        QueryList where;
        where.push_back(std::make_pair("item", ToString(item->vnum)));
        Mud::getInstance().getDbms().deleteFrom("ItemRoom", where);
    }
    else if (item->owner != nullptr)
    {
        character->sendMsg("The item was on the character :" + item->owner->getName() + "\n");
        FindErase(item->owner->inventory, item);
        FindErase(item->owner->equipment, item);

        item->owner = nullptr;

        // Remove the item from the table ItemPlayer.
        QueryList where;
        where.push_back(std::make_pair("item", ToString(item->vnum)));
        Mud::getInstance().getDbms().deleteFrom("ItemPlayer", where);
    }
    else if (item->container != nullptr)
    {
        character->sendMsg("The item was inside the container:" + item->container->getName() + "\n");
        FindErase(item->container->content, item);
        item->container = nullptr;

        // Remove the item from the table Content.
        QueryList where;
        where.push_back(std::make_pair("item", ToString(item->vnum)));
        Mud::getInstance().getDbms().deleteFrom("ItemContent", where);
    }
    else
    {
        character->sendMsg("The item was nowhere, but with your mighty power you have retrieved it.\n");
    }
    if (!character->isMobile())
    {
        character->toPlayer()->addInventoryItem(item);
    }
    else
    {
        character->addInventoryItem(item);
    }
    character->sendMsg("You materialize the desired object in your hands.\n");
}

void DoItemDestroy(Character * character, std::istream & sArgs)
{
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    if (arguments.size() != 1)
    {
        character->sendMsg("You must instert an item vnum.\n");
        return;
    }
    Item * item = Mud::getInstance().findItem(ToInt(arguments[0].first));
    if (item == nullptr)
    {
        character->sendMsg("Invalid vnum.\n");
        return;
    }
    item->destroy();
    character->sendMsg("You have tryed to destroy the desired object.\n");
}

void DoItemInfo(Character * character, std::istream & sArgs)
{
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    if (arguments.size() != 1)
    {
        character->sendMsg("You must instert the item vnum or the name of the item inside the room.\n");
        return;
    }
    Item * item = Mud::getInstance().findItem(ToInt(arguments[0].first));
    if (item == nullptr)
    {
        item = character->findNearbyItem(arguments[0].first, arguments[0].second);
        if (item == nullptr)
        {

            character->sendMsg("Cannot find the target item (provide vnum or item name).\n");
            return;
        }
    }
    std::string msg;
    msg += "Vnum         : " + ToString(item->vnum) + "\n";
    msg += "Model        : [" + ToString(item->model->vnum) + "] " + item->getName() + ".\n";
    msg += "Type         : " + GetModelTypeName(item->model->type) + "\n";
    msg += "Maker        : " + item->maker + "\n";
    msg += "Condition    : " + ToString(item->condition) + " of " + ToString(item->model->condition) + "\n";
    msg += "Composition  : " + item->composition->name + " [" + ToString(item->composition->vnum) + "]\n";
    msg += "Quality      : " + GetItemQualityName(item->quality) + "\n";
    if (item->room != nullptr)
    {
        msg += "Room         : " + item->room->name + " [" + ToString(item->room->vnum) + "]\n";
    }
    else if (item->owner != nullptr)
    {
        msg += "Owner        : " + item->owner->getName() + "\n";
    }
    else if (item->container != nullptr)
    {
        msg += "Inside       : " + item->container->getName() + " [" + ToString(item->container->vnum) + "]\n";
    }
    if (!item->content.empty())
    {
        msg += "Content      :\n";
        for (auto iterator : item->content)
        {
            msg += "             * " + iterator->getName() + " [" + ToString(iterator->vnum) + "]\n";
        }
    }
    if (item->contentLiq.first != nullptr)
    {
        msg += "Content Liq. : ";
        msg += item->contentLiq.first->getNameCapital() + " [" + ToString(item->contentLiq.second) + "]\n";
    }
    msg += "Current Slot : " + item->getCurrentSlotName() + "\n";
    // Send the formatted message.
    character->sendMsg(msg);
}

void DoAreaInfo(Character * character, std::istream & sArgs)
{
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    if (arguments.size() != 1)
    {
        character->sendMsg("You must instert an area vnum.\n");
        return;
    }
    Area * area = Mud::getInstance().findArea(ToInt(arguments[0].first));
    if (area == nullptr)
    {
        character->sendMsg("The selected area does not exist.");
        return;
    }
    std::string msg;
    msg += Telnet::green() + "# Area Informations:\n" + Telnet::reset();
    msg += Telnet::yellow() + " Vnum            " + Telnet::reset() + ":" + ToString(area->vnum) + "\n";
    msg += Telnet::yellow() + " Name            " + Telnet::reset() + ":" + area->name + "\n";
    msg += Telnet::yellow() + " Builder         " + Telnet::reset() + ":" + area->builder + "\n";
    msg += Telnet::yellow() + " Continent       " + Telnet::reset() + ":" + area->continent + "\n";
    msg += Telnet::yellow() + " Width           " + Telnet::reset() + ":" + ToString(area->width) + "\n";
    msg += Telnet::yellow() + " Height          " + Telnet::reset() + ":" + ToString(area->height) + "\n";
    msg += Telnet::yellow() + " Elevation       " + Telnet::reset() + ":" + ToString(area->elevation) + "\n";
    msg += Telnet::yellow() + " Number of Rooms " + Telnet::reset() + ":" + ToString(area->numRooms) + "\n";
    character->sendMsg(msg);
}

void DoRoomInfo(Character * character, std::istream & sArgs)
{
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    Room * room;
    if (arguments.empty())
    {
        room = character->room;
    }
    else if (arguments.size() == 1)
    {
        room = Mud::getInstance().findRoom(ToInt(arguments[0].first));
        if (room == nullptr)
        {
            character->sendMsg("Can't find the desired room.\n");
            return;
        }
    }
    else
    {
        character->sendMsg("CanYu can only provide nothing or a room vnum.\n");
        return;
    }
    std::string msg;
    msg += Telnet::green() + "# Room Informations:\n" + Telnet::reset();
    msg += Telnet::yellow() + " Vnum        " + Telnet::reset() + ":" + ToString(room->vnum) + "\n";
    msg += Telnet::yellow() + " X           " + Telnet::reset() + ":" + ToString(room->coord.x) + "\n";
    msg += Telnet::yellow() + " Y           " + Telnet::reset() + ":" + ToString(room->coord.y) + "\n";
    msg += Telnet::yellow() + " Z           " + Telnet::reset() + ":" + ToString(room->coord.z) + "\n";
    msg += Telnet::yellow() + " Name        " + Telnet::reset() + ":" + room->name + "\n";
    msg += Telnet::yellow() + " Description " + Telnet::reset() + ":" + room->description + "\n";
    msg += Telnet::yellow() + " Terrain     " + Telnet::reset() + ":" + room->terrain + "\n";
    msg += Telnet::yellow() + " Flags       " + Telnet::reset() + ":" + GetRoomFlagString(room->flags) + "\n";
    character->sendMsg(msg);
}

void DoRoomCreate(Character * character, std::istream & sArgs)
{
    // Stop any action the character is executing.
    StopAction(character);
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    // Check the number of arguments.
    if (arguments.size() != 1)
    {
        character->sendMsg("Usage: [direction]\n");
        return; // Skip the rest of the function.
    }
    Room * currentRoom = character->room;
    Area * currentArea = currentRoom->area;
    if (currentArea == nullptr)
    {
        character->sendMsg("Your room's area has not been defined!\n");
        return; // Skip the rest of the function.
    }
    // Check if it's a direction.
    Direction direction = Mud::getInstance().findDirection(arguments[0].first, false);
    if (direction == Direction::None)
    {
        character->sendMsg("You must insert a valid direction!\n");
        return; // Skip the rest of the function.
    }
    // Evaluate the target coordinates.
    Coordinates targetCoord = currentRoom->coord + GetCoordinates(direction);
    // Find the room.
    Room * targetRoom = currentArea->getRoom(targetCoord);
    if (targetRoom)
    {
        character->sendMsg("Sorry but in that direction there is already a room.\n");
        return; // Skip the rest of the function.
    }
    if (!currentArea->inBoundaries(targetCoord))
    {
        character->sendMsg("Sorry but in that direction you will go outside the boundaries.\n");
        return; // Skip the rest of the function.
    }
    if (!CreateRoom(targetCoord, currentRoom))
    {
        character->sendMsg("Sorry but you couldn't create the room.\n");
        return; // Skip the rest of the function.
    }
    character->sendMsg("You have created a room at coordinates :\n");
    std::string msg = "    [";
    msg += ToString(targetCoord.x) + ";";
    msg += ToString(targetCoord.y) + ";";
    msg += ToString(targetCoord.z) + "]\n";
    character->sendMsg(msg);
}

void DoRoomDelete(Character * character, std::istream & sArgs)
{
    // Stop any action the character is executing.
    StopAction(character);
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    // Check the number of arguments.
    if (arguments.size() != 1)
    {
        character->sendMsg("Usage: [direction]\n");
        return; // Skip the rest of the function.
    }
    Area * currentArea = character->room->area;
    if (currentArea == nullptr)
    {
        character->sendMsg("Your room's area has not been defined!\n");
        return; // Skip the rest of the function.
    }
    // Check if it's a direction.
    Direction direction = Mud::getInstance().findDirection(arguments[0].first, false);
    if (direction == Direction::None)
    {
        character->sendMsg("You must insert a valid direction!\n");
        return; // Skip the rest of the function.
    }
    // Evaluate the target coordinates.
    Coordinates targetCoord = character->room->coord + GetCoordinates(direction);
    // Find the room.
    Room * targetRoom = currentArea->getRoom(targetCoord);
    if (targetRoom == nullptr)
    {
        character->sendMsg("Sorry but in that direction there is no room.\n");
        return; // Skip the rest of the function.
    }
    // Remove the room from the Database.
    if (!targetRoom->removeOnDB())
    {
        character->sendMsg("Sorry but you couldn't delete the selected room.\n");
        character->sendMsg("Probably there are items or characters in that room.\n");
        return; // Skip the rest of the function.
    }
    // Remove the room from the list of rooms.
    if (!Mud::getInstance().remRoom(targetRoom))
    {
        character->sendMsg("You cannot remove the room.\n");
        return; // Skip the rest of the function.
    }

    // Delete completely the room.
    delete (targetRoom);

    character->sendMsg("You have destroyed the room at coordinates :\n");
    std::string msg = "    [";
    msg += ToString(targetCoord.x) + ";";
    msg += ToString(targetCoord.y) + ";";
    msg += ToString(targetCoord.z) + "]\n";
    character->sendMsg(msg);
}

void DoRoomEdit(Character * character, std::istream & sArgs)
{
    NoMobile(character);
    Player * player = character->toPlayer();

    string input;
    Room * room = player->room;

    sArgs >> input >> ws;

    if (input == "des")
    {
        getline(sArgs, input);
        if (input.empty())
        {
            player->sendMsg("You can't set an empty description.\n");
            return;
        }

        QueryList value =
        { std::make_pair("description", input) };
        QueryList where =
        { std::make_pair("vnum", ToString(room->vnum)) };

        if (!Mud::getInstance().getDbms().updateInto("Room", value, where))
        {
            player->sendMsg("Command gone wrong.\n");
            return;
        }

        room->description = input;
        player->sendMsg("Room description modified.\n");
    }
    else if (input == "ter")
    {
        getline(sArgs, input);
        if (input.empty())
        {
            player->sendMsg("You can't set an empty terrain.\n");
            return;
        }
        QueryList value =
        { std::make_pair("terrain", input) };
        QueryList where =
        { std::make_pair("vnum", ToString(room->vnum)) };

        if (!Mud::getInstance().getDbms().updateInto("Room", value, where))
        {
            player->sendMsg("Command gone wrong.\n");
            return;
        }

        room->terrain = input;
        player->sendMsg("Room terrain modified.\n");
    }
    else if (input == "nam")
    {
        getline(sArgs, input);
        if (input.empty())
        {
            player->sendMsg("You can't set an empty name.\n");
            return;
        }

        QueryList value =
        { std::make_pair("name", input) };
        QueryList where =
        { std::make_pair("vnum", ToString(room->vnum)) };
        if (!Mud::getInstance().getDbms().updateInto("Room", value, where))
        {
            player->sendMsg("Command gone wrong.\n");
            return;
        }

        room->name = input;
        player->sendMsg("Room name modified.\n");
    }
    else if (input == "sflag")
    {
        getline(sArgs, input);
        if (input.empty())
        {
            player->sendMsg("Wrong flag.\n");
            return;
        }
        if (input == "R")
        {
            SetFlag(room->flags, RoomFlag::Rent);
        }
        else if (input == "P")
        {
            SetFlag(room->flags, RoomFlag::Peaceful);
        }
        else
        {
            player->sendMsg("Not a valid flag.\n");
        }
    }
    else if (input == "cflag")
    {
        getline(sArgs, input);
        if (input.empty())
        {
            player->sendMsg("Wrong flag.\n");
            return;
        }
        if (input == "R")
        {
            ClearFlag(room->flags, RoomFlag::Rent);
        }
        else if (input == "P")
        {
            ClearFlag(room->flags, RoomFlag::Peaceful);
        }
        else
        {
            player->sendMsg("Not a valid flag.\n");
        }
    }
    else
    {
        player->sendMsg("Usage:                           \n");
        player->sendMsg("|  Argument  |  Text            |\n");
        player->sendMsg("| ---------- | ---------------- |\n");
        player->sendMsg("|  nam       |  [name]          |\n");
        player->sendMsg("|  ter       |  [terrain]       |\n");
        player->sendMsg("|  des       |  [description]   |\n");
        player->sendMsg("|  sflag     |  [Set Flag]      |\n");
        player->sendMsg("|            |   R -> kRent     |\n");
        player->sendMsg("|            |   P -> kPeaceful |\n");
        player->sendMsg("|  cflag     |  [Clear Flag]    |\n");
        player->sendMsg("|            |   R -> kRent     |\n");
        player->sendMsg("|            |   P -> kPeaceful |\n");
        player->sendMsg("| ---------- | ---------------- |\n");
        return;
    }
}

void DoMobileInfo(Character * character, std::istream & sArgs)
{
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    if (arguments.size() != 1)
    {
        character->sendMsg("You must instert a mobile id.\n");
        return;
    }
    Mobile * mobile = Mud::getInstance().findMobile(arguments[0].first);
    if (mobile == nullptr)
    {
        character->sendMsg("Mobile not found.\n");
        return;
    }
    std::string msg;
    msg += Telnet::green() + "# Mobile:\n" + Telnet::reset();
    msg += Telnet::yellow() + "    Mobile Id    " + Telnet::reset() + ":" + mobile->id + "\n";
    msg += Telnet::yellow() + "    Spawn Room   " + Telnet::reset() + ":" + ToString(mobile->respawnRoom->vnum) + "\n";
    msg += Telnet::yellow() + "    Keys         " + Telnet::reset() + ": {";
    for (auto it : mobile->keys)
    {
        msg += " " + it;
    }
    msg += "}\n";
    msg += Telnet::yellow() + "    Short Desc   " + Telnet::reset() + ":" + mobile->shortdesc + "\n";
    msg += Telnet::yellow() + "    Static Desc  " + Telnet::reset() + ":" + mobile->staticdesc + "\n";
    msg += Telnet::yellow() + "    Actions      " + Telnet::reset() + ": {";
    for (auto it : mobile->actions)
    {
        msg += " " + it;
    }
    msg += "}\n";
    msg += Telnet::yellow() + "    Alive        " + Telnet::reset() + ":" + ToString(mobile->alive) + "\n";
    if (!mobile->alive)
    {
        // Get the current time.
        TimeClock currentTime = std::chrono::system_clock::now();
        // Evaluate the elapsed time.
        long int respawn = std::chrono::duration_cast<std::chrono::seconds>(currentTime - mobile->nextRespawn).count();
        msg += Telnet::yellow() + "    Respawn In   " + Telnet::reset() + ":" + ToString(respawn) + "\n";
    }
    if (mobile->controller != nullptr)
    {
        msg += Telnet::red() + "    Controller   " + Telnet::reset() + ":" + mobile->controller->getName() + "\n";
    }
    msg += Telnet::red() + "    Lua Script   " + Telnet::reset() + ":" + mobile->lua_script + "\n";

    msg += Telnet::green() + "# Chracter:\n" + Telnet::reset();
    msg += Telnet::yellow() + "    Proper Noun  " + Telnet::reset() + ":" + mobile->name + "\n";
    msg += Telnet::yellow() + "    Description  " + Telnet::reset() + ":" + mobile->description + "\n";
    msg += Telnet::yellow() + "    Sex          " + Telnet::reset() + ":" + ToString(mobile->sex) + "\n";
    msg += Telnet::yellow() + "    Weight       " + Telnet::reset() + ":" + ToString(mobile->weight) + "\n";
    msg += Telnet::yellow() + "    Level        " + Telnet::reset() + ":" + ToString(mobile->level) + "\n";
    msg += Telnet::yellow() + "    Flags        " + Telnet::reset() + ":" + ToString(mobile->flags) + "\n";
    msg += Telnet::yellow() + "    Race         " + Telnet::reset() + ":" + mobile->race->name + "\n";
    msg += Telnet::yellow() + "    Faction      " + Telnet::reset() + ":" + mobile->faction->name + "\n";
    msg += Telnet::yellow() + "    Health       " + Telnet::reset() + ":" + ToString(mobile->health) + "\n";
    msg += Telnet::yellow() + "    Stamina      " + Telnet::reset() + ":" + ToString(mobile->stamina) + "\n";
    msg += Telnet::yellow() + "    Hunger       " + Telnet::reset() + ":" + ToString(mobile->hunger) + "\n";
    msg += Telnet::yellow() + "    Thirst       " + Telnet::reset() + ":" + ToString(mobile->thirst) + "\n";
    msg += Telnet::yellow() + "    Strength     " + Telnet::reset() + ":" + ToString(mobile->strength) + "\n";
    msg += Telnet::yellow() + "    Agility      " + Telnet::reset() + ":" + ToString(mobile->agility) + "\n";
    msg += Telnet::yellow() + "    Perception   " + Telnet::reset() + ":" + ToString(mobile->perception) + "\n";
    msg += Telnet::yellow() + "    Constitution " + Telnet::reset() + ":" + ToString(mobile->constitution) + "\n";
    msg += Telnet::yellow() + "    Intelligence " + Telnet::reset() + ":" + ToString(mobile->intelligence) + "\n";
    msg += Telnet::yellow() + "    Current Room " + Telnet::reset() + ":" + ToString(mobile->room->vnum) + "\n";
    msg += Telnet::yellow() + "    Inventory    " + Telnet::reset() + ":\n";
    for (Item * item : mobile->inventory)
    {
        msg += "        " + item->getName() + "\n";
    }
    msg += Telnet::yellow() + "    Equipment    " + Telnet::reset() + ":\n";
    for (Item * item : mobile->equipment)
    {
        msg += "        " + item->getName() + "\n";
    }
    msg += Telnet::yellow() + "    Posture      " + Telnet::reset() + ":" + GetPostureName(mobile->posture) + "\n";
    msg += Telnet::yellow() + "    Effects      " + Telnet::reset() + ":\n";
    for (auto effect : mobile->effects.activeEffects)
    {
        msg += "                [" + ToString(effect.expires) + "] " + effect.name + "\n";
    }
    msg += Telnet::yellow() + "    Action       " + Telnet::reset() + ":" + GetActionTypeName(mobile->action.getType())
        + "\n";

    character->sendMsg(msg);
}

void DoMobileKill(Character * character, std::istream & sArgs)
{
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    if (arguments.size() != 1)
    {
        character->sendMsg("You must provide a target mobile.\n");
        return;
    }
    Mobile * mobile = character->room->findMobile(arguments[0].first, arguments[0].second);
    if (mobile == nullptr)
    {
        character->sendMsg("Mobile not found.\n");
        return;
    }
    // Kill the mob.
    mobile->triggerDeath();
    // Notify the death.
    character->sendMsg("You snap your fingers.\n");
    character->room->sendToAll(mobile->getNameCapital() + " fall to the ground dead.");
}

void DoMobileReload(Character * character, std::istream & sArgs)
{
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    if (arguments.size() != 1)
    {
        character->sendMsg("You must provide a target mobile.\n");
        return;
    }
    Mobile * mobile = character->room->findMobile(arguments[0].first, arguments[0].second);
    if (mobile == nullptr)
    {
        character->sendMsg("Mobile not found.\n");
        return;
    }
    // Reload the mob.
    mobile->reloadLua();
    // Notify.
    character->sendMsg("Target(" + mobile->getName() + ") Script(" + mobile->lua_script + ")\n");
}
void DoMobileLog(Character * character, std::istream &sArgs)
{
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    if (arguments.size() != 1)
    {
        character->sendMsg("You must provide a mobile id.\n");
        return;
    }
    Mobile * mobile = Mud::getInstance().findMobile(arguments[0].first);
    if (mobile == nullptr)
    {
        character->sendMsg("Mobile not found.\n");
        return;
    }
    // Notify.
    character->sendMsg("Target(" + mobile->getName() + ")\n");
    character->sendMsg("Log:" + mobile->message_buffer + "\n");
}

void DoHurt(Character * character, std::istream & sArgs)
{
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    if (arguments.size() != 1)
    {
        character->sendMsg("Who do you want to hurt?\n");
        return;
    }
    Character * target = character->room->findCharacter(arguments[0].first, arguments[0].second);
    if (target == nullptr)
    {
        character->sendMsg("Target not found.\n");
        return;
    }
    // Set health to 1.
    target->health = 1;
    // Notify.
    character->sendMsg("You point your finger, " + target->getName() + " cry in pain.\n");
    target->sendMsg(character->getNameCapital() + " points the finger towards you, you cry in pain.\n");
}

void DoInvisibility(Character * character, std::istream & sArgs)
{
    // Check no more input.
    NoMore(character, sArgs);
    // Check if already invisibile.
    if (HasFlag(character->flags, CharacterFlag::Invisible))
    {
        character->sendMsg("Don't worry, you are already invisible.\n");
        return;
    }
    // Set the character invisible.
    SetFlag(character->flags, CharacterFlag::Invisible);
    character->sendMsg("You are invisible now.\n");
}

void DoVisible(Character * character, std::istream & sArgs)
{
    // Check no more input.
    NoMore(character, sArgs);
    // Check if already visible.
    if (!HasFlag(character->flags, CharacterFlag::Invisible))
    {
        character->sendMsg("You are not invisible.\n");
        return;
    }
    // Set the character visible.
    ClearFlag(character->flags, CharacterFlag::Invisible);
    character->sendMsg("You are no more invisible.\n");
}

void DoModSkill(Character * character, std::istream & sArgs)
{
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    if (arguments.size() != 3)
    {
        character->sendMsg("Usage: [target] [#skill] [+/-VALUE]\n");
        return;
    }
    Player * target = character->room->findPlayer(arguments[0].first, arguments[0].second);
    if (target == nullptr)
    {
        character->sendMsg("Target not found.\n");
        return;
    }
    Skill * skill = Mud::getInstance().findSkill(ToInt(arguments[1].first));
    if (skill == nullptr)
    {
        character->sendMsg("Cannot find the desired skill.\n");
        return;
    }
    int modifier = ToInt(arguments[2].first);
    if ((modifier == 0) || (modifier < -100) || (modifier > 100))
    {
        character->sendMsg("You must insert a valid modifier.\n");
        return;
    }
    int current = target->skills[skill->vnum];
    if ((current + modifier) <= 0)
    {
        character->sendMsg("You cannot reduce the skill level to 0 or below.\n");
        return;
    }
    if ((current + modifier) >= 100)
    {
        character->sendMsg("You cannot increase the skill to 100 or above.\n");
        return;
    }
    // Change the skill value.
    target->skills[skill->vnum] += modifier;
    // Notify.
    std::string msg;
    msg += "You have successfully ";
    msg += ((modifier > 0) ? "increased " : "decreased");
    msg += " by " + ToString(modifier) + " the \"" + skill->name + "\" skill, ";
    msg += "the new level is " + ToString(target->skills[skill->vnum]) + ".\n";
    character->sendMsg(msg);
}

void DoModAttr(Character * character, std::istream & sArgs)
{
    // Stop any action the character is executing.
    StopAction(character);
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    // Check the number of arguments.
    if (arguments.size() != 3)
    {
        character->sendMsg("Usage: [target] [attribute] [+/-VALUE]\n");
        return; // Skip the rest of the function.
    }
    Character * target = character->room->findCharacter(arguments[0].first, arguments[0].second);
    if (target == nullptr)
    {
        character->sendMsg("Target not found.\n");
        return; // Skip the rest of the function.
    }
    int modifier = ToInt(arguments[2].first);
    if (modifier == 0)
    {
        character->sendMsg("You must insert a valid value.\n");
        return; // Skip the rest of the function.
    }
    std::string attribute = arguments[1].first;
    std::string attrName;
    if (attribute == "str")
    {
        int result = target->strength + modifier;
        if ((result < 0) || (result > 100))
        {
            character->sendMsg("Attribute cannot go below 0 or above 100.");
            return; // Skip the rest of the function.
        }
        character->strength = result;
        attrName = "strength";
    }
    else if (attribute == "agi")
    {
        int result = target->agility + modifier;
        if ((result < 0) || (result > 100))
        {
            character->sendMsg("Attribute cannot go below 0 or above 100.");
            return; // Skip the rest of the function.
        }
        character->agility = result;
        attrName = "agility";
    }
    else if (attribute == "per")
    {
        int result = target->perception + modifier;
        if ((result < 0) || (result > 100))
        {
            character->sendMsg("Attribute cannot go below 0 or above 100.");
            return; // Skip the rest of the function.
        }
        character->perception = result;
        attrName = "perception";
    }
    else if (attribute == "con")
    {
        int result = target->constitution + modifier;
        if ((result < 0) || (result > 100))
        {
            character->sendMsg("Attribute cannot go below 0 or above 100.");
            return; // Skip the rest of the function.
        }
        character->constitution = result;
        attrName = "constitution";
    }
    else if (attribute == "int")
    {
        int result = target->intelligence + modifier;
        if ((result < 0) || (result > 100))
        {
            character->sendMsg("Attribute cannot go below 0 or above 100.");
            return; // Skip the rest of the function.
        }
        character->intelligence = result;
        attrName = "intelligence";
    }
    else
    {
        character->sendMsg("Bad attribute name, accepted : (str, agi, per, con, int).\n");
        return;
    }
    std::string msg;
    msg += "You have successfully ";
    msg += std::string((modifier > 0) ? "increased" : "decreased");
    msg += " by " + ToString(modifier);
    msg += " the " + attrName + " of the target.";
    character->sendMsg(msg);
}

void DoPlayerInfo(Character * character, std::istream & sArgs)
{
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    if (arguments.size() != 1)
    {
        character->sendMsg("You must insert a valide player name.\n");
        return;
    }
    Player * player = Mud::getInstance().findPlayer(arguments[0].first);
    if (player == nullptr)
    {
        character->sendMsg("Player not found.\n");
        return;
    }
    std::string msg;
    msg += Telnet::green() + "# Player:\n" + Telnet::reset();
    msg += Telnet::yellow() + "    Age          " + Telnet::reset() + ":" + ToString(player->age) + "\n";
    msg += Telnet::yellow() + "    Experience   " + Telnet::reset() + ":" + ToString(player->experience) + "\n";
    msg += Telnet::yellow() + "    Prompt       " + Telnet::reset() + ":" + player->prompt + "\n";
    msg += Telnet::yellow() + "    Rent Room    " + Telnet::reset() + ":" + ToString(player->rent_room) + "\n";
    msg += Telnet::yellow() + "    Skills       " + Telnet::reset() + ":\n";
    for (auto iterator : Mud::getInstance().mudSkills)
    {
        msg += "        " + iterator.second.name + "[" + ToString(player->skills[iterator.first]) + "]\n";
    }

    msg += Telnet::green() + "# Chracter:\n" + Telnet::reset();
    msg += Telnet::yellow() + "    Proper Noun  " + Telnet::reset() + ":" + player->name + "\n";
    msg += Telnet::yellow() + "    Description  " + Telnet::reset() + ":" + player->description + "\n";
    msg += Telnet::yellow() + "    Sex          " + Telnet::reset() + ":" + ToString(player->sex) + "\n";
    msg += Telnet::yellow() + "    Weight       " + Telnet::reset() + ":" + ToString(player->weight) + "\n";
    msg += Telnet::yellow() + "    Level        " + Telnet::reset() + ":" + ToString(player->level) + "\n";
    msg += Telnet::yellow() + "    Flags        " + Telnet::reset() + ":" + ToString(player->flags) + "\n";
    msg += Telnet::yellow() + "    Race         " + Telnet::reset() + ":" + player->race->name + "\n";
    msg += Telnet::yellow() + "    Faction      " + Telnet::reset() + ":" + player->faction->name + "\n";
    msg += Telnet::yellow() + "    Health       " + Telnet::reset() + ":" + ToString(player->health) + "\n";
    msg += Telnet::yellow() + "    Stamina      " + Telnet::reset() + ":" + ToString(player->stamina) + "\n";
    msg += Telnet::yellow() + "    Hunger       " + Telnet::reset() + ":" + ToString(player->hunger) + "\n";
    msg += Telnet::yellow() + "    Thirst       " + Telnet::reset() + ":" + ToString(player->thirst) + "\n";
    msg += Telnet::yellow() + "    Strength     " + Telnet::reset() + ":" + ToString(player->strength) + "\n";
    msg += Telnet::yellow() + "    Agility      " + Telnet::reset() + ":" + ToString(player->agility) + "\n";
    msg += Telnet::yellow() + "    Perception   " + Telnet::reset() + ":" + ToString(player->perception) + "\n";
    msg += Telnet::yellow() + "    Constitution " + Telnet::reset() + ":" + ToString(player->constitution) + "\n";
    msg += Telnet::yellow() + "    Intelligence " + Telnet::reset() + ":" + ToString(player->intelligence) + "\n";
    msg += Telnet::yellow() + "    Current Room " + Telnet::reset() + ":" + ToString(player->room->vnum) + "\n";
    msg += Telnet::yellow() + "    Inventory    " + Telnet::reset() + ":\n";
    for (Item * item : player->inventory)
    {
        msg += "        " + item->getName() + "\n";
    }
    msg += Telnet::yellow() + "    Equipment    " + Telnet::reset() + ":\n";
    for (Item * item : player->equipment)
    {
        msg += "        " + item->getName() + "\n";
    }
    msg += Telnet::yellow() + "    Posture      " + Telnet::reset() + ":" + GetPostureName(player->posture) + "\n";
    msg += Telnet::yellow() + "    Effects      " + Telnet::reset() + ":\n";
    for (auto effect : player->effects.activeEffects)
    {
        msg += "                [" + ToString(effect.expires) + "] " + effect.name + "\n";
    }
    msg += Telnet::yellow() + "    Action       " + Telnet::reset() + ":" + GetActionTypeName(player->action.getType())
        + "\n";
    character->sendMsg(msg);
}

void DoMaterialInfo(Character * character, std::istream & sArgs)
{
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    if (arguments.size() != 1)
    {
        character->sendMsg("You must insert a valide material vnum.\n");
        return;
    }
    Material * material = Mud::getInstance().findMaterial(ToInt(arguments[0].first));
    if (material == nullptr)
    {
        character->sendMsg("Can't find the desire material.\n");
        return;
    }
    std::string msg;
    msg += Telnet::yellow() + "Vnum            " + Telnet::reset() + ": " + ToString(material->vnum) + "\n";
    msg += Telnet::yellow() + "Type            " + Telnet::reset() + ": " + GetMaterialTypeName(material->type) + "\n";
    msg += Telnet::yellow() + "Name            " + Telnet::reset() + ": " + material->name + "\n";
    msg += Telnet::yellow() + "Worth           " + Telnet::reset() + ": " + ToString(material->worth) + "\n";
    msg += Telnet::yellow() + "Hardness        " + Telnet::reset() + ": " + ToString(material->hardness) + "\n";
    msg += Telnet::yellow() + "Lightness       " + Telnet::reset() + ": " + ToString(material->lightness) + "\n";
    character->sendMsg(msg);
}

void DoLiquidCreate(Character * character, std::istream & sArgs)
{
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    if (arguments.size() != 3)
    {
        character->sendMsg("Usage: [Item] [Liquid] [Quantity].\n");
        return;
    }
    Item * item = character->findNearbyItem(arguments[0].first, arguments[0].second);
    if (item == nullptr)
    {
        character->sendMsg("Can't find the desire item.\n");
        return;
    }
    if (item->model->type != ModelType::LiqContainer)
    {
        character->sendMsg("The item is not a container of liquids.\n");
        return;
    }
    Liquid * liquid = Mud::getInstance().findLiquid(ToInt(arguments[1].first));
    if (liquid == nullptr)
    {
        character->sendMsg("Can't find the desire liquid.\n");
        return;
    }
    int quantity = ToInt(arguments[2].first);
    if ((quantity <= 0) || (quantity >= 100))
    {
        character->sendMsg("You must insert a valid quantity of liquids between 1 and 99.\n");
        return;
    }
    if (!item->pourIn(liquid, quantity))
    {
        character->sendMsg("The selected item can't contain that quantity of liquid.\n");
        return;
    }
    character->sendMsg(
        "You materialise " + ToString(quantity) + " units of " + liquid->getName() + " inside " + item->getName()
            + ".\n");
}

void DoLiquidInfo(Character * character, std::istream & sArgs)
{
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    if (arguments.size() != 1)
    {
        character->sendMsg("You must provide a liquid vnum.\n");
        return;
    }
    Liquid * liquid = Mud::getInstance().findLiquid(ToInt(arguments[0].first));
    if (liquid == nullptr)
    {
        character->sendMsg("Can't find the desire liquid.\n");
        return;
    }
    std::string msg;
    msg += Telnet::yellow() + "Vnum            " + Telnet::reset() + ": " + ToString(liquid->vnum) + "\n";
    msg += Telnet::yellow() + "Name            " + Telnet::reset() + ": " + liquid->getNameCapital() + "\n";
    msg += Telnet::yellow() + "Worth           " + Telnet::reset() + ": " + ToString(liquid->worth) + "\n";
    character->sendMsg(msg);
}

void DoProductionInfo(Character * character, std::istream & sArgs)
{
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    if (arguments.size() != 1)
    {
        character->sendMsg("You must provide a production vnum.\n");
        return;
    }
    Production * production = Mud::getInstance().findProduction(ToInt(arguments[0].first));
    if (production == nullptr)
    {
        character->sendMsg("Can't find the desire production.\n");
        return;
    }
    std::string msg;
    msg += "Vnum            : " + ToString(production->vnum) + "\n";
    msg += "Name            : " + production->name + "\n";
    msg += "Profession      : " + production->profession->command + "\n";
    msg += "Difficulty      : " + ToString(production->difficulty) + "\n";
    msg += "Time            : " + ToString(production->time) + "\n";
    msg += "Assisted        : " + ToString(production->assisted) + "\n";
    msg += "Outcome         : " + production->outcome.first->name + "[" + ToString(production->outcome.second) + "\n";
    msg += "Tools           :\n";
    for (auto iterator : production->tools)
    {
        msg += "                  " + GetToolTypeName(iterator) + "\n";
    }
    msg += "Ingredients     :\n";
    for (auto iterator : production->ingredients)
    {
        msg += "                  " + GetResourceTypeName(iterator.first) + "(" + ToString(iterator.second) + ")\n";
    }
    character->sendMsg(msg);
}

void DoProfessionInfo(Character * character, std::istream & sArgs)
{
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    if (arguments.size() != 1)
    {
        character->sendMsg("You must provide a profession name.\n");
        return;
    }
    Profession * profession = Mud::getInstance().findProfession(arguments[0].first);
    if (profession == nullptr)
    {
        character->sendMsg("Can't find the desire profession.\n");
        return;
    }
    std::string msg;
    msg += Telnet::yellow() + "Name          " + Telnet::reset() + ": " + profession->name + "\n";
    msg += Telnet::yellow() + "Description   " + Telnet::reset() + ": " + profession->description + "\n";
    msg += Telnet::yellow() + "Command       " + Telnet::reset() + ": " + profession->command + "\n";
    msg += Telnet::yellow() + "Posture       " + Telnet::reset() + ": " + GetPostureName(profession->posture) + "\n";
    msg += Telnet::yellow() + "Action        " + Telnet::reset() + ": " + profession->action + "\n";
    msg += Telnet::yellow() + "    Start     " + Telnet::reset() + ": " + profession->startMessage + "\n";
    msg += Telnet::yellow() + "    Finish    " + Telnet::reset() + ": " + profession->finishMessage + "\n";
    msg += Telnet::yellow() + "    Success   " + Telnet::reset() + ": " + profession->successMessage + "\n";
    msg += Telnet::yellow() + "    Failure   " + Telnet::reset() + ": " + profession->failureMessage + "\n";
    msg += Telnet::yellow() + "    Interrupt " + Telnet::reset() + ": " + profession->interruptMessage + "\n";
    msg += Telnet::yellow() + "    Not Found " + Telnet::reset() + ": " + profession->notFoundMessage + "\n";
    character->sendMsg(msg);
}

void DoModelList(Character * character, std::istream & sArgs)
{
    // Check no more input.
    NoMore(character, sArgs);
    Table table;
    table.addColumn("VNUM", kAlignRight);
    table.addColumn("NAME", kAlignLeft);
    table.addColumn("TYPE", kAlignLeft);
    table.addColumn("SPECIFIC", kAlignLeft);
    table.addColumn("SLOT", kAlignLeft);
    table.addColumn("FLAGS", kAlignRight);
    table.addColumn("WEIGHT", kAlignRight);
    table.addColumn("PRICE", kAlignRight);
    for (auto iterator : Mud::getInstance().mudModels)
    {
        Model model = iterator.second;
        // Prepare the row.
        TableRow row;
        row.push_back(ToString(model.vnum));
        row.push_back(model.name);
        row.push_back(GetModelTypeName(model.type));
        row.push_back(model.getSpecificTypeName());
        row.push_back(GetEquipmentSlotName(model.slot));
        row.push_back(ToString(model.flags));
        row.push_back(ToString(model.weight));
        row.push_back(ToString(model.price));
        // Add the row to the table.
        table.addRow(row);
    }
    character->sendMsg(table.getTable());
}

void DoItemList(Character * character, std::istream & sArgs)
{
    // Check no more input.
    NoMore(character, sArgs);
    Table table;
    table.addColumn("VNUM", kAlignRight);
    table.addColumn("NAME", kAlignLeft);
    table.addColumn("TYPE", kAlignLeft);
    table.addColumn("LOCATION", kAlignLeft);
    for (auto item : Mud::getInstance().mudItems)
    {
        // Prepare the row.
        TableRow row;
        row.push_back(ToString(item->vnum));
        row.push_back(item->getNameCapital());
        row.push_back(GetModelTypeName(item->model->type));
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
}

void DoMobileList(Character * character, std::istream & sArgs)
{
    // Check no more input.
    NoMore(character, sArgs);
    Table table;
    table.addColumn("ALIVE", kAlignCenter);
    table.addColumn("ID", kAlignLeft);
    table.addColumn("NAME", kAlignLeft);
    table.addColumn("LOCATION", kAlignRight);
    for (auto iterator : Mud::getInstance().mudMobiles)
    {
        Mobile * mobile = iterator.second;
        // Prepare the row.
        TableRow row;
        row.push_back((mobile->alive) ? "Yes" : "No");
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
}

void DoPlayerList(Character * character, std::istream & sArgs)
{
    // Check no more input.
    NoMore(character, sArgs);
    Table table;
    table.addColumn("NAME", kAlignLeft);
    table.addColumn("ROOM", kAlignLeft);
    for (auto iterator : Mud::getInstance().mudPlayers)
    {
        // If the player is not playing, continue.
        if (!iterator->isPlaying())
        {
            continue;
        }
        // Prepare the row.
        TableRow row;
        row.push_back(iterator->name);
        row.push_back(iterator->room->name);
        // Add the row to the table.
        table.addRow(row);
    }
    character->sendMsg(table.getTable());
}

void DoAreaList(Character * character, std::istream & sArgs)
{
    // Check no more input.
    NoMore(character, sArgs);
    Table table;
    table.addColumn("VNUM", kAlignCenter);
    table.addColumn("NAME", kAlignLeft);
    table.addColumn("BUILDER", kAlignLeft);
    table.addColumn("CONTINENT", kAlignCenter);
    table.addColumn("ROOMS", kAlignCenter);
    for (auto iterator : Mud::getInstance().mudAreas)
    {
        Area * area = iterator.second;
        // Prepare the row.
        TableRow row;
        row.push_back(ToString(area->vnum));
        row.push_back(area->name);
        row.push_back(area->builder);
        row.push_back(area->continent);
        row.push_back(ToString(area->numRooms));
        // Add the row to the table.
        table.addRow(row);
    }
    character->sendMsg(table.getTable());
}

void DoRoomList(Character * character, std::istream & sArgs)
{
    // Check no more input.
    NoMore(character, sArgs);
    Table table;
    table.addColumn("VNUM", kAlignCenter);
    table.addColumn("AREA", kAlignLeft);
    table.addColumn("COORD", kAlignCenter);
    table.addColumn("TERRAIN", kAlignCenter);
    table.addColumn("NAME", kAlignLeft);
    for (auto iterator : Mud::getInstance().mudRooms)
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
        row.push_back(ToString(room->coord.x) + ' ' + ToString(room->coord.y) + ' ' + ToString(room->coord.z));
        row.push_back(room->terrain);
        row.push_back(room->name);
        // Add the row to the table.
        table.addRow(row);
    }
    character->sendMsg(table.getTable());
}

void DoRaceList(Character * character, std::istream & sArgs)
{
    // Check no more input.
    NoMore(character, sArgs);
    Table table;
    table.addColumn("VNUM", kAlignCenter);
    table.addColumn("NAME", kAlignLeft);
    table.addColumn("ALLOWED", kAlignLeft);
    table.addColumn("STRENGTH", kAlignRight);
    table.addColumn("AGILITY", kAlignRight);
    table.addColumn("PERCEPTION", kAlignRight);
    table.addColumn("CONSTITUTION", kAlignRight);
    table.addColumn("INTELLIGENCE", kAlignRight);
    for (auto iterator : Mud::getInstance().mudRaces)
    {
        Race * race = &iterator.second;
        // Prepare the row.
        TableRow row;
        row.push_back(ToString(race->vnum));
        row.push_back(race->name);
        row.push_back(BoolToString(race->player_allow));
        row.push_back(ToString(race->strength));
        row.push_back(ToString(race->agility));
        row.push_back(ToString(race->perception));
        row.push_back(ToString(race->constitution));
        row.push_back(ToString(race->intelligence));
        // Add the row to the table.
        table.addRow(row);
    }
    character->sendMsg(table.getTable());
}

void DoFactionList(Character * character, std::istream & sArgs)
{
    // Check no more input.
    NoMore(character, sArgs);
    Table table;
    table.addColumn("VNUM", kAlignCenter);
    table.addColumn("NAME", kAlignLeft);
    for (auto iterator : Mud::getInstance().mudFactions)
    {
        Faction * faction = &iterator.second;
        // Prepare the row.
        TableRow row;
        row.push_back(ToString(faction->vnum));
        row.push_back(faction->name);
        // Add the row to the table.
        table.addRow(row);
    }
    character->sendMsg(table.getTable());
}

void DoSkillList(Character * character, std::istream & sArgs)
{
    // Check no more input.
    NoMore(character, sArgs);
    Table table;
    table.addColumn("VNUM", kAlignCenter);
    table.addColumn("NAME", kAlignLeft);
    table.addColumn("ATTRIBUTE", kAlignLeft);
    for (auto iterator : Mud::getInstance().mudSkills)
    {
        Skill * skill = &iterator.second;
        // Prepare the row.
        TableRow row;
        row.push_back(ToString(skill->vnum));
        row.push_back(skill->name);
        row.push_back(GetAttributeName(skill->attribute, true));
        // Add the row to the table.
        table.addRow(row);
    }
    character->sendMsg(table.getTable());
}

void DoWritingList(Character * character, std::istream & sArgs)
{
    // Check no more input.
    NoMore(character, sArgs);
    Table table;
    table.addColumn("VNUM", kAlignCenter);
    table.addColumn("AUTHOR", kAlignLeft);
    table.addColumn("TITLE", kAlignLeft);
    for (auto iterator : Mud::getInstance().mudWritings)
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
}

void DoCorpseList(Character * character, std::istream & sArgs)
{
    // Check no more input.
    NoMore(character, sArgs);
    Table table;
    table.addColumn("VNUM", kAlignRight);
    table.addColumn("NAME", kAlignLeft);
    table.addColumn("LOCATION", kAlignLeft);
    for (auto item : Mud::getInstance().mudCorpses)
    {
        // Prepare the row.
        TableRow row;
        row.push_back(ToString(item->vnum));
        row.push_back(item->getNameCapital());
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
}

void DoContinentList(Character * character, std::istream & sArgs)
{
    // Check no more input.
    NoMore(character, sArgs);
}

void DoMaterialList(Character * character, std::istream & sArgs)
{
    // Check no more input.
    NoMore(character, sArgs);
    Table table;
    table.addColumn("VNUM", kAlignRight);
    table.addColumn("TYPE", kAlignCenter);
    table.addColumn("NAME", kAlignLeft);
    table.addColumn("WORTH", kAlignRight);
    table.addColumn("HARDNESS", kAlignRight);
    table.addColumn("LIGHTNESS", kAlignRight);
    for (auto iterator : Mud::getInstance().mudMaterials)
    {
        Material material = iterator.second;
        // Prepare the row.
        TableRow row;
        row.push_back(ToString(material.vnum));
        row.push_back(GetMaterialTypeName(material.type));
        row.push_back(material.name);
        row.push_back(ToString(material.worth));
        row.push_back(ToString(material.hardness));
        row.push_back(ToString(material.lightness));
        // Add the row to the table.
        table.addRow(row);
    }
    character->sendMsg(table.getTable());
}

void DoProfessionList(Character * character, std::istream & sArgs)
{
    // Check no more input.
    NoMore(character, sArgs);
    Table table;
    table.addColumn("NAME", kAlignCenter);
    table.addColumn("COMMAND", kAlignCenter);
    table.addColumn("POSTURE", kAlignCenter);
    table.addColumn("ACTION", kAlignCenter);
    for (auto iterator : Mud::getInstance().mudProfessions)
    {
        Profession * profession = &(iterator.second);
        // Prepare the row.
        TableRow row;
        row.push_back(profession->name);
        row.push_back(profession->command);
        row.push_back(GetPostureName(profession->posture));
        row.push_back(profession->action);
        // Add the row to the table.
        table.addRow(row);
    }
    character->sendMsg(table.getTable());
}

void DoProductionList(Character * character, std::istream & sArgs)
{
    // Check no more input.
    NoMore(character, sArgs);
    Table table;
    table.addColumn("VNUM", kAlignCenter);
    table.addColumn("NAME", kAlignCenter);
    table.addColumn("PROFESSION", kAlignCenter);
    table.addColumn("DIFFICULTY", kAlignLeft);
    for (auto iterator : Mud::getInstance().mudProductions)
    {
        Production * production = &(iterator.second);
        // Prepare the row.
        TableRow row;
        row.push_back(ToString(production->vnum));
        row.push_back(production->name);
        row.push_back(production->profession->command);
        row.push_back(ToString(production->difficulty));
        // Add the row to the table.
        table.addRow(row);
    }
    character->sendMsg(table.getTable());
}

void DoLiquidList(Character * character, std::istream & sArgs)
{
    // Check no more input.
    NoMore(character, sArgs);
    Table table;
    table.addColumn("VNUM", kAlignCenter);
    table.addColumn("NAME", kAlignRight);
    table.addColumn("WORTH", kAlignRight);
    for (auto iterator : Mud::getInstance().mudLiquids)
    {
        Liquid liquid = iterator.second;
        // Prepare the row.
        TableRow row;
        row.push_back(ToString(liquid.vnum));
        row.push_back(liquid.getNameCapital());
        row.push_back(ToString(liquid.worth));
        // Add the row to the table.
        table.addRow(row);
    }
    character->sendMsg(table.getTable());
}

void DoBuildingList(Character * character, std::istream & sArgs)
{
    // Check no more input.
    NoMore(character, sArgs);
    Table table;
    table.addColumn("VNUM", kAlignCenter);
    table.addColumn("NAME", kAlignCenter);
    table.addColumn("DIFFICULTY", kAlignLeft);
    table.addColumn("TIME", kAlignCenter);
    table.addColumn("UNIQUE", kAlignCenter);
    for (auto iterator : Mud::getInstance().mudBuildings)
    {
        Building * building = &(iterator.second);
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
}
