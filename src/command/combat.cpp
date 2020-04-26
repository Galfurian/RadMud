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

#include "command/combat.hpp"
#include "mud.hpp"
#include "action/scoutAction.hpp"
#include "action/reloadAction.hpp"
#include "action/loadAction.hpp"
#include "action/unloadAction.hpp"
#include "action/aimAction.hpp"
#include "action/combat/basicAttack.hpp"
#include "character/characterUtilities.hpp"
#include "action/combat/flee.hpp"
#include "structure/structureUtils.hpp"

void LoadCombatCommands()
{
	Mud::instance().addCommand(std::make_shared<Command>(
		DoKill, "kill", "(target)", "Engage in combat the desired target.",
		false, true, false));
	Mud::instance().addCommand(std::make_shared<Command>(
		DoFlee, "flee", "", "Try to flee from combat.", false, true, false));
	Mud::instance().addCommand(std::make_shared<Command>(
		DoScout, "scout", "",
		"Provides information about the surrounding area.", false, true,
		false));
	Mud::instance().addCommand(std::make_shared<Command>(
		DoLoad, "load", "(magazine) (projectiles)",
		"Allows to load a magazine with projectiles.", false, true, false));
	Mud::instance().addCommand(std::make_shared<Command>(
		DoUnload, "unload", "(magazine)", "Allows to unload an magazine.",
		false, true, false));
	Mud::instance().addCommand(std::make_shared<Command>(
		DoReload, "reload", "(firearm) (magazine)",
		"Allows to reload a firearm with a magazine.", false, true, false));
	Mud::instance().addCommand(std::make_shared<Command>(
		DoAim, "aim", "(target)",
		"Allows to aim a target in sight.\n"
		"If the target is not inside the same room,"
		"you have first to scout the area.\n"
		"If no arguments is provided it lists the available targets.",
		false, true, false));
	Mud::instance().addCommand(std::make_shared<Command>(
		DoFire, "fire", "(firearm) (magazine)",
		"Allows to fire with an equipped ranged weapon to an aimed target.",
		false, true, false));
}

bool DoKill(Character *character, ArgumentHandler &args)
{
	// Check if the character is sleeping.
	if (character->posture == CharacterPosture::Sleep) {
		character->sendMsg("Not while you're sleeping.\n");
		return false;
	}
	// Stop any action the character is executing.
	StopAction(character);
	// If there are no arguments, show the room.
	if (args.size() != 1) {
		character->sendMsg("You have to specify whom to kill.\n");
		return false;
	}
	// Retrieve the target.
	auto target = character->room->findCharacter(
		args[0].getContent(), args[0].getIndex(), { character });
	if (!target) {
		character->sendMsg("You don't see '%s' anywhere.\n",
						   args[0].getContent());
		return false;
	}
	// Check if the attacker can see the target.
	if (!character->canSee(target)) {
		character->sendMsg("You don't see '%s' anywhere.\n",
						   args[0].getContent());
		return false;
	}
	// Check if the two characters are both already in combat.
	if (character->getAction() == ActionType::Combat) {
		// Check if the character is trying to attack a target
		//  with which is not in combat.
		if (character->combatHandler.hasOpponent(target)) {
			// Check if the current predefined target is the target.
			auto predefinedTarget =
				character->combatHandler.getPredefinedTarget();
			if (predefinedTarget != nullptr) {
				if (predefinedTarget == target) {
					character->sendMsg(
						"You are already doing your best to kill %s!\n",
						target->getName());
					return false;
				}
			}
			// Set the target as predefined target.
			character->combatHandler.setPredefinedTarget(target);
			// Notify to the character.
			character->sendMsg("You focus your attacks on %s!\n",
							   target->getName());
			return true;
		}
		character->sendMsg("You have already your share of troubles!\n");
		return false;
	}
	// Check if the character is attacking a target
	//  which is already in combat.
	// Let the characters enter the combat.
	auto basicAttack = std::make_shared<BasicAttack>(character);
	if (basicAttack->start()) {
		// Add the target to the list of opponents.
		character->combatHandler.addOpponent(target);
		// Add the action to the character's combat queue.
		character->pushAction(basicAttack);
		// Set the predefined target.
		character->combatHandler.setPredefinedTarget(target);
		// Notify the character.
		character->sendMsg("You attack %s.\n", target->getName());
		return true;
	}
	return false;
}

bool DoFlee(Character *character, ArgumentHandler & /*args*/)
{
	// Check if the character is in combat.
	if (character->getAction() != ActionType::Combat) {
		character->sendMsg("You are not fighting.\n");
		return false;
	}
	// Check if the actor is already trying to flee.
	if (character->getAction() == ActionType::Combat) {
		// Check if the character is already trying to flee.
		if (character->getAction()->toCombatAction()->getCombatActionType() ==
			CombatActionType::Flee) {
			character->sendMsg("You are already trying to flee.\n");
			return false;
		}
	}
	auto act = std::make_shared<Flee>(character);
	if (act->start()) {
		character->pushAction(act);
		return true;
	}
	return false;
}

bool DoScout(Character *character, ArgumentHandler & /*args*/)
{
	// Check if the character is sleeping.
	if (character->posture == CharacterPosture::Sleep) {
		character->sendMsg("Not while you're sleeping.\n");
		return false;
	}
	// Stop any action the character is executing.
	StopAction(character);
	auto act = std::make_shared<ScoutAction>(character);
	if (act->start()) {
		character->pushAction(act);
		return true;
	}
	return false;
}

bool DoLoad(Character *character, ArgumentHandler &args)
{
	// Check if the character is sleeping.
	if (character->posture == CharacterPosture::Sleep) {
		character->sendMsg("Not while you're sleeping.\n");
		return false;
	}
	// Stop any action the character is executing.
	StopAction(character);
	if (args.size() != 2) {
		character->sendMsg("What do you want to load with what?\n");
		return false;
	}
	// Search the magazine.
	auto itemToLoad =
		character->findEquipmentItem(args[0].getContent(), args[0].getIndex());
	if (itemToLoad == nullptr) {
		itemToLoad = character->findInventoryItem(args[0].getContent(),
												  args[0].getIndex());
		if (itemToLoad == nullptr) {
			character->sendMsg("You don't have %s.\n", args[0].getContent());
			return false;
		}
	}
	// Check if the item is a magazine.
	if (itemToLoad->getType() != ModelType::Magazine) {
		character->sendMsg("You can't load %s.\n", itemToLoad->getName(true));
		return false;
	}
	// Transform the item into a magazine.
	auto magazine = static_cast<MagazineItem *>(itemToLoad);
	// Search the projectiles.
	auto projectile =
		character->findEquipmentItem(args[1].getContent(), args[1].getIndex());
	if (projectile == nullptr) {
		projectile = character->findInventoryItem(args[1].getContent(),
												  args[1].getIndex());
		if (projectile == nullptr) {
			character->sendMsg("You don't have %s.\n", args[1].getContent());
			return false;
		}
	}
	// Check if the projectiles can be loaded inside the magazine.
	std::string error;
	unsigned int amountToLoad = 0;
	if (!magazine->getAmountToLoad(projectile, amountToLoad, error)) {
		character->sendMsg("%s\n", error);
		return false;
	}
	// Create the load action.
	auto act = std::make_shared<LoadAction>(character, magazine, projectile,
											amountToLoad);
	if (act->start()) {
		character->pushAction(act);
		return true;
	}
	return false;
}

bool DoUnload(Character *character, ArgumentHandler &args)
{
	// Check if the character is sleeping.
	if (character->posture == CharacterPosture::Sleep) {
		character->sendMsg("Not while you're sleeping.\n");
		return false;
	}
	// Stop any action the character is executing.
	StopAction(character);
	if (args.size() != 1) {
		character->sendMsg("What do you want to unload?\n");
		return false;
	}
	// Search the magazine.
	auto itemToUnload =
		character->findEquipmentItem(args[0].getContent(), args[0].getIndex());
	if (itemToUnload == nullptr) {
		itemToUnload = character->findInventoryItem(args[0].getContent(),
													args[0].getIndex());
		if (itemToUnload == nullptr) {
			character->sendMsg("You don't have %s.\n", args[0].getContent());
			return false;
		}
	}
	// Check if the item is a magazine or a ranged weapon.
	if ((itemToUnload->getType() != ModelType::Magazine) &&
		(itemToUnload->getType() != ModelType::RangedWeapon)) {
		character->sendMsg("You can't unload %s.\n",
						   itemToUnload->getName(true));
		return false;
	}
	if (itemToUnload->isEmpty()) {
		character->sendMsg("%s is already empty...\n",
						   itemToUnload->getNameCapital(true));
		return false;
	}
	// Create the unload action.
	auto act = std::make_shared<UnloadAction>(character, itemToUnload);
	if (act->start()) {
		character->pushAction(act);
		return true;
	}
	return false;
}

bool DoReload(Character *character, ArgumentHandler &args)
{
	// Check if the character is sleeping.
	if (character->posture == CharacterPosture::Sleep) {
		character->sendMsg("Not while you're sleeping.\n");
		return false;
	}
	// Stop any action the character is executing.
	StopAction(character);
	if (args.size() != 2) {
		character->sendMsg("What do you want to reload with what?\n");
		return false;
	}
	auto itemToReload =
		character->findEquipmentItem(args[0].getContent(), args[0].getIndex());
	if (itemToReload == nullptr) {
		itemToReload = character->findInventoryItem(args[0].getContent(),
													args[0].getIndex());
		if (itemToReload == nullptr) {
			character->sendMsg("You don't have %s.\n", args[0].getContent());
			return false;
		}
	}
	if (itemToReload->getType() != ModelType::RangedWeapon) {
		character->sendMsg("You can't reload %s.\n",
						   itemToReload->getName(true));
		return false;
	}
	// Transform the item into a ranged weapons.
	auto rangedWeapon = static_cast<RangedWeaponItem *>(itemToReload);
	// Search the magazine.
	auto magazine =
		character->findEquipmentItem(args[1].getContent(), args[1].getIndex());
	if (magazine == nullptr) {
		magazine = character->findInventoryItem(args[1].getContent(),
												args[1].getIndex());
		if (magazine == nullptr) {
			character->sendMsg("You don't have %s.\n", args[1].getContent());
			return false;
		}
	}
	if (magazine->getType() != ModelType::Magazine) {
		character->sendMsg("You can't reload %s with %s.\n",
						   rangedWeapon->getName(true),
						   magazine->getName(true));
		return false;
	}
	auto act =
		std::make_shared<ReloadAction>(character, rangedWeapon, magazine);
	if (act->start()) {
		character->pushAction(act);
		return true;
	}
	return false;
}

bool DoAim(Character *character, ArgumentHandler &args)
{
	// Check if the character is sleeping.
	if (character->posture == CharacterPosture::Sleep) {
		character->sendMsg("Not while you're sleeping.\n");
		return false;
	}
	// Stop any action the character is executing.
	StopAction(character);
	// Check if the character is actually wielding a ranged weapon.
	if (GetActiveWeapons<RangedWeaponItem>(character).empty()) {
		character->sendMsg("You don't have a ranged weapon equipped.\n");
		return false;
	}
	// If no target is provided, update and show the list of characters in sight.
	if (args.empty()) {
		if (character->combatHandler.getAimedTarget()) {
			character->sendMsg(
				"You are aiming at %s.\n",
				character->combatHandler.getAimedTarget()->getName());
		}
		character->combatHandler.updateCharactersInSight();
		if (character->combatHandler.charactersInSight.empty()) {
			character->sendMsg("You have found nothing...\n");
			return true;
		}
		character->sendMsg("You are able to aim at:\n");
		for (auto it : character->combatHandler.charactersInSight) {
			if (!it)
				continue;
			if (!it->room)
				continue;
			character->sendMsg("  [%s] %s\n",
							   StructUtils::getRoomDistance(character->room,
															it->room),
							   it->getName());
		}
		return true;
	}
	if (args.size() > 1) {
		character->sendMsg("Too many arguments.\n");
		return false;
	}
	// Create a single CharacterVector which contains
	//  a unique list of all the targets.
	CharacterVector targets = character->room->characters;
	targets.addUnique(character->combatHandler.charactersInSight);
	// First try to search the target inside the same room.
	auto aimedCharacter =
		targets.findCharacter(args[0].getContent(), args[0].getIndex());
	// Otherwise try to find the target inside the list
	//  of characters in sight.
	if (aimedCharacter == nullptr) {
		character->sendMsg("You don't see '%s' anywhere...\n",
						   args[0].getContent());
		return false;
	}
	// Check the room of the target.
	if (aimedCharacter->room == nullptr) {
		character->sendMsg("%s is out of your line of sight...\n",
						   aimedCharacter->getNameCapital());
		return false;
	}
	// Check if the target is still in sight.
	if (!character->isAtRange(aimedCharacter, character->getViewDistance())) {
		character->sendMsg("%s is out of your line of sight...\n",
						   aimedCharacter->getNameCapital());
		return false;
	}
	auto act = std::make_shared<AimAction>(character, aimedCharacter);
	if (act->start()) {
		character->pushAction(act);
		return true;
	}
	return false;
}

bool DoFire(Character *character, ArgumentHandler & /*args*/)
{
	// Check if the character is sleeping.
	if (character->posture == CharacterPosture::Sleep) {
		character->sendMsg("Not while you're sleeping.\n");
		return false;
	}
	// Stop any action the character is executing.
	StopAction(character);
	// Check if the pointer to the aimed target is still valid.
	if (character->combatHandler.getAimedTarget() == nullptr) {
		character->sendMsg("You first need to aim at someone or something.\n");
		return false;
	}
	auto aimedTarget = character->combatHandler.getAimedTarget();
	// Check if the target is still in sight.
	if (!character->isAtRange(character->combatHandler.getAimedTarget(),
							  character->getViewDistance())) {
		character->sendMsg("%s is out of your line of sight...\n",
						   aimedTarget->getNameCapital());
		return false;
	}
	// Retrieve the active ranged weapons.
	auto rangedWeapons = GetActiveWeapons<RangedWeaponItem>(character);
	// Check if the character has some ranged weapons equipped.
	if (rangedWeapons.empty()) {
		character->sendMsg("You don't have any ranged weapon equipped.\n");
		return false;
	}
	// For each ranged weapon check if it is able to reach the target.
	if (std::find_if(rangedWeapons.begin(), rangedWeapons.end(),
					 [&](RangedWeaponItem *weapon) {
						 return (character->isAtRange(aimedTarget,
													  weapon->getRange()));
					 }) == rangedWeapons.end()) {
		character->sendMsg("%s is out of your reach...\n",
						   aimedTarget->getNameCapital());
		return false;
	}
	auto basicAttack = std::make_shared<BasicAttack>(character);
	std::string error;
	if (!basicAttack->check(error)) {
		character->sendMsg("You were not able to fire at %s.\n",
						   aimedTarget->getName());
		return false;
	}
	// Add the aimedTarget to the list of opponents.
	character->combatHandler.addOpponent(aimedTarget);
	// Add the action to the character's combat queue.
	character->pushAction(basicAttack);
	// Set the predefined target.
	character->combatHandler.setPredefinedTarget(aimedTarget);
	character->sendMsg("You start firing at %s...\n", aimedTarget->getName());
	return true;
}
