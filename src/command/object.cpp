/// @file   object.cpp
/// @brief  Implements the methods used by the player in order to <b>interract with objects</b>.
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

#include "object.hpp"

#include "mud.hpp"
#include "shopItem.hpp"
#include "lightItem.hpp"
#include "currencyModel.hpp"
#include "liquidContainerModel.hpp"

void LoadObjectCommands()
{
    {
        Command command;
        command.name = "take";
        command.help = "Take something from the ground or from a container.";
        command.arguments = "(item) [(container)]";
        command.hndl = DoTake;
        command.canUseInCombat = true;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.name = "drop";
        command.help = "Drop an object.";
        command.arguments = "(item)";
        command.hndl = DoDrop;
        command.canUseInCombat = true;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.name = "give";
        command.help = "Give an object to someone.";
        command.arguments = "(item) (someone)";
        command.hndl = DoGive;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.name = "equipments";
        command.help = "List all the items you are wearing.";
        command.arguments = "";
        command.hndl = DoEquipments;
        command.canUseInCombat = true;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.name = "wield";
        command.help = "Wield a weapon, a shield or maybe a tool.";
        command.arguments = "(item)";
        command.hndl = DoWield;
        command.canUseInCombat = true;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.name = "wear";
        command.help = "Puts on a piece of equipment.";
        command.arguments = "(item)";
        command.hndl = DoWear;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.name = "remove";
        command.help = "Remove a weared or wielded item.";
        command.arguments = "(item)";
        command.hndl = DoRemove;
        command.canUseInCombat = true;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.name = "inventory";
        command.help = "Show character's inventory.";
        command.arguments = "";
        command.hndl = DoInventory;
        command.canUseInCombat = true;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.name = "organize";
        command.help = "Order the desired container or if no target is passed, the room.";
        command.arguments = "(name|weight) [(target)]";
        command.hndl = DoOrganize;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.name = "open";
        command.help = "Open a door in a given direction.";
        command.arguments = "(item)|(direction)";
        command.hndl = DoOpen;
        command.canUseInCombat = true;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.name = "close";
        command.help = "Close a door in a given direction.";
        command.arguments = "(item)|(direction)";
        command.hndl = DoClose;
        command.canUseInCombat = true;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.name = "put";
        command.help = "Put something inside a container.";
        command.arguments = "(something) (container)";
        command.hndl = DoPut;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.name = "drink";
        command.help = "Drink from a container of liquids.";
        command.arguments = "(container)";
        command.hndl = DoDrink;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.name = "fill";
        command.help = "Fill a container of liquids from a source of liquid.";
        command.arguments = "(container) (source)";
        command.hndl = DoFill;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.name = "pour";
        command.help = "Pour the content of the container into another one or on the ground.";
        command.arguments = "(container) [container]";
        command.hndl = DoPour;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.name = "deposit";
        command.help = "Deposit a coin inside a shop.";
        command.arguments = "(item) (shop)";
        command.hndl = DoDeposit;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.name = "sell";
        command.help = "Sell an item to a shop keeper.";
        command.arguments = "(item) (shop)";
        command.hndl = DoSell;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.name = "buy";
        command.help = "Allows to buy an item from a shop.";
        command.arguments = "(item) (shop)";
        command.hndl = DoBuy;
        Mud::instance().addCommand(command);
    }
    Mud::instance().addCommand(
        Command(false, "balance",
                "Shows the character's balance.",
                "",
                DoBalance, false, false));
    Mud::instance().addCommand(
        Command(false, "turn",
                "Allows to turn on and off an activatable item.",
                "(item)",
                DoTurn, true, false));
    Mud::instance().addCommand(
        Command(false, "refill",
                "Allows to refill a light source.",
                "(light_source) (fuel)",
                DoRefill, false, false));
}

bool DoTake(Character * character, ArgumentHandler & args)
{
    // Stop any action the character is executing.
    StopAction(character);
    if ((args.size() != 1) && (args.size() != 2))
    {
        character->sendMsg("Wrong number of parameters.\n");
        return false;
    }
    // Try to take the item inside the room.
    if (args.size() == 1)
    {
        if (ToLower(args[0].getContent()) == "all")
        {
            if (character->room->items.empty())
            {
                character->sendMsg("There is nothing to pick up.\n");
                return false;
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
                character->room->sendToAll("%s has picked up everything %s could.\n",
                                           {character},
                                           character->getNameCapital(),
                                           character->getSubjectPronoun());
                return true;
            }
            character->sendMsg("You've picked up nothing.\n");
            return false;
        }
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
                return false;
            }
        }
        // Check if the item has the flag Static.
        if (HasFlag(item->model->modelFlags, ModelFlag::Static))
        {
            character->sendMsg("You can't pick up %s!\n", item->getName(true));
            return false;
        }
        if (HasFlag(item->flags, ItemFlag::Built))
        {
            character->sendMsg("You can't pick up something which is built!\n");
            return false;
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
            return false;
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
            character->sendMsg("You take %s.\n", item->getName(true));
            character->room->sendToAll("%s has picked up %s.\n",
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
                return false;
            }
            // Add the item to the player's inventory.
            character->addInventoryItem(newStack);
            // Send the messages.
            character->sendMsg("You take a part of %s.\n", item->getName(true));
            character->room->sendToAll("%s has picked up part of %s.\n",
                                       {character},
                                       character->getNameCapital(),
                                       item->getName(true));
        }
        // Conclude the transaction.
        SQLiteDbms::instance().endTransaction();
        return true;
    }

    auto container = character->findNearbyItem(args[1].getContent(), args[1].getIndex());
    if (container == nullptr)
    {
        character->sendMsg("You don't see that container.\n");
        return false;
    }
    if (HasFlag(container->flags, ItemFlag::Locked))
    {
        character->sendMsg("You have first to unlock %s first.\n", container->getName(true));
        return false;
    }
    if (HasFlag(container->flags, ItemFlag::Closed))
    {
        character->sendMsg("You have first to open %s first.\n", container->getName(true));
        return false;
    }
    if (ToLower(args[0].getContent()) == "all")
    {
        if (container->content.empty())
        {
            character->sendMsg("There is nothing inside %s.\n", container->getName(true));
            return false;
        }
        // Make a temporary copy of the character's inventory.
        auto originalList = container->content;
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
            character->sendMsg("You've taken everything you could from %s.\n", container->getName(true));
            character->room->sendToAll("%s has taken everything %s could from %s.\n",
                                       {character},
                                       character->getNameCapital(),
                                       character->getSubjectPronoun(),
                                       container->getName(true));
            return true;
        }
        character->sendMsg("You've taken nothing from %s.\n", container->getName(true));
        return false;
    }
    auto item = container->findContent(args[0].getContent(), args[0].getIndex());
    if (item == nullptr)
    {
        character->sendMsg("You don't see that item inside the container.\n");
        return false;
    }
    // Check if the item has the flag kNoPick.
    if (HasFlag(item->model->modelFlags, ModelFlag::Static))
    {
        character->sendMsg("You can't pick up this kind of items!\n");
        return false;
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
        return false;
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
        character->sendMsg("You take out %s from %s.\n", item->getName(true), container->getName(true));
        character->room->sendToAll("%s takes out %s from %s.\n",
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
            return false;
        }
        // Add the item to the player's inventory.
        character->addInventoryItem(newStack);
        // Send the messages.
        character->sendMsg("You take out part of %s from %s.\n",
                           item->getName(true),
                           container->getName(true));
        character->room->sendToAll("%s takes out %s from %s.\n",
                                   {character},
                                   character->getNameCapital(),
                                   item->getName(true),
                                   container->getName(true));
    }
    // Conclude the transaction.
    SQLiteDbms::instance().endTransaction();
    return true;
}

bool DoDrop(Character * character, ArgumentHandler & args)
{
    // Stop any action the character is executing.
    StopAction(character);
    // Check the number of arguments.
    if (args.size() == 0)
    {
        character->sendMsg("Drop what?\n");
        return false;
    }
    if (args.size() > 1)
    {
        character->sendMsg("Too many arguments!\n");
        return false;
    }
    if (character->inventory.empty())
    {
        character->sendMsg("You have nothing to drop.\n");
        return false;
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
        character->room->sendToAll("%s has dropped all %s items.\n",
                                   {character},
                                   character->getNameCapital(),
                                   character->getPossessivePronoun());
        return true;
    }
    // Get the item.
    auto item = character->findInventoryItem(args[0].getContent(), args[0].getIndex());
    // Check if the player has the item that he want to drop.
    if (item == nullptr)
    {
        character->sendMsg("You don't have that item.\n");
        return false;
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
        character->sendMsg("You drop %s.\n", item->getName(true));
        character->room->sendToAll("%s has dropped %s.\n",
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
            return false;
        }
        // Put the item inside the room.
        character->room->addItem(newStack);
        // Send the messages.
        character->sendMsg("You drop part of %s.\n", item->getName(true));
        character->room->sendToAll("%s has dropped %s.\n",
                                   {character},
                                   character->getNameCapital(),
                                   item->getName(true));
    }
    // Conclude the transaction.
    SQLiteDbms::instance().endTransaction();
    return true;
}

bool DoGive(Character * character, ArgumentHandler & args)
{
    // Stop any action the character is executing.
    StopAction(character);
    // Check the number of arguments.
    if (args.size() != 2)
    {
        character->sendMsg("Give what to whom?\n");
        return false;
    }
    // Get the item.
    auto item = character->findInventoryItem(args[0].getContent(), args[0].getIndex());
    if (item == nullptr)
    {
        character->sendMsg("You don't have that item.\n");
        return false;
    }
    auto target = character->room->findCharacter(args[1].getContent(), args[1].getIndex(), {
        character});
    if (target == nullptr)
    {
        character->sendMsg("You don't see that person.\n");
        return false;
    }
    // Check if the target player can carry the item.
    if (!target->canCarry(item, item->quantity))
    {
        character->sendMsg(target->getNameCapital() + " can't carry anymore items.\n");
        return false;
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
        character->sendMsg("You give %s to %s.\n", item->getName(true), target->getName());
        target->sendMsg("%s gives you %s.\n\n", character->getNameCapital(), item->getName(true));
        character->room->sendToAll("%s gives %s to %s.\n",
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
            character->sendMsg("You failed to give part of %s to %s.\n", item->getName(true), target->getName());
            // Rollback the transaction.
            SQLiteDbms::instance().rollbackTransection();
            return false;
        }
        // Add the stack to the target inventory.
        target->addInventoryItem(newStack);
        // Send all the messages.
        character->sendMsg("You give part of %s to %s.\n", item->getName(true), target->getName());
        target->sendMsg("%s gives you %s.\n\n", character->getNameCapital(), item->getName(true));
        character->room->sendToAll("%s gives %s to %s.\n",
                                   {character, target},
                                   character->getNameCapital(),
                                   item->getName(true),
                                   target->getName());
    }
    // Conclude the transaction.
    SQLiteDbms::instance().endTransaction();
    return true;
}

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
    output += Formatter::yellow() + "#------------ Equipment -----------#\n" + Formatter::reset();
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
    output += Formatter::yellow() + "#----------------------------------#\n" + Formatter::reset();
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
    auto whereOthers = character->getPossessivePronoun() + " " + item->getCurrentSlotName();
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
            // Notify that something has been weared.
            wearedSomething = true;
        }
        // Handle output only if the player has really weared something.
        if (!wearedSomething)
        {
            character->sendMsg("You had nothing to wear.\n");
            return false;
        }
        character->sendMsg("You have weared everything you could.\n");
        // Send the message inside the room.
        character->room->sendToAll("%s has weared all %s could.\n",
                                   {character},
                                   character->getNameCapital(),
                                   character->getSubjectPronoun());
    }
    else
    {
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
        character->sendMsg("You wear %s on your %s.\n", item->getName(true), ToLower(item->getCurrentSlotName()));
        // Send the message inside the room.
        character->room->sendToAll("%s wears %s on %s %s.\n",
                                   {character},
                                   character->getNameCapital(),
                                   item->getName(true),
                                   character->getPossessivePronoun(),
                                   ToLower(item->getCurrentSlotName()));
    }
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
        character->room->sendToAll("%s has undressed all he could.\n", {character}, character->getNameCapital());
        // Check if we have just removed ALL the USED Ranged Weapons.
        if (character->combatHandler.getAimedTarget() != nullptr)
        {
            if (CheckIfRemovedAllRangedWeapons())
            {
                character->sendMsg("You stop aiming %s.\n", character->combatHandler.getAimedTarget()->getName());
                character->combatHandler.setAimedTarget(nullptr);
            }
        }
        return false;
    }
    // Get the item.
    auto item = character->findEquipmentItem(args[0].getContent(), args[0].getIndex());
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
    character->sendMsg("You remove %s from your %s.\n", item->getName(true), ToLower(item->getCurrentSlotName()));
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
            character->sendMsg("You stop aiming %s.\n", character->combatHandler.getAimedTarget()->getName());
            character->combatHandler.setAimedTarget(nullptr);
        }
    }
    return true;
}

bool DoInventory(Character * character, ArgumentHandler & /*args*/)
{
    if (character->inventory.empty())
    {
        character->sendMsg(Formatter::gray() + "    You are carrying anything.\n" + Formatter::reset());
    }
    bool roomIsLit = false;
    bool inventoryLight = false;
    // Check if the room is lit.
    if (character->room != nullptr)
    {
        if ((!character->room->terrain->inside) && (MudUpdater::instance().getDayPhase() != DayPhase::Night))
        {
            roomIsLit = true;
        }
        else
        {
            roomIsLit = character->room->isLit();
        }
    }
    // Check if the inventory contains a lit light source.
    for (auto item : character->inventory)
    {
        if (item->getType() == ModelType::Light)
        {
            if (item->toLightItem()->active)
            {
                inventoryLight = true;
                break;
            }
        }
    }
    Table table = Table("Inventory");
    table.addColumn("Item", StringAlign::Left);
    table.addColumn("Quantity", StringAlign::Right);
    table.addColumn("Weight", StringAlign::Right);
    // List all the items in inventory
    for (auto it : character->inventory)
    {
        TableRow row;
        row.emplace_back((roomIsLit || inventoryLight) ? it->getNameCapital() : "Something");
        if (roomIsLit || inventoryLight)
        {
            row.emplace_back(ToString(it->quantity));
        }
        else
        {
            if (it->quantity == 1)
            {
                row.emplace_back((roomIsLit || inventoryLight) ? it->getNameCapital() : "One");
            }
            else
            {
                row.emplace_back((roomIsLit || inventoryLight) ? it->getNameCapital() : "Some");
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

bool DoOrganize(Character * character, ArgumentHandler & args)
{
    // Stop any action the character is executing.
    StopAction(character);
    // Check the number of arguments.
    if (args.empty())
    {
        character->sendMsg("Organize what?\n");
        return false;
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
                return true;
            }
        }
        else if (BeginWith("inventory", args[1].getContent()))
        {
            character->inventory.orderBy(order);
            // Organize the target container.
            character->sendMsg("You have organized your inventory, by %s.\n", name);
            return true;
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
    return false;
}

bool DoOpen(Character * character, ArgumentHandler & args)
{
    // Stop any action the character is executing.
    StopAction(character);
    // Check the arguments.
    if (args.size() != 1)
    {
        character->sendMsg("What do you want to open?\n");
        return false;
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
            return false;
        }
        auto destination = roomExit->destination;
        if (destination == nullptr)
        {
            character->sendMsg("There is nothing in that direction.\n");
            return false;
        }
        auto door = destination->findDoor();
        if (door == nullptr)
        {
            character->sendMsg("There is no door in that direction.\n");
            return false;
        }
        if (HasFlag(door->flags, ItemFlag::Locked))
        {
            character->sendMsg("You have first to unlock it.\n");
            return false;
        }
        if (!HasFlag(door->flags, ItemFlag::Closed))
        {
            character->sendMsg("There door it's already opened.\n");
            return false;
        }

        ClearFlag(&door->flags, ItemFlag::Closed);

        // Display message.
        if (HasFlag(roomExit->flags, ExitFlag::Hidden))
        {
            character->sendMsg("You have opened a hidden door!\n");
            // Send the message inside the room.
            character->room->sendToAll("%s opens a hidden door!\n", {character}, character->getNameCapital());
        }
        else
        {
            character->sendMsg("You have opened the door.\n");
            // Send the message inside the room.
            character->room->sendToAll("%s opens a door.\n", {character}, character->getNameCapital());
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
                it->destination->sendToAll("Someone opens a secret passage from the other side.\n", {});
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
            return false;
        }
        if (HasFlag(container->flags, ItemFlag::Locked))
        {
            character->sendMsg("You have first to unlock it.\n");
            return false;
        }
        if (!HasFlag(container->flags, ItemFlag::Closed))
        {
            character->sendMsg("It is already opened.\n");
            return false;
        }
        ClearFlag(&container->flags, ItemFlag::Closed);
        // Send the message to the character.
        character->sendMsg("You open %s.\n", container->getName(true));
        // Send the message inside the room.
        character->room->sendToAll("%s opens %s.\n",
                                   {character},
                                   character->getNameCapital(),
                                   container->getName(true));
    }
    return true;
}

bool DoClose(Character * character, ArgumentHandler & args)
{
    // Stop any action the character is executing.
    StopAction(character);
    // Check the arguments.
    if (args.size() != 1)
    {
        character->sendMsg("What do you want to close?\n");
        return false;
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
            return false;
        }
        auto destination = roomExit->destination;
        if (destination == nullptr)
        {
            character->sendMsg("There is nothing in that direction.\n");
            return false;
        }
        auto door = destination->findDoor();
        if (door == nullptr)
        {
            character->sendMsg("There is no door in that direction.\n");
            return false;
        }
        if (HasFlag(door->flags, ItemFlag::Closed))
        {
            character->sendMsg("There door it's already closed.\n");
            return false;
        }
        if (destination->items.size() > 1)
        {
            character->sendMsg("There are items on the way, you can't close the door.\n");
            return false;
        }
        if (destination->characters.size() >= 1)
        {
            character->sendMsg("There are someone on the way, you can't close the door.\n");
            return false;
        }
        SetFlag(&door->flags, ItemFlag::Closed);
        // Display message.
        if (HasFlag(roomExit->flags, ExitFlag::Hidden))
        {
            character->sendMsg("You have closed a hidden door!\n");
            // Send the message inside the room.
            character->room->sendToAll("%s closes a hidden door!\n", {character}, character->getNameCapital());
        }
        else
        {
            character->sendMsg("You have closed the door.\n");
            // Send the message inside the room.
            character->room->sendToAll("%s closes a door.\n", {character}, character->getNameCapital());
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
                it->destination->sendToAll("Someone closes a secret passage from the other side.\n", {});
            }
            else
            {
                // Send the message inside the room.
                it->destination->sendToAll("Someone closes a door from the other side.\n", {});
            }
        }
    }
    else
    {
        auto container = character->findNearbyItem(args[0].getContent(), args[0].getIndex());
        if (container == nullptr)
        {
            character->sendMsg("What do you want to close?\n");
            return false;
        }
        if (HasFlag(container->flags, ItemFlag::Closed))
        {
            character->sendMsg("It is already closed.\n");
            return false;
        }
        if (!HasFlag(container->model->modelFlags, ModelFlag::CanClose))
        {
            character->sendMsg("It cannot be closed.\n");
            return false;
        }
        SetFlag(&container->flags, ItemFlag::Closed);
        // Send the message to the character.
        character->sendMsg("You close %s.\n", container->getName(true));
        // Send the message inside the room.
        character->room->sendToAll("%s closes %s.\n",
                                   {character},
                                   character->getNameCapital(),
                                   container->getName(true));
    }
    return true;
}

bool DoPut(Character * character, ArgumentHandler & args)
{
    // Stop any action the character is executing.
    StopAction(character);
    if (args.size() != 2)
    {
        character->sendMsg("Put what inside what?\n");
        return false;
    }
    auto container = character->findNearbyItem(args[1].getContent(), args[1].getIndex());
    if (container == nullptr)
    {
        character->sendMsg("You don't see any container named '%s' here.\n", args[1].getContent());
        return false;
    }
    if (!container->isAContainer())
    {
        character->sendMsg("%s is not a valid container.\n", container->getNameCapital());
        return false;
    }
    if (container->model->getType() == ModelType::Corpse)
    {
        character->sendMsg("You don't really want to put something inside that body...\n");
        return false;
    }
    if (HasFlag(container->flags, ItemFlag::Locked))
    {
        character->sendMsg("You have first to unlock %s.\n", container->getName(true));
        return false;
    }
    if (HasFlag(container->flags, ItemFlag::Closed))
    {
        character->sendMsg("You have first to open %s.\n", container->getName(true));
        return false;
    }
    if (character->inventory.empty())
    {
        character->sendMsg("You don't have anything to put in a container.");
        return false;
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
        character->room->sendToAll("%s puts everything %s could inside %s.\n",
                                   {character},
                                   character->getNameCapital(),
                                   character->getSubjectPronoun(),
                                   container->getName(true));
    }
    else
    {
        // Find the specific item inside the inventory.
        auto item = character->findInventoryItem(args[0].getContent(), args[0].getIndex());
        if (item == nullptr)
        {
            character->sendMsg("You don't have any '%s'.\n", args[0].getContent());
            return false;
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
            return false;
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
            character->sendMsg("You put %s inside %s.\n", item->getName(true), container->getName(true));
            character->room->sendToAll("%s puts %s inside %s.\n",
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
                character->sendMsg("You failed to put part of %s inside %s.\n",
                                   item->getName(true),
                                   container->getName(true));
                // Rollback the transaction.
                SQLiteDbms::instance().rollbackTransection();
                return false;
            }
            // Put the stack inside the container.
            container->putInside(newStack);
            // Send the messages.
            character->sendMsg("You put part of %s inside %s.\n", item->getName(true), container->getName(true));
            character->room->sendToAll("%s puts %s inside %s.\n",
                                       {character},
                                       character->getNameCapital(),
                                       item->getName(true),
                                       container->getName(true));
        }
        // Conclude the transaction.
        SQLiteDbms::instance().endTransaction();
    }
    return true;
}

bool DoDrink(Character * character, ArgumentHandler & args)
{
    // Stop any action the character is executing.
    StopAction(character);
    if (args.empty())
    {
        character->sendMsg("Drink from what?\n");
        return false;
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
                return false;
            }
        }
    }
    // Execute every necessary checks.
    if (HasFlag(container->flags, ItemFlag::Locked))
    {
        character->sendMsg("You have first to unlock %s.\n", container->getName(true));
        return false;
    }
    if (HasFlag(container->flags, ItemFlag::Closed))
    {
        character->sendMsg("You have first to open %s.\n", container->getName(true));
        return false;
    }
    if (container->model->getType() != ModelType::LiquidContainer)
    {
        character->sendMsg("%s is not a container for liquids.\n", container->getNameCapital(true));
        return false;
    }
    if (container->isEmpty())
    {
        character->sendMsg("%s is empty.\n", container->getNameCapital(true));
        return false;
    }
    // Take out the liquid.
    auto liquid = container->contentLiq.first;
    if (!container->pourOut(1))
    {
        character->sendMsg("You were not able to drink some %s from %s.\n",
                           liquid->getName(),
                           container->getName(true));
        return false;
    }
    character->sendMsg("You drink some %s from %s.\n", liquid->getName(), container->getName(true));
    // Send the message inside the room.
    character->room->sendToAll("%s drinks some %s from %s.\n",
                               {character},
                               character->getNameCapital(),
                               liquid->getName(),
                               container->getName(true));
    return true;
}

bool DoFill(Character * character, ArgumentHandler & args)
{
    // Stop any action the character is executing.
    StopAction(character);
    // Check the arguments.
    if (args.size() != 2)
    {
        character->sendMsg("You have to fill something from a source.\n");
        return false;
    }
    // Search the container.
    auto container = character->findInventoryItem(args[0].getContent(), args[0].getIndex());
    if (container == nullptr)
    {
        container = character->findEquipmentItem(args[0].getContent(), args[0].getIndex());
        if (container == nullptr)
        {
            character->sendMsg("You don't have any '%s' with you.\n", args[0].getContent());
            return false;
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
                return false;
            }
        }
    }
    if (HasFlag(source->flags, ItemFlag::Locked))
    {
        character->sendMsg("You have first to unlock %s.\n", source->getName(true));
        return false;
    }
    if (HasFlag(source->flags, ItemFlag::Closed))
    {
        character->sendMsg("You have first to open %s.\n", source->getName(true));
        return false;
    }
    if (HasFlag(container->flags, ItemFlag::Locked))
    {
        character->sendMsg("You have first to unlock %s.\n", container->getName(true));
        return false;
    }
    if (HasFlag(container->flags, ItemFlag::Closed))
    {
        character->sendMsg("You have first to open %s.\n", container->getName(true));
        return false;
    }
    // Check if the items are suitable source and container of liquids.
    if (container->model->getType() != ModelType::LiquidContainer)
    {
        character->sendMsg("%s is not a suitable container.\n", container->getNameCapital());
        return false;
    }
    if (source->model->getType() != ModelType::LiquidContainer)
    {
        character->sendMsg("%s is not a suitable source of liquids.\n", source->getNameCapital());
        return false;
    }
    auto liquidModelSource = source->model->toLiquidContainer();
    // Check if source is empty.
    if (source->isEmpty())
    {
        character->sendMsg("%s is empty.\n", source->getNameCapital());
        return false;
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
            return false;
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
        character->sendMsg("You failed to take out the liquid from a %s.\n", source->getNameCapital());
        return false;
    }
    if (!container->pourIn(sourLiquid, quantity))
    {
        character->sendMsg("You failed to fill the container with the liquid.\n");
        return false;
    }
    // Send the messages.
    character->sendMsg("You fill %s with %s from %s.\n",
                       container->getName(true),
                       sourLiquid->getName(),
                       source->getName(true));
    character->room->sendToAll("%s fills %s with %s from %s.\n",
                               {character},
                               character->getNameCapital(),
                               container->getName(true),
                               sourLiquid->getName(),
                               source->getName(true));
    return true;
}

bool DoPour(Character * character, ArgumentHandler & args)
{
    // Stop any action the character is executing.
    StopAction(character);
    // Check the arguments.
    if (args.size() != 2)
    {
        character->sendMsg("You have to pour something into something else.\n");
        return false;
    }
    // Search the container.
    auto source = character->findInventoryItem(args[0].getContent(), args[0].getIndex());
    if (source == nullptr)
    {
        source = character->findEquipmentItem(args[0].getContent(), args[0].getIndex());
        if (source == nullptr)
        {
            character->sendMsg("You don't have any '%s' with you.\n", args[0].getContent());
            return false;
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
                return false;
            }
        }
    }
    if (HasFlag(source->flags, ItemFlag::Locked))
    {
        character->sendMsg("You have first to unlock %s.\n", source->getName(true));
        return false;
    }
    if (HasFlag(source->flags, ItemFlag::Closed))
    {
        character->sendMsg("You have first to open %s.\n", source->getName(true));
        return false;
    }

    if (HasFlag(container->flags, ItemFlag::Locked))
    {
        character->sendMsg("You have first to unlock %s.\n", container->getName(true));
        return false;
    }
    if (HasFlag(container->flags, ItemFlag::Closed))
    {
        character->sendMsg("You have first to open %s.\n", container->getName(true));
        return false;
    }
    // Check if the items are suitable source and container of liquids.
    if (container->model->getType() != ModelType::LiquidContainer)
    {
        character->sendMsg("%s is not a suitable container.\n", container->getNameCapital());
        return false;
    }
    if (source->model->getType() != ModelType::LiquidContainer)
    {
        character->sendMsg("%s is not a suitable source of liquids.\n", source->getNameCapital());
        return false;
    }
    auto liquidModelSource = source->model->toLiquidContainer();
    // Check if source is empty.
    if (source->isEmpty())
    {
        character->sendMsg("%s is empty.\n", source->getNameCapital());
        return false;
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
            return false;
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
        return false;
    }
    if (!container->pourIn(sourLiquid, quantity))
    {
        character->sendMsg("You failed to pour the liquid into %s.\n", source->getName(true));
        return false;
    }
    // Send the messages.
    character->sendMsg("You pour %s of %s into %s.\n",
                       sourLiquid->getName(),
                       source->getName(true),
                       container->getName(true));
    character->room->sendToAll("%s pour %s of %s into %s.\n",
                               {character},
                               character->getNameCapital(),
                               sourLiquid->getName(),
                               source->getName(true),
                               container->getName(true));
    return true;
}

bool DoDeposit(Character * character, ArgumentHandler & args)
{
    // Stop any action the character is executing.
    StopAction(character);
    // Check the number of arguments.
    if (args.size() != 2)
    {
        character->sendMsg("What do you want to deposit?\n");
        return false;
    }
    auto item = character->findInventoryItem(args[0].getContent(), args[0].getIndex());
    auto building = character->room->findBuilding(args[1].getContent(), args[1].getIndex());
    // Check the item.
    if (item == nullptr)
    {
        character->sendMsg("You don't have that item.\n");
        return false;
    }
    if (item->getType() != ModelType::Currency)
    {
        character->sendMsg("You can't deposit %s.\n", item->getName(true));
        return false;
    }
    // Check the building.
    if (building == nullptr)
    {
        character->sendMsg("You don't see that building.\n");
        return false;
    }
    if (building->getType() != ModelType::Shop)
    {
        character->sendMsg("You can't deposit %s in %s.\n", item->getName(true), building->getName(true));
        return false;
    }
    // Set the quantity.
    auto quantity = args[0].getMultiplier();
    if (item->quantity <= quantity)
    {
        building->toShopItem()->balance += item->getPrice(true);
        MudUpdater::instance().addItemToDestroy(item);
    }
    else
    {
        item->quantity -= quantity;
        building->toShopItem()->balance += item->getPrice(false) * quantity;
    }
    return true;
}

bool DoSell(Character * character, ArgumentHandler & args)
{
    // Stop any action the character is executing.
    StopAction(character);
    // Check the number of arguments.
    if (args.size() != 2)
    {
        character->sendMsg("Sell what to whom?\n");
        return false;
    }

    // Get the item and the target.
    auto item = character->findInventoryItem(args[0].getContent(), args[0].getIndex());
    // Check the item.
    if (item == nullptr)
    {
        character->sendMsg("You don't have that item.\n");
        return false;
    }
    if (item->getType() == ModelType::Currency)
    {
        character->sendMsg("You can't sell %s.\n", item->getName(true));
        return false;
    }

    // Get the target.
    auto target = character->room->findItem(args[1].getContent(), args[1].getIndex());
    if (target == nullptr)
    {
        character->sendMsg("You don't see '%s' here.\n", args[1].getContent());
        return false;
    }
    if (target->getType() != ModelType::Shop)
    {
        character->sendMsg("%s is not a shop.\n", target->getNameCapital());
        return false;
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
        return false;
    }

    auto shopKeeper = shop->shopKeeper;
    if (!shop->canContain(item, quantity))
    {
        auto phrase = "The shop is full, come back another day.";
        shopKeeper->doCommand("say " + character->getName() + " " + phrase);
        return false;
    }

    auto price = shop->evaluateSellPrice(item) * quantity;
    if (shop->balance < price)
    {
        auto phrase = "I can't afford to buy your goods.";
        shopKeeper->doCommand("say " + character->getName() + " " + phrase);
        return false;
    }

    auto currency = shopKeeper->faction->currency;

    // Give the coins to the character.
    auto coins = currency->generateCurrency(shopKeeper->getName(), price);
    if (coins.empty())
    {
        character->sendMsg("You failed to sell %s.\n", item->getName(true));
        return false;
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
            return false;
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
    return true;
}

bool DoBuy(Character * character, ArgumentHandler & args)
{
    // Stop any action the character is executing.
    StopAction(character);
    // Check the number of arguments.
    if (args.size() != 2)
    {
        character->sendMsg("Buy what from which shop?\n");
        return false;
    }

    // Get the target.
    auto target = character->room->findItem(args[1].getContent(), args[1].getIndex());
    if (target == nullptr)
    {
        character->sendMsg("You don't see '%s' here.\n", args[1].getContent());
        return false;
    }
    if (target->getType() != ModelType::Shop)
    {
        character->sendMsg("%s is not a shop.\n", target->getNameCapital(true));
        return false;
    }

    // Get the shop.
    auto shop = target->toShopItem();
    std::string error;
    if (!shop->canUse(error))
    {
        character->sendMsg(error + "\n");
        return false;
    }
    auto shopKeeper = shop->shopKeeper;

    // Get the item.
    auto item = shop->findContent(args[0].getContent(), args[0].getIndex());
    // Check the item.
    if (item == nullptr)
    {
        auto phrase = "There is no " + args[0].getContent() + " on sale.\n";
        shopKeeper->doCommand("say " + character->getName() + " " + phrase);
        return false;
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
        return false;
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
        return false;
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
            return false;
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
            return false;
        }
        character->addInventoryItem(newStack);
        character->sendMsg("You buy part of %s from %s.\n", item->getName(true), shop->getName(true));
    }
    for (auto coin : changedCoins)
    {
        character->addInventoryItem(coin);
    }
    for (auto iterator : givenCoins)
    {
        auto coin = iterator.first;
        if (iterator.second == coin->quantity)
        {
            MudUpdater::instance().addItemToDestroy(coin);
        }
        else
        {
            coin->quantity -= iterator.second;
            coin->updateOnDB();
        }
    }
    SQLiteDbms::instance().endTransaction();
    return true;
}

bool DoBalance(Character * character, ArgumentHandler & args)
{
    // Check the number of arguments.
    if (!args.empty())
    {
        character->sendMsg("You have provided unecessary arguments.\n");
        return false;
    }
    unsigned int balance = 0;
    for (auto coin : character->findCoins())
    {
        balance += coin->getPrice(true);
    }
    character->sendMsg("Your balance is: %s.\n", ToString(balance));
    return true;
}

bool DoTurn(Character * character, ArgumentHandler & args)
{
    // Stop any action the character is executing.
    StopAction(character);
    // Check the number of arguments.
    if (args.size() != 1)
    {
        character->sendMsg("What do you want to turn on/off?\n");
        return false;
    }
    // Retrieve the item that has to be turned on/off.
    auto item = character->findEquipmentItem(args[0].getContent(), args[0].getIndex());
    if (item == nullptr)
    {
        item = character->findInventoryItem(args[0].getContent(), args[0].getIndex());
        if (item == nullptr)
        {
            item = character->room->findItem(args[0].getContent(), args[0].getIndex());
            if (item == nullptr)
            {
                character->sendMsg("You don't see '%s' anywhere.\n", args[0].getContent());
                return false;
            }
        }
    }
    if (item->getType() == ModelType::Light)
    {
        auto lightItem = item->toLightItem();
        if (lightItem->active)
        {
            character->sendMsg("You turn off %s.\n", item->getName(true));
            lightItem->active = false;
            return true;
        }
        else
        {
            if (lightItem->getRemainingFuel() > 0)
            {
                character->sendMsg("You turn on %s.\n", item->getName(true));
                lightItem->active = true;
                return true;
            }
            else
            {
                character->sendMsg("You cannot turn on %s.\n", item->getName(true));
            }
        }
    }
    return false;
}

bool DoRefill(Character * character, ArgumentHandler & args)
{
    // Stop any action the character is executing.
    StopAction(character);
    // Check the number of arguments.
    if (args.size() != 2)
    {
        character->sendMsg("What do you want to refill with what?\n");
        return false;
    }
    // Retrieve the item that has to be refilled.
    auto itemToRefill = character->findEquipmentItem(args[0].getContent(), args[0].getIndex());
    if (itemToRefill == nullptr)
    {
        itemToRefill = character->findInventoryItem(args[0].getContent(), args[0].getIndex());
        if (itemToRefill == nullptr)
        {
            character->sendMsg("You don't have %s.\n", args[0].getContent());
            return false;
        }
    }
    // Retrieve the fuel.
    auto fuel = character->findInventoryItem(args[1].getContent(), args[1].getIndex());
    if (fuel == nullptr)
    {
        character->sendMsg("You don't have %s.\n", args[1].getContent());
        return false;
    }
    std::string error;
    unsigned int ammountToLoad = 0;
    if (!itemToRefill->toLightItem()->getAmmountToRefill(fuel, ammountToLoad, error))
    {
        character->sendMsg(error + "\n");
        return false;
    }
    // Start a transaction.
    SQLiteDbms::instance().beginTransaction();
    if (fuel->quantity <= ammountToLoad)
    {
        // Remove the item from the player's inventory.
        character->remInventoryItem(fuel);
        // Put the item inside the container.
        itemToRefill->putInside(fuel);
        // Send the messages.
        character->sendMsg("You refill %s with %s.\n", itemToRefill->getName(true), fuel->getName(true));
        character->room->sendToAll("%s refills %s with %s.\n",
                                   {character},
                                   character->getNameCapital(), itemToRefill->getName(true), fuel->getName(true));
    }
    else
    {
        // Remove from the stack.
        auto newStack = fuel->removeFromStack(character, ammountToLoad);
        if (newStack == nullptr)
        {
            character->sendMsg("You failed to refill %s with part of %s.\n",
                               itemToRefill->getName(true),
                               fuel->getName(true));
            // Rollback the transaction.
            SQLiteDbms::instance().rollbackTransection();
            return false;
        }
        // Put the stack inside the container.
        itemToRefill->putInside(newStack);
        // Send the messages.
        character->sendMsg("You put refill %s with part of %s.\n", itemToRefill->getName(true), fuel->getName(true));
        character->room->sendToAll("%s refills %s with part of %s.\n",
                                   {character},
                                   character->getNameCapital(), itemToRefill->getName(true), fuel->getName(true));
    }
    // Conclude the transaction.
    SQLiteDbms::instance().endTransaction();
    return true;
}
