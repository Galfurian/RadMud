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
    Mud::instance().broadcastMsg(0, character->getNameCapital() + " has shut down the game!");
    Mud::instance().shutDownSignal();
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
    Room * destination = Mud::instance().findRoom(ToInt(arguments[0].first));
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
        target = Mud::instance().findPlayer(arguments[0].first);
        if (target == nullptr)
        {
            target = Mud::instance().findMobile(arguments[0].first);
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
    Player * target = Mud::instance().findPlayer(arguments[0].first);
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
    Player * target = Mud::instance().findPlayer(arguments[0].first);
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
    Model * model = Mud::instance().findModel(ToInt(arguments[0].first));
    if (model == nullptr)
    {
        character->sendMsg("Model not found.\n");
        return;
    }

    std::string msg;
    msg += Formatter::yellow() + "Model Vnum      " + Formatter::reset() + ": " + ToString(model->vnum) + "\n";
    msg += Formatter::yellow() + "Name            " + Formatter::reset() + ": " + model->name + "\n";
    msg += Formatter::yellow() + "Article         " + Formatter::reset() + ": " + model->article + "\n";
    msg += Formatter::yellow() + "Short Descr.    " + Formatter::reset() + ": " + model->shortdesc + "\n";
    msg += Formatter::yellow() + "Keys            " + Formatter::reset() + ": {";
    for (auto it : model->keys)
    {
        msg += " " + it;
    }
    msg += "}\n";
    msg += Formatter::yellow() + "Description     " + Formatter::reset() + ": " + model->description + "\n";
    msg += Formatter::yellow() + "Type            " + Formatter::reset() + ": " + GetModelTypeName(model->type) + "\n";
    msg += Formatter::yellow() + "Slot            " + Formatter::reset() + ": " + GetEquipmentSlotName(model->slot) + "\n";
    msg += Formatter::yellow() + "Flags           " + Formatter::reset() + ": " + GetModelFlagString(model->flags) + "\n";
    msg += Formatter::yellow() + "Weight          " + Formatter::reset() + ": " + ToString(model->weight) + " " + mud_measure + ".\n";
    msg += Formatter::yellow() + "Price           " + Formatter::reset() + ": " + ToString(model->price) + " " + mud_currency + ".\n";
    msg += Formatter::yellow() + "Condition       " + Formatter::reset() + ": " + ToString(model->condition) + "\n";
    msg += Formatter::yellow() + "Decay           " + Formatter::reset() + ": " + ToString(model->decay) + "\n";
    msg += Formatter::yellow() + "Material        " + Formatter::reset() + ": " + GetMaterialTypeName(model->material) + "\n";
    msg += Formatter::yellow() + "Tile Set        " + Formatter::reset() + ": " + ToString(model->tileSet) + "\n";
    msg += Formatter::yellow() + "Tile Id         " + Formatter::reset() + ": " + ToString(model->tileId) + "\n";
    msg += Formatter::yellow() + "Functions       " + Formatter::reset() + ": {";
    for (auto it : model->functions)
    {
        msg += " " + ToString(it);
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
            msg += Formatter::brown() + "Type            " + Formatter::reset() + ": " + GetWeaponTypeName(func.type) + "\n";
            msg += Formatter::brown() + "Minimum Damage  " + Formatter::reset() + ": " + ToString(func.minDamage) + "\n";
            msg += Formatter::brown() + "Maximum Damage  " + Formatter::reset() + ": " + ToString(func.maxDamage) + "\n";
            msg += Formatter::brown() + "Range           " + Formatter::reset() + ": " + ToString(func.range) + "\n";
            break;
        }
        case ModelType::Armor:
        {
            ArmorFunc func = model->getArmorFunc();
            msg += Formatter::brown() + "Size            " + Formatter::reset() + ": " + GetArmorSizeName(func.size) + "\n";
            msg += Formatter::brown() + "Damage Absorb. " + Formatter::reset() + ": " + ToString(func.damageAbs) + "\n";
            msg += Formatter::brown() + "Allowed Anatom. " + Formatter::reset() + ": " + ToString(func.allowedAnatomy) + "\n";
            break;
        }
        case ModelType::Shield:
        {
            ShieldFunc func = model->getShieldFunc();
            msg += Formatter::brown() + "Size            " + Formatter::reset() + ": " + GetShieldSizeName(func.size) + "\n";
            msg += Formatter::brown() + "Parry Chance    " + Formatter::reset() + ": " + ToString(func.parryChance) + "\n";
            break;
        }
        case ModelType::Projectile:
        {
            ProjectileFunc func = model->getProjectileFunc();
            msg += Formatter::brown() + "Damage Bonus    " + Formatter::reset() + ": " + ToString(func.damageBonus) + "\n";
            msg += Formatter::brown() + "Range Bonus     " + Formatter::reset() + ": " + ToString(func.rangeBonus) + "\n";
            break;
        }
        case ModelType::Container:
        {
            ContainerFunc func = model->getContainerFunc();
            msg += Formatter::brown() + "Max Weight      " + Formatter::reset() + ": " + ToString(func.maxWeight) + "\n";
            msg += Formatter::brown() + "Flags           " + Formatter::reset() + ": {" + GetContainerFlagString(func.flags) + "}\n";
            msg += Formatter::brown() + "Key Vnum        " + Formatter::reset() + ": " + ToString(func.keyVnum) + "\n";
            msg += Formatter::brown() + "Lockpicking Lv. " + Formatter::reset() + ": " + ToString(func.difficulty) + "\n";
            break;
        }
        case ModelType::LiqContainer:
        {
            LiqContainerFunc func = model->getLiqContainerFunc();
            msg += Formatter::brown() + "Max Weight      " + Formatter::reset() + ": " + ToString(func.maxWeight) + "\n";
            msg += Formatter::brown() + "Flags           " + Formatter::reset() + ": {" + GetLiqContainerFlagString(func.flags) + "}\n";
            break;
        }
        case ModelType::Tool:
        {
            ToolFunc func = model->getToolFunc();
            msg += Formatter::brown() + "Type            " + Formatter::reset() + ": " + GetToolTypeName(func.type) + "\n";
            break;
        }
        case ModelType::Node:
        {
            NodeFunc func = model->getNodeFunc();
            msg += Formatter::brown() + "Type            " + Formatter::reset() + ": " + GetNodeTypeName(func.type) + "\n";
            break;
        }
        case ModelType::Resource:
        {
            ResourceFunc func = model->getResourceFunc();
            msg += Formatter::brown() + "Type            " + Formatter::reset() + ": " + GetResourceTypeName(func.type) + "\n";
            break;
        }
        case ModelType::Seed:
        {
            SeedFunc func = model->getSeedFunc();
            msg += Formatter::brown() + "Type            " + Formatter::reset() + ": " + GetSeedTypeName(func.type) + "\n";
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
            msg += Formatter::brown() + "Feeding         " + Formatter::reset() + ": " + ToString(func.hours) + "\n";
            msg += Formatter::brown() + "Flags           " + Formatter::reset() + ": {" + GetFoodFlagString(func.flags) + "}\n";
            break;
        }
        case ModelType::Light:
        {
            LightFunc func = model->getLightFunc();
            msg += Formatter::brown() + "Autonomy        " + Formatter::reset() + ": " + ToString(func.maxHours) + "\n";
            msg += Formatter::brown() + "Rechargeable    " + Formatter::reset() + ": " + ToString(func.policy) + "\n";
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
            msg += Formatter::brown() + "Capacity        " + Formatter::reset() + ": " + ToString(func.maxParchments) + "\n";
            break;
        }
        case ModelType::Rope:
        {
            RopeFunc func = model->getRopeFunc();
            msg += Formatter::brown() + "Difficulty      " + Formatter::reset() + ": " + ToString(func.difficulty) + "\n";
            msg += Formatter::brown() + "Type            " + Formatter::reset() + ": " + ToString(func.type) + "\n";
            break;
        }
        case ModelType::Trash:
        {
            break;
        }
        case ModelType::Mechanism:
        {
            MechanismFunc func = model->getMechanismFunc();
            msg += Formatter::brown() + "Type            " + Formatter::reset() + ": " + GetMechanismTypeName(func.type) + "\n";
            switch (func.type)
            {
                case MechanismType::Door:
                case MechanismType::Lock:
                    msg += Formatter::brown() + "Key             " + Formatter::reset() + ": " + ToString(func.key) + "\n";
                    msg += Formatter::brown() + "Difficulty      " + Formatter::reset() + ": " + ToString(func.difficulty) + "\n";
                    break;
                case MechanismType::Picklock:
                    msg += Formatter::brown() + "Efficency       " + Formatter::reset() + ": " + ToString(func.efficency) + "\n";
                    break;
                case MechanismType::Lever:
                    msg += Formatter::brown() + "Command         " + Formatter::reset() + ": " + ToString(func.command) + "\n";
                    msg += Formatter::brown() + "Target          " + Formatter::reset() + ": " + ToString(func.target) + "\n";
                    break;
                case MechanismType::None:
                default:
                    msg += Formatter::brown() + "None" + Formatter::reset() + "\n";
            }
            break;
        }
        case ModelType::Currency:
        {
            break;
        }
        default:
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
    Model * model = Mud::instance().findModel(ToInt(arguments[0].first));
    Material * material = Mud::instance().findMaterial(ToInt(arguments[1].first));
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
        "You produce " + Formatter::yellow() + item->getName() + Formatter::reset() + " out of your apparently empty top hat.\n");
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
    Item * item = Mud::instance().findItem(ToInt(arguments[0].first));
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
        SQLiteDbms::instance().deleteFrom("ItemRoom", where);
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
        SQLiteDbms::instance().deleteFrom("ItemPlayer", where);
    }
    else if (item->container != nullptr)
    {
        character->sendMsg("The item was inside the container:" + item->container->getName() + "\n");
        FindErase(item->container->content, item);
        item->container = nullptr;

        // Remove the item from the table Content.
        QueryList where;
        where.push_back(std::make_pair("item", ToString(item->vnum)));
        SQLiteDbms::instance().deleteFrom("ItemContent", where);
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
    Item * item = Mud::instance().findItem(ToInt(arguments[0].first));
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
    Item * item = Mud::instance().findItem(ToInt(arguments[0].first));
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
    Area * area = Mud::instance().findArea(ToInt(arguments[0].first));
    if (area == nullptr)
    {
        character->sendMsg("The selected area does not exist.");
        return;
    }
    std::string msg;
    msg += Formatter::green() + "# Area Informations:\n" + Formatter::reset();
    msg += Formatter::yellow() + " Vnum            " + Formatter::reset() + ":" + ToString(area->vnum) + "\n";
    msg += Formatter::yellow() + " Name            " + Formatter::reset() + ":" + area->name + "\n";
    msg += Formatter::yellow() + " Builder         " + Formatter::reset() + ":" + area->builder + "\n";
    msg += Formatter::yellow() + " Continent       " + Formatter::reset() + ":" + area->continent + "\n";
    msg += Formatter::yellow() + " Width           " + Formatter::reset() + ":" + ToString(area->width) + "\n";
    msg += Formatter::yellow() + " Height          " + Formatter::reset() + ":" + ToString(area->height) + "\n";
    msg += Formatter::yellow() + " Elevation       " + Formatter::reset() + ":" + ToString(area->elevation) + "\n";
    msg += Formatter::yellow() + " Number of Rooms " + Formatter::reset() + ":" + ToString(area->numRooms) + "\n";
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
        room = Mud::instance().findRoom(ToInt(arguments[0].first));
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
    msg += Formatter::green() + "# Room Informations:\n" + Formatter::reset();
    msg += Formatter::yellow() + " Vnum        " + Formatter::reset() + ":" + ToString(room->vnum) + "\n";
    msg += Formatter::yellow() + " X           " + Formatter::reset() + ":" + ToString(room->coord.x) + "\n";
    msg += Formatter::yellow() + " Y           " + Formatter::reset() + ":" + ToString(room->coord.y) + "\n";
    msg += Formatter::yellow() + " Z           " + Formatter::reset() + ":" + ToString(room->coord.z) + "\n";
    msg += Formatter::yellow() + " Name        " + Formatter::reset() + ":" + room->name + "\n";
    msg += Formatter::yellow() + " Description " + Formatter::reset() + ":" + room->description + "\n";
    msg += Formatter::yellow() + " Terrain     " + Formatter::reset() + ":" + room->terrain + "\n";
    msg += Formatter::yellow() + " Flags       " + Formatter::reset() + ":" + GetRoomFlagString(room->flags) + "\n";
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
    Direction direction = Mud::instance().findDirection(arguments[0].first, false);
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
    Direction direction = Mud::instance().findDirection(arguments[0].first, false);
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
    if (!Mud::instance().remRoom(targetRoom))
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

        if (!SQLiteDbms::instance().updateInto("Room", value, where))
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

        if (!SQLiteDbms::instance().updateInto("Room", value, where))
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
        if (!SQLiteDbms::instance().updateInto("Room", value, where))
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
    Mobile * mobile = Mud::instance().findMobile(arguments[0].first);
    if (mobile == nullptr)
    {
        std::string msgFound;
        for (auto it : Mud::instance().mudMobiles)
        {
            if (BeginWith(it.first, arguments[0].first))
            {
                if (msgFound.empty())
                {
                    msgFound += "May you meant:\n";
                }
                msgFound += "    " + it.first + "\n";
            }
        }
        character->sendMsg("Mobile not found.\n" + msgFound);
        return;
    }
    std::string msg;
    msg += Formatter::green() + "# Mobile:\n" + Formatter::reset();
    msg += Formatter::yellow() + "    Mobile Id    " + Formatter::reset() + ":" + mobile->id + "\n";
    msg += Formatter::yellow() + "    Spawn Room   " + Formatter::reset() + ":" + ToString(mobile->respawnRoom->vnum) + "\n";
    msg += Formatter::yellow() + "    Keys         " + Formatter::reset() + ": {";
    for (auto it : mobile->keys)
    {
        msg += " " + it;
    }
    msg += "}\n";
    msg += Formatter::yellow() + "    Short Desc   " + Formatter::reset() + ":" + mobile->shortdesc + "\n";
    msg += Formatter::yellow() + "    Static Desc  " + Formatter::reset() + ":" + mobile->staticdesc + "\n";
    msg += Formatter::yellow() + "    Actions      " + Formatter::reset() + ": {";
    for (auto it : mobile->actions)
    {
        msg += " " + it;
    }
    msg += "}\n";
    msg += Formatter::yellow() + "    Alive        " + Formatter::reset() + ":" + ToString(mobile->alive) + "\n";
    if (!mobile->alive)
    {
        // Get the current time.
        TimeClock currentTime = std::chrono::system_clock::now();
        // Evaluate the elapsed time.
        long int respawn = std::chrono::duration_cast<std::chrono::seconds>(currentTime - mobile->nextRespawn).count();
        msg += Formatter::yellow() + "    Respawn In   " + Formatter::reset() + ":" + ToString(respawn) + "\n";
    }
    if (mobile->controller != nullptr)
    {
        msg += Formatter::red() + "    Controller   " + Formatter::reset() + ":" + mobile->controller->getName() + "\n";
    }
    msg += Formatter::red() + "    Lua Script   " + Formatter::reset() + ":" + mobile->lua_script + "\n";

    msg += Formatter::green() + "# Chracter:\n" + Formatter::reset();
    msg += Formatter::yellow() + "    Proper Noun  " + Formatter::reset() + ":" + mobile->name + "\n";
    msg += Formatter::yellow() + "    Description  " + Formatter::reset() + ":" + mobile->description + "\n";
    msg += Formatter::yellow() + "    Gender       " + Formatter::reset() + ":" + GetGenderTypeName(mobile->gender) + "\n";
    msg += Formatter::yellow() + "    Weight       " + Formatter::reset() + ":" + ToString(mobile->weight) + "\n";
    msg += Formatter::yellow() + "    Level        " + Formatter::reset() + ":" + ToString(mobile->level) + "\n";
    msg += Formatter::yellow() + "    Flags        " + Formatter::reset() + ":" + ToString(mobile->flags) + "\n";
    msg += Formatter::yellow() + "    Race         " + Formatter::reset() + ":" + mobile->race->name + "\n";
    msg += Formatter::yellow() + "    Faction      " + Formatter::reset() + ":" + mobile->faction->name + "\n";
    msg += Formatter::yellow() + "    Health       " + Formatter::reset() + ":" + ToString(mobile->health) + "\n";
    msg += Formatter::yellow() + "    Stamina      " + Formatter::reset() + ":" + ToString(mobile->stamina) + "\n";
    msg += Formatter::yellow() + "    Hunger       " + Formatter::reset() + ":" + ToString(mobile->hunger) + "\n";
    msg += Formatter::yellow() + "    Thirst       " + Formatter::reset() + ":" + ToString(mobile->thirst) + "\n";
    msg += Formatter::yellow() + "    Strength     " + Formatter::reset() + ":" + ToString(mobile->strength) + "\n";
    msg += Formatter::yellow() + "    Agility      " + Formatter::reset() + ":" + ToString(mobile->agility) + "\n";
    msg += Formatter::yellow() + "    Perception   " + Formatter::reset() + ":" + ToString(mobile->perception) + "\n";
    msg += Formatter::yellow() + "    Constitution " + Formatter::reset() + ":" + ToString(mobile->constitution) + "\n";
    msg += Formatter::yellow() + "    Intelligence " + Formatter::reset() + ":" + ToString(mobile->intelligence) + "\n";
    msg += Formatter::yellow() + "    Current Room " + Formatter::reset() + ":" + ToString(mobile->room->vnum) + "\n";
    msg += Formatter::yellow() + "    Inventory    " + Formatter::reset() + ":\n";
    for (Item * item : mobile->inventory)
    {
        msg += "        " + item->getName() + "\n";
    }
    msg += Formatter::yellow() + "    Equipment    " + Formatter::reset() + ":\n";
    for (Item * item : mobile->equipment)
    {
        msg += "        " + item->getName() + "\n";
    }
    msg += Formatter::yellow() + "    Posture      " + Formatter::reset() + ":" + GetPostureName(mobile->posture) + "\n";
    msg += Formatter::yellow() + "    Effects      " + Formatter::reset() + ":\n";
    for (auto effect : mobile->effects.activeEffects)
    {
        msg += "                [" + ToString(effect.expires) + "] " + effect.name + "\n";
    }
    msg += Formatter::yellow() + "    Action       " + Formatter::reset() + ":" + GetActionTypeName(mobile->getAction()->getType()) + "\n";

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
    Mobile * mobile = Mud::instance().findMobile(arguments[0].first);
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
    Skill * skill = Mud::instance().findSkill(ToInt(arguments[1].first));
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
    Player * player = Mud::instance().findPlayer(arguments[0].first);
    if (player == nullptr)
    {
        character->sendMsg("Player not found.\n");
        return;
    }
    std::string msg;
    msg += Formatter::green() + "# Player:\n" + Formatter::reset();
    msg += Formatter::yellow() + "    Age          " + Formatter::reset() + ":" + ToString(player->age) + "\n";
    msg += Formatter::yellow() + "    Experience   " + Formatter::reset() + ":" + ToString(player->experience) + "\n";
    msg += Formatter::yellow() + "    Prompt       " + Formatter::reset() + ":" + player->prompt + "\n";
    msg += Formatter::yellow() + "    Rent Room    " + Formatter::reset() + ":" + ToString(player->rent_room) + "\n";
    msg += Formatter::yellow() + "    Skills       " + Formatter::reset() + ":\n";
    for (auto iterator : Mud::instance().mudSkills)
    {
        msg += "        " + iterator.second.name + "[" + ToString(player->skills[iterator.first]) + "]\n";
    }

    msg += Formatter::green() + "# Chracter:\n" + Formatter::reset();
    msg += Formatter::yellow() + "    Proper Noun  " + Formatter::reset() + ":" + player->name + "\n";
    msg += Formatter::yellow() + "    Description  " + Formatter::reset() + ":" + player->description + "\n";
    msg += Formatter::yellow() + "    Gender       " + Formatter::reset() + ":" + GetGenderTypeName(player->gender) + "\n";
    msg += Formatter::yellow() + "    Weight       " + Formatter::reset() + ":" + ToString(player->weight) + "\n";
    msg += Formatter::yellow() + "    Level        " + Formatter::reset() + ":" + ToString(player->level) + "\n";
    msg += Formatter::yellow() + "    Flags        " + Formatter::reset() + ":" + ToString(player->flags) + "\n";
    msg += Formatter::yellow() + "    Race         " + Formatter::reset() + ":" + player->race->name + "\n";
    msg += Formatter::yellow() + "    Faction      " + Formatter::reset() + ":" + player->faction->name + "\n";
    msg += Formatter::yellow() + "    Health       " + Formatter::reset() + ":" + ToString(player->health) + "\n";
    msg += Formatter::yellow() + "    Stamina      " + Formatter::reset() + ":" + ToString(player->stamina) + "\n";
    msg += Formatter::yellow() + "    Hunger       " + Formatter::reset() + ":" + ToString(player->hunger) + "\n";
    msg += Formatter::yellow() + "    Thirst       " + Formatter::reset() + ":" + ToString(player->thirst) + "\n";
    msg += Formatter::yellow() + "    Strength     " + Formatter::reset() + ":" + ToString(player->strength) + "\n";
    msg += Formatter::yellow() + "    Agility      " + Formatter::reset() + ":" + ToString(player->agility) + "\n";
    msg += Formatter::yellow() + "    Perception   " + Formatter::reset() + ":" + ToString(player->perception) + "\n";
    msg += Formatter::yellow() + "    Constitution " + Formatter::reset() + ":" + ToString(player->constitution) + "\n";
    msg += Formatter::yellow() + "    Intelligence " + Formatter::reset() + ":" + ToString(player->intelligence) + "\n";
    msg += Formatter::yellow() + "    Current Room " + Formatter::reset() + ":" + ToString(player->room->vnum) + "\n";
    msg += Formatter::yellow() + "    Inventory    " + Formatter::reset() + ":\n";
    for (Item * item : player->inventory)
    {
        msg += "        " + item->getName() + "\n";
    }
    msg += Formatter::yellow() + "    Equipment    " + Formatter::reset() + ":\n";
    for (Item * item : player->equipment)
    {
        msg += "        " + item->getName() + "\n";
    }
    msg += Formatter::yellow() + "    Posture      " + Formatter::reset() + ":" + GetPostureName(player->posture) + "\n";
    msg += Formatter::yellow() + "    Effects      " + Formatter::reset() + ":\n";
    for (auto effect : player->effects.activeEffects)
    {
        msg += "                [" + ToString(effect.expires) + "] " + effect.name + "\n";
    }
    msg += Formatter::yellow() + "    Action       " + Formatter::reset() + ":" + GetActionTypeName(player->getAction()->getType()) + "\n";
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
    Material * material = Mud::instance().findMaterial(ToInt(arguments[0].first));
    if (material == nullptr)
    {
        character->sendMsg("Can't find the desire material.\n");
        return;
    }
    std::string msg;
    msg += Formatter::yellow() + "Vnum            " + Formatter::reset() + ": " + ToString(material->vnum) + "\n";
    msg += Formatter::yellow() + "Type            " + Formatter::reset() + ": " + GetMaterialTypeName(material->type) + "\n";
    msg += Formatter::yellow() + "Name            " + Formatter::reset() + ": " + material->name + "\n";
    msg += Formatter::yellow() + "Worth           " + Formatter::reset() + ": " + ToString(material->worth) + "\n";
    msg += Formatter::yellow() + "Hardness        " + Formatter::reset() + ": " + ToString(material->hardness) + "\n";
    msg += Formatter::yellow() + "Lightness       " + Formatter::reset() + ": " + ToString(material->lightness) + "\n";
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
    Liquid * liquid = Mud::instance().findLiquid(ToInt(arguments[1].first));
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
    character->sendMsg("You materialise " + ToString(quantity) + " units of " + liquid->getName() + " inside " + item->getName() + ".\n");
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
    Liquid * liquid = Mud::instance().findLiquid(ToInt(arguments[0].first));
    if (liquid == nullptr)
    {
        character->sendMsg("Can't find the desire liquid.\n");
        return;
    }
    std::string msg;
    msg += Formatter::yellow() + "Vnum            " + Formatter::reset() + ": " + ToString(liquid->vnum) + "\n";
    msg += Formatter::yellow() + "Name            " + Formatter::reset() + ": " + liquid->getNameCapital() + "\n";
    msg += Formatter::yellow() + "Worth           " + Formatter::reset() + ": " + ToString(liquid->worth) + "\n";
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
    Production * production = Mud::instance().findProduction(ToInt(arguments[0].first));
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
    Profession * profession = Mud::instance().findProfession(arguments[0].first);
    if (profession == nullptr)
    {
        character->sendMsg("Can't find the desire profession.\n");
        return;
    }
    std::string msg;
    msg += Formatter::yellow() + "Name          " + Formatter::reset() + ": " + profession->name + "\n";
    msg += Formatter::yellow() + "Description   " + Formatter::reset() + ": " + profession->description + "\n";
    msg += Formatter::yellow() + "Command       " + Formatter::reset() + ": " + profession->command + "\n";
    msg += Formatter::yellow() + "Posture       " + Formatter::reset() + ": " + GetPostureName(profession->posture) + "\n";
    msg += Formatter::yellow() + "Action        " + Formatter::reset() + ": " + profession->action + "\n";
    msg += Formatter::yellow() + "    Start     " + Formatter::reset() + ": " + profession->startMessage + "\n";
    msg += Formatter::yellow() + "    Finish    " + Formatter::reset() + ": " + profession->finishMessage + "\n";
    msg += Formatter::yellow() + "    Success   " + Formatter::reset() + ": " + profession->successMessage + "\n";
    msg += Formatter::yellow() + "    Failure   " + Formatter::reset() + ": " + profession->failureMessage + "\n";
    msg += Formatter::yellow() + "    Interrupt " + Formatter::reset() + ": " + profession->interruptMessage + "\n";
    msg += Formatter::yellow() + "    Not Found " + Formatter::reset() + ": " + profession->notFoundMessage + "\n";
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
    for (auto iterator : Mud::instance().mudModels)
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
    for (auto item : Mud::instance().mudItems)
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
    for (auto iterator : Mud::instance().mudMobiles)
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
    for (auto iterator : Mud::instance().mudAreas)
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
    for (auto iterator : Mud::instance().mudRaces)
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
    for (auto iterator : Mud::instance().mudFactions)
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
    for (auto iterator : Mud::instance().mudSkills)
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
}

void DoCorpseList(Character * character, std::istream & sArgs)
{
    // Check no more input.
    NoMore(character, sArgs);
    Table table;
    table.addColumn("VNUM", kAlignRight);
    table.addColumn("NAME", kAlignLeft);
    table.addColumn("LOCATION", kAlignLeft);
    for (auto item : Mud::instance().mudCorpses)
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
    for (auto iterator : Mud::instance().mudMaterials)
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
    for (auto iterator : Mud::instance().mudProfessions)
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
    for (auto iterator : Mud::instance().mudProductions)
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
    for (auto iterator : Mud::instance().mudLiquids)
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
    for (auto iterator : Mud::instance().mudBuildings)
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
