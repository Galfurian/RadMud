/// @file   manager.cpp
/// @brief  Implements the commands used to <b>manage mobiles</b>.
/// @author Enrico Fraccaroli
/// @date   Aug 01 2016
/// @copyright
/// Copyright (c) 2016 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
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

#include "../room.hpp"
#include "../mobile.hpp"
#include "../item/shopItem.hpp"
#include "../sqlite/sqliteDbms.hpp"

#include "argumentHandler.hpp"

void DoAssign(Character * character, ArgumentHandler & args)
{
    // Check if the player it's already doing something.
    StopAction(character);
    if (args.size() != 2)
    {
        character->sendMsg("You need to specify who you want assign to which building.\n");
        return;
    }
    Mobile * mobile = character->room->findMobile(args[0].getContent(), args[0].getIndex(), { });
    if (mobile == nullptr)
    {
        character->sendMsg("You don't see that person.\n");
        return;
    }
    Item * building = character->room->findBuilding(args[1].getContent(), args[1].getIndex());
    if (building == nullptr)
    {
        character->sendMsg("You don't see the desired building here.\n");
        return;
    }
    if (building->getType() == ModelType::Shop)
    {
        ShopItem * shop = building->toShopItem();
        if (mobile->managedItem != nullptr)
        {
            if (mobile->managedItem == shop)
            {
                character->sendMsg(
                    "%s is already assigned to %s.\n",
                    mobile->getNameCapital(),
                    building->getName());
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
            character->sendMsg("You assign %s to %s.\n", mobile->getName(), building->getName());
        }
    }
    else
    {
        character->sendMsg("You cannot assign %s to %s.\n", mobile->getName(), building->getName());
    }
}
