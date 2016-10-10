/// @file   combat.cpp
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

#include "combat.hpp"
#include "../mud.hpp"
#include "../action/scoutAction.hpp"
#include "../action/reloadAction.hpp"
#include "../action/loadAction.hpp"
#include "../action/unloadAction.hpp"
#include "../model/magazineModel.hpp"

void LoadCombatCommands()
{
    Command command;
    command.level = 0;
    {
        command.name = "kill";
        command.help = "Engage in combat the desired target.";
        command.args = "(target)";
        command.hndl = DoKill;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "flee";
        command.help = "Try to flee from combat.";
        command.args = "";
        command.hndl = DoFlee;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "scout";
        command.help = "Provides information about the surrounding area.";
        command.args = "";
        command.hndl = DoScout;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "load";
        command.help = "Allows to load a magazine with projectiles.";
        command.args = "";
        command.hndl = DoLoad;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "unload";
        command.help = "Allows to unload a magazine.";
        command.args = "";
        command.hndl = DoUnload;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "reload";
        command.help = "Allows to reload a firearm.";
        command.args = "";
        command.hndl = DoReload;
        Mud::instance().addCommand(command);
    }
}

void DoKill(Character * character, ArgumentHandler & args)
{
    // If there are no arguments, show the room.
    if (args.size() != 1)
    {
        character->sendMsg("You have to specify whom to kill.\n");
    }
    // Retrieve the target.
    auto target = character->room->findCharacter(args[0].getContent(), args[0].getIndex(), {
        character});
    if (!target)
    {
        character->sendMsg("You don't see '%s' anywhere.\n", args[0].getContent());
        return;
    }
    // Check if the attacker can see the target.
    if (!character->canSee(target))
    {
        character->sendMsg("You don't see '%s' anywhere.\n", args[0].getContent());
        return;
    }

    // Check if the two characters are both already in combat.
    if (character->getAction()->getType() == ActionType::Combat)
    {
        // Check if the character is trying to attack a target with which is not in combat.
        if (!character->opponents.hasOpponent(target))
        {
            character->sendMsg("You have already your share of troubles!\n");
            return;
        }
        // Set the target as a top aggro.
        if (character->opponents.moveToTopAggro(target))
        {
            character->sendMsg("You focus your attacks on %s!\n", target->getName());
        }
        else
        {
            character->sendMsg("You are already doing your best to kill %s!\n", target->getName());
        }
    }
        // Check if the character is attacking a target which is already in combat.
    else if ((character->getAction()->getType() != ActionType::Combat)
             && (target->getAction()->getType() == ActionType::Combat))
    {
        // Set the opponents.
        if (!character->opponents.addOpponent(target))
        {
            character->sendMsg("You are already fighting againts %s.\n", target->getName());
            return;
        }
        if (!target->opponents.addOpponent(character))
        {
            character->sendMsg("You were not ablet to attack %s.\n", target->getName());
            character->opponents.remOpponent(target);
            return;
        }
        // Notify the character.
        character->sendMsg("You attack %s.\n", target->getName());
        // Notify the target.
        target->sendMsg("%s attacks you.\n\n", character->getNameCapital());
        // Notify the others.
        character->room->sendToAll(
            "%s attacks %s.\n",
            {character, target},
            character->getNameCapital(),
            target->getName());
        // Let the characters enter the combat.
        if (!character->setNextCombatAction(CombatActionType::BasicAttack))
        {
            character->sendMsg("You were not ablet to attack %s.\n", target->getName());
            character->opponents.remOpponent(target);
            target->opponents.remOpponent(character);
            return;
        }
    }
    else
    {
        // Set the opponents.
        if (!character->opponents.addOpponent(target))
        {
            character->sendMsg("You are already fighting againts %s.\n", target->getName());
            return;
        }
        if (!target->opponents.addOpponent(character))
        {
            character->sendMsg("You were not ablet to attack %s.\n", target->getName());
            character->opponents.remOpponent(target);
            return;
        }
        // Notify the character.
        character->sendMsg("You attack %s.\n", target->getName());
        // Notify the target.
        target->sendMsg("%s attacks you.\n\n", character->getNameCapital());
        // Notify the others.
        character->room->sendToAll(
            "%s attacks %s.\n",
            {character, target},
            character->getNameCapital(),
            target->getName());

        // Let the characters enter the combat.
        if (!character->setNextCombatAction(CombatActionType::BasicAttack))
        {
            character->sendMsg("You were not ablet to attack %s.\n", target->getName());
            character->opponents.remOpponent(target);
            target->opponents.remOpponent(character);
            return;
        }
        if (!target->setNextCombatAction(CombatActionType::BasicAttack))
        {
            character->sendMsg("You were not ablet to attack %s.\n", target->getName());
            character->opponents.remOpponent(target);
            target->opponents.remOpponent(character);
            return;
        }
    }
}

void DoFlee(Character * character, ArgumentHandler & /*args*/)
{
    // Check if the character is in combat.
    if (character->getAction()->getType() != ActionType::Combat)
    {
        character->sendMsg("You are not fighting.\n");
        return;
    }
    auto combatAction = character->getAction()->toCombatAction();
    // Check if the character is already trying to flee.
    if (combatAction->getCombatActionType() == CombatActionType::Flee)
    {
        character->sendMsg("You are already trying to flee.\n");
        return;
    }
    character->sendMsg("You prepare to flee...\n");
    character->setNextCombatAction(CombatActionType::Flee);
}

void DoScout(Character * character, ArgumentHandler & /*args*/)
{
    auto requiredTime = 3;
    auto newAction = std::make_shared<ScoutAction>(character, requiredTime);
    // Check the new action.
    if (!newAction->check())
    {
        character->sendMsg("You can't scout the area.\n");
        return;
    }
    // Send the starting message.
    character->sendMsg("You start scouting the area...\n");
    // Set the new action.
    character->setAction(newAction);
}

void DoLoad(Character * character, ArgumentHandler & args)
{
    if (args.size() != 2)
    {
        character->sendMsg("What do you want to load with what?\n");
    }
    else
    {
        auto magazine = character->findNearbyItem(args[0].getContent(), args[0].getIndex());
        if (magazine == nullptr)
        {
            character->sendMsg("You don't have %s.\n", args[0].getContent());
        }
        else
        {
            if (magazine->getType() != ModelType::Magazine)
            {
                character->sendMsg("You can't load %s.\n", magazine->getName(true));
            }
            else
            {
                auto projectile = character->findNearbyItem(args[1].getContent(), args[1].getIndex());
                if (projectile == nullptr)
                {
                    character->sendMsg("You don't have %s.\n", args[1].getContent());
                }
                else
                {
                    if (projectile->getType() != ModelType::Projectile)
                    {
                        character->sendMsg("You can't load %s with %s.\n",
                                           magazine->getName(true),
                                           projectile->getName(true));
                    }
                    else
                    {
                        auto ammountToLoad = magazine->model->toMagazine()->maxAmmount;
                        if (!magazine->isEmpty())
                        {
                            auto alreadyLoadedProjectile = magazine->content.front();
                            unsigned int ammountAlreadyLoaded = 0;
                            if (alreadyLoadedProjectile == nullptr)
                            {
                                character->sendMsg("Something is gone wrong while you were loading %s...\n\n",
                                                   magazine->getName(true));
                                return;
                            }
                            // If there are projectiles inside, check if the two types of projectiles are compatible.
                            if (!projectile->canStackWith(alreadyLoadedProjectile))
                            {
                                character->sendMsg(
                                    "The magazine already contains a different type of projectiles...\n\n");
                                return;
                            }
                            // Set the ammount of already loaded projectiles.
                            ammountAlreadyLoaded = alreadyLoadedProjectile->quantity;
                            if (ammountToLoad <= ammountAlreadyLoaded)
                            {
                                character->sendMsg("The magazine is already at full capacity...\n\n");
                                return;
                            }
                            ammountToLoad -= ammountAlreadyLoaded;
                        }
                        double requiredTime = 0.5 * ammountToLoad;
                        auto newAction = std::make_shared<LoadAction>(magazine, projectile, character, requiredTime);
                        // Check the new action.
                        if (!newAction->check())
                        {
                            character->sendMsg("You can't load %s.\n", magazine->getName(true));
                        }
                        else
                        {
                            // Send the starting message.
                            character->sendMsg("You start loading %s with %s.\n",
                                               magazine->getName(true),
                                               projectile->getName(true));
                            // Set the new action.
                            character->setAction(newAction);
                        }
                    }
                }
            }
        }
    }
}

void DoUnload(Character * character, ArgumentHandler & args)
{
    if (args.size() != 1)
    {
        character->sendMsg("What do you want to unload?\n");
        return;
    }
    auto itemToUnload = character->findEquipmentItem(args[0].getContent(), args[0].getIndex());
    if (itemToUnload == nullptr)
    {
        itemToUnload = character->findInventoryItem(args[0].getContent(), args[0].getIndex());
        if (itemToUnload == nullptr)
        {
            character->sendMsg("You don't have %s.\n", args[0].getContent());
            return;
        }
    }
    if ((itemToUnload->getType() != ModelType::Magazine) && (itemToUnload->getType() != ModelType::RangedWeapon))
    {
        character->sendMsg("You can't unload %s.\n", itemToUnload->getName(true));
        return;
    }
    if (itemToUnload->isEmpty())
    {
        character->sendMsg("%s is already empty...\n", itemToUnload->getNameCapital(true));
        return;
    }
    auto loadedItem = itemToUnload->content.front();
    if (loadedItem == nullptr)
    {
        character->sendMsg("Something is gone wrong while you were unloading %s...\n\n",
                           itemToUnload->getName(true));
        return;
    }
    // Set the required time to unloaded the item.
    double requiredTime = 0.0;
    if (itemToUnload->getType() == ModelType::Magazine)
    {
        requiredTime = 0.25 * loadedItem->quantity;
    }
    else if (itemToUnload->getType() != ModelType::RangedWeapon)
    {
        requiredTime = 1.0;
    }
    // Create the unload action.
    auto newAction = std::make_shared<UnloadAction>(loadedItem, character, requiredTime);
    // Check the new action.
    if (!newAction->check())
    {
        character->sendMsg("You can't unload %s.\n", itemToUnload->getName(true));
    }
    else
    {
        // Send the starting message.
        character->sendMsg("You start unloading %s.\n", itemToUnload->getName(true));
        // Set the new action.
        character->setAction(newAction);
    }
}

void DoReload(Character * character, ArgumentHandler & args)
{
    if (args.size() != 2)
    {
        character->sendMsg("What do you want to reload with what?\n");
        return;
    }
    auto posessedItem = character->findEquipmentItem(args[0].getContent(), args[0].getIndex());
    if (posessedItem == nullptr)
    {
        posessedItem = character->findInventoryItem(args[0].getContent(), args[0].getIndex());
        if (posessedItem == nullptr)
        {
            character->sendMsg("You don't have %s.\n", args[0].getContent());
            return;
        }
    }
    if (posessedItem->getType() != ModelType::RangedWeapon)
    {
        character->sendMsg("You can't reload %s.\n", posessedItem->getName(true));
        return;
    }
    auto weapon = posessedItem->toRangedWeaponItem();
    auto magazine = character->findNearbyItem(args[1].getContent(), args[1].getIndex());
    if (magazine == nullptr)
    {
        character->sendMsg("You don't have %s.\n", args[1].getContent());
        return;
    }
    if (magazine->getType() != ModelType::Magazine)
    {
        character->sendMsg("You can't reload %s with %s.\n",
                           weapon->getName(true),
                           magazine->getName(true));
        return;
    }
    auto requiredTime = 2;
    auto newAction = std::make_shared<ReloadAction>(weapon, magazine, character, requiredTime);
    // Check the new action.
    if (!newAction->check())
    {
        character->sendMsg("You can't reload %s.\n", weapon->getName(true));
    }
    else
    {
        // Send the starting message.
        character->sendMsg("You start reloading %s with %s.\n",
                           weapon->getName(true),
                           magazine->getName(true));
        // Set the new action.
        character->setAction(newAction);
    }
}
