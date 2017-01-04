/// @file   commandObjectContainer.cpp
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

#include "commandObjectContainer.hpp"

#include "sqliteDbms.hpp"
#include "lightModel.hpp"
#include "lightItem.hpp"
#include "command.hpp"
#include "room.hpp"

bool DoOrganize(Character * character, ArgumentHandler & args)
{
    // Stop any action the character is executing.
    StopAction(character);
    // Check if the room is lit.
    if (!character->room->isLit())
    {
        character->sendMsg("You can't see.\n");
        return false;
    }
    // Check the number of arguments.
    if (args.empty())
    {
        character->sendMsg("Organize what?\n");
        return false;
    }
    auto order = ItemVector::Order::ByName;
    if (BeginWith("name", ToLower(args[0].getContent())))
    {
        order = ItemVector::Order::ByName;
    }
    else if (BeginWith("weight", ToLower(args[0].getContent())))
    {
        order = ItemVector::Order::ByWeight;
    }
    else if (BeginWith("price", ToLower(args[0].getContent())))
    {
        order = ItemVector::Order::ByPrice;
    }
    auto name = ItemVector::orderToString(order);
    if (args.size() == 1)
    {
        character->room->items.orderBy(order);
        character->sendMsg("You have organized the room by %s.\n", name);
    }
    else if (args.size() == 2)
    {
        auto container = character->findNearbyItem(args[1].getContent(),
                                                   args[1].getIndex());
        if (container != nullptr)
        {
            if (container->content.empty())
            {
                character->sendMsg(
                    "You can't organize " + container->getName(true) + "\n");
            }
            else
            {
                container->content.orderBy(order);
                // Organize the target container.
                character->sendMsg("You have organized %s, by %s.\n",
                                   container->getName(true), name);
                return true;
            }
        }
        else if (BeginWith("inventory", args[1].getContent()))
        {
            character->inventory.orderBy(order);
            // Organize the target container.
            character->sendMsg("You have organized your inventory, by %s.\n",
                               name);
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
    // Check if the character want to open something in another direction.
    auto direction = Direction(args[0].getContent(), false);
    if (direction != Direction::None)
    {
        // If the room is NOT lit and HAS some exits, pick a random direction.
        auto directions = character->room->getAvailableDirections();
        if (!character->room->isLit() && !directions.empty())
        {
            auto it = TRandInteger<size_t>(0, directions.size() - 1);
            direction = directions[it];
        }
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
            character->room->sendToAll("%s opens a hidden door!\n",
                                       {character},
                                       character->getNameCapital());
        }
        else
        {
            character->sendMsg("You have opened the door.\n");
            // Send the message inside the room.
            character->room->sendToAll("%s opens a door.\n",
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
                it->destination->sendToAll(
                    "Someone opens a door from the other side.\n",
                    {});
            }
        }
    }
    else
    {
        Item * container = nullptr;
        // Check if the room is lit.
        bool roomIsLit = character->room->isLit();
        // Check if the inventory is lit.
        auto inventoryIsLit = character->inventoryIsLit();
        // If the room is lit.
        if (roomIsLit)
        {
            container = character->findNearbyItem(args[0].getContent(),
                                                  args[0].getIndex());
        }
        else
        {
            // If the room is not lit but the inventory is.
            if (inventoryIsLit)
            {
                container = character->findInventoryItem(args[0].getContent(),
                                                         args[0].getIndex());
            }
            else
            {
                character->sendMsg("You can't see.\n");
                return false;
            }
        }
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
    // Check if the character want to open something in another direction.
    auto direction = Direction(args[0].getContent(), false);
    if (direction != Direction::None)
    {
        // If the room is NOT lit and HAS some exits, pick a random direction.
        auto directions = character->room->getAvailableDirections();
        if (!character->room->isLit() && !directions.empty())
        {
            auto it = TRandInteger<size_t>(0, directions.size() - 1);
            direction = directions[it];
        }
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
            character->sendMsg(
                "There are items on the way, you can't close the door.\n");
            return false;
        }
        if (destination->characters.size() >= 1)
        {
            character->sendMsg(
                "There are someone on the way, you can't close the door.\n");
            return false;
        }
        SetFlag(&door->flags, ItemFlag::Closed);
        // Display message.
        if (HasFlag(roomExit->flags, ExitFlag::Hidden))
        {
            character->sendMsg("You have closed a hidden door!\n");
            // Send the message inside the room.
            character->room->sendToAll("%s closes a hidden door!\n",
                                       {character},
                                       character->getNameCapital());
        }
        else
        {
            character->sendMsg("You have closed the door.\n");
            // Send the message inside the room.
            character->room->sendToAll("%s closes a door.\n",
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
                it->destination->sendToAll(
                    "Someone closes a door from the other side.\n",
                    {});
            }
        }
    }
    else
    {
        Item * container = nullptr;
        // Check if the room is lit.
        bool roomIsLit = character->room->isLit();
        // Check if the inventory is lit.
        auto inventoryIsLit = character->inventoryIsLit();
        // If the room is lit.
        if (roomIsLit)
        {
            container = character->findNearbyItem(args[0].getContent(),
                                                  args[0].getIndex());
        }
        else
        {
            // If the room is not lit but the inventory is.
            if (inventoryIsLit)
            {
                container = character->findInventoryItem(args[0].getContent(),
                                                         args[0].getIndex());
            }
            else
            {
                character->sendMsg("You can't see.\n");
                return false;
            }
        }
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
    Item * item = nullptr;
    // Check if the room is lit.
    bool roomIsLit = character->room->isLit();
    // Check if the inventory is lit.
    auto inventoryIsLit = character->inventoryIsLit();
    // If the room is lit.
    if (roomIsLit)
    {
        item = character->findNearbyItem(args[0].getContent(),
                                         args[0].getIndex());
    }
    else
    {
        // If the room is not lit but the inventory is.
        if (inventoryIsLit)
        {
            item = character->findInventoryItem(args[0].getContent(),
                                                args[0].getIndex());
        }
        else if (!character->inventory.empty())
        {
            // If the inventory is NOT empty, pick a random item.
            auto it = TRandInteger<size_t>(0, character->inventory.size() - 1);
            item = character->inventory[it];
        }
    }
    if (item == nullptr)
    {
        character->sendMsg("You don't see '%s' anywhere.\n",
                           args[0].getContent());
        return false;
    }
    if (item->getType() == ModelType::Light)
    {
        auto lightItem = item->toLightItem();
        if (lightItem->active)
        {
            if (lightItem->model->toLight()->alwaysActive)
            {
                character->sendMsg("You cannot turn off %s.\n",
                                   item->getName(true));
                return false;
            }
            character->sendMsg("You turn off %s.\n", item->getName(true));
            lightItem->active = false;
            return true;
        }
        else
        {
            if (lightItem->model->toLight()->alwaysActive)
            {
                character->sendMsg("You cannot turn on %s.\n",
                                   item->getName(true));
                return false;
            }
            if (lightItem->getAutonomy() > 0)
            {
                character->sendMsg("You turn on %s.\n", item->getName(true));
                lightItem->active = true;
                return true;
            }
            else
            {
                character->sendMsg("You cannot turn on %s.\n",
                                   item->getName(true));
            }
        }
    }
    else
    {
        character->sendMsg("%s is not a light source.\n",
                           item->getNameCapital(true));
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
    Item * itemToRefill = nullptr;
    Item * fuel = nullptr;
    // Check if the room is lit.
    bool roomIsLit = character->room->isLit();
    // Check if the inventory is lit.
    auto inventoryIsLit = character->inventoryIsLit();
    // If the room is lit.
    if (roomIsLit)
    {
        itemToRefill = character->findNearbyItem(args[0].getContent(),
                                                 args[0].getIndex());
        fuel = character->findNearbyItem(args[1].getContent(),
                                         args[1].getIndex());
    }
    else
    {
        // If the room is not lit but the inventory is.
        if (inventoryIsLit)
        {
            itemToRefill = character->findInventoryItem(args[0].getContent(),
                                                        args[0].getIndex());
            fuel = character->findInventoryItem(args[1].getContent(),
                                                args[1].getIndex());
        }
        else
        {
            character->sendMsg("You can't do that without seeing,"
                                   "you could waste most of the fuel.\n");
            return false;
        }
    }
    if (itemToRefill == nullptr)
    {
        character->sendMsg("You don't have '%s'.\n", args[0].getContent());
        return false;
    }
    if (fuel == nullptr)
    {
        character->sendMsg("You don't have '%s'.\n", args[1].getContent());
        return false;
    }
    std::string error;
    unsigned int amountToLoad = 0;
    if (!itemToRefill->toLightItem()->getAmountToRefill(fuel,
                                                        amountToLoad,
                                                        error))
    {
        character->sendMsg(error + "\n");
        return false;
    }
    // Start a transaction.
    SQLiteDbms::instance().beginTransaction();
    if (fuel->quantity <= amountToLoad)
    {
        // Remove the item from the player's inventory.
        character->remInventoryItem(fuel);
        // Put the item inside the container.
        itemToRefill->putInside(fuel);
        // Send the messages.
        character->sendMsg("You refill %s with %s.\n",
                           itemToRefill->getName(true),
                           fuel->getName(true));
        character->room->sendToAll("%s refills %s with %s.\n",
                                   {character},
                                   character->getNameCapital(),
                                   itemToRefill->getName(true),
                                   fuel->getName(true));
    }
    else
    {
        // Remove from the stack.
        auto newStack = fuel->removeFromStack(character, amountToLoad);
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
        character->sendMsg("You put refill %s with part of %s.\n",
                           itemToRefill->getName(true),
                           fuel->getName(true));
        character->room->sendToAll("%s refills %s with part of %s.\n",
                                   {character},
                                   character->getNameCapital(),
                                   itemToRefill->getName(true),
                                   fuel->getName(true));
    }
    // Conclude the transaction.
    SQLiteDbms::instance().endTransaction();
    return true;
}
