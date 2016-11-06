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
#include "mud.hpp"
#include "scoutAction.hpp"
#include "reloadAction.hpp"
#include "loadAction.hpp"
#include "unloadAction.hpp"
#include "aimAction.hpp"
#include "magazineItem.hpp"

void LoadCombatCommands()
{
    Command command;
    command.gods = false;
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
    {
        command.name = "aim";
        command.help = "Allows to aim a target.";
        command.args = "";
        command.hndl = DoAim;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "fire";
        command.help = "Allows to fire with an equipped ranged weapon to an aimed target.";
        command.args = "";
        command.hndl = DoFire;
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
        if (!character->combatHandler.hasOpponent(target))
        {
            character->sendMsg("You have already your share of troubles!\n");
            return;
        }
        auto predefinedTarget = character->combatHandler.getPredefinedTarget();
        if (predefinedTarget != nullptr)
        {
            if (predefinedTarget == target)
            {
                character->sendMsg("You are already doing your best to kill %s!\n", target->getName());
                return;
            }
        }
        character->sendMsg("You focus your attacks on %s!\n", target->getName());
        character->combatHandler.setPredefinedTarget(character->combatHandler.getAimedTarget());
    }
        // Check if the character is attacking a target which is already in combat.
    else if ((character->getAction()->getType() != ActionType::Combat)
             && (target->getAction()->getType() == ActionType::Combat))
    {
        // Set the combatHandler.
        if (!character->combatHandler.addOpponent(target))
        {
            character->sendMsg("You are already fighting againts %s.\n", target->getName());
            return;
        }
        if (!target->combatHandler.addOpponent(character))
        {
            character->sendMsg("You were not ablet to attack %s.\n", target->getName());
            character->combatHandler.remOpponent(target);
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
            character->combatHandler.remOpponent(target);
            target->combatHandler.remOpponent(character);
            return;
        }
    }
    else
    {
        // Set the combatHandler.
        if (!character->combatHandler.addOpponent(target))
        {
            character->sendMsg("You are already fighting againts %s.\n", target->getName());
            return;
        }
        if (!target->combatHandler.addOpponent(character))
        {
            character->sendMsg("You were not ablet to attack %s.\n", target->getName());
            character->combatHandler.remOpponent(target);
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
            character->combatHandler.remOpponent(target);
            target->combatHandler.remOpponent(character);
            return;
        }
        if (!target->setNextCombatAction(CombatActionType::BasicAttack))
        {
            character->sendMsg("You were not ablet to attack %s.\n", target->getName());
            character->combatHandler.remOpponent(target);
            target->combatHandler.remOpponent(character);
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
    std::string error;
    if (newAction->check(error))
    {
        // Send the starting message.
        character->sendMsg("You start scouting the area...\n");
        // Set the new action.
        character->setAction(newAction);
    }
    else
    {
        character->sendMsg("%s\n", error);
    }
}

void DoLoad(Character * character, ArgumentHandler & args)
{
    if (args.size() != 2)
    {
        character->sendMsg("What do you want to load with what?\n");
        return;
    }
    // Search the magazine.
    auto itemToLoad = character->findEquipmentItem(args[0].getContent(), args[0].getIndex());
    if (itemToLoad == nullptr)
    {
        itemToLoad = character->findInventoryItem(args[0].getContent(), args[0].getIndex());
        if (itemToLoad == nullptr)
        {
            character->sendMsg("You don't have %s.\n", args[0].getContent());
            return;
        }
    }
    // Check if the item is a magazine.
    if (itemToLoad->getType() != ModelType::Magazine)
    {
        character->sendMsg("You can't load %s.\n", itemToLoad->getName(true));
        return;
    }
    // Transform the item into a magazine.
    auto magazine = itemToLoad->toMagazineItem();
    // Search the projectiles.
    auto projectile = character->findEquipmentItem(args[1].getContent(), args[1].getIndex());
    if (projectile == nullptr)
    {
        projectile = character->findInventoryItem(args[1].getContent(), args[1].getIndex());
        if (projectile == nullptr)
        {
            character->sendMsg("You don't have %s.\n", args[1].getContent());
            return;
        }
    }
    // Check if the projectiles can be loaded inside the magazine.
    std::string error;
    unsigned int ammountToLoad = 0;
    if (!magazine->getAmountToLoad(projectile, ammountToLoad, error))
    {
        character->sendMsg("%s\n", error);
        return;
    }
    // Evaluates the required time for loading the magazine.
    double requiredTime = 0.5 * ammountToLoad;
    // Create the load action.
    auto newAction = std::make_shared<LoadAction>(magazine, projectile, character, requiredTime);
    // Check the new action.
    error = std::string();
    if (newAction->check(error))
    {
        // Send the starting message.
        character->sendMsg("You start loading %s with %s.\n",
                           magazine->getName(true),
                           projectile->getName(true));
        // Set the new action.
        character->setAction(newAction);
    }
    else
    {
        character->sendMsg("%s\n", error);
    }
}

void DoUnload(Character * character, ArgumentHandler & args)
{
    if (args.size() != 1)
    {
        character->sendMsg("What do you want to unload?\n");
        return;
    }
    // Search the magazine.
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
    // Check if the item is a magazine or a ranged weapon.
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
    // Set the required time to unloaded the item.
    double requiredTime = 2.0;
    if (itemToUnload->getType() == ModelType::Magazine)
    {
        auto loadedItem = itemToUnload->toMagazineItem()->getAlreadyLoadedProjectile();
        if (loadedItem == nullptr)
        {
            character->sendMsg("Something is gone wrong while you were unloading %s...\n\n",
                               itemToUnload->getName(true));
            return;
        }
        requiredTime = 0.25 * loadedItem->quantity;
    }
    Logger::log(LogLevel::Debug, "Required Time : %s", ToString(requiredTime));
    // Create the unload action.
    auto newAction = std::make_shared<UnloadAction>(itemToUnload, character, requiredTime);
    std::string error;
    // Check the new action.
    if (newAction->check(error))
    {
        // Send the starting message.
        character->sendMsg("You start unloading %s.\n", itemToUnload->getName(true));
        // Set the new action.
        character->setAction(newAction);
    }
    else
    {
        character->sendMsg("%s\n", error);
    }
}

void DoReload(Character * character, ArgumentHandler & args)
{
    if (args.size() != 2)
    {
        character->sendMsg("What do you want to reload with what?\n");
        return;
    }
    auto itemToReload = character->findEquipmentItem(args[0].getContent(), args[0].getIndex());
    if (itemToReload == nullptr)
    {
        itemToReload = character->findInventoryItem(args[0].getContent(), args[0].getIndex());
        if (itemToReload == nullptr)
        {
            character->sendMsg("You don't have %s.\n", args[0].getContent());
            return;
        }
    }
    if (itemToReload->getType() != ModelType::RangedWeapon)
    {
        character->sendMsg("You can't reload %s.\n", itemToReload->getName(true));
        return;
    }
    // Transform the item into a ranged weapons.
    auto rangedWeapon = itemToReload->toRangedWeaponItem();
    // Search the magazine.
    auto magazine = character->findEquipmentItem(args[1].getContent(), args[1].getIndex());
    if (magazine == nullptr)
    {
        magazine = character->findInventoryItem(args[1].getContent(), args[1].getIndex());
        if (magazine == nullptr)
        {
            character->sendMsg("You don't have %s.\n", args[1].getContent());
            return;
        }
    }
    if (magazine->getType() != ModelType::Magazine)
    {
        character->sendMsg("You can't reload %s with %s.\n",
                           rangedWeapon->getName(true),
                           magazine->getName(true));
        return;
    }
    auto requiredTime = 2;
    auto newAction = std::make_shared<ReloadAction>(rangedWeapon, magazine, character, requiredTime);
    std::string error;
    // Check the new action.
    if (newAction->check(error))
    {
        // Send the starting message.
        character->sendMsg("You start reloading %s with %s.\n",
                           rangedWeapon->getName(true),
                           magazine->getName(true));
        // Set the new action.
        character->setAction(newAction);
    }
    else
    {
        character->sendMsg("%s\n", error);
    }
}

void DoAim(Character * character, ArgumentHandler & args)
{
    if (args.size() != 1)
    {
        character->sendMsg("Who or what do you want to aim?\n");
        return;
    }
    if (character->getActiveRangedWeapons().empty())
    {
        character->sendMsg("You don't have a ranged weapon equipped.\n");
        return;
    }
    // Prepare a pointer to the aimed character.
    Character * aimedCharacter = nullptr;
    // Create a single CharacterContainer which contains a unique list of all the targets.
    CharacterContainer targets = character->room->characters;
    targets.addUnique(character->charactersInSight);
    // First try to search the target inside the same room.
    aimedCharacter = targets.findCharacter(args[0].getContent(), args[0].getIndex());
    // Otherwise try to find the target inside the list of characters in sight.
    if (aimedCharacter == nullptr)
    {
        character->sendMsg("You don't see '%s' anywhere...\n", args[0].getContent());
        return;
    }
    // Check the room of the target.
    if (aimedCharacter->room == nullptr)
    {
        character->sendMsg("%s is out of your line of sight...\n", aimedCharacter->getNameCapital());
    }
    else
    {
        // Check if the target is still in sight.
        if (character->isAtRange(aimedCharacter, character->getViewDistance()))
        {
            auto newAction = std::make_shared<AimAction>(character,
                                                         aimedCharacter,
                                                         AimAction::getAimTime(character, aimedCharacter));
            // Check the new action.
            std::string error;
            if (newAction->check(error))
            {
                // Send the starting message.
                character->sendMsg("You start aiming at %s...\n", aimedCharacter->getName());
                // Set the new action.
                character->setAction(newAction);
            }
            else
            {
                character->sendMsg("%s\n", error);
            }
        }
        else
        {
            character->sendMsg("%s is out of your line of sight...\n", aimedCharacter->getNameCapital());
        }
    }
}

void DoFire(Character * character, ArgumentHandler & /*args*/)
{
    // Check if the pointer to the aimed target is still valid.
    if (character->combatHandler.getAimedTarget() == nullptr)
    {
        character->sendMsg("You first need to aim at someone or something.\n");
        return;
    }
    auto aimedTarget = character->combatHandler.getAimedTarget();
    // Check if the target is still in sight.
    if (!character->isAtRange(character->combatHandler.getAimedTarget(), character->getViewDistance()))
    {
        character->sendMsg("%s is out of your line of sight...\n", aimedTarget->getNameCapital());
        return;
    }
    // Retrive the active ranged weapons.
    auto rangedWeapons = character->getActiveRangedWeapons();
    // Check if the character has some ranged weapons equipped.
    if (rangedWeapons.empty())
    {
        character->sendMsg("You don't have any ranged weapon equipped.\n");
        return;
    }
    // For each ranged weapon check if it is able to reach the target.
    bool canAttack = false;
    for (auto weapon : rangedWeapons)
    {
        auto weaponRange = weapon->getRange();
        if (character->isAtRange(aimedTarget, weaponRange))
        {
            canAttack = true;
            break;
        }
    }
    if (canAttack)
    {
        // Let the characters enter the combat.
        if (character->setNextCombatAction(CombatActionType::BasicAttack))
        {
            // Set the predefined target.
            character->combatHandler.setPredefinedTarget(aimedTarget);
            character->sendMsg("You start firing at %s...\n", aimedTarget->getName());
        }
        else
        {
            character->sendMsg("You were not able to fire at %s.\n", aimedTarget->getName());
        }
    }
    else
    {
        character->sendMsg("%s is out of your reach...\n", aimedTarget->getNameCapital());
    }
}
