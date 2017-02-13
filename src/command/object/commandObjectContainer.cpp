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
#include "roomUtilityFunctions.hpp"
#include "command.hpp"
#include "room.hpp"

bool DoOrganize(Character * character, ArgumentHandler & args)
{
    // Check if the character is sleeping.
    if (character->posture == CharacterPosture::Sleep)
    {
        character->sendMsg("Not while you're sleeping.\n");
        return false;
    }
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
        auto item = character->findNearbyItem(args[1].getContent(),
                                              args[1].getIndex());
        if (item != nullptr)
        {
            if (item->isAContainer())
            {
                // Cast the item to container.
                if (item->isEmpty())
                {
                    character->sendMsg("%s is empty\n",
                                       item->getNameCapital(true));
                    return false;
                }
                // Order the content of the container.
                item->content.orderBy(order);
                // Organize the target container.
                character->sendMsg("You have organized %s, by %s.\n",
                                   item->getName(true), name);
                return true;
            }
            character->sendMsg("You can't organize %s\n", item->getName(true));
            return false;
        }
        else if (BeginWith("inventory", args[1].getContent()))
        {
            character->inventory.orderBy(order);
            // Organize the target container.
            character->sendMsg("You have organized your inventory, by %s.\n",
                               name);
            return true;
        }
        character->sendMsg("What do you want to organize?\n");
    }
    else
    {
        character->sendMsg("Too much arguments.\n");
    }
    return false;
}

bool DoOpen(Character * character, ArgumentHandler & args)
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
        auto directions = GetAvailableDirections(character->room);
        if (!character->room->isLit() && !directions.empty())
        {
            auto it = TRand<size_t>(0, directions.size() - 1);
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

        ClearFlag(door->flags, ItemFlag::Closed);

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
        ClearFlag(container->flags, ItemFlag::Closed);
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
    // Check if the character is sleeping.
    if (character->posture == CharacterPosture::Sleep)
    {
        character->sendMsg("Not while you're sleeping.\n");
        return false;
    }
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
        auto directions = GetAvailableDirections(character->room);
        if (!character->room->isLit() && !directions.empty())
        {
            auto it = TRand<size_t>(0, directions.size() - 1);
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
        SetFlag(door->flags, ItemFlag::Closed);
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
        SetFlag(container->flags, ItemFlag::Closed);
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
