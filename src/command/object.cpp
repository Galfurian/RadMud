/// @file   object.cpp
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

#include "command.hpp"

#include "../mud.hpp"
#include "../utilities/table.hpp"
#include "../item/shopItem.hpp"

#include "../model/liquidContainerModel.hpp"
#include "../model/currencyModel.hpp"

#include <algorithm>

using namespace std;

void DoTake(Character * character, std::istream & sArgs)
{
    // Stop any action the character is executing.
    StopAction(character);
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    // Try to take the item inside the room.
    if (arguments.size() == 1)
    {
        if (ToLower(arguments[0].first) == "all")
        {
            std::vector<Item *> untouchedList = character->room->items;
            std::vector<Item *> actuallyTaken;
            for (auto iterator : untouchedList)
            {
                // Check if the item is static.
                if (HasFlag(iterator->model->modelFlags, ModelFlag::Static))
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
                // Add the item to the list of actually taken items.
                actuallyTaken.push_back(iterator);
            }
            // Handle output only if the player has really taken something.
            if (actuallyTaken.empty())
            {
                character->sendMsg("You've picked up nothing.\n");
                return;
            }
            character->sendMsg("You've picked up everything you could.\n");
            // Send the message inside the room.
            character->room->sendToAll(
                "%s has picked up everything %s could.\n",
                { character },
                character->getNameCapital(),
                character->getSubjectPronoun());
            return;
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
        // Check if the item is still null.
        if (item == nullptr)
        {
            character->sendMsg("You don't see that item inside the room.\n");
            return;
        }
        // Check if the item has the flag Static.
        if (HasFlag(item->model->modelFlags, ModelFlag::Static))
        {
            character->sendMsg("You can't pick up %s!\n", item->getName());
            return;
        }
        if (HasFlag(item->flags, ItemFlag::Built))
        {
            character->sendMsg("You can't pick up something which is built!\n");
            return;
        }
        // Check if the player can carry the item.
        if (!character->canCarry(item))
        {
            character->sendMsg("You can't carry %s!\n", item->getName());
            return;
        }
        // Remove the item from the room.
        character->room->removeItem(item);
        // Add the item to the player's inventory.
        character->addInventoryItem(item);
        // Notify to player.
        character->sendMsg(
            "You take %s.\n",
            Formatter::cyan() + ToLower(item->getName()) + Formatter::reset());
        // Send the message inside the room.
        character->room->sendToAll(
            "%s has picked up %s.\n",
            { character },
            character->getNameCapital(),
            Formatter::cyan() + ToLower(item->getName()) + Formatter::reset());
        return;
    }
    else if (arguments.size() == 2)
    {
        Item * container = character->findNearbyItem(arguments[1].first, arguments[1].second);
        if (container == nullptr)
        {
            character->sendMsg("You don't see that container.\n");
            return;
        }
        if (HasFlag(container->flags, ItemFlag::Locked))
        {
            character->sendMsg("You have first to unlock %s.\n", container->getName());
            return;
        }
        if (HasFlag(container->flags, ItemFlag::Closed))
        {
            character->sendMsg("You have first to open %s.\n", container->getName());
            return;
        }
        if (ToLower(arguments[0].first) == "all")
        {
            auto takenSomething = false;
            auto untouchedList = container->content;
            for (auto iterator : untouchedList)
            {
                // Check if the item is static.
                if (HasFlag(iterator->model->modelFlags, ModelFlag::Static))
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
                takenSomething = true;
            }
            // Handle output only if the player has really taken something.
            if (!takenSomething)
            {
                character->sendMsg("You've taken nothing from %s.\n", container->getName());
                return;
            }
            character->sendMsg(
                "You've taken everything you could from %s.\n",
                container->getName());
            // Send the message inside the room.
            character->room->sendToAll(
                "%s has taken everything %s could from %s.\n",
                { character },
                character->getNameCapital(),
                character->getSubjectPronoun(),
                Formatter::cyan() + ToLower(container->getName()) + Formatter::reset());
            return;
        }
        Item * item = container->findContent(arguments[0].first, arguments[0].second);
        if (item == nullptr)
        {
            character->sendMsg("You don't see that item inside the container.\n");
            return;
        }
        // Check if the item has the flag kNoPick.
        if (HasFlag(item->model->modelFlags, ModelFlag::Static))
        {
            character->sendMsg("You can't pick up this kind of items!\n");
            return;
        }
        // Check if the player can carry the item.
        if (!character->canCarry(item))
        {
            character->sendMsg("You are not strong enough to carry that object.\n");
            return;
        }
        // Remove the item from the container.
        container->takeOut(item);
        // Add the item to the player's inventory.
        character->addInventoryItem(item);
        character->sendMsg(
            "You take out %s from %s.\n",
            Formatter::cyan() + ToLower(item->getName()) + Formatter::reset(),
            Formatter::cyan() + ToLower(container->getName()) + Formatter::reset());
        // Send the message inside the room.
        character->room->sendToAll(
            "%s takes out %s from %s.\n",
            { character },
            character->getNameCapital(),
            Formatter::cyan() + ToLower(item->getName()) + Formatter::reset(),
            Formatter::cyan() + ToLower(container->getName()) + Formatter::reset());
    }
    else
    {
        character->sendMsg("What do you want to pick up?\n");
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
        return;
    }
    if (arguments.size() > 1)
    {
        character->sendMsg("Too many arguments!\n");
        return;
    }
    if (ToLower(arguments[0].first) == "all")
    {
        // Handle output only if the player has really taken something.
        if (character->inventory.empty())
        {
            character->sendMsg("You have nothing to drop.\n");
            return;
        }
        auto untouchedList = character->inventory;
        for (auto iterator : untouchedList)
        {
            // Remove the item from the player's inventory.
            character->remInventoryItem(iterator);
            // Add the item to the room.
            character->room->addItem(iterator);
        }
        character->sendMsg("You dropped all.\n");
        // Send the message inside the room.
        character->room->sendToAll(
            "%s has dropped all %s items.\n",
            { character },
            character->getNameCapital(),
            character->getPossessivePronoun());
        return;
    }
    // Get the item.
    Item * item = character->findInventoryItem(arguments[0].first, arguments[0].second);
    // Check if the player has the item that he want to drop.
    if (item == nullptr)
    {
        character->sendMsg("You don't have that item.\n");
        return;
    }
    // Update the item iside the Database.
    character->remInventoryItem(item);
    character->room->addItem(item);
    // Active message.
    character->sendMsg(
        "You drop " + Formatter::cyan() + ToLower(item->getName()) + Formatter::reset() + ".\n");
    // Send the message inside the room.
    character->room->sendToAll(
        "%s has dropped %s.\n",
        { character },
        character->getNameCapital(),
        Formatter::cyan() + ToLower(item->getName()) + Formatter::reset());
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
        return;
    }
    // Get the item.
    Item * item = character->findInventoryItem(arguments[0].first, arguments[0].second);
    if (item == nullptr)
    {
        character->sendMsg("You don't have that item.\n");
        return;
    }
    Character * target = character->room->findCharacter(arguments[1].first, arguments[1].second, {
        character });
    if (target == nullptr)
    {
        character->sendMsg("You don't see that person.\n");
        return;
    }
    // Check if the target player can carry the item.
    if (!target->canCarry(item))
    {
        character->sendMsg(target->getNameCapital() + " can't carry anymore items.\n");
        return;
    }
    // Remove the item from the character inventory.
    character->remInventoryItem(item);
    // Add the item to the target inventory.
    target->addInventoryItem(item);
    // Check if the character is invisible.
    auto customName = (target->canSee(character)) ? "Someone" : character->getNameCapital();
    // GIVE Message.
    character->sendMsg(
        "You give %s to %s.\n",
        Formatter::cyan() + ToLower(item->getName()) + Formatter::reset(),
        target->getName());
    // RECEIVE Message.
    target->sendMsg(
        "%s gives you %s.\n\n",
        customName,
        Formatter::cyan() + ToLower(item->getName()) + Formatter::reset());
    // Check if the character is invisible.
    std::string broadcast;
    broadcast += character->getNameCapital() + " gives ";
    broadcast += Formatter::cyan() + ToLower(item->getName()) + Formatter::reset() + " to ";
    broadcast += target->getName() + ".\n";
    // Send the message inside the room.
    character->room->sendToAll(broadcast, { character, target });
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
    output += Formatter::yellow() + "#------------ Equipment -----------#\n" + Formatter::reset();
    // Equipment Slot : HEAD
    output += "    " + Formatter::yellow() + "Head" + Formatter::reset() + "       : ";
    output +=
        (head != nullptr) ?
            Formatter::cyan() + head->getNameCapital() : Formatter::gray() + "Nothing";
    output += Formatter::reset() + ".\n";
    // Equipment Slot : BACK
    output += "    " + Formatter::yellow() + "Back" + Formatter::reset() + "       : ";
    output +=
        (back != nullptr) ?
            Formatter::cyan() + back->getNameCapital() : Formatter::gray() + "Nothing";
    output += Formatter::reset() + ".\n";
    // Equipment Slot : TORSO
    output += "    " + Formatter::yellow() + "Torso" + Formatter::reset() + "      : ";
    output +=
        (torso != nullptr) ?
            Formatter::cyan() + torso->getNameCapital() : Formatter::gray() + "Nothing";
    output += Formatter::reset() + ".\n";
    // Equipment Slot : LEGS
    output += "    " + Formatter::yellow() + "Legs" + Formatter::reset() + "       : ";
    output +=
        (legs != nullptr) ?
            Formatter::cyan() + legs->getNameCapital() : Formatter::gray() + "Nothing";
    output += Formatter::reset() + ".\n";
    // Equipment Slot : FEET
    output += "    " + Formatter::yellow() + "Feet" + Formatter::reset() + "       : ";
    output +=
        (feet != nullptr) ?
            Formatter::cyan() + feet->getNameCapital() : Formatter::gray() + "Nothing";
    output += Formatter::reset() + ".\n";

    // Print what is wielding.
    if (right != nullptr)
    {
        if (HasFlag(right->model->modelFlags, ModelFlag::TwoHand))
        {
            output += "    " + Formatter::yellow() + "Both Hands" + Formatter::reset() + " : ";
        }
        else
        {
            output += "    " + Formatter::yellow() + "Right Hand" + Formatter::reset() + " : ";
        }
        output += Formatter::cyan() + right->getNameCapital();
    }
    else
    {
        output += "    " + Formatter::yellow() + "Right Hand" + Formatter::reset() + " : "
            + Formatter::gray() + "Nothing";
    }
    output += Formatter::reset() + ".\n";

    if (left != nullptr)
    {
        output += "    " + Formatter::yellow() + "Left Hand" + Formatter::reset() + "  : ";
        output += Formatter::cyan() + left->getNameCapital() + Formatter::reset() + ".\n";
    }
    output += Formatter::yellow() + "#----------------------------------#\n" + Formatter::reset();

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
        return;
    }
    if (arguments.size() > 1)
    {
        character->sendMsg("Too many arguments.\n");
        return;
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
        return;
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
        return;
    }
    // String where the error message will be put.
    std::string errMessage;
    // The destination slot.
    EquipmentSlot destinationSlot;
    // Check if the character can wield the item.
    if (!character->canWield(item, errMessage, destinationSlot))
    {
        character->sendMsg(errMessage);
        return;
    }
    // Set the item slot.
    item->setCurrentSlot(destinationSlot);
    // Remove the item from the inventory.
    character->remInventoryItem(item);
    // Equip the item.
    character->addEquipmentItem(item);
    // Show the proper message.
    std::string message = "You wield " + Formatter::cyan() + ToLower(item->getName())
        + Formatter::reset() + " ";
    if (HasFlag(item->model->modelFlags, ModelFlag::TwoHand))
    {
        message += "with both your hands.\n";
    }
    else
    {
        message += "with your " + item->getCurrentSlotName() + ".\n";
    }
    character->sendMsg(message);
    // Send the message inside the room.
    character->room->sendToAll(
        "%s wields %s in %s %s.\n",
        { character },
        character->getNameCapital(),
        Formatter::cyan() + ToLower(item->getName()) + Formatter::reset(),
        character->getPossessivePronoun(),
        ToLower(item->getCurrentSlotName()));
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
        return;
    }
    if (arguments.size() > 1)
    {
        character->sendMsg("Too many arguments.\n");
        return;
    }
    if (arguments[0].first == "all")
    {
        bool wearedSomething = false;
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
            // Notify that something has been weared.
            wearedSomething = true;
        }
        // Handle output only if the player has really weared something.
        if (!wearedSomething)
        {
            character->sendMsg("You had nothing to wear.\n");
            return;
        }
        character->sendMsg("You have weared everything you could.\n");
        // Send the message inside the room.
        character->room->sendToAll(
            "%s has weared all %s could.\n",
            { character },
            character->getNameCapital(),
            character->getSubjectPronoun());
        return;
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
        return;
    }
    std::string errMessage;
    if (!character->canWear(item, errMessage))
    {
        character->sendMsg(errMessage);
        return;
    }
    // Remove the item from the player's inventory.
    character->remInventoryItem(item);
    // Add the item to the equipment.
    character->addEquipmentItem(item);
    // Notify to character.
    character->sendMsg(
        "You wear " + Formatter::cyan() + ToLower(item->getName()) + Formatter::reset() + ".\n");
    // Send the message inside the room.
    character->room->sendToAll(
        "%s wears %s on %s %s.\n",
        { character },
        character->getNameCapital(),
        Formatter::cyan() + ToLower(item->getName()) + Formatter::reset(),
        character->getPossessivePronoun(),
        ToLower(item->getCurrentSlotName()));
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
        return;
    }
    if (arguments.size() > 1)
    {
        character->sendMsg("Too many arguments.\n");
        return;
    }
    if (arguments[0].first == "all")
    {
        // Handle output only if the player has really removed something.
        if (character->equipment.empty())
        {
            character->sendMsg("You have nothing to remove.\n");
            return;
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
        character->room->sendToAll(
            "%s has undressed all he could.\n",
            { character },
            character->getNameCapital());
        return;
    }
    // Get the item.
    Item * item = character->findEquipmentItem(arguments[0].first, arguments[0].second);
    // Check if the player has the item equipped.
    if (item == nullptr)
    {
        character->sendMsg("You don't have that item equipped.\n");
        return;
    }
    // Remove the item from the player's equipment.
    character->remEquipmentItem(item);
    // Add the item to the inventory.
    character->addInventoryItem(item);
    // Notify the character.
    character->sendMsg(
        "You remove %s.\n",
        Formatter::cyan() + ToLower(item->getName()) + Formatter::reset());
    // Send the message inside the room.
    character->room->sendToAll(
        "%s removes %s from %s %s.\n",
        { character },
        character->getNameCapital(),
        Formatter::cyan() + ToLower(item->getName()) + Formatter::reset(),
        character->getPossessivePronoun(),
        ToLower(item->getCurrentSlotName()));
}

void DoInventory(Character * character, std::istream & sArgs)
{
    NoMore(character, sArgs);
    if (character->inventory.empty())
    {
        character->sendMsg(
            Formatter::gray() + "    You are carrying anything.\n" + Formatter::reset());
    }
    Table table = Table("Inventory");
    table.addColumn("Item", StringAlign::Left);
    table.addColumn("Quantity", StringAlign::Right);
    table.addColumn("Weight", StringAlign::Right);
    // List all the items in inventory
    for (auto it : character->inventory)
    {
        table.addRow( { it->getNameCapital(), ToString(it->quantity), ToString(it->getWeight()) });
    }
    character->sendMsg(table.getTable());
    std::string carried = ToString(character->getCarryingWeight());
    std::string maximum = ToString(character->getMaxCarryingWeight());
    character->sendMsg(
        Formatter::yellow() + "\nTotal carrying weight: " + Formatter::reset() + carried + " of "
            + maximum + Formatter::reset() + " " + mud_measure + ".\n");
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
        return;
    }
    ItemContainer::Order order;
    if (BeginWith("name", ToLower(arguments[0].first)))
    {
        order = ItemContainer::Order::ByName;
    }
    else if (BeginWith("weight", ToLower(arguments[0].first)))
    {
        order = ItemContainer::Order::ByWeight;
    }
    else
    {
        character->sendMsg("You can organize by: name, weight,...\n");
        return;
    }

    if (arguments.size() == 1)
    {
        character->room->items.orderBy(order);
        character->sendMsg(
            "You have organized the room by %s.\n",
            ItemContainer::orderToString(order));
    }
    else if (arguments.size() == 2)
    {
        Item * container = character->findNearbyItem(arguments[1].first, arguments[1].second);
        if (container != nullptr)
        {
            if (container->content.empty())
            {
                character->sendMsg("You can't organize " + container->getName() + "\n");
            }
            else
            {
                container->content.orderBy(order);
                // Organize the target container.
                character->sendMsg(
                    "You have organized %s, by %s.\n",
                    container->getName(),
                    ItemContainer::orderToString(order));
            }
        }
        else if (BeginWith("inventory", arguments[1].first))
        {
            character->inventory.orderBy(order);
            // Organize the target container.
            character->sendMsg(
                "You have organized your inventory, by %s.\n",
                ItemContainer::orderToString(order));
        }
        else
        {
            character->sendMsg("What do you want to organize?\n");
        }
    }
    else
    {
        character->sendMsg("Too much arguments.\n");
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
        return;
    }
    // Check if the character want to open something in onother direction.
    Direction direction = Mud::instance().findDirection(arguments[0].first, false);
    if (direction != Direction::None)
    {
        // Check if the direction exists.
        std::shared_ptr<Exit> roomExit = character->room->findExit(direction);
        if (roomExit == nullptr)
        {
            character->sendMsg("There is nothing in that direction.\n");
            return;
        }
        Room * destination = roomExit->destination;
        if (destination == nullptr)
        {
            character->sendMsg("There is nothing in that direction.\n");
            return;
        }
        Item * door = destination->findDoor();
        if (door == nullptr)
        {
            character->sendMsg("There is no door in that direction.\n");
            return;
        }
        if (HasFlag(door->flags, ItemFlag::Locked))
        {
            character->sendMsg("You have first to unlock it.\n");
            return;
        }
        if (!HasFlag(door->flags, ItemFlag::Closed))
        {
            character->sendMsg("There door it's already opened.\n");
            return;
        }

        ClearFlag(door->flags, ItemFlag::Closed);

        // Display message.
        if (HasFlag(roomExit->flags, ExitFlag::Hidden))
        {
            character->sendMsg("You have opened a hidden door!\n");
            // Send the message inside the room.
            character->room->sendToAll(
                "%s opens a hidden door!\n",
                { character },
                character->getNameCapital());
        }
        else
        {
            character->sendMsg("You have opened the door.\n");
            // Send the message inside the room.
            character->room->sendToAll(
                "%s opens a door.\n",
                { character },
                character->getNameCapital());
        }
        for (auto it : destination->exits)
        {
            if (it->destination == nullptr)
            {
                continue;
            }
            if (it->destination == character->room)
            {
                continue;
            }
            if (HasFlag(it->flags, ExitFlag::Hidden))
            {
                // Show the action in the next room.
                it->destination->sendToAll(
                    "Someone opens a secret passage from the other side.\n",
                    { });
            }
            else
            {
                it->destination->sendToAll("Someone opens a door from the other side.\n", { });
            }
        }
    }
    else
    {
        Item * container = character->findNearbyItem(arguments[0].first, arguments[0].second);
        if (container == nullptr)
        {
            character->sendMsg("What do you want to open?\n");
            return;
        }
        if (HasFlag(container->flags, ItemFlag::Locked))
        {
            character->sendMsg("You have first to unlock it.\n");
            return;
        }
        if (!HasFlag(container->flags, ItemFlag::Closed))
        {
            character->sendMsg("It is already opened.\n");
            return;
        }
        ClearFlag(container->flags, ItemFlag::Closed);
        // Send the message to the character.
        character->sendMsg("You open %s.\n", container->getName());
        // Send the message inside the room.
        character->room->sendToAll(
            "%s opens %s.\n",
            { character },
            character->getNameCapital(),
            container->getName());
    }
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
        return;
    }
    // Check if the character want to open something in onother direction.
    Direction direction = Mud::instance().findDirection(arguments[0].first, false);
    if (direction != Direction::None)
    {
        // Check if the direction exists.
        std::shared_ptr<Exit> roomExit = character->room->findExit(direction);
        if (roomExit == nullptr)
        {
            character->sendMsg("There is nothing in that direction.\n");
            return;
        }
        Room * destination = roomExit->destination;
        if (destination == nullptr)
        {
            character->sendMsg("There is nothing in that direction.\n");
            return;
        }
        Item * door = destination->findDoor();
        if (door == nullptr)
        {
            character->sendMsg("There is no door in that direction.\n");
            return;
        }
        if (HasFlag(door->flags, ItemFlag::Closed))
        {
            character->sendMsg("There door it's already closed.\n");
            return;
        }
        if (destination->items.size() > 1)
        {
            character->sendMsg("There are items on the way, you can't close the door.\n");
            return;
        }
        if (destination->characters.size() >= 1)
        {
            character->sendMsg("There are someone on the way, you can't close the door.\n");
            return;
        }

        SetFlag(door->flags, ItemFlag::Closed);

        // Display message.
        if (HasFlag(roomExit->flags, ExitFlag::Hidden))
        {
            character->sendMsg("You have closed a hidden door!\n");
            // Send the message inside the room.
            character->room->sendToAll(
                "%s closes a hidden door!\n",
                { character },
                character->getNameCapital());
        }
        else
        {
            character->sendMsg("You have closed the door.\n");
            // Send the message inside the room.
            character->room->sendToAll(
                "%s closes a door.\n",
                { character },
                character->getNameCapital());
        }
        for (auto it : destination->exits)
        {
            if (it->destination == nullptr)
            {
                continue;
            }
            if (it->destination == character->room)
            {
                continue;
            }
            if (HasFlag(it->flags, ExitFlag::Hidden))
            {
                // Send the message inside the room.
                it->destination->sendToAll(
                    "Someone closes a secret passage from the other side.\n",
                    { });
            }
            else
            {
                // Send the message inside the room.
                it->destination->sendToAll("Someone closes a door from the other side.\n", { });
            }
        }
        return;
    }
    else
    {
        Item * container = character->findNearbyItem(arguments[0].first, arguments[0].second);
        if (container == nullptr)
        {
            character->sendMsg("What do you want to close?\n");
            return;
        }
        if (HasFlag(container->flags, ItemFlag::Closed))
        {
            character->sendMsg("It is already closed.\n");
            return;
        }
        if (!HasFlag(container->model->modelFlags, ModelFlag::CanClose))
        {
            character->sendMsg("It cannot be closed.\n");
            return;
        }
        SetFlag(container->flags, ItemFlag::Closed);
        // Send the message to the character.
        character->sendMsg("You close %s.\n", container->getName());
        // Send the message inside the room.
        character->room->sendToAll(
            "%s closes %s.\n",
            { character },
            character->getNameCapital(),
            container->getName());
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
        character->sendMsg("You don't see any container named '%s' here.\n", arguments[1].first);
        return;
    }
    if (!container->isAContainer())
    {
        character->sendMsg("%s is not a valid container.\n", container->getNameCapital());
        return;
    }
    if (container->model->getType() == ModelType::Corpse)
    {
        character->sendMsg("You don't really want to put something inside that body...\n");
        return;
    }
    if (HasFlag(container->flags, ItemFlag::Locked))
    {
        character->sendMsg("You have first to unlock %s.\n", container->getName());
        return;
    }
    if (HasFlag(container->flags, ItemFlag::Closed))
    {
        character->sendMsg("You have first to open %s.\n", container->getName());
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
        for (auto iterator : originalList)
        {
            if ((iterator != container) && container->canContain(iterator))
            {
                // Remove the item from the player's inventory.
                character->remInventoryItem(iterator);
                // Put the item inside the container.
                container->putInside(iterator);
            }
        }
        character->sendMsg("You put everything you could in %s.\n", container->getName());
        // Send the message inside the room.
        character->room->sendToAll(
            "%s puts everything %s could inside %s.\n",
            { character },
            character->getNameCapital(),
            character->getSubjectPronoun(),
            Formatter::cyan() + ToLower(container->getName()) + Formatter::reset());
        return;
    }
    Item * item = character->findInventoryItem(arguments[0].first, arguments[0].second);
    // Gather the item.
    if (item == nullptr)
    {
        character->sendMsg("You don't have any '%s'.\n", arguments[0].first);
        return;
    }
    // Try to put the item inside the container.
    if (!container->canContain(item))
    {
        character->sendMsg("%s can't contain any more items.\n", container->getNameCapital());
        return;
    }
    // Remove the item from the player's inventory.
    character->remInventoryItem(item);
    // Put the item inside the container.
    container->putInside(item);
    // Notify to player.
    character->sendMsg(
        "You put %s inside %s.\n",
        Formatter::cyan() + ToLower(item->getName()) + Formatter::reset(),
        Formatter::cyan() + ToLower(container->getName()) + Formatter::reset());
    // Send the message inside the room.
    character->room->sendToAll(
        "%s puts %s inside %s.\n",
        { character },
        character->getNameCapital(),
        Formatter::cyan() + ToLower(item->getName()) + Formatter::reset(),
        Formatter::cyan() + ToLower(container->getName()) + Formatter::reset());
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
        character->sendMsg("Drink what?\n");
        return;
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
                character->sendMsg("You don't see that container.\n");
                return;
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    // Execute every necessary checks.
    if (HasFlag(container->flags, ItemFlag::Locked))
    {
        character->sendMsg("You have first to unlock %s.\n", container->getName());
        return;
    }
    if (HasFlag(container->flags, ItemFlag::Closed))
    {
        character->sendMsg("You have first to open %s.\n", container->getName());
        return;
    }
    if (container->model->getType() != ModelType::LiquidContainer)
    {
        character->sendMsg("%s is not a container for liquids.\n", container->getNameCapital());
        return;
    }
    if (container->isEmpty())
    {
        character->sendMsg("%s is empty.\n", container->getNameCapital());
        return;
    }

    ///////////////////////////////////////////////////////////////////////////
    // Take out the liquid.
    Liquid * liquid = container->contentLiq.first;

    if (!container->pourOut(1))
    {
        character->sendMsg(
            "You were not able to drink some %s from %s.\n",
            liquid->getName(),
            container->getName());
        return;
    }

    character->sendMsg("You drink some %s from %s.\n", liquid->getName(), container->getName());
    // Send the message inside the room.
    character->room->sendToAll(
        "%s drinks some %s from %s.\n",
        { character },
        character->getNameCapital(),
        liquid->getName(),
        container->getName());
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
        return;
    }

    // Search the container.
    Item * container = character->findInventoryItem(arguments[0].first, arguments[0].second);
    if (container == nullptr)
    {
        container = character->findEquipmentItem(arguments[0].first, arguments[0].second);
        if (container == nullptr)
        {
            character->sendMsg("You don't have any '%s' with you.\n", arguments[0].first);
            return;
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
                character->sendMsg("You don't see any '%s'.\n", arguments[1].first);
                return;
            }
        }
    }

    if (HasFlag(source->flags, ItemFlag::Locked))
    {
        character->sendMsg("You have first to unlock %s.\n", source->getName());
        return;
    }
    if (HasFlag(source->flags, ItemFlag::Closed))
    {
        character->sendMsg("You have first to open %s.\n", source->getName());
        return;
    }
    if (HasFlag(container->flags, ItemFlag::Locked))
    {
        character->sendMsg("You have first to unlock %s.\n", container->getName());
        return;
    }
    if (HasFlag(container->flags, ItemFlag::Closed))
    {
        character->sendMsg("You have first to open %s.\n", container->getName());
        return;
    }

    // Check if the items are suitable source and container of liquids.
    if (container->model->getType() != ModelType::LiquidContainer)
    {
        character->sendMsg("%s is not a suitable container.\n", container->getNameCapital());
        return;
    }
    if (source->model->getType() != ModelType::LiquidContainer)
    {
        character->sendMsg("%s is not a suitable source of liquids.\n", source->getNameCapital());
        return;
    }

    LiquidContainerModel * liquidModelSource = source->model->toLiquidContainer();

    // Check if source is empty.
    if (source->isEmpty())
    {
        character->sendMsg("%s is empty.\n", source->getNameCapital());
        return;
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
            return;
        }
    }

    // Fill the container from the source.
    unsigned int atDisposal = source->contentLiq.second;
    unsigned int quantity = container->getFreeSpace();
    if (!HasFlag(liquidModelSource->liquidFlags, LiqContainerFlag::Endless))
    {
        if (atDisposal < quantity)
        {
            quantity = atDisposal;
        }
    }

    if (!source->pourOut(quantity))
    {
        character->sendMsg(
            "You failed to take out the liquid from a %s.\n",
            source->getNameCapital());
        return;
    }
    if (!container->pourIn(sourLiquid, quantity))
    {
        character->sendMsg("You failed to fill the container with the liquid.\n");
        return;
    }

    character->sendMsg(
        "You fill %s with %s from %s.\n",
        container->getName(),
        sourLiquid->getName(),
        source->getName());
    // Send the message inside the room.
    character->room->sendToAll(
        "%s fills %s with %s from %s.\n",
        { character },
        character->getNameCapital(),
        container->getName(),
        sourLiquid->getName(),
        source->getName());
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
        return;
    }

    // Search the container.
    Item * source = character->findInventoryItem(arguments[0].first, arguments[0].second);
    if (source == nullptr)
    {
        source = character->findEquipmentItem(arguments[0].first, arguments[0].second);
        if (source == nullptr)
        {
            character->sendMsg("You don't have any '%s' with you.\n", arguments[0].first);
            return;
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
                character->sendMsg("You don't see any '%s'.\n", arguments[1].first);
                return;
            }
        }
    }

    if (HasFlag(source->flags, ItemFlag::Locked))
    {
        character->sendMsg("You have first to unlock %s.\n", source->getName());
        return;
    }
    if (HasFlag(source->flags, ItemFlag::Closed))
    {
        character->sendMsg("You have first to open %s.\n", source->getName());
        return;
    }

    if (HasFlag(container->flags, ItemFlag::Locked))
    {
        character->sendMsg("You have first to unlock %s.\n", container->getName());
        return;
    }
    if (HasFlag(container->flags, ItemFlag::Closed))
    {
        character->sendMsg("You have first to open %s.\n", container->getName());
        return;
    }

    // Check if the items are suitable source and container of liquids.
    if (container->model->getType() != ModelType::LiquidContainer)
    {
        character->sendMsg("%s is not a suitable container.\n", container->getNameCapital());
        return;
    }
    if (source->model->getType() != ModelType::LiquidContainer)
    {
        character->sendMsg("%s is not a suitable source of liquids.\n", source->getNameCapital());
        return;
    }
    LiquidContainerModel * liquidModelSource = source->model->toLiquidContainer();

    // Check if source is empty.
    if (source->isEmpty())
    {
        character->sendMsg("%s is empty.\n", source->getNameCapital());
        return;
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
            return;
        }
    }

    // Fill the container from the source.
    unsigned int atDisposal = source->contentLiq.second;
    unsigned int quantity = container->getFreeSpace();

    if (!HasFlag(liquidModelSource->liquidFlags, LiqContainerFlag::Endless))
    {
        if (atDisposal < quantity)
        {
            quantity = atDisposal;
        }
    }
    if (!source->pourOut(quantity))
    {
        character->sendMsg("You failed to pour out the liquid from %s.\n", source->getName());
        return;
    }
    if (!container->pourIn(sourLiquid, quantity))
    {
        character->sendMsg("You failed to pour the liquid into %s.\n", source->getName());
        return;
    }
    character->sendMsg(
        "You pour %s of %s into %s.\n",
        sourLiquid->getName(),
        source->getName(),
        container->getName());
    // Send the message inside the room.
    character->room->sendToAll(
        "%s pour %s of %s into %s.\n",
        { character },
        character->getNameCapital(),
        sourLiquid->getName(),
        source->getName(),
        container->getName());
}

void DoDeposit(Character * character, std::istream & sArgs)
{
    // Stop any action the character is executing.
    StopAction(character);
    // Get the arguments of the command.
    ArgumentList args = ParseArgs(sArgs);
    // Check the number of arguments.
    if (args.size() != 2)
    {
        character->sendMsg("What do you want to deposit?\n");
        return;
    }
    auto item = character->findInventoryItem(args[0].first, args[0].second);
    auto building = character->room->findBuilding(args[1].first, args[1].second);
    // Check the item.
    if (item == nullptr)
    {
        character->sendMsg("You don't have that item.\n");
        return;
    }
    if (item->getType() != ModelType::Currency)
    {
        character->sendMsg("You can't deposit %s.\n", item->getName());
        return;
    }
    // Check the building.
    if (building == nullptr)
    {
        character->sendMsg("You don't see that building.\n");
        return;
    }
    if (building->getType() != ModelType::Shop)
    {
        character->sendMsg("You can't deposit %s in %s.\n", item->getName(), building->getName());
        return;
    }
    building->toShopItem()->balance += item->getPrice();
    item->removeFromMud();
    delete (item);
}

void DoSell(Character * character, std::istream & sArgs)
{
    // Stop any action the character is executing.
    StopAction(character);
    // Get the arguments of the command.
    ArgumentList args = ParseArgs(sArgs);
    // Check the number of arguments.
    if (args.size() != 2)
    {
        character->sendMsg("Sell what to whom?\n");
        return;
    }
    // Get the item and the target.
    auto item = character->findInventoryItem(args[0].first, args[0].second);
    // Check the item.
    if (item == nullptr)
    {
        character->sendMsg("You don't have that item.\n");
        return;
    }
    if (item->getType() == ModelType::Currency)
    {
        character->sendMsg("You can't sell %s.\n", item->getName());
        return;
    }
    // Get the target.
    auto target = character->room->findItem(args[1].first, args[1].second);
    if (target == nullptr)
    {
        character->sendMsg("You don't see '%s' here.\n", args[1].first);
        return;
    }
    if (target->getType() != ModelType::Shop)
    {
        character->sendMsg("%s is not a shop.\n", target->getNameCapital());
        return;
    }
    auto shop = target->toShopItem();
    std::string error;
    if (!shop->canUse(error))
    {
        character->sendMsg(error + "\n");
        return;
    }
    auto shopKeeper = shop->shopKeeper;
    if (!shop->canContain(item))
    {
        auto phrase = "The shop is full, come back another day.";
        shopKeeper->doCommand("say " + character->getName() + " " + phrase);
        return;
    }
    auto price = shop->evaluateSellPrice(item);
    if (shop->balance < price)
    {
        auto phrase = "I can't afford to buy your goods.";
        shopKeeper->doCommand("say " + character->getName() + " " + phrase);
        return;
    }
    auto currency = shopKeeper->faction->currency;
    // Give the coins to the character.
    auto coins = currency->generateCurrency(shopKeeper->getName(), price);
    if (coins.empty())
    {
        character->sendMsg("You failed to sell %s.\n", item->getName());
        return;
    }
    SQLiteDbms::instance().beginTransaction();
    // Remove the item from the player's inventory.
    character->remInventoryItem(item);
    // Put the item inside the container.
    shop->putInside(item);
    for (auto coin : coins)
    {
        character->addInventoryItem(coin);
    }
    SQLiteDbms::instance().endTransaction();
    shop->balance -= price;
    character->sendMsg("You sell %s to %s.\n", item->getName(), shop->getName());
}

void DoBuy(Character * character, std::istream & sArgs)
{
    // Stop any action the character is executing.
    StopAction(character);
    // Get the arguments of the command.
    ArgumentList args = ParseArgs(sArgs);
    // Check the number of arguments.
    if (args.size() != 2)
    {
        character->sendMsg("Buy what from which shop?\n");
        return;
    }
    // Get the target.
    auto target = character->room->findItem(args[1].first, args[1].second);
    if (target == nullptr)
    {
        character->sendMsg("You don't see '%s' here.\n", args[1].first);
        return;
    }
    if (target->getType() != ModelType::Shop)
    {
        character->sendMsg("%s is not a shop.\n", target->getNameCapital());
        return;
    }
    auto shop = target->toShopItem();
    std::string error;
    if (!shop->canUse(error))
    {
        character->sendMsg(error + "\n");
        return;
    }
    auto shopKeeper = shop->shopKeeper;
    // Get the item.
    auto item = shop->findContent(args[0].first, args[0].second);
    // Check the item.
    if (item == nullptr)
    {
        auto phrase = "There is no " + args[0].first + " on sale.\n";
        shopKeeper->doCommand("say " + character->getName() + " " + phrase);
        return;
    }
    if (!character->canCarry(item))
    {
        auto phrase = "It seems that you can't carry " + item->getName() + ".\n";
        shopKeeper->doCommand("say " + character->getName() + " " + phrase);
        return;
    }
    std::vector<Item *> availableCoins = character->findCoins();
    std::vector<Item *> changedCoins;
    std::vector<std::pair<Item *, unsigned int>> givenCoins;
    unsigned int providedValue = 0, requiredValue = shop->evaluateBuyPrice(item);
    for (auto coin : availableCoins)
    {
        unsigned int coinValue = coin->getPrice();
        unsigned int quantity = 0;
        for (; (quantity < coin->quantity) && (providedValue < requiredValue); ++quantity)
        {
            providedValue += coinValue;
        }
        givenCoins.push_back(std::make_pair(coin, quantity));
        if (providedValue >= requiredValue)
        {
            break;
        }
    }
    if (givenCoins.empty())
    {
        auto phrase = "You can't afford to buy " + item->getName() + ".\n";
        shopKeeper->doCommand("say " + character->getName() + " " + phrase);
        return;
    }
    if (providedValue > requiredValue)
    {
        auto change = providedValue - requiredValue;
        auto currency = shopKeeper->faction->currency;
        changedCoins = currency->generateCurrency(shopKeeper->getName(), change);
        if (changedCoins.empty())
        {
            auto phrase = "Sorry but I cannot sell " + item->getName() + " to you.\n";
            shopKeeper->doCommand("say " + character->getName() + " " + phrase);
            return;
        }
    }
    SQLiteDbms::instance().beginTransaction();
    for (auto coin : changedCoins)
    {
        character->addInventoryItem(coin);
    }
    for (auto coin : givenCoins)
    {
        if (coin.second == coin.first->quantity)
        {
            coin.first->removeFromMud();
            coin.first->removeOnDB();
            delete (coin.first);
        }
        else
        {
            coin.first->quantity -= coin.second;
            coin.first->updateOnDB();
        }
    }
    shop->takeOut(item);
    character->addInventoryItem(item);
    SQLiteDbms::instance().endTransaction();
    shop->balance += requiredValue;
    character->sendMsg("You buy %s from %s.\n", item->getName(), shop->getName());
}

void DoBalance(Character * character, std::istream & sArgs)
{
    // Get the arguments of the command.
    ArgumentList args = ParseArgs(sArgs);
    // Check the number of arguments.
    if (!args.empty())
    {
        character->sendMsg("You have provided unecessary arguments.\n");
        return;
    }
    unsigned int balance = 0;
    for (auto coin : character->findCoins())
    {
        balance += coin->quantity * coin->getPrice();
    }
    character->sendMsg("Your balance is: %s.\n", ToString(balance));
}
