/// @file   manager.cpp
/// @brief  Implements the commands used to <b>manage mobiles</b>.
/// @author Enrico Fraccaroli
/// @date   Aug 01 2016
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

#include "manager.hpp"

#include "mud.hpp"
#include "shopItem.hpp"

void LoadManagerCommands()
{
    {
        Command command;
        command.name = "assign";
        command.help = "Allows to assign a mobile to a task/building.";
        command.args = "(mobile)(building)";
        command.hndl = DoAssign;
        Mud::instance().addCommand(command);
    }
}

void DoAssign(Character * character, ArgumentHandler & args)
{
    // Stop any action the character is executing.
    StopAction(character);
    if (args.size() != 2)
    {
        character->sendMsg("You need to specify who you want assign to which building.\n");
        return;
    }
    auto mobile = character->room->findMobile(args[0].getContent(), args[0].getIndex(), {});
    if (mobile == nullptr)
    {
        character->sendMsg("You don't see that person.\n");
        return;
    }
    auto building = character->room->findBuilding(args[1].getContent(), args[1].getIndex());
    if (building == nullptr)
    {
        character->sendMsg("You don't see the desired building here.\n");
        return;
    }
    if (building->getType() == ModelType::Shop)
    {
        auto shop = building->toShopItem();
        if (mobile->managedItem != nullptr)
        {
            if (mobile->managedItem == shop)
            {
                character->sendMsg(
                    "%s is already assigned to %s.\n",
                    mobile->getNameCapital(),
                    building->getName(true));
            }
            else
            {
                character->sendMsg(
                    "%s is already assigned to another shop.\n",
                    mobile->getNameCapital());
            }
        }
        else
        {
            shop->setNewShopKeeper(mobile);
            character->sendMsg("You assign %s to %s.\n", mobile->getName(), building->getName(true));
        }
    }
    else
    {
        character->sendMsg("You cannot assign %s to %s.\n", mobile->getName(), building->getName(true));
    }
}
