/// @file   commObjectLiquids.cpp
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

#include "commandObjectLiquids.hpp"
#include "liquidContainerModel.hpp"
#include "command.hpp"
#include "room.hpp"

bool DoDrink(Character * character, ArgumentHandler & args)
{
    // Stop any action the character is executing.
    StopAction(character);
    if (args.empty())
    {
        character->sendMsg("Drink from what?\n");
        return false;
    }
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
            // If the inventory is NOT empty, pick a random item.
            if (!character->inventory.empty())
            {
                auto it = TRandInteger<size_t>(0,
                                               character->inventory.size() - 1);
                container = character->inventory[it];
            }
        }
    }
    if (container == nullptr)
    {
        character->sendMsg("You don't see '%s'.\n", args[0].getContent());
        return false;
    }
    // Execute every necessary checks.
    if (HasFlag(container->flags, ItemFlag::Locked))
    {
        character->sendMsg("You have first to unlock %s.\n",
                           container->getName(true));
        return false;
    }
    if (HasFlag(container->flags, ItemFlag::Closed))
    {
        character->sendMsg("You have first to open %s.\n",
                           container->getName(true));
        return false;
    }
    if (container->model->getType() != ModelType::LiquidContainer)
    {
        character->sendMsg("%s is not a container for liquids.\n",
                           container->getNameCapital(true));
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
    character->sendMsg("You drink some %s from %s.\n",
                       liquid->getName(),
                       container->getName(true));
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
    Item * container = nullptr;
    Item * source = nullptr;
    // Check if the room is lit.
    bool roomIsLit = character->room->isLit();
    // Check if the inventory is lit.
    auto inventoryIsLit = character->inventoryIsLit();
    // If the room is lit.
    if (roomIsLit)
    {
        container = character->findNearbyItem(args[0].getContent(),
                                              args[0].getIndex());
        source = character->findNearbyItem(args[1].getContent(),
                                           args[1].getIndex());
    }
    else
    {
        // If the room is not lit but the inventory is.
        if (inventoryIsLit)
        {
            container = character->findInventoryItem(args[0].getContent(),
                                                     args[0].getIndex());
            source = character->findInventoryItem(args[1].getContent(),
                                                  args[1].getIndex());
        }
        else
        {
            character->sendMsg("You can't do that without seeing,"
                                   "you could waste most of the liquid.\n");
            return false;
        }
    }
    if (container == nullptr)
    {
        character->sendMsg("You don't have any '%s' with you.\n",
                           args[0].getContent());
        return false;
    }
    if (source == nullptr)
    {
        character->sendMsg("You don't see any '%s'.\n", args[1].getContent());
        return false;
    }
    if (HasFlag(source->flags, ItemFlag::Locked))
    {
        character->sendMsg("You have first to unlock %s.\n",
                           source->getName(true));
        return false;
    }
    if (HasFlag(source->flags, ItemFlag::Closed))
    {
        character->sendMsg("You have first to open %s.\n",
                           source->getName(true));
        return false;
    }
    if (HasFlag(container->flags, ItemFlag::Locked))
    {
        character->sendMsg("You have first to unlock %s.\n",
                           container->getName(true));
        return false;
    }
    if (HasFlag(container->flags, ItemFlag::Closed))
    {
        character->sendMsg("You have first to open %s.\n",
                           container->getName(true));
        return false;
    }
    // Check if the items are suitable source and container of liquids.
    if (container->model->getType() != ModelType::LiquidContainer)
    {
        character->sendMsg("%s is not a suitable container.\n",
                           container->getNameCapital());
        return false;
    }
    if (source->model->getType() != ModelType::LiquidContainer)
    {
        character->sendMsg("%s is not a suitable source of liquids.\n",
                           source->getNameCapital());
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
        character->sendMsg("You failed to take out the liquid from a %s.\n",
                           source->getNameCapital());
        return false;
    }
    if (!container->pourIn(sourLiquid, quantity))
    {
        character->sendMsg(
            "You failed to fill the container with the liquid.\n");
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
    Item * container = nullptr;
    Item * source = nullptr;
    // Check if the room is lit.
    bool roomIsLit = character->room->isLit();
    // Check if the inventory is lit.
    auto inventoryIsLit = character->inventoryIsLit();
    // If the room is lit.
    if (roomIsLit)
    {
        source = character->findNearbyItem(args[0].getContent(),
                                           args[0].getIndex());
        container = character->findNearbyItem(args[1].getContent(),
                                              args[1].getIndex());
    }
    else
    {
        // If the room is not lit but the inventory is.
        if (inventoryIsLit)
        {
            source = character->findInventoryItem(args[0].getContent(),
                                                  args[0].getIndex());
            container = character->findInventoryItem(args[1].getContent(),
                                                     args[1].getIndex());
        }
        else
        {
            character->sendMsg("You can't do that without seeing,"
                                   "you could waste most of the liquid.\n");
            return false;
        }
    }
    if (HasFlag(source->flags, ItemFlag::Locked))
    {
        character->sendMsg("You have first to unlock %s.\n",
                           source->getName(true));
        return false;
    }
    if (HasFlag(source->flags, ItemFlag::Closed))
    {
        character->sendMsg("You have first to open %s.\n",
                           source->getName(true));
        return false;
    }

    if (HasFlag(container->flags, ItemFlag::Locked))
    {
        character->sendMsg("You have first to unlock %s.\n",
                           container->getName(true));
        return false;
    }
    if (HasFlag(container->flags, ItemFlag::Closed))
    {
        character->sendMsg("You have first to open %s.\n",
                           container->getName(true));
        return false;
    }
    // Check if the items are suitable source and container of liquids.
    if (container->model->getType() != ModelType::LiquidContainer)
    {
        character->sendMsg("%s is not a suitable container.\n",
                           container->getNameCapital());
        return false;
    }
    if (source->model->getType() != ModelType::LiquidContainer)
    {
        character->sendMsg("%s is not a suitable source of liquids.\n",
                           source->getNameCapital());
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
        character->sendMsg("You failed to pour out the liquid from %s.\n",
                           source->getName(true));
        return false;
    }
    if (!container->pourIn(sourLiquid, quantity))
    {
        character->sendMsg("You failed to pour the liquid into %s.\n",
                           source->getName(true));
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
