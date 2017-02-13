/// @file   commandObjectCharacter.cpp
/// @author Enrico Fraccaroli
/// @date   29/12/2016
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

#include "commandObjectCharacter.hpp"

#include "characterUtilities.hpp"
#include "skillRank.hpp"
#include "formatter.hpp"
#include "command.hpp"
#include "room.hpp"
#include "mud.hpp"

bool DoEquipments(Character * character, ArgumentHandler & /*args*/)
{
    // Check if the character is sleeping.
    if (character->posture == CharacterPosture::Sleep)
    {
        character->sendMsg("Not while you're sleeping.\n");
        return false;
    }
    // Check if the room is lit.
    bool roomIsLit = false;
    if (character->room != nullptr)
    {
        roomIsLit = character->room->isLit();
    }
    // Go through all the body parts of the race.
    std::string output;
    for (auto bodyPart : character->race->bodyParts)
    {
        // Skip the internal body parts.
        if (HasFlag(bodyPart->flags, BodyPartFlag::Internal))
        {
            continue;
        }
        // Add the body part name to the row.
        output += AlignString(bodyPart->getDescription(true),
                              StringAlign::Left, 15);
        auto item = character->findItemAtBodyPart(bodyPart);
        if (item != nullptr)
        {
            if (roomIsLit)
            {
                output += " - " + item->getNameCapital(true);
            }
            else
            {
                output += " - Something";
            }
        }
        output += "\n";
    }
    character->sendMsg(output);
    return true;
}

bool DoWield(Character * character, ArgumentHandler & args)
{
    // Check if the character is sleeping.
    if (character->posture == CharacterPosture::Sleep)
    {
        character->sendMsg("Not while you're sleeping.\n");
        return false;
    }
    // Stop any action the character is executing.
    StopAction(character);
    // Check the arguments.
    if (args.empty())
    {
        character->sendMsg("Wield what?\n");
        return false;
    }
    if (args.size() > 1)
    {
        character->sendMsg("Too many arguments.\n");
        return false;
    }
    // Get the item.
    auto item = character->findInventoryItem(args[0].getContent(),
                                             args[0].getIndex());
    // If the room is not lit, check if the inventory contains a light.
    if (!character->room->isLit())
    {
        // If the inventory is NOT lit and NOT empty, pick a random item.
        if (!character->inventoryIsLit() && !character->inventory.empty())
        {
            auto it = TRand<size_t>(0, character->inventory.size() - 1);
            item = character->inventory[it];
        }
    }
    // Check if the item is null.
    if (item == nullptr)
    {
        // Try to check if the character is a mobile, since mobiles can take
        //  items by providing the specific vnum.
        if (character->isMobile() && IsNumber(args[0].getContent()))
        {
            for (auto it : character->inventory)
            {
                if (it->vnum == ToNumber<int>(args[0].getContent()))
                {
                    item = it;
                }
            }
        }
    }
    // Check if the item is null for the last time.
    if (item == nullptr)
    {
        character->sendMsg("You don't have that item.\n");
        return false;
    }
    // String where the error message will be put.
    std::string errMessage;
    // The occupied body parts.
    auto bodyParts = character->canWield(item, errMessage);
    // Check if the character can wield the item.
    if (bodyParts.empty())
    {
        character->sendMsg(errMessage + "\n");
        return false;
    }
    // Set the item body part.
    item->setOccupiedBodyParts(bodyParts);
    // Remove the item from the inventory.
    character->remInventoryItem(item);
    // Equip the item.
    character->addEquipmentItem(item);
    // Show the proper message.
    std::string where, whereOthers;
    for (auto it = bodyParts.begin(); it != bodyParts.end();)
    {
        where += "your " + (*it)->getDescription();
        whereOthers += character->getPossessivePronoun() + " ";
        whereOthers += (*it)->getDescription();
        it++;
        if (it != bodyParts.end())
        {
            where += ", ";
            whereOthers += ", ";
        }
    }
    character->sendMsg("You wield %s with %s.\n",
                       item->getName(true),
                       where);
    // Send the message inside the room.
    character->room->sendToAll("%s wields %s with %s.\n",
                               {character},
                               character->getNameCapital(),
                               item->getName(true),
                               whereOthers);
    return true;
}

bool DoWear(Character * character, ArgumentHandler & args)
{
    // Check if the character is sleeping.
    if (character->posture == CharacterPosture::Sleep)
    {
        character->sendMsg("Not while you're sleeping.\n");
        return false;
    }
    // Stop any action the character is executing.
    StopAction(character);
    // Check the arguments.
    if (args.size() == 0)
    {
        character->sendMsg("Wear what?\n");
        return false;
    }
    if (args.size() > 1)
    {
        character->sendMsg("Too many arguments.\n");
        return false;
    }
    if (args[0].getContent() == "all")
    {
        auto wornSomething = false;
        auto untouchedList = character->inventory;
        for (auto iterator : untouchedList)
        {
            // String where the error message will be put.
            std::string errMessage;
            // The destination bodyParts.
            auto bodyParts = character->canWear(iterator, errMessage);
            if (bodyParts.empty())
            {
                continue;
            }
            // Set the body part occupied by the item.
            iterator->setOccupiedBodyParts(bodyParts);
            // Remove the item from the player's inventory.
            character->remInventoryItem(iterator);
            // Add the item to the equipment.
            character->addEquipmentItem(iterator);
            // Notify that something was worn.
            wornSomething = true;
        }
        // Handle output only if the player has really worn something.
        if (!wornSomething)
        {
            character->sendMsg("You had nothing to wear.\n");
            return false;
        }
        character->sendMsg("You have worn everything you could.\n");
        // Send the message inside the room.
        character->room->sendToAll("%s has worn all %s could.\n",
                                   {character},
                                   character->getNameCapital(),
                                   character->getSubjectPronoun());
        return true;
    }
    auto item = character->findInventoryItem(args[0].getContent(),
                                             args[0].getIndex());
    // If the room is not lit, check if the inventory contains a light.
    if (!character->room->isLit())
    {
        // If the inventory is NOT lit and NOT empty, pick a random item.
        if (!character->inventoryIsLit() && !character->inventory.empty())
        {
            auto it = TRand<size_t>(0, character->inventory.size() - 1);
            item = character->inventory[it];
        }
    }
    // Check if the item is null.
    if (item == nullptr)
    {
        // Try to check if the character is a mobile, since mobiles can take
        //  items by providing the specific vnum.
        if (character->isMobile() && IsNumber(args[0].getContent()))
        {
            for (auto it : character->inventory)
            {
                if (it->vnum == ToNumber<int>(args[0].getContent()))
                {
                    item = it;
                    break;
                }
            }
        }
    }
    if (item == nullptr)
    {
        character->sendMsg("You don't have that item.\n");
        return false;
    }
    // String where the error message will be put.
    std::string errMessage;
    // Get the body parts required by the item.
    auto bodyParts = character->canWear(item, errMessage);
    if (bodyParts.empty())
    {
        character->sendMsg(errMessage + "\n");
        return false;
    }
    // Set the body parts occupied by the item.
    item->setOccupiedBodyParts(bodyParts);
    // Remove the item from the player's inventory.
    character->remInventoryItem(item);
    // Add the item to the equipment.
    character->addEquipmentItem(item);
    // Show the proper message.
    std::string where, whereOthers;
    for (auto it = bodyParts.begin(); it != bodyParts.end();)
    {
        where += "your " + (*it)->getDescription();
        whereOthers += character->getPossessivePronoun() + " ";
        whereOthers += (*it)->getDescription();
        it++;
        if (it != bodyParts.end())
        {
            where += ", ";
            whereOthers += ", ";
        }
    }
    // Notify to character.
    character->sendMsg("You wear %s on your %s.\n",
                       item->getName(true),
                       where);
    // Send the message inside the room.
    character->room->sendToAll("%s wears %s on %s.\n",
                               {character},
                               character->getNameCapital(),
                               item->getName(true),
                               whereOthers);
    return true;
}

bool DoRemove(Character * character, ArgumentHandler & args)
{
    // Check if the character is sleeping.
    if (character->posture == CharacterPosture::Sleep)
    {
        character->sendMsg("Not while you're sleeping.\n");
        return false;
    }
    // Stop any action the character is executing.
    StopAction(character);
    // Check the arguments.
    if (args.size() == 0)
    {
        character->sendMsg("Remove what?\n");
        return false;
    }
    if (args.size() > 1)
    {
        character->sendMsg("Too many arguments.\n");
        return false;
    }
    if (args[0].getContent() == "all")
    {
        // Handle output only if the player has really removed something.
        if (character->equipment.empty())
        {
            character->sendMsg("You have nothing to remove.\n");
            return false;
        }
        auto untouchedList = character->equipment;
        for (auto iterator : untouchedList)
        {
            // Remove the item from the player's equipment.
            character->remEquipmentItem(iterator);
            // Add the item to the inventory.
            character->addInventoryItem(iterator);
        }
        character->sendMsg("You have removed everything.\n");
        // Send the message inside the room.
        character->room->sendToAll("%s has undressed all he could.\n",
                                   {character},
                                   character->getNameCapital());
        // Check if we have just removed ALL the USED Ranged Weapons.
        if (character->combatHandler.getAimedTarget() != nullptr)
        {
            if (GetActiveRangedWeapons(character).empty())
            {
                character->sendMsg("You stop aiming %s.\n",
                                   character->combatHandler
                                            .getAimedTarget()
                                            ->getName());
                character->combatHandler.setAimedTarget(nullptr);
            }
        }
        return false;
    }
    // Get the item.
    auto item = character->findEquipmentItem(args[0].getContent(),
                                             args[0].getIndex());
    // If the room is NOT lit and NOT empty, pick a random item.
    if (!character->room->isLit() && !character->equipment.empty())
    {
        auto it = TRand<size_t>(0, character->equipment.size() - 1);
        item = character->equipment[it];
    }
    // Check if the player has the item equipped.
    if (item == nullptr)
    {
        character->sendMsg("You don't have that item equipped.\n");
        return false;
    }
    // Prepare the message showing from the where the item has been removed.
    std::string where, whereOthers;
    for (auto it = item->occupiedBodyParts.begin();
         it != item->occupiedBodyParts.end();)
    {
        where += "your " + (*it)->getDescription();
        whereOthers += character->getPossessivePronoun() + " ";
        whereOthers += (*it)->getDescription();
        it++;
        if (it != item->occupiedBodyParts.end())
        {
            where += ", ";
            whereOthers += ", ";
        }
    }
    // Remove the item from the player's equipment.
    character->remEquipmentItem(item);
    // Add the item to the inventory.
    character->addInventoryItem(item);
    // Notify the character.
    character->sendMsg("You remove %s from %s.\n",
                       item->getName(true),
                       where);
    // Send the message inside the room.
    character->room->sendToAll("%s removes %s from %s.\n",
                               {character},
                               character->getNameCapital(),
                               item->getName(true),
                               whereOthers);
    // Check if we have just removed ALL the USED Ranged Weapons.
    if (character->combatHandler.getAimedTarget() != nullptr)
    {
        if (GetActiveRangedWeapons(character).empty())
        {
            character->sendMsg("You stop aiming %s.\n",
                               character->combatHandler
                                        .getAimedTarget()
                                        ->getName());
            character->combatHandler.setAimedTarget(nullptr);
        }
    }
    return true;
}

bool DoInventory(Character * character, ArgumentHandler & /*args*/)
{
    // Check if the character is sleeping.
    if (character->posture == CharacterPosture::Sleep)
    {
        character->sendMsg("Not while you're sleeping.\n");
        return false;
    }
    if (character->inventory.empty())
    {
        character->sendMsg(
            Formatter::gray() + "    You are carrying anything.\n" +
            Formatter::reset());
    }
    // Check if the room is lit.
    bool roomIsLit = character->room->isLit();
    // Check if the inventory contains a lit light source.
    bool inventoryIsLit = character->inventoryIsLit();
    // Prepare the table for the inventory.
    Table table = Table("Inventory");
    table.addColumn("Item", StringAlign::Left);
    table.addColumn("Quantity", StringAlign::Right);
    table.addColumn("Weight", StringAlign::Right);
    // List all the items in inventory
    for (auto it : character->inventory)
    {
        TableRow row;
        row.emplace_back((roomIsLit || inventoryIsLit) ?
                         it->getNameCapital() : "Something");
        if (roomIsLit || inventoryIsLit)
        {
            row.emplace_back(ToString(it->quantity));
        }
        else
        {
            if (it->quantity == 1)
            {
                row.emplace_back((roomIsLit || inventoryIsLit) ?
                                 it->getNameCapital() : "One");
            }
            else
            {
                row.emplace_back((roomIsLit || inventoryIsLit) ?
                                 it->getNameCapital() : "Some");
            }
        }
        row.emplace_back(ToString(it->getWeight(true)));
        table.addRow(row);
    }
    character->sendMsg(table.getTable());
    character->sendMsg("\n%sTotal carrying weight%s: %s of %s %s.\n",
                       Formatter::yellow(),
                       Formatter::reset(),
                       ToString(character->getCarryingWeight()),
                       ToString(character->getMaxCarryingWeight()),
                       Mud::instance().getWeightMeasure());
    return true;
}
