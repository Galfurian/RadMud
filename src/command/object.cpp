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

#include "object.hpp"

#include "../mud.hpp"
#include "../item/shopItem.hpp"
#include "../model/submodel/currencyModel.hpp"
#include "../model/submodel/liquidContainerModel.hpp"

void LoadObjectCommands()
{
    Command command;
    command.gods = false;
    {
        command.name = "take";
        command.help = "Take something from the ground or from a container.";
        command.args = "(item) [(container)]";
        command.hndl = DoTake;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "drop";
        command.help = "Drop an object.";
        command.args = "(item)";
        command.hndl = DoDrop;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "give";
        command.help = "Give an object to someone.";
        command.args = "(item) (someone)";
        command.hndl = DoGive;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "equipments";
        command.help = "List all the items you are wearing.";
        command.args = "";
        command.hndl = DoEquipments;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "wield";
        command.help = "Wield a weapon, a shield or maybe a tool.";
        command.args = "(item)";
        command.hndl = DoWield;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "wear";
        command.help = "Puts on a piece of equipment.";
        command.args = "(item)";
        command.hndl = DoWear;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "remove";
        command.help = "Remove a weared or wielded item.";
        command.args = "(item)";
        command.hndl = DoRemove;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "inventory";
        command.help = "Show character's inventory.";
        command.args = "";
        command.hndl = DoInventory;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "organize";
        command.help = "Order the desired container or if no target is passed, the room.";
        command.args = "(name|weight) [(target)]";
        command.hndl = DoOrganize;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "open";
        command.help = "Open a door in a given direction.";
        command.args = "(item)|(direction)";
        command.hndl = DoOpen;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "close";
        command.help = "Close a door in a given direction.";
        command.args = "(item)|(direction)";
        command.hndl = DoClose;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "put";
        command.help = "Put something inside a container.";
        command.args = "(something) (container)";
        command.hndl = DoPut;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "drink";
        command.help = "Drink from a container of liquids.";
        command.args = "(container)";
        command.hndl = DoDrink;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "fill";
        command.help = "Fill a container of liquids from a source of liquid.";
        command.args = "(container) (source)";
        command.hndl = DoFill;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "pour";
        command.help = "Pour the content of the container into another one or on the ground.";
        command.args = "(container) [container]";
        command.hndl = DoPour;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "deposit";
        command.help = "Deposit a coin inside a shop.";
        command.args = "(item) (shop)";
        command.hndl = DoDeposit;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "sell";
        command.help = "Sell an item to a shop keeper.";
        command.args = "(item) (shop)";
        command.hndl = DoSell;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "buy";
        command.help = "Allows to buy an item from a shop.";
        command.args = "(item) (shop)";
        command.hndl = DoBuy;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "balance";
        command.help = "Shows the character's balance.";
        command.args = "";
        command.hndl = DoBalance;
        Mud::instance().addCommand(command);
    }
}

void DoTake(Character * character, ArgumentHandler & args)
{
    // Stop any action the character is executing.
    StopAction(character);
    // Try to take the item inside the room.
    if (args.size() == 1)
    {
        if (ToLower(args[0].getContent()) == "all")
        {
            if (character->room->items.empty())
            {
                character->sendMsg("There is nothing to pick up.\n");
                return;
            }
            // Make a temporary copy of the character's inventory.
            auto originalList = character->room->items;
            // Start a transaction.
            SQLiteDbms::instance().beginTransaction();
            // Used to determine if the character has picked up something.
            auto pickedUpSomething = false;
            for (auto iterator : originalList)
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
                if (!character->canCarry(iterator, iterator->quantity))
                {
                    continue;
                }
                // Remove the item from the room.
                character->room->removeItem(iterator);
                // Add the item to the player's inventory.
                character->addInventoryItem(iterator);
                // Set that he has picked up something.
                pickedUpSomething = true;
            }
            // Conclude the transaction.
            SQLiteDbms::instance().endTransaction();
            // Handle output only if the player has really taken something.
            if (pickedUpSomething)
            {
                // Send the messages.
                character->sendMsg("You've picked up everything you could.\n");
                character->room->sendToAll(
                    "%s has picked up everything %s could.\n",
                    {character},
                    character->getNameCapital(),
                    character->getSubjectPronoun());
            }
            else
            {
                character->sendMsg("You've picked up nothing.\n");
            }
        }
        else
        {
            auto item = character->room->findItem(args[0].getContent(), args[0].getIndex());
            // If the item is null.
            if (item == nullptr)
            {
                // Try to check if the character is a mobile, since mobiles can take
                //  items by providing the specific vnum.
                if (character->isMobile() && IsNumber(args[0].getContent()))
                {
                    for (auto it : character->room->items)
                    {
                        if (it->vnum == ToNumber<int>(args[0].getContent()))
                        {
                            item = it;
                        }
                    }
                }
                // Check if the item is still null.
                if (item == nullptr)
                {
                    character->sendMsg("You don't see that item inside the room.\n");
                    return;
                }
            }
            // Check if the item has the flag Static.
            if (HasFlag(item->model->modelFlags, ModelFlag::Static))
            {
                character->sendMsg("You can't pick up %s!\n", item->getName(true));
                return;
            }
            if (HasFlag(item->flags, ItemFlag::Built))
            {
                character->sendMsg("You can't pick up something which is built!\n");
                return;
            }
            // Set the quantity.
            auto quantity = args[0].getMultiplier();
            if (item->quantity < quantity)
            {
                quantity = item->quantity;
            }
            // Check if the player can carry the item.
            if (!character->canCarry(item, quantity))
            {
                character->sendMsg("You can't carry %s!\n", item->getName(true));
                return;
            }
            // Start a transaction.
            SQLiteDbms::instance().beginTransaction();
            if (item->quantity <= quantity)
            {
                // Remove the item from the room.
                character->room->removeItem(item);
                // Add the item to the player's inventory.
                character->addInventoryItem(item);
                // Send the messages.
                character->sendMsg(
                    "You take %s.\n",
                    item->getName(true));
                character->room->sendToAll(
                    "%s has picked up %s.\n",
                    {character},
                    character->getNameCapital(),
                    item->getName(true));
            }
            else
            {
                // Remove a stack from item.
                auto newStack = item->removeFromStack(character, quantity);
                if (newStack == nullptr)
                {
                    character->sendMsg("You failed to drop %s.\n", item->getName(true));
                    // Rollback the transaction.
                    SQLiteDbms::instance().rollbackTransection();
                    return;
                }
                // Add the item to the player's inventory.
                character->addInventoryItem(newStack);
                // Send the messages.
                character->sendMsg(
                    "You take a part of %s.\n",
                    item->getName(true));
                character->room->sendToAll(
                    "%s has picked up part of %s.\n",
                    {character},
                    character->getNameCapital(),
                    item->getName(true));
            }
            // Conclude the transaction.
            SQLiteDbms::instance().endTransaction();
        }
    }
    else if (args.size() == 2)
    {
        auto container = character->findNearbyItem(args[1].getContent(), args[1].getIndex());
        if (container == nullptr)
        {
            character->sendMsg("You don't see that container.\n");
            return;
        }
        if (HasFlag(container->flags, ItemFlag::Locked))
        {
            character->sendMsg("You have first to unlock %s first.\n", container->getName(true));
            return;
        }
        if (HasFlag(container->flags, ItemFlag::Closed))
        {
            character->sendMsg("You have first to open %s first.\n", container->getName(true));
            return;
        }
        if (ToLower(args[0].getContent()) == "all")
        {
            if (container->content.empty())
            {
                character->sendMsg("There is nothing inside %s.\n", container->getName(true));
                return;
            }
            // Make a temporary copy of the character's inventory.
            auto originalList = character->room->items;
            // Start a transaction.
            SQLiteDbms::instance().beginTransaction();
            // Used to determine if the character has picked up something.
            auto takenSomething = false;
            for (auto iterator : originalList)
            {
                // Check if the item is static.
                if (HasFlag(iterator->model->modelFlags, ModelFlag::Static))
                {
                    continue;
                }
                // Check if the player can carry the item.
                if (!character->canCarry(iterator, iterator->quantity))
                {
                    continue;
                }
                // Remove the item from the container.
                container->takeOut(iterator);
                // Add the item to the player's inventory.
                character->addInventoryItem(iterator);
                // Set that he has picked up something.
                takenSomething = true;
            }
            // Conclude the transaction.
            SQLiteDbms::instance().endTransaction();
            // Handle output only if the player has really taken something.
            if (takenSomething)
            {
                // Send the messages.
                character->sendMsg(
                    "You've taken everything you could from %s.\n",
                    container->getName(true));
                character->room->sendToAll(
                    "%s has taken everything %s could from %s.\n",
                    {character},
                    character->getNameCapital(),
                    character->getSubjectPronoun(),
                    container->getName(true));
            }
            else
            {
                character->sendMsg("You've taken nothing from %s.\n", container->getName(true));
            }
        }
        else
        {
            auto item = container->findContent(args[0].getContent(), args[0].getIndex());
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
            // Set the quantity.
            auto quantity = args[0].getMultiplier();
            if (item->quantity < quantity)
            {
                quantity = item->quantity;
            }
            // Check if the player can carry the item.
            if (!character->canCarry(item, quantity))
            {
                character->sendMsg("You are not strong enough to carry that object.\n");
                return;
            }
            // Start a transaction.
            SQLiteDbms::instance().beginTransaction();
            if (item->quantity <= quantity)
            {
                // Remove the item from the container.
                container->takeOut(item);
                // Add the item to the player's inventory.
                character->addInventoryItem(item);
                // Send the messages.
                character->sendMsg(
                    "You take out %s from %s.\n",
                    item->getName(true),
                    container->getName(true));
                character->room->sendToAll(
                    "%s takes out %s from %s.\n",
                    {character},
                    character->getNameCapital(),
                    item->getName(true),
                    container->getName(true));
            }
            else
            {
                // Remove a stack from item.
                auto newStack = item->removeFromStack(character, quantity);
                if (newStack == nullptr)
                {
                    character->sendMsg("You failed to take part of %s.\n", item->getName(true));
                    // Rollback the transaction.
                    SQLiteDbms::instance().rollbackTransection();
                    return;
                }
                // Add the item to the player's inventory.
                character->addInventoryItem(newStack);
                // Send the messages.
                character->sendMsg(
                    "You take out part of %s from %s.\n",
                    item->getName(true),
                    container->getName(true));
                character->room->sendToAll(
                    "%s takes out %s from %s.\n",
                    {character},
                    character->getNameCapital(),
                    item->getName(true),
                    container->getName(true));
            }
            // Conclude the transaction.
            SQLiteDbms::instance().endTransaction();
        }
    }
    else
    {
        character->sendMsg("What do you want to pick up?\n");
    }
}

void DoDrop(Character * character, ArgumentHandler & args)
{
    // Stop any action the character is executing.
    StopAction(character);
    // Check the number of arguments.
    if (args.size() == 0)
    {
        character->sendMsg("Drop what?\n");
        return;
    }
    if (args.size() > 1)
    {
        character->sendMsg("Too many arguments!\n");
        return;
    }
    if (character->inventory.empty())
    {
        character->sendMsg("You have nothing to drop.\n");
        return;
    }
    if (ToLower(args[0].getContent()) == "all")
    {
        // Make a temporary copy of the character's inventory.
        auto originalList = character->inventory;
        // Start a transaction.
        SQLiteDbms::instance().beginTransaction();
        for (auto iterator : originalList)
        {
            // Remove the item from the player's inventory.
            character->remInventoryItem(iterator);
            // Add the item to the room.
            character->room->addItem(iterator);
        }
        // Conclude the transaction.
        SQLiteDbms::instance().endTransaction();
        // Send the messages.
        character->sendMsg("You dropped all.\n");
        character->room->sendToAll(
            "%s has dropped all %s items.\n",
            {character},
            character->getNameCapital(),
            character->getPossessivePronoun());
    }
    else
    {
        // Get the item.
        auto item = character->findInventoryItem(args[0].getContent(), args[0].getIndex());
        // Check if the player has the item that he want to drop.
        if (item == nullptr)
        {
            character->sendMsg("You don't have that item.\n");
            return;
        }
        // Set the quantity.
        auto quantity = args[0].getMultiplier();
        if (item->quantity < quantity)
        {
            quantity = item->quantity;
        }
        // Start a transaction.
        SQLiteDbms::instance().beginTransaction();
        if (item->quantity <= quantity)
        {
            // Remove the item from the player's inventory.
            character->remInventoryItem(item);
            // Put the item inside the room.
            character->room->addItem(item);
            // Send the messages.
            character->sendMsg(
                "You drop %s.\n",
                item->getName(true));
            character->room->sendToAll(
                "%s has dropped %s.\n",
                {character},
                character->getNameCapital(),
                item->getName(true));
        }
        else
        {
            // Remove from the stack.
            auto newStack = item->removeFromStack(character, quantity);
            if (newStack == nullptr)
            {
                character->sendMsg("You failed to drop %s.\n", item->getName(true));
                // Rollback the transaction.
                SQLiteDbms::instance().rollbackTransection();
                return;
            }
            // Put the item inside the room.
            character->room->addItem(newStack);
            // Send the messages.
            character->sendMsg(
                "You drop part of %s.\n",
                item->getName(true));
            character->room->sendToAll(
                "%s has dropped %s.\n",
                {character},
                character->getNameCapital(),
                item->getName(true));
        }
        // Conclude the transaction.
        SQLiteDbms::instance().endTransaction();
    }
}

void DoGive(Character * character, ArgumentHandler & args)
{
    // Stop any action the character is executing.
    StopAction(character);
    // Check the number of arguments.
    if (args.size() != 2)
    {
        character->sendMsg("Give what to whom?\n");
        return;
    }
    // Get the item.
    auto item = character->findInventoryItem(args[0].getContent(), args[0].getIndex());
    if (item == nullptr)
    {
        character->sendMsg("You don't have that item.\n");
        return;
    }
    auto target = character->room->findCharacter(args[1].getContent(), args[1].getIndex(), {
        character});
    if (target == nullptr)
    {
        character->sendMsg("You don't see that person.\n");
        return;
    }
    // Check if the target player can carry the item.
    if (!target->canCarry(item, item->quantity))
    {
        character->sendMsg(target->getNameCapital() + " can't carry anymore items.\n");
        return;
    }
    // Set the quantity.
    auto quantity = args[0].getMultiplier();
    if (item->quantity < quantity)
    {
        quantity = item->quantity;
    }
    // Start a transaction.
    SQLiteDbms::instance().beginTransaction();
    if (item->quantity <= quantity)
    {
        // Remove the item from the character inventory.
        character->remInventoryItem(item);
        // Add the item to the target inventory.
        target->addInventoryItem(item);
        // Send all the messages.
        character->sendMsg(
            "You give %s to %s.\n",
            item->getName(true),
            target->getName());
        target->sendMsg(
            "%s gives you %s.\n\n",
            character->getNameCapital(),
            item->getName(true));
        character->room->sendToAll(
            "%s gives %s to %s.\n",
            {character, target},
            character->getNameCapital(),
            item->getName(true),
            target->getName());
    }
    else
    {
        // Remove from the stack.
        auto newStack = item->removeFromStack(character, quantity);
        if (newStack == nullptr)
        {
            character->sendMsg(
                "You failed to give part of %s to %s.\n",
                item->getName(true),
                target->getName());
            // Rollback the transaction.
            SQLiteDbms::instance().rollbackTransection();
            return;
        }
        // Add the stack to the target inventory.
        target->addInventoryItem(newStack);
        // Send all the messages.
        character->sendMsg(
            "You give part of %s to %s.\n",
            item->getName(true),
            target->getName());
        target->sendMsg(
            "%s gives you %s.\n\n",
            character->getNameCapital(),
            item->getName(true));
        character->room->sendToAll(
            "%s gives %s to %s.\n",
            {character, target},
            character->getNameCapital(),
            item->getName(true),
            target->getName());
    }
    // Conclude the transaction.
    SQLiteDbms::instance().endTransaction();
}

void DoEquipments(Character * character, ArgumentHandler & /*args*/)
{
    auto head = character->findEquipmentSlotItem(EquipmentSlot::Head);
    auto back = character->findEquipmentSlotItem(EquipmentSlot::Back);
    auto torso = character->findEquipmentSlotItem(EquipmentSlot::Torso);
    auto legs = character->findEquipmentSlotItem(EquipmentSlot::Legs);
    auto feet = character->findEquipmentSlotItem(EquipmentSlot::Feet);
    auto right = character->findEquipmentSlotItem(EquipmentSlot::RightHand);
    auto left = character->findEquipmentSlotItem(EquipmentSlot::LeftHand);
    std::string output;
    // Print what is wearing.
    output += Formatter::yellow() + "#------------ Equipment -----------#\n" + Formatter::reset();
    // Equipment Slot : HEAD
    output += "\tHead       : " + ((head != nullptr) ? head->getNameCapital(true) : "Nothing") + "\n";
    // Equipment Slot : BACK
    output += "\tBack       : " + ((back != nullptr) ? back->getNameCapital(true) : "Nothing") + "\n";
    // Equipment Slot : TORSO
    output += "\tTorso      : " + ((torso != nullptr) ? torso->getNameCapital(true) : "Nothing") + "\n";
    // Equipment Slot : LEGS
    output += "\tLegs       : " + ((legs != nullptr) ? legs->getNameCapital(true) : "Nothing") + "\n";
    // Equipment Slot : FEET
    output += "\tFeet       : " + ((feet != nullptr) ? feet->getNameCapital(true) : "Nothing") + "\n";
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
        output += right->getNameCapital(true) + "\n";
    }
    else
    {
        output += "\tRight Hand : Nothing\n";
    }

    if (left != nullptr)
    {
        output += "\tLeft Hand  : " + left->getNameCapital(true) + "\n";
    }
    output += Formatter::yellow() + "#----------------------------------#\n" + Formatter::reset();

    character->sendMsg(output);
}

void DoWield(Character * character, ArgumentHandler & args)
{
    // Stop any action the character is executing.
    StopAction(character);
    // Check the arguments.
    if (args.empty())
    {
        character->sendMsg("Wield what?\n");
        return;
    }
    if (args.size() > 1)
    {
        character->sendMsg("Too many arguments.\n");
        return;
    }
    // Get the item.
    auto item = character->findInventoryItem(args[0].getContent(), args[0].getIndex());
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
    auto object = item->getName(true);
    auto where = "with your " + item->getCurrentSlotName();
    auto whereOthers = character->getPossessivePronoun() + " " + item->getCurrentSlotName();
    if (HasFlag(item->model->modelFlags, ModelFlag::TwoHand))
    {
        where = "both your hands";
        whereOthers = "both " + character->getPossessivePronoun() + " hands";
    }
    character->sendMsg("You wield %s with %s.\n", object, where);
    // Send the message inside the room.
    character->room->sendToAll(
        "%s wields %s with %s.\n",
        {character},
        character->getNameCapital(),
        item->getName(true),
        whereOthers);
}

void DoWear(Character * character, ArgumentHandler & args)
{
    // Stop any action the character is executing.
    StopAction(character);
    // Check the arguments.
    if (args.size() == 0)
    {
        character->sendMsg("Wear what?\n");
        return;
    }
    if (args.size() > 1)
    {
        character->sendMsg("Too many arguments.\n");
        return;
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
            {character},
            character->getNameCapital(),
            character->getSubjectPronoun());
        return;
    }
    auto item = character->findInventoryItem(args[0].getContent(), args[0].getIndex());
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
        "You wear %s on your %s.\n",
        item->getName(true),
        ToLower(item->getCurrentSlotName()));
    // Send the message inside the room.
    character->room->sendToAll(
        "%s wears %s on %s %s.\n",
        {character},
        character->getNameCapital(),
        item->getName(true),
        character->getPossessivePronoun(),
        ToLower(item->getCurrentSlotName()));
}

void DoRemove(Character * character, ArgumentHandler & args)
{
    // Stop any action the character is executing.
    StopAction(character);
    // Check the arguments.
    if (args.size() == 0)
    {
        character->sendMsg("Remove what?\n");
        return;
    }
    if (args.size() > 1)
    {
        character->sendMsg("Too many arguments.\n");
        return;
    }
    if (args[0].getContent() == "all")
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
            {character},
            character->getNameCapital());
        return;
    }
    // Get the item.
    auto item = character->findEquipmentItem(args[0].getContent(), args[0].getIndex());
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
        "You remove %s from your %s.\n",
        item->getName(true),
        ToLower(item->getCurrentSlotName()));
    // Send the message inside the room.
    character->room->sendToAll(
        "%s removes %s from %s %s.\n",
        {character},
        character->getNameCapital(),
        item->getName(true),
        character->getPossessivePronoun(),
        ToLower(item->getCurrentSlotName()));
}

void DoInventory(Character * character, ArgumentHandler & /*args*/)
{
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
        table.addRow({it->getNameCapital(), ToString(it->quantity), ToString(it->getWeight(true))});
    }
    character->sendMsg(table.getTable());
    character->sendMsg(
        "\n%sTotal carrying weight%s: %s of %s %s.\n",
        Formatter::yellow(),
        Formatter::reset(),
        ToString(character->getCarryingWeight()),
        ToString(character->getMaxCarryingWeight()),
        Mud::instance().getWeightMeasure());
}

void DoOrganize(Character * character, ArgumentHandler & args)
{
    // Stop any action the character is executing.
    StopAction(character);
    // Check the number of arguments.
    if (args.empty())
    {
        character->sendMsg("Organize what?\n");
        return;
    }
    auto order = ItemContainer::Order::ByName;
    if (BeginWith("name", ToLower(args[0].getContent())))
    {
        order = ItemContainer::Order::ByName;
    }
    else if (BeginWith("weight", ToLower(args[0].getContent())))
    {
        order = ItemContainer::Order::ByWeight;
    }
    else if (BeginWith("price", ToLower(args[0].getContent())))
    {
        order = ItemContainer::Order::ByPrice;
    }
    auto name = ItemContainer::orderToString(order);
    if (args.size() == 1)
    {
        character->room->items.orderBy(order);
        character->sendMsg("You have organized the room by %s.\n", name);
    }
    else if (args.size() == 2)
    {
        auto container = character->findNearbyItem(args[1].getContent(), args[1].getIndex());
        if (container != nullptr)
        {
            if (container->content.empty())
            {
                character->sendMsg("You can't organize " + container->getName(true) + "\n");
            }
            else
            {
                container->content.orderBy(order);
                // Organize the target container.
                character->sendMsg("You have organized %s, by %s.\n", container->getName(true), name);
            }
        }
        else if (BeginWith("inventory", args[1].getContent()))
        {
            character->inventory.orderBy(order);
            // Organize the target container.
            character->sendMsg("You have organized your inventory, by %s.\n", name);
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

void DoOpen(Character * character, ArgumentHandler & args)
{
    // Stop any action the character is executing.
    StopAction(character);
    // Check the arguments.
    if (args.size() != 1)
    {
        character->sendMsg("What do you want to open?\n");
        return;
    }
    // Check if the character want to open something in onother direction.
    auto direction = Mud::instance().findDirection(args[0].getContent(), false);
    if (direction != Direction::None)
    {
        // Check if the direction exists.
        auto roomExit = character->room->findExit(direction);
        if (roomExit == nullptr)
        {
            character->sendMsg("There is nothing in that direction.\n");
            return;
        }
        auto destination = roomExit->destination;
        if (destination == nullptr)
        {
            character->sendMsg("There is nothing in that direction.\n");
            return;
        }
        auto door = destination->findDoor();
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

        ClearFlag(&door->flags, ItemFlag::Closed);

        // Display message.
        if (HasFlag(roomExit->flags, ExitFlag::Hidden))
        {
            character->sendMsg("You have opened a hidden door!\n");
            // Send the message inside the room.
            character->room->sendToAll(
                "%s opens a hidden door!\n",
                {character},
                character->getNameCapital());
        }
        else
        {
            character->sendMsg("You have opened the door.\n");
            // Send the message inside the room.
            character->room->sendToAll(
                "%s opens a door.\n",
                {character},
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
                    {});
            }
            else
            {
                it->destination->sendToAll("Someone opens a door from the other side.\n", {});
            }
        }
    }
    else
    {
        auto container = character->findNearbyItem(args[0].getContent(), args[0].getIndex());
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
        ClearFlag(&container->flags, ItemFlag::Closed);
        // Send the message to the character.
        character->sendMsg("You open %s.\n", container->getName(true));
        // Send the message inside the room.
        character->room->sendToAll(
            "%s opens %s.\n",
            {character},
            character->getNameCapital(),
            container->getName(true));
    }
}

void DoClose(Character * character, ArgumentHandler & args)
{
    // Stop any action the character is executing.
    StopAction(character);
    // Check the arguments.
    if (args.size() != 1)
    {
        character->sendMsg("What do you want to close?\n");
        return;
    }
    // Check if the character want to open something in onother direction.
    auto direction = Mud::instance().findDirection(args[0].getContent(), false);
    if (direction != Direction::None)
    {
        // Check if the direction exists.
        auto roomExit = character->room->findExit(direction);
        if (roomExit == nullptr)
        {
            character->sendMsg("There is nothing in that direction.\n");
            return;
        }
        auto destination = roomExit->destination;
        if (destination == nullptr)
        {
            character->sendMsg("There is nothing in that direction.\n");
            return;
        }
        auto door = destination->findDoor();
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
        SetFlag(&door->flags, ItemFlag::Closed);
        // Display message.
        if (HasFlag(roomExit->flags, ExitFlag::Hidden))
        {
            character->sendMsg("You have closed a hidden door!\n");
            // Send the message inside the room.
            character->room->sendToAll(
                "%s closes a hidden door!\n",
                {character},
                character->getNameCapital());
        }
        else
        {
            character->sendMsg("You have closed the door.\n");
            // Send the message inside the room.
            character->room->sendToAll(
                "%s closes a door.\n",
                {character},
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
                    {});
            }
            else
            {
                // Send the message inside the room.
                it->destination->sendToAll("Someone closes a door from the other side.\n", {});
            }
        }
        return;
    }
    else
    {
        auto container = character->findNearbyItem(args[0].getContent(), args[0].getIndex());
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
        SetFlag(&container->flags, ItemFlag::Closed);
        // Send the message to the character.
        character->sendMsg("You close %s.\n", container->getName(true));
        // Send the message inside the room.
        character->room->sendToAll(
            "%s closes %s.\n",
            {character},
            character->getNameCapital(),
            container->getName(true));
    }
    return;
}

void DoPut(Character * character, ArgumentHandler & args)
{
    // Stop any action the character is executing.
    StopAction(character);
    if (args.size() != 2)
    {
        character->sendMsg("Put what inside what?\n");
        return;
    }
    auto container = character->findNearbyItem(args[1].getContent(), args[1].getIndex());
    if (container == nullptr)
    {
        character->sendMsg("You don't see any container named '%s' here.\n", args[1].getContent());
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
        character->sendMsg("You have first to unlock %s.\n", container->getName(true));
        return;
    }
    if (HasFlag(container->flags, ItemFlag::Closed))
    {
        character->sendMsg("You have first to open %s.\n", container->getName(true));
        return;
    }
    if (character->inventory.empty())
    {
        character->sendMsg("You don't have anything to put in a container.");
        return;
    }
    // Check if the player wants to put all in the container.
    if (args[0].getContent() == "all")
    {
        // Make a temporary copy of the character's inventory.
        auto originalList = character->inventory;
        // Start a transaction.
        SQLiteDbms::instance().beginTransaction();
        // Move all the items inside the container.
        for (auto iterator : originalList)
        {
            // Skip the item if it cannot be contained inside the destination.
            if (!container->canContain(iterator, iterator->quantity)) continue;
            // Skip the item if it is the destination.
            if (iterator->vnum == container->vnum) continue;
            // Remove the item from the player's inventory.
            character->remInventoryItem(iterator);
            // Put the item inside the container.
            container->putInside(iterator);
        }
        // Conclude the transaction.
        SQLiteDbms::instance().endTransaction();
        // Send the messages.
        character->sendMsg("You put everything you could in %s.\n", container->getName(true));
        character->room->sendToAll(
            "%s puts everything %s could inside %s.\n",
            {character},
            character->getNameCapital(),
            character->getSubjectPronoun(),
            container->getName(true));
        return;
    }
    else
    {
        // Find the specific item inside the inventory.
        auto item = character->findInventoryItem(args[0].getContent(), args[0].getIndex());
        if (item == nullptr)
        {
            character->sendMsg("You don't have any '%s'.\n", args[0].getContent());
            return;
        }
        // Set the quantity.
        auto quantity = args[0].getMultiplier();
        if (item->quantity < quantity)
        {
            quantity = item->quantity;
        }
        // Check if the item can be contained inside the destination.
        if (!container->canContain(item, quantity))
        {
            character->sendMsg("%s can't contain any more items.\n", container->getNameCapital());
            return;
        }
        // Start a transaction.
        SQLiteDbms::instance().beginTransaction();
        if (item->quantity <= quantity)
        {
            // Remove the item from the player's inventory.
            character->remInventoryItem(item);
            // Put the item inside the container.
            container->putInside(item);
            // Send the messages.
            character->sendMsg(
                "You put %s inside %s.\n",
                item->getName(true),
                container->getName(true));
            character->room->sendToAll(
                "%s puts %s inside %s.\n",
                {character},
                character->getNameCapital(),
                item->getName(true),
                container->getName(true));
        }
        else
        {
            // Remove from the stack.
            auto newStack = item->removeFromStack(character, quantity);
            if (newStack == nullptr)
            {
                character->sendMsg(
                    "You failed to put part of %s inside %s.\n",
                    item->getName(true),
                    container->getName(true));
                // Rollback the transaction.
                SQLiteDbms::instance().rollbackTransection();
                return;
            }
            // Put the stack inside the container.
            container->putInside(newStack);
            // Send the messages.
            character->sendMsg(
                "You put part of %s inside %s.\n",
                item->getName(true),
                container->getName(true));
            character->room->sendToAll(
                "%s puts %s inside %s.\n",
                {character},
                character->getNameCapital(),
                item->getName(true),
                container->getName(true));
        }
        // Conclude the transaction.
        SQLiteDbms::instance().endTransaction();
    }
}

void DoDrink(Character * character, ArgumentHandler & args)
{
    if (args.empty())
    {
        character->sendMsg("Drink from what?\n");
        return;
    }
    auto container = character->findEquipmentItem(args[0].getContent(), args[0].getIndex());
    if (container == nullptr)
    {
        container = character->findInventoryItem(args[0].getContent(), args[0].getIndex());
        if (container == nullptr)
        {
            container = character->room->findItem(args[0].getContent(), args[0].getIndex());
            if (container == nullptr)
            {
                character->sendMsg("You don't see that container.\n");
                return;
            }
        }
    }
    // Execute every necessary checks.
    if (HasFlag(container->flags, ItemFlag::Locked))
    {
        character->sendMsg("You have first to unlock %s.\n", container->getName(true));
        return;
    }
    if (HasFlag(container->flags, ItemFlag::Closed))
    {
        character->sendMsg("You have first to open %s.\n", container->getName(true));
        return;
    }
    if (container->model->getType() != ModelType::LiquidContainer)
    {
        character->sendMsg("%s is not a container for liquids.\n", container->getNameCapital(true));
        return;
    }
    if (container->isEmpty())
    {
        character->sendMsg("%s is empty.\n", container->getNameCapital(true));
        return;
    }
    // Take out the liquid.
    auto liquid = container->contentLiq.first;
    if (!container->pourOut(1))
    {
        character->sendMsg(
            "You were not able to drink some %s from %s.\n",
            liquid->getName(),
            container->getName(true));
        return;
    }
    character->sendMsg("You drink some %s from %s.\n", liquid->getName(), container->getName(true));
    // Send the message inside the room.
    character->room->sendToAll(
        "%s drinks some %s from %s.\n",
        {character},
        character->getNameCapital(),
        liquid->getName(),
        container->getName(true));
}

void DoFill(Character * character, ArgumentHandler & args)
{
    // Stop any action the character is executing.
    StopAction(character);
    // Check the arguments.
    if (args.size() != 2)
    {
        character->sendMsg("You have to fill something from a source.\n");
        return;
    }
    // Search the container.
    auto container = character->findInventoryItem(args[0].getContent(), args[0].getIndex());
    if (container == nullptr)
    {
        container = character->findEquipmentItem(args[0].getContent(), args[0].getIndex());
        if (container == nullptr)
        {
            character->sendMsg("You don't have any '%s' with you.\n", args[0].getContent());
            return;
        }
    }
    // Search the source.
    auto source = character->findInventoryItem(args[1].getContent(), args[1].getIndex());
    if ((source == nullptr) || (source == container))
    {
        source = character->findEquipmentItem(args[1].getContent(), args[1].getIndex());
        if ((source == nullptr) || (source == container))
        {
            source = character->room->findItem(args[1].getContent(), args[1].getIndex());
            if (source == nullptr)
            {
                character->sendMsg("You don't see any '%s'.\n", args[1].getContent());
                return;
            }
        }
    }
    if (HasFlag(source->flags, ItemFlag::Locked))
    {
        character->sendMsg("You have first to unlock %s.\n", source->getName(true));
        return;
    }
    if (HasFlag(source->flags, ItemFlag::Closed))
    {
        character->sendMsg("You have first to open %s.\n", source->getName(true));
        return;
    }
    if (HasFlag(container->flags, ItemFlag::Locked))
    {
        character->sendMsg("You have first to unlock %s.\n", container->getName(true));
        return;
    }
    if (HasFlag(container->flags, ItemFlag::Closed))
    {
        character->sendMsg("You have first to open %s.\n", container->getName(true));
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
    auto liquidModelSource = source->model->toLiquidContainer();
    // Check if source is empty.
    if (source->isEmpty())
    {
        character->sendMsg("%s is empty.\n", source->getNameCapital());
        return;
    }

    // Get the liquid from the source and eventually from the container.
    auto sourLiquid = source->contentLiq.first;
    auto contLiquid = container->contentLiq.first;
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
    auto atDisposal = source->contentLiq.second;
    auto quantity = container->getFreeSpace();
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
    // Send the messages.
    character->sendMsg(
        "You fill %s with %s from %s.\n",
        container->getName(true),
        sourLiquid->getName(),
        source->getName(true));
    character->room->sendToAll(
        "%s fills %s with %s from %s.\n",
        {character},
        character->getNameCapital(),
        container->getName(true),
        sourLiquid->getName(),
        source->getName(true));
}

void DoPour(Character * character, ArgumentHandler & args)
{
    // Stop any action the character is executing.
    StopAction(character);
    // Check the arguments.
    if (args.size() != 2)
    {
        character->sendMsg("You have to pour something into something else.\n");
        return;
    }
    // Search the container.
    auto source = character->findInventoryItem(args[0].getContent(), args[0].getIndex());
    if (source == nullptr)
    {
        source = character->findEquipmentItem(args[0].getContent(), args[0].getIndex());
        if (source == nullptr)
        {
            character->sendMsg("You don't have any '%s' with you.\n", args[0].getContent());
            return;
        }
    }
    // Search the source.
    auto container = character->findInventoryItem(args[1].getContent(), args[1].getIndex());
    if ((container == nullptr) || (container == source))
    {
        container = character->findEquipmentItem(args[1].getContent(), args[1].getIndex());
        if ((container == nullptr) || (container == source))
        {
            container = character->room->findItem(args[1].getContent(), args[1].getIndex());
            if (container == nullptr)
            {
                character->sendMsg("You don't see any '%s'.\n", args[1].getContent());
                return;
            }
        }
    }
    if (HasFlag(source->flags, ItemFlag::Locked))
    {
        character->sendMsg("You have first to unlock %s.\n", source->getName(true));
        return;
    }
    if (HasFlag(source->flags, ItemFlag::Closed))
    {
        character->sendMsg("You have first to open %s.\n", source->getName(true));
        return;
    }

    if (HasFlag(container->flags, ItemFlag::Locked))
    {
        character->sendMsg("You have first to unlock %s.\n", container->getName(true));
        return;
    }
    if (HasFlag(container->flags, ItemFlag::Closed))
    {
        character->sendMsg("You have first to open %s.\n", container->getName(true));
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
    auto liquidModelSource = source->model->toLiquidContainer();
    // Check if source is empty.
    if (source->isEmpty())
    {
        character->sendMsg("%s is empty.\n", source->getNameCapital());
        return;
    }
    // Get the liquid from the source and eventually from the container.
    auto sourLiquid = source->contentLiq.first;
    auto contLiquid = container->contentLiq.first;
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
    auto atDisposal = source->contentLiq.second;
    auto quantity = container->getFreeSpace();
    if (!HasFlag(liquidModelSource->liquidFlags, LiqContainerFlag::Endless))
    {
        if (atDisposal < quantity)
        {
            quantity = atDisposal;
        }
    }
    if (!source->pourOut(quantity))
    {
        character->sendMsg("You failed to pour out the liquid from %s.\n", source->getName(true));
        return;
    }
    if (!container->pourIn(sourLiquid, quantity))
    {
        character->sendMsg("You failed to pour the liquid into %s.\n", source->getName(true));
        return;
    }
    // Send the messages.
    character->sendMsg(
        "You pour %s of %s into %s.\n",
        sourLiquid->getName(),
        source->getName(true),
        container->getName(true));
    character->room->sendToAll(
        "%s pour %s of %s into %s.\n",
        {character},
        character->getNameCapital(),
        sourLiquid->getName(),
        source->getName(true),
        container->getName(true));
}

void DoDeposit(Character * character, ArgumentHandler & args)
{
    // Stop any action the character is executing.
    StopAction(character);
    // Check the number of arguments.
    if (args.size() != 2)
    {
        character->sendMsg("What do you want to deposit?\n");
        return;
    }
    auto item = character->findInventoryItem(args[0].getContent(), args[0].getIndex());
    auto building = character->room->findBuilding(args[1].getContent(), args[1].getIndex());
    // Check the item.
    if (item == nullptr)
    {
        character->sendMsg("You don't have that item.\n");
        return;
    }
    if (item->getType() != ModelType::Currency)
    {
        character->sendMsg("You can't deposit %s.\n", item->getName(true));
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
        character->sendMsg("You can't deposit %s in %s.\n", item->getName(true), building->getName(true));
        return;
    }
    // Set the quantity.
    auto quantity = args[0].getMultiplier();
    if (item->quantity <= quantity)
    {
        building->toShopItem()->balance += item->getPrice(true);
        item->removeFromMud();
        delete (item);
    }
    else
    {
        item->quantity -= quantity;
        building->toShopItem()->balance += item->getPrice(false) * quantity;
    }
}

void DoSell(Character * character, ArgumentHandler & args)
{
    // Stop any action the character is executing.
    StopAction(character);
    // Check the number of arguments.
    if (args.size() != 2)
    {
        character->sendMsg("Sell what to whom?\n");
        return;
    }

    // Get the item and the target.
    auto item = character->findInventoryItem(args[0].getContent(), args[0].getIndex());
    // Check the item.
    if (item == nullptr)
    {
        character->sendMsg("You don't have that item.\n");
        return;
    }
    if (item->getType() == ModelType::Currency)
    {
        character->sendMsg("You can't sell %s.\n", item->getName(true));
        return;
    }

    // Get the target.
    auto target = character->room->findItem(args[1].getContent(), args[1].getIndex());
    if (target == nullptr)
    {
        character->sendMsg("You don't see '%s' here.\n", args[1].getContent());
        return;
    }
    if (target->getType() != ModelType::Shop)
    {
        character->sendMsg("%s is not a shop.\n", target->getNameCapital());
        return;
    }

    // Set the quantity.
    auto quantity = args[0].getMultiplier();
    if (item->quantity < quantity)
    {
        quantity = item->quantity;
    }

    auto shop = target->toShopItem();
    std::string error;
    if (!shop->canUse(error))
    {
        character->sendMsg(error + "\n");
        return;
    }

    auto shopKeeper = shop->shopKeeper;
    if (!shop->canContain(item, quantity))
    {
        auto phrase = "The shop is full, come back another day.";
        shopKeeper->doCommand("say " + character->getName() + " " + phrase);
        return;
    }

    auto price = shop->evaluateSellPrice(item) * quantity;
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
        character->sendMsg("You failed to sell %s.\n", item->getName(true));
        return;
    }

    SQLiteDbms::instance().beginTransaction();
    if (item->quantity <= quantity)
    {
        // Remove the item from the player's inventory.
        character->remInventoryItem(item);
        // Put the item inside the container.
        shop->putInside(item);
        // Decrese the shop balance.
        shop->balance -= price;
        // Send a message.
        character->sendMsg("You sell %s to %s.\n", item->getName(true), shop->getName(true));
    }
    else
    {
        // Remove from the stack.
        auto newStack = item->removeFromStack(character, quantity);
        if (newStack == nullptr)
        {
            character->sendMsg("You failed sell part of %s to %s.\n", item->getName(true), shop->getName(true));
            for (auto coin : coins)
            {
                delete (coin);
            }
            // Rollback the transaction.
            SQLiteDbms::instance().rollbackTransection();
            return;
        }
        // Put the item inside the container.
        shop->putInside(newStack);
        // Decrese the shop balance.
        shop->balance -= price;
        // Send a message.
        character->sendMsg("You sell part of %s to %s.\n", item->getName(true), shop->getName(true));
    }
    for (auto coin : coins)
    {
        character->addInventoryItem(coin);
    }
    SQLiteDbms::instance().endTransaction();
}

void DoBuy(Character * character, ArgumentHandler & args)
{
    // Stop any action the character is executing.
    StopAction(character);
    // Check the number of arguments.
    if (args.size() != 2)
    {
        character->sendMsg("Buy what from which shop?\n");
        return;
    }

    // Get the target.
    auto target = character->room->findItem(args[1].getContent(), args[1].getIndex());
    if (target == nullptr)
    {
        character->sendMsg("You don't see '%s' here.\n", args[1].getContent());
        return;
    }
    if (target->getType() != ModelType::Shop)
    {
        character->sendMsg("%s is not a shop.\n", target->getNameCapital(true));
        return;
    }

    // Get the shop.
    auto shop = target->toShopItem();
    std::string error;
    if (!shop->canUse(error))
    {
        character->sendMsg(error + "\n");
        return;
    }
    auto shopKeeper = shop->shopKeeper;

    // Get the item.
    auto item = shop->findContent(args[0].getContent(), args[0].getIndex());
    // Check the item.
    if (item == nullptr)
    {
        auto phrase = "There is no " + args[0].getContent() + " on sale.\n";
        shopKeeper->doCommand("say " + character->getName() + " " + phrase);
        return;
    }
    // Set the quantity.
    auto quantity = args[0].getMultiplier();
    if (item->quantity < quantity)
    {
        quantity = item->quantity;
    }
    if (!character->canCarry(item, quantity))
    {
        auto phrase = "It seems that you can't carry " + item->getName(true) + ".\n";
        shopKeeper->doCommand("say " + character->getName() + " " + phrase);
        return;
    }

    // Check if the character has enough coins.
    auto availableCoins = character->findCoins();
    std::vector<Item *> changedCoins;
    std::vector<std::pair<Item *, unsigned int>> givenCoins;
    unsigned int requiredValue = shop->evaluateBuyPrice(item) * quantity;
    unsigned int providedValue = 0;
    for (auto coin : availableCoins)
    {
        unsigned int coinValue = coin->getPrice(false);
        unsigned int coinQuantity = 0;
        for (; (coinQuantity < coin->quantity) && (providedValue < requiredValue); ++coinQuantity)
        {
            providedValue += coinValue;
        }
        givenCoins.push_back(std::make_pair(coin, coinQuantity));
        if (providedValue >= requiredValue)
        {
            break;
        }
    }
    if (givenCoins.empty())
    {
        auto phrase = "You can't afford to buy " + item->getName(true) + ".\n";
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
            auto phrase = "Sorry but I cannot sell " + item->getName(true) + " to you.\n";
            shopKeeper->doCommand("say " + character->getName() + " " + phrase);
            return;
        }
    }
    SQLiteDbms::instance().beginTransaction();
    if (item->quantity <= quantity)
    {
        shop->balance += requiredValue;

        shop->takeOut(item);
        character->addInventoryItem(item);
        character->sendMsg("You buy %s from %s.\n", item->getName(true), shop->getName(true));
    }
    else
    {
        // Remove from the stack.
        auto newStack = item->removeFromStack(character, quantity);
        if (newStack == nullptr)
        {
            // Rollback the transaction.
            SQLiteDbms::instance().rollbackTransection();
            return;
        }
        character->addInventoryItem(newStack);
        character->sendMsg("You buy part of %s from %s.\n", item->getName(true), shop->getName(true));
    }
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
    SQLiteDbms::instance().endTransaction();
}

void DoBalance(Character * character, ArgumentHandler & args)
{
    // Check the number of arguments.
    if (!args.empty())
    {
        character->sendMsg("You have provided unecessary arguments.\n");
        return;
    }
    unsigned int balance = 0;
    for (auto coin : character->findCoins())
    {
        balance += coin->getPrice(true);
    }
    character->sendMsg("Your balance is: %s.\n", ToString(balance));
}
