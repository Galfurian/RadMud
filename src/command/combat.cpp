/// @file   combat.cpp
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

#include "combat.hpp"
#include "mud.hpp"
#include "scoutAction.hpp"
#include "reloadAction.hpp"
#include "loadAction.hpp"
#include "unloadAction.hpp"
#include "aimAction.hpp"
#include "basicAttack.hpp"
#include "flee.hpp"

void LoadCombatCommands()
{
    {
        Command command;
        command.name = "kill";
        command.help = "Engage in combat the desired target.";
        command.arguments = "(target)";
        command.hndl = DoKill;
        command.canUseInCombat = true;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.name = "flee";
        command.help = "Try to flee from combat.";
        command.arguments = "NONE";
        command.hndl = DoFlee;
        command.canUseInCombat = true;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.name = "scout";
        command.help = "Provides information about the surrounding area.";
        command.arguments = "NONE";
        command.hndl = DoScout;
        command.canUseInCombat = true;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.name = "load";
        command.help = "Allows to load a magazine with projectiles.";
        command.arguments = "(magazine)(projectiles)";
        command.hndl = DoLoad;
        command.canUseInCombat = true;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.name = "unload";
        command.help = "Allows to unload an magazine.";
        command.arguments = "(magazine)";
        command.hndl = DoUnload;
        command.canUseInCombat = true;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.name = "reload";
        command.help = "Allows to reload a firearm with a magazine.";
        command.arguments = "(firearm)(magazine)";
        command.hndl = DoReload;
        command.canUseInCombat = true;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.name = "aim";
        command.help = "Allows to aim a target in sight.\n If the target is not inside the same room";
        command.help += "If the target is not inside the same room, you have first to scout the area.";
        command.arguments = "(target)";
        command.hndl = DoAim;
        command.canUseInCombat = true;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.name = "fire";
        command.help = "Allows to fire with an equipped ranged weapon to an aimed target.";
        command.arguments = "NONE";
        command.hndl = DoFire;
        command.canUseInCombat = true;
        Mud::instance().addCommand(command);
    }
}

bool DoKill(Character * character, ArgumentHandler & args)
{
    // Stop any action the character is executing.
    StopAction(character);
    // If there are no arguments, show the room.
    if (args.size() != 1)
    {
        character->sendMsg("You have to specify whom to kill.\n");
        return false;
    }
    // Retrieve the target.
    auto target = character->room->findCharacter(args[0].getContent(), args[0].getIndex(), {character});
    if (!target)
    {
        character->sendMsg("You don't see '%s' anywhere.\n", args[0].getContent());
        return false;
    }
    // Check if the attacker can see the target.
    if (!character->canSee(target))
    {
        character->sendMsg("You don't see '%s' anywhere.\n", args[0].getContent());
        return false;
    }
    // Check if the two characters are both already in combat.
    if (character->getAction()->getType() == ActionType::Combat)
    {
        // Check if the character is trying to attack a target with which is not in combat.
        if (character->combatHandler.hasOpponent(target))
        {
            // Check if the current predefined target is the target.
            auto predefinedTarget = character->combatHandler.getPredefinedTarget();
            if (predefinedTarget != nullptr)
            {
                if (predefinedTarget == target)
                {
                    character->sendMsg("You are already doing your best to kill %s!\n", target->getName());
                    return false;
                }
            }
            // Set the target as predefined target.
            character->combatHandler.setPredefinedTarget(target);
            // Notify to the character.
            character->sendMsg("You focus your attacks on %s!\n", target->getName());
            return true;
        }
        else
        {
            character->sendMsg("You have already your share of troubles!\n");
        }
    }
    else
    {
        // Check if the character is attacking a target which is already in combat.
        // Let the characters enter the combat.
        auto basicAttack = std::make_shared<BasicAttack>(character);
        std::string error;
        if (basicAttack->check(error))
        {
            // Add the target to the list of opponents.
            character->combatHandler.addOpponent(target);
            // Add the action to the character's combat queue.
            character->setAction(basicAttack);
            // Set the predefined target.
            character->combatHandler.setPredefinedTarget(target);
            // Notify the character.
            character->sendMsg("You attack %s.\n", target->getName());
            return true;
        }
        else
        {
            character->sendMsg("You were not able to attack %s.\n", target->getName());
        }
    }
    return false;
}

bool DoFlee(Character * character, ArgumentHandler & /*args*/)
{
    // Check if the character is in combat.
    if (character->getAction()->getType() != ActionType::Combat)
    {
        character->sendMsg("You are not fighting.\n");
        return false;
    }
    // Check if the actor is already trying to flee.
    if (character->getAction()->getType() == ActionType::Combat)
    {
        // Check if the character is already trying to flee.
        if (character->getAction()->toCombatAction()->getCombatActionType() == CombatActionType::Flee)
        {
            character->sendMsg("You are already trying to flee.\n");
            return false;
        }
    }
    auto flee = std::make_shared<Flee>(character);
    std::string error;
    if (flee->check(error))
    {
        // Add the action to the character's combat queue.
        character->setAction(flee);
        // Notify the character.
        character->sendMsg("You prepare to flee...\n");
        return true;
    }
    character->sendMsg(error + "\n");
    return false;
}

bool DoScout(Character * character, ArgumentHandler & /*args*/)
{
    // Stop any action the character is executing.
    StopAction(character);
    auto newAction = std::make_shared<ScoutAction>(character);
    // Check the new action.
    std::string error;
    if (newAction->check(error))
    {
        // Send the starting message.
        character->sendMsg("You start scouting the area...\n");
        // Set the new action.
        character->setAction(newAction);
        return true;
    }
    character->sendMsg("%s\n", error);
    return false;
}

bool DoLoad(Character * character, ArgumentHandler & args)
{
    // Stop any action the character is executing.
    StopAction(character);
    if (args.size() != 2)
    {
        character->sendMsg("What do you want to load with what?\n");
        return false;
    }
    // Search the magazine.
    auto itemToLoad = character->findEquipmentItem(args[0].getContent(), args[0].getIndex());
    if (itemToLoad == nullptr)
    {
        itemToLoad = character->findInventoryItem(args[0].getContent(), args[0].getIndex());
        if (itemToLoad == nullptr)
        {
            character->sendMsg("You don't have %s.\n", args[0].getContent());
            return false;
        }
    }
    // Check if the item is a magazine.
    if (itemToLoad->getType() != ModelType::Magazine)
    {
        character->sendMsg("You can't load %s.\n", itemToLoad->getName(true));
        return false;
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
            return false;
        }
    }
    // Check if the projectiles can be loaded inside the magazine.
    std::string error;
    unsigned int amountToLoad = 0;
    if (!magazine->getAmountToLoad(projectile, amountToLoad, error))
    {
        character->sendMsg("%s\n", error);
        return false;
    }
    // Create the load action.
    auto newAction = std::make_shared<LoadAction>(character, magazine, projectile, amountToLoad);
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
        return true;
    }
    character->sendMsg("%s\n", error);
    return false;
}

bool DoUnload(Character * character, ArgumentHandler & args)
{
    // Stop any action the character is executing.
    StopAction(character);
    if (args.size() != 1)
    {
        character->sendMsg("What do you want to unload?\n");
        return false;
    }
    // Search the magazine.
    auto itemToUnload = character->findEquipmentItem(args[0].getContent(), args[0].getIndex());
    if (itemToUnload == nullptr)
    {
        itemToUnload = character->findInventoryItem(args[0].getContent(), args[0].getIndex());
        if (itemToUnload == nullptr)
        {
            character->sendMsg("You don't have %s.\n", args[0].getContent());
            return false;
        }
    }
    // Check if the item is a magazine or a ranged weapon.
    if ((itemToUnload->getType() != ModelType::Magazine) && (itemToUnload->getType() != ModelType::RangedWeapon))
    {
        character->sendMsg("You can't unload %s.\n", itemToUnload->getName(true));
        return false;
    }
    if (itemToUnload->isEmpty())
    {
        character->sendMsg("%s is already empty...\n", itemToUnload->getNameCapital(true));
        return false;
    }
    // Set the required time to unloaded the item.
    if (itemToUnload->getType() == ModelType::Magazine)
    {
        auto loadedItem = itemToUnload->toMagazineItem()->getAlreadyLoadedProjectile();
        if (loadedItem == nullptr)
        {
            character->sendMsg("Something is gone wrong while you were unloading %s...\n\n",
                               itemToUnload->getName(true));
            return false;
        }
    }
    // Create the unload action.
    auto newAction = std::make_shared<UnloadAction>(character, itemToUnload);
    std::string error;
    // Check the new action.
    if (newAction->check(error))
    {
        // Send the starting message.
        character->sendMsg("You start unloading %s.\n", itemToUnload->getName(true));
        // Set the new action.
        character->setAction(newAction);
        return true;
    }
    character->sendMsg("%s\n", error);
    return false;
}

bool DoReload(Character * character, ArgumentHandler & args)
{
    // Stop any action the character is executing.
    StopAction(character);
    if (args.size() != 2)
    {
        character->sendMsg("What do you want to reload with what?\n");
        return false;
    }
    auto itemToReload = character->findEquipmentItem(args[0].getContent(), args[0].getIndex());
    if (itemToReload == nullptr)
    {
        itemToReload = character->findInventoryItem(args[0].getContent(), args[0].getIndex());
        if (itemToReload == nullptr)
        {
            character->sendMsg("You don't have %s.\n", args[0].getContent());
            return false;
        }
    }
    if (itemToReload->getType() != ModelType::RangedWeapon)
    {
        character->sendMsg("You can't reload %s.\n", itemToReload->getName(true));
        return false;
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
            return false;
        }
    }
    if (magazine->getType() != ModelType::Magazine)
    {
        character->sendMsg("You can't reload %s with %s.\n",
                           rangedWeapon->getName(true),
                           magazine->getName(true));
        return false;
    }
    auto newAction = std::make_shared<ReloadAction>(character, rangedWeapon, magazine);
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
        return true;
    }
    character->sendMsg("%s\n", error);
    return false;
}

bool DoAim(Character * character, ArgumentHandler & args)
{
    // Stop any action the character is executing.
    StopAction(character);
    if (args.size() != 1)
    {
        character->sendMsg("Who or what do you want to aim?\n");
        return false;
    }
    if (character->getActiveRangedWeapons().empty())
    {
        character->sendMsg("You don't have a ranged weapon equipped.\n");
        return false;
    }
    // Prepare a pointer to the aimed character.
    Character * aimedCharacter = nullptr;
    // Create a single CharacterContainer which contains a unique list of all the targets.
    CharacterContainer targets = character->room->characters;
    targets.addUnique(character->combatHandler.charactersInSight);
    // First try to search the target inside the same room.
    aimedCharacter = targets.findCharacter(args[0].getContent(), args[0].getIndex());
    // Otherwise try to find the target inside the list of characters in sight.
    if (aimedCharacter == nullptr)
    {
        character->sendMsg("You don't see '%s' anywhere...\n", args[0].getContent());
        return false;
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
            auto newAction = std::make_shared<AimAction>(character, aimedCharacter);
            // Check the new action.
            std::string error;
            if (newAction->check(error))
            {
                // Send the starting message.
                character->sendMsg("You start aiming at %s...\n", aimedCharacter->getName());
                // Set the new action.
                character->setAction(newAction);
                return true;
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
    return false;
}

bool DoFire(Character * character, ArgumentHandler & /*args*/)
{
    // Stop any action the character is executing.
    StopAction(character);
    // Check if the pointer to the aimed target is still valid.
    if (character->combatHandler.getAimedTarget() == nullptr)
    {
        character->sendMsg("You first need to aim at someone or something.\n");
        return false;
    }
    auto aimedTarget = character->combatHandler.getAimedTarget();
    // Check if the target is still in sight.
    if (!character->isAtRange(character->combatHandler.getAimedTarget(), character->getViewDistance()))
    {
        character->sendMsg("%s is out of your line of sight...\n", aimedTarget->getNameCapital());
        return false;
    }
    // Retrive the active ranged weapons.
    auto rangedWeapons = character->getActiveRangedWeapons();
    // Check if the character has some ranged weapons equipped.
    if (rangedWeapons.empty())
    {
        character->sendMsg("You don't have any ranged weapon equipped.\n");
        return false;
    }
    // For each ranged weapon check if it is able to reach the target.
    bool canAttack = false;
    for (auto weapon : rangedWeapons)
    {
        if (character->isAtRange(aimedTarget, weapon->getRange()))
        {
            canAttack = true;
            break;
        }
    }
    if (canAttack)
    {
        auto basicAttack = std::make_shared<BasicAttack>(character);
        std::string error;
        if (basicAttack->check(error))
        {
            // Add the aimedTarget to the list of opponents.
            character->combatHandler.addOpponent(aimedTarget);
            // Add the action to the character's combat queue.
            character->setAction(basicAttack);
            // Set the predefined target.
            character->combatHandler.setPredefinedTarget(aimedTarget);
            character->sendMsg("You start firing at %s...\n", aimedTarget->getName());
            return true;
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
    return false;
}
