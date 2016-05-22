/// @file   comm_object.cpp
/// @brief  Implements the methods used by the player in order to <b>interract with objects</b>.
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
#include <algorithm>
#include <istream>

#include "commands.hpp"
#include "mud.hpp"
// Other Include.
#include "utilities/table.hpp"

using namespace std;

void DoTake(Character * character, std::istream & sArgs)
{
    // Stop any action the character is executing.
    StopAction(character);
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    // Check the number of arguments.
    if (arguments.size() == 0)
    {
        character->sendMsg("Take what?\n");
        return; // Skip the rest of the function.
    }
    if ((arguments.size() != 1) && (arguments.size() != 2))
    {
        character->sendMsg("Wrong number of arguments.\n");
        return; // Skip the rest of the function.
    }
    // Try to take the item inside the room.
    if (arguments.size() == 1)
    {
        if (ToLower(arguments[0].first) == "all")
        {
            bool takenSomething = false;
            auto untouchedList = character->room->items;
            Mud::getInstance().getDbms().beginTransaction();
            for (auto iterator : untouchedList)
            {
                // Check if the item is static.
                if (HasFlag(iterator->model->flags, ModelFlag::Static))
                {
                    continue;
                }
                // Check if the item is built
                if (HasFlag(iterator->flags, ItemFlag::Built))
                {
                    continue;
                }
                // Check if the player can carry the item.
                if (!character->canCarry(iterator))
                {
                    continue;
                }
                // Remove the item from the room.
                character->room->removeItem(iterator);
                // Add the item to the player's inventory.
                character->addInventoryItem(iterator);
                // Update the item on database.
                iterator->updateOnDB();
                // Notify that something has been taken.
                takenSomething = true;
            }
            Mud::getInstance().getDbms().endTransaction();
            // Handle output only if the player has really taken something.
            if (!takenSomething)
            {
                character->sendMsg("You've picked up nothing.\n");
                return; // Skip the rest of the function.
            }
            character->sendMsg("You've picked up everything you could.\n");
            character->room->sendToAll(
                character->getNameCapital() + " has picked up everything " + character->getPrononun() + " could.\n",
                character);
            return; // Skip the rest of the function.
        }
        Item * item = character->room->findItem(arguments[0].first, arguments[0].second);
        // If the item is null.
        if (item == nullptr)
        {
            // Try to check if the character is a mobile, since mobiles can take
            //  items by providing the specific vnum.
            if (character->isMobile() && IsNumber(arguments[0].first))
            {
                for (auto it : character->room->items)
                {
                    if (it->vnum == ToInt(arguments[0].first))
                    {
                        item = it;
                    }
                }
            }
        }
        // Check if the item is null for the last time.
        if (item == nullptr)
        {
            character->sendMsg("You don't see that item inside the room.\n");
            return; // Skip the rest of the function.
        }
        // Check if the item has the flag kNoPick.
        if (HasFlag(item->model->flags, ModelFlag::Static) || HasFlag(item->flags, ItemFlag::Built))
        {
            character->sendMsg("You can't pick up this kind of items!\n");
            return; // Skip the rest of the function.
        }
        // Check if the player can carry the item.
        if (!character->canCarry(item))
        {
            character->sendMsg("You are not strong enough to carry that object.\n");
            return; // Skip the rest of the function.
        }
        // Remove the item from the room.
        character->room->removeItem(item);
        // Add the item to the player's inventory.
        character->addInventoryItem(item);
        // Update the item on database.
        Mud::getInstance().getDbms().beginTransaction();
        if (item->updateOnDB())
        {
            Mud::getInstance().getDbms().endTransaction();
        }
        else
        {
            Mud::getInstance().getDbms().rollbackTransection();
        }
        // Notify to player.
        character->sendMsg("You take " + Telnet::cyan() + ToLower(item->getName()) + Telnet::reset() + ".\n");
        character->room->sendToAll(
            character->getNameCapital() + " has picked up " + Telnet::cyan() + ToLower(item->getName())
                + Telnet::reset() + ".\n", character);
        return; // Skip the rest of the function.
    }
    if (arguments.size() == 2)
    {
        Item * container = character->findNearbyItem(arguments[1].first, arguments[1].second);
        if (container == nullptr)
        {
            character->sendMsg("You don't see that container.\n");
            return; // Skip the rest of the function.
        }
        if (ToLower(arguments[0].first) == "all")
        {
            bool takenSomething = false;
            auto untouchedList = container->content;
            Mud::getInstance().getDbms().beginTransaction();
            for (auto iterator : untouchedList)
            {
                // Check if the item is static.
                if (HasFlag(iterator->model->flags, ModelFlag::Static))
                {
                    continue;
                }
                // Check if the player can carry the item.
                if (!character->canCarry(iterator))
                {
                    continue;
                }
                // Remove the item from the container.
                container->takeOut(iterator);
                // Add the item to the player's inventory.
                character->addInventoryItem(iterator);
                // Update the item on database.
                iterator->updateOnDB();
                // Notify that something has been taken.
                takenSomething = true;
            }
            Mud::getInstance().getDbms().endTransaction();
            // Handle output only if the player has really taken something.
            if (takenSomething)
            {
                character->sendMsg("You've taken everything you could from " + container->getName() + ".\n");
            }
            else
            {
                character->sendMsg("You've taken nothing from " + container->getName() + ".\n");
            }
            return; // Skip the rest of the function.
        }
        Item * item = container->findContent(arguments[0].first, arguments[0].second);
        if (item == nullptr)
        {
            character->sendMsg("You don't see that item inside the container.\n");
            return; // Skip the rest of the function.
        }
        // Check if the item has the flag kNoPick.
        if (HasFlag(item->model->flags, ModelFlag::Static))
        {
            character->sendMsg("You can't pick up this kind of items!\n");
            return; // Skip the rest of the function.
        }
        // Check if the player can carry the item.
        if (!character->canCarry(item))
        {
            character->sendMsg("You are not strong enough to carry that object.\n");
            return; // Skip the rest of the function.
        }
        // Remove the item from the container.
        container->takeOut(item);
        // Add the item to the player's inventory.
        character->addInventoryItem(item);
        // Update the item on database.
        Mud::getInstance().getDbms().beginTransaction();
        if (item->updateOnDB())
        {
            Mud::getInstance().getDbms().endTransaction();
        }
        else
        {
            Mud::getInstance().getDbms().rollbackTransection();
        }

        character->sendMsg(
            "You take out " + Telnet::cyan() + ToLower(item->getName()) + Telnet::reset() + " from " + Telnet::cyan()
                + ToLower(container->getName()) + Telnet::reset() + ".\n");

        // Check if the player is invisible.
        character->room->sendToAll(
            character->getNameCapital() + " takes out " + Telnet::cyan() + ToLower(item->getName()) + Telnet::reset()
                + " from " + Telnet::cyan() + ToLower(container->getName()) + Telnet::reset() + ".\n", character);
    }
}

void DoDrop(Character * character, std::istream & sArgs)
{
    // Stop any action the character is executing.
    StopAction(character);
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    // Check the number of arguments.
    if (arguments.size() == 0)
    {
        character->sendMsg("Drop what?\n");
        return; // Skip the rest of the function.
    }
    if (arguments.size() > 1)
    {
        character->sendMsg("Too many arguments!\n");
        return; // Skip the rest of the function.
    }
    if (ToLower(arguments[0].first) == "all")
    {
        // Handle output only if the player has really taken something.
        if (character->inventory.empty())
        {
            character->sendMsg("You have nothing to drop.\n");
            return; // Skip the rest of the function.
        }
        auto untouchedList = character->inventory;
        Mud::getInstance().getDbms().beginTransaction();
        for (auto iterator : untouchedList)
        {
            // Remove the item from the player's inventory.
            character->remInventoryItem(iterator);
            // Add the item to the room.
            character->room->addItem(iterator);
            // Update the item on database.
            iterator->updateOnDB();
        }
        Mud::getInstance().getDbms().endTransaction();
        character->sendMsg("You dropped all.\n");
        character->room->sendToAll(
            character->getNameCapital() + " has dropped all " + character->getPrononun() + " items.\n", character);
        return; // Skip the rest of the function.
    }
    // Get the item.
    Item * item = character->findInventoryItem(arguments[0].first, arguments[0].second);
    // Check if the player has the item that he want to drop.
    if (item == nullptr)
    {
        character->sendMsg("You don't have that item.\n");
        return; // Skip the rest of the function.
    }
    // Update the item iside the Database.
    character->remInventoryItem(item);
    character->room->addItem(item);
    Mud::getInstance().getDbms().beginTransaction();
    if (item->updateOnDB())
    {
        Mud::getInstance().getDbms().endTransaction();
    }
    else
    {
        Mud::getInstance().getDbms().rollbackTransection();
    }
    // Active message.
    character->sendMsg("You drop " + Telnet::cyan() + ToLower(item->getName()) + Telnet::reset() + ".\n");
    character->room->sendToAll(
        character->getNameCapital() + " has dropped " + Telnet::cyan() + ToLower(item->getName()) + Telnet::reset() + ".\n",
        character);
}

void DoGive(Character * character, std::istream & sArgs)
{
    // Stop any action the character is executing.
    StopAction(character);
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    // Check the number of arguments.
    if (arguments.size() != 2)
    {
        character->sendMsg("Give what to whom?\n");
        return; // Skip the rest of the function.
    }
    // Get the item.
    Item * item = character->findInventoryItem(arguments[0].first, arguments[0].second);
    if (item == nullptr)
    {
        character->sendMsg("You don't have that item.\n");
        return; // Skip the rest of the function.
    }
    // Get the target.
    Character * target = character->room->findCharacter(arguments[1].first, arguments[1].second, character);
    if (target == nullptr)
    {
        character->sendMsg("You don't see that person.\n");
        return; // Skip the rest of the function.
    }
    // Check if the target player can carry the item.
    if (!target->canCarry(item))
    {
        character->sendMsg(target->getNameCapital() + " can't carry anymore items.\n");
        return; // Skip the rest of the function.
    }
    // Remove the item from the character inventory.
    if (!character->remInventoryItem(item))
    {
        character->sendMsg("You cannot give " + item->getName() + " to " + target->getName() + ".\n");
        return; // Skip the rest of the function.
    }
    // Add the item to the target inventory.
    if (!target->addInventoryItem(item))
    {
        character->sendMsg("You cannot give " + item->getName() + " to " + target->getName() + ".\n");
        return; // Skip the rest of the function.
    }
    // Check if the character is invisible.
    std::string viewdName = (HasFlag(character->flags, CharacterFlag::Invisible)) ? "Someone" : character->getNameCapital();
    // Update the item iside the Database.
    Mud::getInstance().getDbms().beginTransaction();
    if (item->updateOnDB())
    {
        Mud::getInstance().getDbms().endTransaction();
    }
    else
    {
        Mud::getInstance().getDbms().rollbackTransection();
    }
    // GIVE Message.
    character->sendMsg(
        "You give " + Telnet::cyan() + ToLower(item->getName()) + Telnet::reset() + " to " + target->getName() + ".\n");
    // RECEIVE Message.
    target->sendMsg(viewdName + " gives you " + Telnet::cyan() + ToLower(item->getName()) + Telnet::reset() + ".\n\n");
    // Check if the character is invisible.
    std::string broadcast;
    broadcast += character->getNameCapital() + " gives ";
    broadcast += Telnet::cyan() + ToLower(item->getName()) + Telnet::reset() + " to ";
    broadcast += target->getName() + ".\n";
    character->room->sendToAll(broadcast, character, target);
}

void DoEquipments(Character * character, std::istream & sArgs)
{
    // Check no more input.
    NoMore(character, sArgs);

    Item * head = character->findEquipmentSlotItem(EquipmentSlot::Head);
    Item * back = character->findEquipmentSlotItem(EquipmentSlot::Back);
    Item * torso = character->findEquipmentSlotItem(EquipmentSlot::Torso);
    Item * legs = character->findEquipmentSlotItem(EquipmentSlot::Legs);
    Item * feet = character->findEquipmentSlotItem(EquipmentSlot::Feet);
    Item * right = character->findEquipmentSlotItem(EquipmentSlot::RightHand);
    Item * left = character->findEquipmentSlotItem(EquipmentSlot::LeftHand);

    string output;
    // Print what is wearing.
    output += Telnet::yellow() + "#------------ Equipment -----------#\n" + Telnet::reset();
    // Equipment Slot : HEAD
    output += "    " + Telnet::yellow() + "Head" + Telnet::reset() + "       : ";
    output += (head != nullptr) ? Telnet::cyan() + head->getNameCapital() : Telnet::gray() + "Nothing";
    output += Telnet::reset() + ".\n";
    // Equipment Slot : BACK
    output += "    " + Telnet::yellow() + "Back" + Telnet::reset() + "       : ";
    output += (back != nullptr) ? Telnet::cyan() + back->getNameCapital() : Telnet::gray() + "Nothing";
    output += Telnet::reset() + ".\n";
    // Equipment Slot : TORSO
    output += "    " + Telnet::yellow() + "Torso" + Telnet::reset() + "      : ";
    output += (torso != nullptr) ? Telnet::cyan() + torso->getNameCapital() : Telnet::gray() + "Nothing";
    output += Telnet::reset() + ".\n";
    // Equipment Slot : LEGS
    output += "    " + Telnet::yellow() + "Legs" + Telnet::reset() + "       : ";
    output += (legs != nullptr) ? Telnet::cyan() + legs->getNameCapital() : Telnet::gray() + "Nothing";
    output += Telnet::reset() + ".\n";
    // Equipment Slot : FEET
    output += "    " + Telnet::yellow() + "Feet" + Telnet::reset() + "       : ";
    output += (feet != nullptr) ? Telnet::cyan() + feet->getNameCapital() : Telnet::gray() + "Nothing";
    output += Telnet::reset() + ".\n";

    // Print what is wielding.
    if (right != nullptr)
    {
        if (HasFlag(right->model->flags, ModelFlag::TwoHand))
        {
            output += "    " + Telnet::yellow() + "Both Hands" + Telnet::reset() + " : ";
        }
        else
        {
            output += "    " + Telnet::yellow() + "Right Hand" + Telnet::reset() + " : ";
        }
        output += Telnet::cyan() + right->getNameCapital();
    }
    else
    {
        output += "    " + Telnet::yellow() + "Right Hand" + Telnet::reset() + " : " + Telnet::gray() + "Nothing";
    }
    output += Telnet::reset() + ".\n";

    if (left != nullptr)
    {
        output += "    " + Telnet::yellow() + "Left Hand" + Telnet::reset() + "  : ";
        output += Telnet::cyan() + left->getNameCapital() + Telnet::reset() + ".\n";
    }
    output += Telnet::yellow() + "#----------------------------------#\n" + Telnet::reset();

    character->sendMsg(output);
}

void DoWield(Character * character, std::istream & sArgs)
{
    // Stop any action the character is executing.
    StopAction(character);
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    // Check the arguments.
    if (arguments.size() == 0)
    {
        character->sendMsg("Wield what?\n");
        return; // Skip the rest of the function.
    }
    if (arguments.size() > 1)
    {
        character->sendMsg("Too many arguments.\n");
        return; // Skip the rest of the function.
    }
    // Get the item.
    Item * item = character->findInventoryItem(arguments[0].first, arguments[0].second);
    // Check if the item is null.
    if (item == nullptr)
    {
        // Try to check if the character is a mobile, since mobiles can take
        //  items by providing the specific vnum.
        if (character->isMobile() && IsNumber(arguments[0].first))
        {
            for (auto it : character->inventory)
            {
                if (it->vnum == ToInt(arguments[0].first))
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
        return; // Skip the rest of the function.
    }
    // Check if can be wielded.
    if (!item->model->mustBeWielded())
    {
        character->sendMsg("This item it's not meant to be wield.\n");
        // In case the item must be weared, advise the player.
        if (item->model->type == ModelType::Armor)
        {
            character->sendMsg("Try to wear it instead.\n");
        }
        return; // Skip the rest of the function.
    }
    // String where the error message will be put.
    std::string errMessage;
    // The destination slot.
    EquipmentSlot destinationSlot;
    // Check if the character can wield the item.
    if (!character->canWield(item, errMessage, destinationSlot))
    {
        character->sendMsg(errMessage);
        return; // Skip the rest of the function.
    }
    // Set the item slot.
    item->setCurrentSlot(destinationSlot);
    // Remove the item from the inventory.
    character->remInventoryItem(item);
    // Equip the item.
    character->addEquipmentItem(item);
    // Save the item on the Database.
    Mud::getInstance().getDbms().beginTransaction();
    if (item->updateOnDB())
    {
        Mud::getInstance().getDbms().endTransaction();
    }
    else
    {
        Mud::getInstance().getDbms().rollbackTransection();
    }
    // Show the proper message.
    std::string message = "You wield " + Telnet::cyan() + ToLower(item->getName()) + Telnet::reset() + " ";
    if (HasFlag(item->model->flags, ModelFlag::TwoHand))
    {
        message += "with both your hands.\n";
    }
    else
    {
        message += "with your " + item->getCurrentSlotName() + ".\n";
    }
    character->sendMsg(message);
    // Notify to room.
    character->room->sendToAll(
        character->getNameCapital() + " wields " + Telnet::cyan() + ToLower(item->getName()) + Telnet::reset() + ".\n", character);
}

void DoWear(Character * character, std::istream & sArgs)
{
    // Stop any action the character is executing.
    StopAction(character);
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    // Check the arguments.
    if (arguments.size() == 0)
    {
        character->sendMsg("Wear what?\n");
        return; // Skip the rest of the function.
    }
    if (arguments.size() > 1)
    {
        character->sendMsg("Too many arguments.\n");
        return; // Skip the rest of the function.
    }
    if (arguments[0].first == "all")
    {
        bool wearedSomething = false;
        auto untouchedList = character->inventory;
        Mud::getInstance().getDbms().beginTransaction();
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
            // Update the item on database.
            iterator->updateOnDB();
            // Notify that something has been weared.
            wearedSomething = true;
        }
        Mud::getInstance().getDbms().endTransaction();
        // Handle output only if the player has really weared something.
        if (!wearedSomething)
        {
            character->sendMsg("You had nothing to wear.\n");
            return; // Skip the rest of the function.
        }
        character->sendMsg("You have weared everything you could.\n");
        character->room->sendToAll(character->getNameCapital() + " has weared all he could.\n", character);
        return; // Skip the rest of the function.
    }

    Item * item = character->findInventoryItem(arguments[0].first, arguments[0].second);
    // Check if the item is null.
    if (item == nullptr)
    {
        // Try to check if the character is a mobile, since mobiles can take
        //  items by providing the specific vnum.
        if (character->isMobile() && IsNumber(arguments[0].first))
        {
            for (auto it : character->inventory)
            {
                if (it->vnum == ToInt(arguments[0].first))
                {
                    item = it;
                }
            }
        }
    }
    if (item == nullptr)
    {
        character->sendMsg("You don't have that item.\n");
        return; // Skip the rest of the function.
    }
    std::string errMessage;
    if (!character->canWear(item, errMessage))
    {
        character->sendMsg(errMessage);
        return; // Skip the rest of the function.
    }
    // Remove the item from the player's inventory.
    character->remInventoryItem(item);
    // Add the item to the equipment.
    character->addEquipmentItem(item);
    // Update the item on database.
    Mud::getInstance().getDbms().beginTransaction();
    if (item->updateOnDB())
    {
        Mud::getInstance().getDbms().endTransaction();
    }
    else
    {
        Mud::getInstance().getDbms().rollbackTransection();
    }
    // Notify to character.
    character->sendMsg("You wear " + Telnet::cyan() + ToLower(item->getName()) + Telnet::reset() + ".\n");
    character->room->sendToAll(
        character->getNameCapital() + " wears " + Telnet::cyan() + ToLower(item->getName()) + Telnet::reset() + ".\n", character);
}

void DoRemove(Character * character, std::istream & sArgs)
{
    // Stop any action the character is executing.
    StopAction(character);
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    // Check the arguments.
    if (arguments.size() == 0)
    {
        character->sendMsg("Remove what?\n");
        return; // Skip the rest of the function.
    }
    if (arguments.size() > 1)
    {
        character->sendMsg("Too many arguments.\n");
        return; // Skip the rest of the function.
    }
    if (arguments[0].first == "all")
    {
        // Handle output only if the player has really removed something.
        if (character->equipment.empty())
        {
            character->sendMsg("You have nothing to remove.\n");
            return; // Skip the rest of the function.
        }
        auto untouchedList = character->equipment;
        Mud::getInstance().getDbms().beginTransaction();
        for (auto iterator : untouchedList)
        {
            // Remove the item from the player's equipment.
            character->remEquipmentItem(iterator);
            // Add the item to the inventory.
            character->addInventoryItem(iterator);
            // Update the item on database.
            iterator->updateOnDB();
        }
        Mud::getInstance().getDbms().endTransaction();
        character->sendMsg("You have removed everything.\n");
        // Check if the character is invisible.
        character->room->sendToAll(character->getNameCapital() + " has undressed all he could.\n", character);
        return; // Skip the rest of the function.
    }
    // Get the item.
    Item * item = character->findEquipmentItem(arguments[0].first, arguments[0].second);
    // Check if the player has the item equipped.
    if (item == nullptr)
    {
        character->sendMsg("You don't have that item equipped.\n");
        return; // Skip the rest of the function.
    }
    // Remove the item from the player's equipment.
    character->remEquipmentItem(item);
    // Add the item to the inventory.
    character->addInventoryItem(item);
    // Update the item on database.
    Mud::getInstance().getDbms().beginTransaction();
    if (item->updateOnDB())
    {
        Mud::getInstance().getDbms().endTransaction();
    }
    else
    {
        Mud::getInstance().getDbms().rollbackTransection();
    }
    // Notify the character.
    character->sendMsg("You remove " + Telnet::cyan() + ToLower(item->getName()) + Telnet::reset() + ".\n");
    // Check if the player is invisible.
    character->room->sendToAll(
        character->getNameCapital() + " removes " + Telnet::cyan() + ToLower(item->getName()) + Telnet::reset() + ".\n", character);
}

void DoInventory(Character * character, std::istream & sArgs)
{
    NoMore(character, sArgs);
    if (character->inventory.empty())
    {
        character->sendMsg(Telnet::gray() + "    You are carrying anything.\n" + Telnet::reset());
    }
    Table table = Table("Inventory");
    table.addColumn("Item", kAlignLeft);
    table.addColumn("Quantity", kAlignRight);
    table.addColumn("Weight", kAlignRight);
    // List all the items in inventory
    for (auto it : GroupItems(character->inventory))
    {
        table.addRow(
        { it.first->getNameCapital(), ToString(it.second), ToString(it.first->getWeight()) });
    }
    character->sendMsg(table.getTable());
    std::string carried = ToString(character->getCarryingWeight());
    std::string maximum = ToString(character->getMaxCarryingWeight());
    character->sendMsg(
        Telnet::yellow() + "\nTotal carrying weight: " + Telnet::reset() + carried + " of " + maximum + Telnet::reset()
            + " " + mud_measure + ".\n");
}

void DoOrganize(Character * character, std::istream & sArgs)
{
    // Stop any action the character is executing.
    StopAction(character);
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    // Check the number of arguments.
    if (arguments.empty())
    {
        character->sendMsg("Organize what?\n");
        return; // Skip the rest of the function.
    }
    ItemSorter sorter = OrderItemByName;
    std::string sorterTag = "name";
    if (BeginWith("name", ToLower(arguments[0].first)))
    {
        sorter = OrderItemByName;
        sorterTag = "name";
    }
    else if (BeginWith("weight", ToLower(arguments[0].first)))
    {
        sorter = OrderItemByWeight;
        sorterTag = "weight";
    }
    else
    {
        character->sendMsg("You can organize by: name, weight,...\n");
        return; // Skip the rest of the function.
    }
    if (arguments.size() == 1)
    {
        ItemList * list = &character->room->items;
        sort(list->begin(), list->end(), sorter);
        character->sendMsg("You have organized the room by " + sorterTag + ".\n");
        return; // Skip the rest of the function.
    }
    else if (arguments.size() == 2)
    {
        Item * container = character->findNearbyItem(arguments[1].first, arguments[1].second);
        if (container == nullptr)
        {
            character->sendMsg("What do you want to organize?\n");
            return; // Skip the rest of the function.
        }
        if (container->content.empty())
        {
            character->sendMsg("You can't organize " + container->getName() + "\n");
            return; // Skip the rest of the function.
        }
        // Organize the target container.
        ItemList * list = &container->content;
        sort(list->begin(), list->end(), sorter);
        character->sendMsg("You have organized " + container->getName() + ", by " + sorterTag + ".\n");
    }
    else
    {
        character->sendMsg("Too much arguments.\n");
        return; // Skip the rest of the function.
    }
}

void DoOpen(Character * character, std::istream & sArgs)
{
    // Stop any action the character is executing.
    StopAction(character);
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    // Check the arguments.
    if (arguments.size() != 1)
    {
        character->sendMsg("What do you want to open?\n");
        return; // Skip the rest of the function.
    }
    // Check if the character want to open something in onother direction.
    Direction direction = Mud::getInstance().findDirection(arguments[0].first, false);
    if (direction != Direction::None)
    {
        // Check if the direction exists.
        Exit * roomExit = character->room->findExit(direction);
        if (roomExit == nullptr)
        {
            character->sendMsg("There is nothing in that direction.\n");
            return; // Skip the rest of the function.
        }
        Room * destination = roomExit->destination;
        if (destination == nullptr)
        {
            character->sendMsg("There is nothing in that direction.\n");
            return; // Skip the rest of the function.
        }
        Item * door = destination->findDoor();
        if (door == nullptr)
        {
            character->sendMsg("There is no door in that direction.\n");
            return; // Skip the rest of the function.
        }
        if (HasFlag(door->flags, ItemFlag::Locked))
        {
            character->sendMsg("You have first to unlock it.\n");
            return; // Skip the rest of the function.
        }
        if (!HasFlag(door->flags, ItemFlag::Closed))
        {
            character->sendMsg("There door it's already opened.\n");
            return; // Skip the rest of the function.
        }

        ClearFlag(door->flags, ItemFlag::Closed);

        // Display message.
        if (HasFlag(roomExit->flags, ExitFlag::Hidden))
        {
            character->sendMsg("You have opened a hidden door!\n");
            character->room->sendToAll(character->getNameCapital() + " opens a hidden door!\n", character);
        }
        else
        {
            character->sendMsg("You have opened the door.\n");
            character->room->sendToAll(character->getNameCapital() + " closes a door.\n", character);
        }

        for (auto destExit : destination->exits)
        {
            Room * otherSide = destExit->destination;
            if (otherSide == nullptr)
            {
                continue;
            }
            if (otherSide == character->room)
            {
                continue;
            }
            if (HasFlag(destExit->flags, ExitFlag::Hidden))
            {
                // Show the action in the next room.
                otherSide->sendToAll("Someone opens a secret passage from the other side.\n");
            }
            else
            {
                otherSide->sendToAll("Someone opens a door from the other side.\n");
            }
        }
    }
    else
    {
        //Item * container = character->findNearbyItem(arguments[0].first, arguments[0].second);
    }
    return;
}

void DoClose(Character * character, std::istream & sArgs)
{
    // Stop any action the character is executing.
    StopAction(character);
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    // Check the arguments.
    if (arguments.size() != 1)
    {
        character->sendMsg("What do you want to close?\n");
        return; // Skip the rest of the function.
    }
    // Check if the character want to open something in onother direction.
    Direction direction = Mud::getInstance().findDirection(arguments[0].first, false);
    if (direction != Direction::None)
    {
        // Check if the direction exists.
        Exit * roomExit = character->room->findExit(direction);
        if (roomExit == nullptr)
        {
            character->sendMsg("There is nothing in that direction.\n");
            return; // Skip the rest of the function.
        }
        Room * destination = roomExit->destination;
        if (destination == nullptr)
        {
            character->sendMsg("There is nothing in that direction.\n");
            return; // Skip the rest of the function.
        }
        Item * door = destination->findDoor();
        if (door == nullptr)
        {
            character->sendMsg("There is no door in that direction.\n");
            return; // Skip the rest of the function.
        }
        if (HasFlag(door->flags, ItemFlag::Closed))
        {
            character->sendMsg("There door it's already closed.\n");
            return; // Skip the rest of the function.
        }
        if (destination->items.size() > 1)
        {
            character->sendMsg("There are items on the way, you can't close the door.\n");
            return; // Skip the rest of the function.
        }
        if (destination->characters.size() >= 1)
        {
            character->sendMsg("There are someone on the way, you can't close the door.\n");
            return; // Skip the rest of the function.
        }

        SetFlag(door->flags, ItemFlag::Closed);

        // Display message.
        if (HasFlag(roomExit->flags, ExitFlag::Hidden))
        {
            character->sendMsg("You have closed a hidden door!\n");
            character->room->sendToAll(character->getNameCapital() + " closes a hidden door!\n", character);
        }
        else
        {
            character->sendMsg("You have closed the door.\n");
            character->room->sendToAll(character->getNameCapital() + " closes a door.\n", character);
        }

        for (auto destExit : destination->exits)
        {
            Room * otherSide = destExit->destination;
            if (otherSide == nullptr)
            {
                continue;
            }
            if (otherSide == character->room)
            {
                continue;
            }
            if (HasFlag(destExit->flags, ExitFlag::Hidden))
            {
                // Show the action in the next room.
                otherSide->sendToAll("Someone closes a secret passage from the other side.\n");
            }
            else
            {
                otherSide->sendToAll("Someone closes a door from the other side.\n");
            }
        }
        return; // Skip the rest of the function.
    }
    else
    {
        //Item * container = character->findNearbyItem(arguments[0].first, arguments[0].second);
    }
    return;
}

void DoPut(Character * character, std::istream & sArgs)
{
    // Stop any action the character is executing.
    StopAction(character);
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    if (arguments.size() != 2)
    {
        character->sendMsg("Put what inside what?\n");
        return;
    }
    Item * container = character->findNearbyItem(arguments[1].first, arguments[1].second);
    if (container == nullptr)
    {
        character->sendMsg("You don't see any container named '" + arguments[1].first + "' here.\n");
        return;
    }
    if (container->model->type != ModelType::Container)
    {
        character->sendMsg("'" + container->getName() + "' is not a container.\n");
        return;
    }

    // Check if the player wants to put all in the container.
    if (arguments[0].first == "all")
    {
        if (character->inventory.empty())
        {
            character->sendMsg("You don't have anything to put in a container.");
            return;
        }
        auto originalList = character->inventory;
        Mud::getInstance().getDbms().beginTransaction();
        for (auto iterator : originalList)
        {
            if (iterator == container)
            {
                continue;
            }
            if (!container->putInside(iterator))
            {
                continue;
            }
            // Remove the item from the player's inventory.
            character->remInventoryItem(iterator);
        }
        Mud::getInstance().getDbms().endTransaction();
        character->sendMsg("You put everything you could in " + container->getName() + ".\n");
        return;
    }
    Item * item = character->findInventoryItem(arguments[0].first, arguments[0].second);
    // Gather the item.
    if (item == nullptr)
    {
        character->sendMsg("You don't have any '" + arguments[0].first + "'.\n");
        return;
    }
    // Try to put the item inside the container.
    Mud::getInstance().getDbms().beginTransaction();
    if (!container->putInside(item))
    {
        Mud::getInstance().getDbms().rollbackTransection();
        character->sendMsg(container->getName() + " can't contain any more items.\n");
        return;
    }
    if (!character->remInventoryItem(item))
    {
        container->putInside(item);
        Mud::getInstance().getDbms().endTransaction();
        character->sendMsg("You can't let go " + item->getName() + ".\n");
        return;
    }
    Mud::getInstance().getDbms().endTransaction();
    // Notify to player.
    character->sendMsg(
        "You put " + Telnet::cyan() + ToLower(item->getName()) + Telnet::reset() + " inside " + Telnet::cyan()
            + ToLower(container->getName()) + Telnet::reset() + ".\n");

    // Check if the player is invisible.
    character->room->sendToAll(
        character->getNameCapital() + " puts " + Telnet::cyan() + ToLower(item->getName()) + Telnet::reset() + " inside "
            + Telnet::cyan() + ToLower(container->getName()) + Telnet::reset() + ".\n", character);
}

void DoDrink(Character * character, std::istream & sArgs)
{
    std::string containerName;
    int containerNumber = 1;
    Item * container;

    // Read the argument.
    sArgs >> containerName >> ws;

    // Check no more input.
    NoMore(character, sArgs);

    if (containerName.empty())
    {
        throw std::runtime_error("Drink what?\n");
    }

    ///////////////////////////////////////////////////////////////////////////
    // Extract the CONTAINER number, if the character has provided one.
    ExtractNumber(containerName, containerNumber);

    ///////////////////////////////////////////////////////////////////////////
    // Search the container.
    container = character->findEquipmentItem(containerName, containerNumber);
    if (container == nullptr)
    {
        container = character->findInventoryItem(containerName, containerNumber);
        if (container == nullptr)
        {
            container = character->room->findItem(containerName, containerNumber);
            if (container == nullptr)
            {
                throw std::runtime_error("You don't see that container.\n");
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    // Execute every necessary checks.
    if (container->model->type != ModelType::LiqContainer)
    {
        throw std::runtime_error(container->getNameCapital() + " is not a container for liquids.\n");
    }
    if (container->isEmpty())
    {
        throw std::runtime_error(container->getNameCapital() + " is empty.\n");
    }

    ///////////////////////////////////////////////////////////////////////////
    // Take out the liquid.
    Liquid * liquid = container->contentLiq.first;

    Mud::getInstance().getDbms().beginTransaction();
    bool result = container->pourOut(1);
    Mud::getInstance().getDbms().endTransaction();

    if (!result)
    {
        throw std::runtime_error(
            "You were not able to drink some " + liquid->getName() + " from " + container->getName() + ".\n");
    }

    character->sendMsg("You drink some " + liquid->getName() + " from " + container->getName() + ".\n");
    character->room->sendToAll(
        character->getNameCapital() + " drinks some " + liquid->getName() + " from " + container->getName() + ".\n", character);
}

void DoFill(Character * character, std::istream & sArgs)
{
    // Stop any action the character is executing.
    StopAction(character);
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    // Check the arguments.
    if (arguments.size() != 2)
    {
        character->sendMsg("You have to fill something from a source.\n");
        return; // Skip the rest of the function.
    }

    // Search the container.
    Item * container = character->findInventoryItem(arguments[0].first, arguments[0].second);
    if (container == nullptr)
    {
        container = character->findEquipmentItem(arguments[0].first, arguments[0].second);
        if (container == nullptr)
        {
            character->sendMsg("You don't have any '" + arguments[0].first + "' with you.\n");
            return; // Skip the rest of the function.
        }
    }

    // Search the source.
    Item * source = character->findInventoryItem(arguments[1].first, arguments[1].second);
    if ((source == nullptr) || (source == container))
    {
        source = character->findEquipmentItem(arguments[1].first, arguments[1].second);
        if ((source == nullptr) || (source == container))
        {
            source = character->room->findItem(arguments[1].first, arguments[1].second);
            if (source == nullptr)
            {
                character->sendMsg("You don't see any '" + arguments[1].first + "'.\n");
                return; // Skip the rest of the function.
            }
        }
    }

    // Check if the items are suitable source and container of liquids.
    if (container->model->type != ModelType::LiqContainer)
    {
        character->sendMsg(container->getNameCapital() + " is not a suitable container.\n");
        return; // Skip the rest of the function.
    }
    if (source->model->type != ModelType::LiqContainer)
    {
        character->sendMsg(source->getNameCapital() + " is not a suitable source of liquids.\n");
        return; // Skip the rest of the function.
    }

    // Check if source is empty.
    if (source->isEmpty())
    {
        character->sendMsg(source->getNameCapital() + " is empty.\n");
        return; // Skip the rest of the function.
    }

    // Get the liquid from the source and eventually from the container.
    Liquid * sourLiquid = source->contentLiq.first;
    Liquid * contLiquid = container->contentLiq.first;

    // Check compatibility between liquids.
    if (contLiquid != nullptr)
    {
        if (sourLiquid != contLiquid)
        {
            character->sendMsg("You can't mix those two liquids.\n");
            return; // Skip the rest of the function.
        }
    }

    // Fill the container from the source.
    int atDisposal = source->contentLiq.second;
    int quantity = container->getFreeSpace();

    int sourceFlags = source->model->getLiqContainerFunc().flags;
    if (!HasFlag(sourceFlags, LiqContainerFlag::Endless))
    {
        if (atDisposal < quantity)
        {
            quantity = atDisposal;
        }
    }

    Mud::getInstance().getDbms().beginTransaction();
    if (!source->pourOut(quantity))
    {
        character->sendMsg("You failed to take out the liquid from a" + source->getNameCapital() + ".\n");
        Mud::getInstance().getDbms().endTransaction();
        return; // Skip the rest of the function.
    }
    if (!container->pourIn(sourLiquid, quantity))
    {
        character->sendMsg("You failed to fill the container with the liquid.\n");
        Mud::getInstance().getDbms().endTransaction();
        return; // Skip the rest of the function.
    }
    Mud::getInstance().getDbms().endTransaction();

    character->sendMsg(
        "You fill " + container->getName() + " with the " + sourLiquid->getName() + " of " + source->getName() + "\n");

    // Check if the character is invisible.
    character->room->sendToAll(
        character->getNameCapital() + " fills " + container->getName() + " from " + sourLiquid->getName() + ".\n", character);
}

void DoPour(Character * character, std::istream & sArgs)
{
    // Stop any action the character is executing.
    StopAction(character);
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    // Check the arguments.
    if (arguments.size() != 2)
    {
        character->sendMsg("You have to pour something into something else.\n");
        return; // Skip the rest of the function.
    }

    // Search the container.
    Item * source = character->findInventoryItem(arguments[0].first, arguments[0].second);
    if (source == nullptr)
    {
        source = character->findEquipmentItem(arguments[0].first, arguments[0].second);
        if (source == nullptr)
        {
            character->sendMsg("You don't have any '" + arguments[0].first + "' with you.\n");
            return; // Skip the rest of the function.
        }
    }

    // Search the source.
    Item * container = character->findInventoryItem(arguments[1].first, arguments[1].second);
    if ((container == nullptr) || (container == source))
    {
        container = character->findEquipmentItem(arguments[1].first, arguments[1].second);
        if ((container == nullptr) || (container == source))
        {
            container = character->room->findItem(arguments[1].first, arguments[1].second);
            if (container == nullptr)
            {
                character->sendMsg("You don't see any '" + arguments[1].first + "'.\n");
                return; // Skip the rest of the function.
            }
        }
    }

    // Check if the items are suitable source and container of liquids.
    if (container->model->type != ModelType::LiqContainer)
    {
        character->sendMsg(container->getNameCapital() + " is not a suitable container.\n");
        return; // Skip the rest of the function.
    }
    if (source->model->type != ModelType::LiqContainer)
    {
        character->sendMsg(source->getNameCapital() + " is not a suitable source of liquids.\n");
        return; // Skip the rest of the function.
    }

    // Check if source is empty.
    if (source->isEmpty())
    {
        character->sendMsg(source->getNameCapital() + " is empty.\n");
        return; // Skip the rest of the function.
    }

    // Get the liquid from the source and eventually from the container.
    Liquid * sourLiquid = source->contentLiq.first;
    Liquid * contLiquid = container->contentLiq.first;

    // Check compatibility between liquids.
    if (contLiquid != nullptr)
    {
        if (sourLiquid != contLiquid)
        {
            character->sendMsg("You can't mix those two liquids.\n");
            return; // Skip the rest of the function.
        }
    }

    // Fill the container from the source.
    int atDisposal = source->contentLiq.second;
    int quantity = container->getFreeSpace();

    int sourceFlags = source->model->getLiqContainerFunc().flags;
    if (!HasFlag(sourceFlags, LiqContainerFlag::Endless))
    {
        if (atDisposal < quantity)
        {
            quantity = atDisposal;
        }
    }

    Mud::getInstance().getDbms().beginTransaction();
    if (!source->pourOut(quantity))
    {
        character->sendMsg("You failed to pour out the liquid from " + source->getNameCapital() + ".\n");
        Mud::getInstance().getDbms().endTransaction();
        return; // Skip the rest of the function.
    }
    if (!container->pourIn(sourLiquid, quantity))
    {
        character->sendMsg("You failed to pour the liquid into " + source->getNameCapital() + ".\n");
        Mud::getInstance().getDbms().endTransaction();
        return; // Skip the rest of the function.
    }
    Mud::getInstance().getDbms().endTransaction();

    character->sendMsg(
        "You pour " + sourLiquid->getName() + " of " + source->getName() + " into " + container->getName() + ".\n");

    // Check if the character is invisible.
    character->room->sendToAll(
        character->getNameCapital() + " pour " + sourLiquid->getName() + " of " + source->getName() + " into "
            + container->getName() + ".\n", character);
}