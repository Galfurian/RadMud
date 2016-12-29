/// @file   commandObjectCharacter.cpp
/// @brief  
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

#include "formatter.hpp"
#include "command.hpp"
#include "room.hpp"
#include "mud.hpp"

bool DoEquipments(Character * character, ArgumentHandler & /*args*/)
{
    // Retrieve the equipment.
    auto head = character->findEquipmentSlotItem(EquipmentSlot::Head);
    auto back = character->findEquipmentSlotItem(EquipmentSlot::Back);
    auto torso = character->findEquipmentSlotItem(EquipmentSlot::Torso);
    auto legs = character->findEquipmentSlotItem(EquipmentSlot::Legs);
    auto feet = character->findEquipmentSlotItem(EquipmentSlot::Feet);
    auto right = character->findEquipmentSlotItem(EquipmentSlot::RightHand);
    auto left = character->findEquipmentSlotItem(EquipmentSlot::LeftHand);
    // Check if the room is lit.
    bool roomIsLit = false;
    if (character->room != nullptr)
    {
        roomIsLit = character->room->isLit();
    }
    std::string output;
    // Print what is wearing.
    output += Formatter::yellow();
    output += "#------------ Equipment -----------#\n";
    output += Formatter::reset();
    // Equipment Slot : HEAD
    output += "\tHead       : ";
    if (head != nullptr)
    {
        if (roomIsLit) output += head->getNameCapital(true);
        else output += "Something";
    }
    output += "\n";
    // Equipment Slot : BACK
    output += "\tBack       : ";
    if (back != nullptr)
    {
        if (roomIsLit) output += back->getNameCapital(true);
        else output += "Something";
    }
    output += "\n";
    // Equipment Slot : TORSO
    output += "\tTorso      : ";
    if (torso != nullptr)
    {
        if (roomIsLit) output += torso->getNameCapital(true);
        else output += "Something";
    }
    output += "\n";
    // Equipment Slot : LEGS
    output += "\tLegs       : ";
    if (legs != nullptr)
    {
        if (roomIsLit) output += legs->getNameCapital(true);
        else output += "Something";
    }
    output += "\n";
    // Equipment Slot : FEET
    output += "\tFeet       : ";
    if (feet != nullptr)
    {
        if (roomIsLit) output += feet->getNameCapital(true);
        else output += "Something";
    }
    output += "\n";
    // Print what is wielding.
    if (right != nullptr)
    {
        if (HasFlag(right->model->modelFlags, ModelFlag::TwoHand))
        {
            output += "\tBoth Hands : ";
        }
        else
        {
            output += "\tRight Hand : ";
        }
        if (roomIsLit) output += right->getNameCapital(true);
        else output += "Something";
        output += "\n";
    }
    if (left != nullptr)
    {
        output += "\tLeft Hand  : ";

        if (roomIsLit) output += left->getNameCapital(true);
        else output += "Something";
        output += "\n";
    }
    output += Formatter::yellow();
    output += "#----------------------------------#\n";
    output += Formatter::reset();
    character->sendMsg(output);
    return true;
}

bool DoWield(Character * character, ArgumentHandler & args)
{
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
        // If the inventory is NOT lit, pick a random item.
        if (!character->inventoryIsLit())
        {
            auto it = TRandInteger<size_t>(0, character->inventory.size() - 1);
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
    // Check if can be wielded.
    if (!item->model->mustBeWielded())
    {
        character->sendMsg("This item it's not meant to be wield.\n");
        // In case the item must be weared, advise the player.
        if (item->model->getType() == ModelType::Armor)
        {
            character->sendMsg("Try to wear it instead.\n");
        }
        return false;
    }
    // String where the error message will be put.
    std::string errMessage;
    // The destination slot.
    EquipmentSlot destinationSlot;
    // Check if the character can wield the item.
    if (!character->canWield(item, errMessage, destinationSlot))
    {
        character->sendMsg(errMessage);
        return false;
    }
    // Set the item slot.
    item->setCurrentSlot(destinationSlot);
    // Remove the item from the inventory.
    character->remInventoryItem(item);
    // Equip the item.
    character->addEquipmentItem(item);
    // Show the proper message.
    auto object = item->getName(true);
    auto where = "with your " + item->getCurrentSlotName();
    auto whereOthers = character->getPossessivePronoun();
    whereOthers += " " + item->getCurrentSlotName();
    if (HasFlag(item->model->modelFlags, ModelFlag::TwoHand))
    {
        where = "both your hands";
        whereOthers = "both " + character->getPossessivePronoun() + " hands";
    }
    character->sendMsg("You wield %s with %s.\n", object, where);
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
        auto wearedSomething = false;
        auto untouchedList = character->inventory;
        for (auto iterator : untouchedList)
        {
            std::string errMessage;
            if (!character->canWear(iterator, errMessage))
            {
                continue;
            }
            // Remove the item from the player's inventory.
            character->remInventoryItem(iterator);
            // Add the item to the equipment.
            character->addEquipmentItem(iterator);
            // Notify that something was worn.
            wearedSomething = true;
        }
        // Handle output only if the player has really worn something.
        if (!wearedSomething)
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
        // If the inventory is NOT lit, pick a random item.
        if (!character->inventoryIsLit())
        {
            auto it = TRandInteger<size_t>(0, character->inventory.size() - 1);
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
    if (item == nullptr)
    {
        character->sendMsg("You don't have that item.\n");
        return false;
    }
    std::string errMessage;
    if (!character->canWear(item, errMessage))
    {
        character->sendMsg(errMessage);
        return false;
    }
    // Remove the item from the player's inventory.
    character->remInventoryItem(item);
    // Add the item to the equipment.
    character->addEquipmentItem(item);
    // Notify to character.
    character->sendMsg("You wear %s on your %s.\n", item->getName(true),
                       ToLower(item->getCurrentSlotName()));
    // Send the message inside the room.
    character->room->sendToAll("%s wears %s on %s %s.\n",
                               {character},
                               character->getNameCapital(),
                               item->getName(true),
                               character->getPossessivePronoun(),
                               ToLower(item->getCurrentSlotName()));
    return true;
}

bool DoRemove(Character * character, ArgumentHandler & args)
{
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
    // Define a function which checks if all the ranged weapons have been removed.
    auto CheckIfRemovedAllRangedWeapons = [&character]()
    {
        if (character->combatHandler.getAimedTarget() == nullptr)
        {
            return false;
        }
        auto left = character->findEquipmentSlotItem(EquipmentSlot::LeftHand);
        if (left != nullptr)
        {
            if (left->getType() == ModelType::RangedWeapon)
            {
                return false;
            }
        }
        auto right = character->findEquipmentSlotItem(EquipmentSlot::RightHand);
        if (right != nullptr)
        {
            if (right->getType() == ModelType::RangedWeapon)
            {
                return false;
            }
        }
        return true;
    };

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
            if (CheckIfRemovedAllRangedWeapons())
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
    // If the room is not lit, pick a random item.
    if (!character->room->isLit())
    {
        auto it = TRandInteger<size_t>(0, character->equipment.size() - 1);
        item = character->equipment[it];
    }
    // Check if the player has the item equipped.
    if (item == nullptr)
    {
        character->sendMsg("You don't have that item equipped.\n");
        return false;
    }
    // Remove the item from the player's equipment.
    character->remEquipmentItem(item);
    // Add the item to the inventory.
    character->addInventoryItem(item);
    // Notify the character.
    character->sendMsg("You remove %s from your %s.\n",
                       item->getName(true),
                       ToLower(item->getCurrentSlotName()));
    // Send the message inside the room.
    character->room->sendToAll("%s removes %s from %s %s.\n",
                               {character},
                               character->getNameCapital(),
                               item->getName(true),
                               character->getPossessivePronoun(),
                               ToLower(item->getCurrentSlotName()));
    // Check if we have just removed ALL the USED Ranged Weapons.
    if (character->combatHandler.getAimedTarget() != nullptr)
    {
        if (CheckIfRemovedAllRangedWeapons())
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
