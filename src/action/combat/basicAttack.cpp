/// @file   basicAttack.cpp
/// @brief  Contais the implementation of the functions used for executing
///          basic attacks.
/// @author Enrico Fraccaroli
/// @date   Nov 2 2016
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

#include "basicAttack.hpp"

#include "characterUtilities.hpp"
#include "rangedWeaponModel.hpp"
#include "formatter.hpp"
#include "updater.hpp"
#include "chase.hpp"
#include "room.hpp"
#include "area.hpp"

BasicAttack::BasicAttack(Character *_actor) : CombatAction(_actor)
{
	// Debugging message.
	MudLog(LogLevel::Debug, "Created BasicAttack.");
	// Reset the cooldown of the action.
	this->resetCooldown(this->getCooldown());
}

BasicAttack::~BasicAttack()
{
	MudLog(LogLevel::Debug, "Deleted BasicAttack.");
}

bool BasicAttack::check(std::string &error) const
{
	return CombatAction::check(error);
}

ActionType BasicAttack::getType() const
{
	return ActionType::Combat;
}

std::string BasicAttack::getDescription() const
{
	return "fighting";
}

bool BasicAttack::start()
{
	std::string error;
	if (!this->check(error)) {
		actor->sendMsg(error + "\n\n");
		return false;
	}
	// Send the starting message.
	return true;
}

std::string BasicAttack::stop()
{
	return "You stop fighting.";
}

ActionStatus BasicAttack::perform()
{
	std::string error;
	MudLog(LogLevel::Debug, "%s performs a BasicAttack.",
		   actor->getNameCapital());
	if (!this->check(error)) {
		actor->sendMsg(error + "\n\n");
		return ActionStatus::Error;
	}
	// If there are no enemies, just stop fighting.
	if (actor->combatHandler.empty()) {
		MudLog(LogLevel::Debug, "%s has no opponents.",
			   actor->getNameCapital());
		// Stop the combat.
		this->handleStop();
		// Return that the action is finished.
		return ActionStatus::Finished;
	}

	// Set the progressive number of the attack to 0.
	unsigned int attacks = 0;

	// Retrieve all the melee, ranged, and natural weapons.
	auto meleeWeapons = GetActiveWeapons<MeleeWeaponItem>(actor);
	auto rangedWeapons = GetActiveWeapons<RangedWeaponItem>(actor);
	auto naturalWeapons = GetActiveNaturalWeapons(actor);

	// Prepare iterators for each category of weapons.
	auto melee_it = meleeWeapons.begin();
	auto rangd_it = rangedWeapons.begin();
	auto natrl_it = naturalWeapons.begin();

	// Iterate until we have used all the weapons.
	while ((melee_it != meleeWeapons.end()) ||
		   (rangd_it != rangedWeapons.end()) ||
		   (natrl_it != naturalWeapons.end())) {
		// If we cannot find a valid target, stop.
		if (!this->setPredefinedTarget()) {
			MudLog(LogLevel::Debug, "%s cannot set predefined target, break.",
				   actor->getNameCapital(), (*melee_it)->getName());
			break;
		}
		// Get the predefined target.
		Character *pred = actor->combatHandler.getPredefinedTarget();
		// If the actor and the pred-target are in the same room,
		//  and there are still melee weapons to use, attack.
		if (melee_it != meleeWeapons.end()) {
			MudLog(LogLevel::Debug, "%s can MELEE attack with %s.",
				   actor->getNameCapital(), (*melee_it)->getName());
			// Check if the target is at range of the weapon.
			if (!actor->isAtRange(pred, (*melee_it)->getRange())) {
				actor->sendMsg("You can't reach %s with %s.\n\n",
							   pred->getName(), (*melee_it)->getName());
				++melee_it;
				continue;
			}
			// Increment the number of executed attacks.
			++attacks;
			// Perform the attack, if we kill the target, continue.
			if (this->performMeleeAttack(pred, *melee_it++, attacks) ==
				AttackStatus ::Killed) {
				continue;
			}
		}
		if (rangd_it != rangedWeapons.end()) {
			MudLog(LogLevel::Debug, "%s can RANGED attack with %s.",
				   actor->getNameCapital(), (*rangd_it)->getName());
			// Check if the target is at range of the weapon.
			if (!actor->isAtRange(pred, (*rangd_it)->getRange())) {
				actor->sendMsg("You can't reach %s with %s.\n\n",
							   pred->getName(), (*rangd_it)->getName());
				++rangd_it;
				continue;
			}
			// Increment the number of executed attacks.
			++attacks;
			// Perform the attack, if we kill the target, continue.
			if (this->performRangedAttack(pred, *rangd_it++, attacks) ==
				AttackStatus ::Killed) {
				continue;
			}
		}
		// Perform the attack for each natural weapon.
		if (natrl_it != naturalWeapons.end()) {
			MudLog(LogLevel::Debug, "%s can NAT attack with %s.",
				   actor->getNameCapital(), (*natrl_it)->getName());
			// Check if the target is at range of the weapon.
			if (!actor->isAtRange(pred, (*natrl_it)->range)) {
				actor->sendMsg("You can't reach %s with %s.\n\n",
							   pred->getName(), (*natrl_it)->getName());
				++natrl_it;
				continue;
			}
			// Increment the number of executed attacks.
			++attacks;
			// Perform the attack, if we kill the target, continue.
			if (this->performAttackNaturalWeapon(pred, *natrl_it++, attacks) ==
				AttackStatus ::Killed) {
				continue;
			}
		}
	}
	// Check if all the opponents are dead.
	if (actor->combatHandler.empty()) {
		MudLog(LogLevel::Debug, "%s has no more opponents.",
			   actor->getNameCapital());
		// Stop the combat.
		this->handleStop();
		// Return that the action is finished.
		return ActionStatus::Finished;
	}
	// Check if the actor has attacked someone.
	if (attacks > 0) {
		// Reset the cooldown.
		actor->getAction()->resetCooldown();
		// Return that the action is still running.
		return ActionStatus::Running;
	}
	// Check if the actor is a mobile.
	if (actor->isMobile()) {
		// Take the enemy with the higher value of aggro.
		auto topAggro = actor->combatHandler.getTopAggro();
		if (topAggro != nullptr) {
			auto chase = std::make_shared<Chase>(actor, topAggro->aggressor);
			if (chase->check(error)) {
				// Add the action.
				actor->pushAction(chase);
				// Return that the action is still running.
				return ActionStatus::Running;
			}
			actor->sendMsg(error + "\n\n");
			return ActionStatus::Error;
		}
		MudLog(LogLevel::Error, "Top aggro is a nullptr!");
		return ActionStatus::Error;
	}
	MudLog(LogLevel::Error, "Something went wrong!");
	// Stop the combat.
	this->handleStop();
	// Return that the action is finished.
	return ActionStatus::Finished;
}

CombatActionType BasicAttack::getCombatActionType() const
{
	return CombatActionType::BasicAttack;
}

unsigned int BasicAttack::getConsumedStamina(Character *character, Item *weapon)
{
	// BASE     [+1.0]
	// STRENGTH [-0.0 to -1.40]
	// WEIGHT   [+1.6 to +2.51]
	// CARRIED  [+0.0 to +2.48]
	unsigned int consumedStamina = 1;
	consumedStamina -= character->getAbilityLog(Ability::Strength);
	consumedStamina = SafeSum(consumedStamina, SafeLog10(character->weight));
	consumedStamina =
		SafeSum(consumedStamina, SafeLog10(character->getCarryingWeight()));
	if (weapon == nullptr) {
		return consumedStamina;
	}
	if (weapon->getType() == ModelType::MeleeWeapon) {
		consumedStamina =
			SafeSum(consumedStamina, SafeLog10(weapon->getWeight(true)));
	} else if (weapon->getType() == ModelType::RangedWeapon) {
		if (weapon->model->to<RangedWeaponModel>()->rangedWeaponType !=
			RangedWeaponType::Thrown) {
			return 0;
		}
		consumedStamina =
			SafeSum(consumedStamina, SafeLog10(weapon->getWeight(true)));
	}
	return consumedStamina;
}

unsigned int
BasicAttack::getConsumedStamina(Character *character,
								const std::shared_ptr<BodyPart::BodyWeapon> &)
{
	// BASE     [+1.0]
	// STRENGTH [-0.0 to -1.40]
	// WEIGHT   [+1.6 to +2.51]
	// CARRIED  [+0.0 to +2.48]
	unsigned int result = 1;
	result -= character->getAbilityLog(Ability::Strength);
	result = SafeSum(result, SafeLog10(character->weight));
	result = SafeSum(result, SafeLog10(character->getCarryingWeight()));
	return result;
}

bool BasicAttack::setPredefinedTarget()
{
	// If there is a predefined target, check if it is a valid target.
	if (actor->combatHandler.getPredefinedTarget() != nullptr) {
		MudLog(LogLevel::Debug, "[%s] Has a predefined target.",
			   actor->getNameCapital());
		if (this->checkTarget(actor->combatHandler.getPredefinedTarget())) {
			MudLog(LogLevel::Debug, "[%s] Predefined target is a valid target.",
				   actor->getNameCapital());
			return true;
		}
		MudLog(LogLevel::Debug, "[%s] Predefined target is NOT a valid target.",
			   actor->getNameCapital());
	} else {
		MudLog(LogLevel::Debug, "[%s] Has no predefined target.",
			   actor->getNameCapital());
	}
	// Take a valid target.
	for (auto const &it : actor->combatHandler) {
		if (this->checkTarget(it->aggressor)) {
			MudLog(LogLevel::Debug, "[%s] Has a new predefined target: %s",
				   actor->getNameCapital(), it->aggressor->getNameCapital());
			actor->combatHandler.setPredefinedTarget(it->aggressor);
			return true;
		}
	}
	return false;
}

bool BasicAttack::checkTarget(Character *target)
{
	// Check characters.
	if ((actor == nullptr) || (target == nullptr)) {
		MudLog(LogLevel::Debug, "Either the actor or the target is a nullptr.");
		return false;
	}
	// Check their rooms.
	if ((actor->room == nullptr) || (target->room == nullptr)) {
		MudLog(LogLevel::Debug,
			   "[%s] Either the actor or the target are in a nullptr room.",
			   actor->getNameCapital());
		return false;
	}
	// Check if they are at close range.
	if (actor->room->coord == target->room->coord) {
		MudLog(LogLevel::Debug,
			   "[%s] The actor and the target are in the same room.",
			   actor->getNameCapital());
		return true;
	}
	// Check if there is no aimed character.
	if (actor->combatHandler.getAimedTarget() == nullptr) {
		MudLog(
			LogLevel::Debug,
			"[%s] The actor has no aimed character, so the target cannot be attacked.",
			actor->getNameCapital());
		return false;
	}
	// If at long range, check if the target is the aimed character.
	if (actor->combatHandler.getAimedTarget() == target) {
		MudLog(
			LogLevel::Debug,
			"[%s] The aimed character and the target are the same character.",
			actor->getNameCapital());
		// Retrieve all the ranged weapons.
		auto rangedWeapons = GetActiveWeapons<RangedWeaponItem>(actor);
		// Check if the actor has no ranged weapon equipped.
		if (rangedWeapons.empty()) {
			MudLog(LogLevel::Debug,
				   "[%s] The actor has no ranged weapon equipped.",
				   actor->getNameCapital());
			return false;
		}
		// Just check if AT LEAST one of the equipped ranged weapons can be used
		//  to attack the target.
		// TODO: This does not check if the weapon is USABLE!
		for (auto weapon : rangedWeapons) {
			if (actor->isAtRange(target, weapon->getRange())) {
				MudLog(LogLevel::Debug, "[%s] The target is at range with %s.",
					   actor->getNameCapital(), weapon->getName(false));
				return true;
			}
		}
	}
	MudLog(LogLevel::Debug, "[%s] No valid target has been found.",
		   actor->getNameCapital());
	return false;
}

BasicAttack::AttackStatus BasicAttack::performAttackNaturalWeapon(
	Character *target, const std::shared_ptr<BodyPart::BodyWeapon> &weapon,
	unsigned int attackNumber)
{
	// -------------------------------------------------------------------------
	// Preamble: Evaluate all the values which are required by the function.
	// -------------------------------------------------------------------------
	// Get the strength modifier.
	auto strengthMod = actor->getAbilityModifier(Ability::Strength);
	// Get the required stamina.
	auto consumedStamina = this->getConsumedStamina(actor, weapon);
	// -------------------------------------------------------------------------
	// Phase 1: Check if the actor has enough stamina to execute the action.
	// -------------------------------------------------------------------------
	if (consumedStamina > actor->stamina) {
		actor->sendMsg("You are too tired to attack with %s.\n",
					   weapon->getName(true));
		return AttackStatus::Failed;
	}
	// -------------------------------------------------------------------------
	// Phase 2: Roll the hit and damage.
	// -------------------------------------------------------------------------
	// Evaluate the natural roll for the attack.
	unsigned int hitRoll = TRand<unsigned int>(1, 20);
	// Boolean variable which identifies if the hit is a critical hit.
	bool isCritical = (hitRoll == 20);
	// Apply the penalty due to multi-wield, check if:
	//  1. The number of already executed attacks is greater-than 0.
	//  2. The attack is not a critical hit.
	if ((attackNumber > 0) && (!isCritical)) {
		// Evaluate the penalty to the hit roll.
		unsigned int penalty = (attackNumber * 3);
		// Safely apply the penalty.
		hitRoll = (hitRoll < penalty) ? 0 : (hitRoll - penalty);
	}
	// Apply the modifier to the hit roll when unarmed fighting.
	hitRoll = SafeSum(hitRoll, actor->effectManager.getCombatMod(
								   CombatModifier::UnarmedHitRoll));
	// Improve the skills which has contributed to the hit roll.
	actor->skillManager.improveCombat(CombatModifier::UnarmedHitRoll);
	// -------------------------------------------------------------------------
	// Phase 3: Check if the target is hit.
	// -------------------------------------------------------------------------
	// Evaluate the armor class of the enemy.
	auto armorClass = target->getArmorClass();
	// Check if:
	//  1. The hit roll is lesser than the armor class of the opponent.
	//  2. The hit roll is not a critical.
	if ((hitRoll < armorClass) && (!isCritical)) {
		// Show miss messages.
		this->handleNaturalWeaponMiss(target, weapon, hitRoll, armorClass);
		// Consume half the stamina.
		actor->remStamina(consumedStamina / 2, true);
	} else {
		// The amount of damage dealt.
		unsigned int damageRoll = 0;
		// Natural roll for the damage.
		damageRoll = weapon->rollDamage();
		// Add the strenght modifier.
		damageRoll += strengthMod;
		// Apply the modifier to the damage roll when unarmed fighting.
		damageRoll = SafeSum(damageRoll, actor->effectManager.getCombatMod(
											 CombatModifier::UnarmedDamage));
		// Improve the skills which has contributed to the damage roll.
		actor->skillManager.improveCombat(CombatModifier::UnarmedDamage);
		// Show hit messages.
		this->handleNaturalWeaponHit(target, weapon, hitRoll, armorClass,
									 damageRoll);
		// Consume the stamina.
		actor->remStamina(consumedStamina, true);
		// If the character has rolled a critical: multiply the damage by two.
		if (isCritical)
			damageRoll *= 2;
		// Proceed and remove the damage from the health of the target.
		if (!target->remHealth(damageRoll)) {
			// Notify the others.
			target->room->sendToAll("%s%s screams in pain and then die!%s\n",
									{ target }, Formatter::red(),
									target->getNameCapital(),
									Formatter::reset());
			// The target has received the damage and now it is dead.
			target->kill();
			// TODO: This is a solution in case the attack is a one-shot and
			// the target does not has the actor among the opponents.
			// Remove the target from the list of opponents.
			if (actor->combatHandler.hasOpponent(target))
				actor->combatHandler.remOpponent(target);
			// Interrupt the function.
			return AttackStatus::Killed;
		}
		// Update the aggro of the target towards the actor.
		auto aggro = target->combatHandler.getAggro(actor);
		aggro += ((damageRoll * 100) / target->getMaxHealth(true));
		target->combatHandler.setAggro(actor, aggro);
	}
	// -------------------------------------------------------------------------
	// Phase 4: Check if the target and the actor are in combat.
	// -------------------------------------------------------------------------
	// Check if the target has the actor as opponent.
	if (!target->combatHandler.hasOpponent(actor)) {
		// Add the actor to the list of opponents of the target.
		target->combatHandler.addOpponent(actor);
		// If the target is not already fighting, add a new basic attack
		// action to its queue.
		if (target->getAction() != ActionType::Combat) {
			target->pushAction(std::make_shared<BasicAttack>(target));
		}
	}
	// Check if the actor has the target as opponent.
	if (!actor->combatHandler.hasOpponent(target)) {
		actor->combatHandler.addOpponent(target);
	}
	return AttackStatus::Success;
}

BasicAttack::AttackStatus
BasicAttack::performMeleeAttack(Character *target, MeleeWeaponItem *weapon,
								unsigned int attackNumber)
{
	// -------------------------------------------------------------------------
	// Preamble: Evaluate all the values which are required by the function.
	// -------------------------------------------------------------------------
	// Get the strength modifier.
	auto strengthMod = actor->getAbilityModifier(Ability::Strength);
	// Get the required stamina.
	auto consumedStamina = this->getConsumedStamina(actor, weapon);
	// -------------------------------------------------------------------------
	// Phase 1: Check if the actor has enough stamina to execute the action.
	// -------------------------------------------------------------------------
	if (consumedStamina > actor->stamina) {
		actor->sendMsg("You are too tired to attack with %s.\n",
					   weapon->getName(true));
		return AttackStatus::Failed;
	}
	// -------------------------------------------------------------------------
	// Phase 2: Roll the hit and damage.
	// -------------------------------------------------------------------------
	// Evaluate the natural roll for the attack.
	auto hitRoll = TRand<unsigned int>(1, 20);
	// Boolean variable which identifies if the hit is a critical hit.
	bool isCritical = (hitRoll == 20);
	// Apply the penalty due to multi-wield, check if:
	//  1. The number of already executed attacks is greater-than 0.
	//  2. The attack is not a critical hit.
	if ((attackNumber > 0) && (!isCritical)) {
		// Evaluate the penalty to the hit roll.
		unsigned int penalty = (attackNumber * 3);
		// Safely apply the penalty.
		hitRoll = (hitRoll < penalty) ? 0 : (hitRoll - penalty);
	}
	// Apply the modifier to the hit roll with melee weapons.
	hitRoll = SafeSum(hitRoll, actor->effectManager.getCombatMod(
								   CombatModifier::MeleeWeaponHitRoll));
	// Improve the skills which has contributed to the hit roll.
	actor->skillManager.improveCombat(CombatModifier::MeleeWeaponHitRoll);
	// -------------------------------------------------------------------------
	// Phase 3: Check if the target is hit.
	// -------------------------------------------------------------------------
	// Evaluate the armor class of the enemy.
	auto armorClass = target->getArmorClass();
	// Check if:
	//  1. The hit roll is lesser than the armor class of the opponent.
	//  2. The hit roll is not a critical.
	if ((hitRoll < armorClass) && (!isCritical)) {
		// Show miss messages.
		this->handleMeleeMiss(target, weapon, hitRoll, armorClass);
		// Consume half the stamina.
		actor->remStamina(consumedStamina / 2, true);
	} else {
		// The amount of damage dealt.
		unsigned int damageRoll = 0;
		// Natural roll for the damage.
		damageRoll = weapon->rollDamage();
		// Check if the character is wielding a two-handed weapon.
		if (HasFlag(weapon->model->modelFlags, ModelFlag::TwoHand)) {
			// Add to the damage roll one and half the strength value.
			damageRoll += strengthMod + (strengthMod / 2);
		}
		// Apply the modifier to the damage roll with melee weapons.
		damageRoll =
			SafeSum(damageRoll, actor->effectManager.getCombatMod(
									CombatModifier::MeleeWeaponDamage));
		// Improve the skills which has contributed to the damage roll.
		actor->skillManager.improveCombat(CombatModifier::MeleeWeaponDamage);
		// Consume the stamina.
		actor->remStamina(consumedStamina, true);
		// If the character has rolled a critical: multiply the damage by two.
		if (isCritical)
			damageRoll *= 2;
		// Show hit messages.
		this->handleMeleeHit(target, weapon, hitRoll, armorClass, damageRoll);
		// Proceed and remove the damage from the health of the target.
		if (!target->remHealth(damageRoll)) {
			// Notify the others.
			target->room->sendToAll("%s%s screams in pain and then die!%s\n",
									{ target }, Formatter::red(),
									target->getNameCapital(),
									Formatter::reset());
			// The target has received the damage and now it is dead.
			target->kill();
			// TODO: This is a solution in case the attack is a one-shot and
			// the target does not has the actor among the opponents.
			// Remove the target from the list of opponents.
			if (actor->combatHandler.hasOpponent(target))
				actor->combatHandler.remOpponent(target);
			// Interrupt the function.
			return AttackStatus::Killed;
		}
		// Update the aggro of the target towards the actor.
		auto aggro = target->combatHandler.getAggro(actor);
		aggro += ((damageRoll * 100) / target->getMaxHealth(true));
		target->combatHandler.setAggro(actor, aggro);
	}
	if (!target->combatHandler.hasOpponent(actor)) {
		target->combatHandler.addOpponent(actor);
		if (target->getAction() != ActionType::Combat) {
			target->pushAction(std::make_shared<BasicAttack>(target));
		}
	}
	if (!actor->combatHandler.hasOpponent(target)) {
		actor->combatHandler.addOpponent(target);
	}
	return AttackStatus::Success;
}

BasicAttack::AttackStatus
BasicAttack::performRangedAttack(Character *target, RangedWeaponItem *weapon,
								 unsigned int attackNumber)
{
	// -------------------------------------------------------------------------
	// Preamble: Evaluate all the values which are required by the function.
	// -------------------------------------------------------------------------
	// Get the required stamina.
	auto consumedStamina = this->getConsumedStamina(actor, weapon);
	// -------------------------------------------------------------------------
	// Phase 1: Check if the actor has enough stamina to execute the action.
	// -------------------------------------------------------------------------
	if (consumedStamina > actor->stamina) {
		actor->sendMsg("You are too tired to attack with %s.\n",
					   weapon->getName(true));
		return AttackStatus::Failed;
	}
	// -------------------------------------------------------------------------
	// Phase 2: Check if the ranged weapon has enough projectiles.
	// -------------------------------------------------------------------------
	std::string error;
	auto projectile = weapon->retrieveProjectile(error);
	if (projectile == nullptr) {
		actor->sendMsg(error + ".\n\n");
		return AttackStatus::Failed;
	}
	// -------------------------------------------------------------------------
	// Phase 3: Roll the hit and damage.
	// -------------------------------------------------------------------------
	// Evaluate the natural roll for the attack.
	auto hitRoll = TRand<unsigned int>(1, 20);
	// Boolean variable which identifies if the hit is a critical hit.
	bool isCritical = (hitRoll == 20);
	// Apply the penalty due to multi-wield, check if:
	//  1. The number of already executed attacks is greater-than 0.
	//  2. The attack is not a critical hit.
	if ((attackNumber > 0) && (!isCritical)) {
		// Evaluate the penalty to the hit roll.
		unsigned int penalty = (attackNumber * 3);
		// Safely apply the penalty.
		hitRoll = (hitRoll < penalty) ? 0 : (hitRoll - penalty);
	}
	// Apply the modifier to the hit roll with ranged weapons.
	hitRoll = SafeSum(hitRoll, actor->effectManager.getCombatMod(
								   CombatModifier::RangedWeaponHitRoll));
	// Improve the skills which has contributed to the hit roll.
	actor->skillManager.improveCombat(CombatModifier::RangedWeaponHitRoll);
	// -------------------------------------------------------------------------
	// Phase 3: Consume the projectiles.
	// -------------------------------------------------------------------------
	// Check if it is the last projectile.
	if (projectile->quantity == 1) {
		MudUpdater::instance().addItemToDestroy(projectile);
	} else {
		projectile->quantity -= 1;
	}
	// -------------------------------------------------------------------------
	// Phase 4: Check if the target is hit.
	// -------------------------------------------------------------------------
	// Evaluate the armor class of the aimed character.
	auto armorClass = target->getArmorClass();
	// Check if:
	//  1. The hit roll is lesser than the armor class of the opponent.
	//  2. The hit roll is not a natural 20.
	if ((hitRoll < armorClass) && (!isCritical)) {
		this->handleRangedMiss(target, weapon, hitRoll, armorClass);
		// Consume half the stamina.
		actor->remStamina(consumedStamina / 2, true);
	} else {
		// Natural roll for the damage.
		unsigned int damageRoll = weapon->rollDamage();
		// Apply the modifier to the damage roll with ranged weapons.
		damageRoll =
			SafeSum(damageRoll, actor->effectManager.getCombatMod(
									CombatModifier::RangedWeaponDamage));
		// Improve the skills which has contributed to the damage roll.
		actor->skillManager.improveCombat(CombatModifier::RangedWeaponDamage);
		// Consume the stamina.
		actor->remStamina(consumedStamina, true);
		// If the character has rolled a critical: multiply the damage by two.
		if (isCritical)
			damageRoll *= 2;
		// Show the message.
		this->handleRangedHit(target, weapon, hitRoll, armorClass, damageRoll);
		// Proceed and remove the damage from the health of the target.
		if (!target->remHealth(damageRoll)) {
			// Notify the others.
			target->room->sendToAll("%s%s screams in pain and then die!%s\n",
									{ target }, Formatter::red(),
									target->getNameCapital(),
									Formatter::reset());
			// The target has received the damage and now it is dead.
			target->kill();
			// TODO: This is a solution in case the attack is a one-shot and
			// the target does not has the actor among the opponents.
			// Remove the target from the list of opponents.
			if (actor->combatHandler.hasOpponent(target))
				actor->combatHandler.remOpponent(target);
			// Interrupt the function.
			return AttackStatus::Killed;
		}
		// Update the aggro of the target towards the actor.
		auto aggro = target->combatHandler.getAggro(actor);
		aggro += ((damageRoll * 100) / target->getMaxHealth(true));
		target->combatHandler.setAggro(actor, aggro);
	}
	// -------------------------------------------------------------------------
	// Phase 5: Check if the target and the actor are in combat.
	// -------------------------------------------------------------------------
	// Check if the target has the actor as opponent.
	if (!target->combatHandler.hasOpponent(actor)) {
		target->combatHandler.addOpponent(actor);
		if (target->getAction() != ActionType::Combat) {
			target->pushAction(std::make_shared<BasicAttack>(target));
		}
	}
	if (!actor->combatHandler.hasOpponent(target)) {
		actor->combatHandler.addOpponent(target);
	}
	return AttackStatus::Success;
}

void BasicAttack::handleNaturalWeaponHit(
	Character *target, const std::shared_ptr<BodyPart::BodyWeapon> &weapon,
	unsigned int hitRoll, unsigned int armorClass, unsigned int damageRoll)
{
	// The target has received the damage but it is still alive.
	actor->sendMsg("You hit %s with your %s (%s vs %s for %s).\n\n",
				   target->getName(), weapon->getName(true), hitRoll,
				   armorClass, damageRoll);
	// Notify the target.
	target->sendMsg("%s hits you with %s %s (%s vs %s for %s).\n\n",
					actor->getNameCapital(), actor->getPossessivePronoun(),
					weapon->getName(true), hitRoll, armorClass, damageRoll);
	// Notify the other characters.
	target->room->funcSendToAll(
		"%s hits %s with %s %s (%s vs %s for %s).\n",
		[&](Character *character) {
			return !((character == actor) || (character == target));
		},
		actor->getNameCapital(), target->getName(),
		actor->getPossessivePronoun(), weapon->getName(true), hitRoll,
		armorClass, damageRoll);
}

void BasicAttack::handleMeleeHit(Character *target, MeleeWeaponItem *weapon,
								 unsigned int hitRoll, unsigned int armorClass,
								 unsigned int damageRoll)
{
	// The target has received the damage but it is still alive.
	actor->sendMsg("You hit %s with %s (%s vs %s for %s).\n\n",
				   target->getName(), weapon->getName(true), hitRoll,
				   armorClass, damageRoll);
	// Notify the target.
	target->sendMsg("%s hits you with %s (%s vs %s for %s).\n\n",
					actor->getNameCapital(), weapon->getName(true), hitRoll,
					armorClass, damageRoll);
	target->room->funcSendToAll(
		"%s hits %s with %s (%s vs %s for %s).\n",
		[&](Character *character) {
			return !((character == actor) || (character == target));
		},
		actor->getNameCapital(), target->getName(), weapon->getName(true),
		hitRoll, armorClass, damageRoll);
}

void BasicAttack::handleRangedHit(Character *target, RangedWeaponItem *weapon,
								  unsigned int hitRoll, unsigned int armorClass,
								  unsigned int damageRoll)
{
	// The enemy has received the damage but it is still alive.
	actor->sendMsg("You hit %s with %s (%s vs %s for %s).\n\n",
				   target->getName(), weapon->getName(true), hitRoll,
				   armorClass, damageRoll);
	// Notify the other characters.
	if (actor->room == target->room) {
		target->sendMsg(
			"%s fires a projectile which hits you (%s vs %s for %s).\n\n",
			actor->getNameCapital(), hitRoll, armorClass, damageRoll);
		actor->room->funcSendToAll(
			"%s fires and hits %s with %s (%s vs %s for %s).\n",
			[&](Character *character) {
				return !((character == actor) || (character == target));
			},
			actor->getNameCapital(), target->getName(), weapon->getName(true),
			hitRoll, armorClass, damageRoll);
		return;
	}
	// Notify the enemy.
	if (target->combatHandler.getAimedTarget() == actor) {
		target->sendMsg(
			"%s fires a projectile which hits you (%s vs %s for %s).\n\n",
			actor->getNameCapital(), hitRoll, armorClass, damageRoll);
	} else {
		target->sendMsg(
			"Someone fires a projectile that hits you (%s vs %s for %s).\n\n",
			hitRoll, armorClass, damageRoll);
	}
	actor->room->funcSendToAll(
		"%s fires and hits %s with %s (%s vs %s for %s).\n",
		[&](Character *character) {
			if ((character == actor) || (character == target))
				return false;
			if (character->combatHandler.charactersInSight.containsCharacter(
					target))
				return true;
			return character->combatHandler.getAimedTarget() == target;
		},
		actor->getNameCapital(), target->getName(), weapon->getName(true),
		hitRoll, armorClass, damageRoll);

	actor->room->funcSendToAll(
		"%s fires at someone or something with %s (%s vs %s for %s).\n",
		[&](Character *character) {
			if ((character == actor) || (character == target))
				return false;
			if (character->combatHandler.charactersInSight.containsCharacter(
					target))
				return false;
			return character->combatHandler.getAimedTarget() != target;
		},
		actor->getNameCapital(), weapon->getName(true), hitRoll, armorClass,
		damageRoll);

	target->room->funcSendToAll(
		"%s fires a projectile which hits %s (%s vs %s for %s).\n",
		[&](Character *character) {
			if ((character == actor) || (character == target))
				return false;
			if (character->combatHandler.charactersInSight.containsCharacter(
					actor))
				return true;
			return character->combatHandler.getAimedTarget() == actor;
		},
		actor->getNameCapital(), target->getName(), weapon->getName(true),
		hitRoll, armorClass, damageRoll);

	target->room->funcSendToAll(
		"Someone fires a projectile which hits %s (%s vs %s for %s).\n",
		[&](Character *character) {
			if ((character == actor) || (character == target))
				return false;
			if (character->combatHandler.charactersInSight.containsCharacter(
					actor))
				return false;
			return character->combatHandler.getAimedTarget() != actor;
		},
		target->getName(), hitRoll, armorClass, damageRoll);
}

void BasicAttack::handleNaturalWeaponMiss(
	Character *target, const std::shared_ptr<BodyPart::BodyWeapon> &weapon,
	unsigned int hitRoll, unsigned int armorClass)
{
	actor->sendMsg("You miss %s with your %s (%s vs %s).\n\n",
				   target->getName(), weapon->getName(true), hitRoll,
				   armorClass);
	target->sendMsg("%s misses you with %s %s (%s vs %s).\n\n",
					actor->getNameCapital(), actor->getPossessivePronoun(),
					weapon->getName(true), hitRoll, armorClass);
	target->room->sendToAll("%s misses %s with %s %s (%s vs %s).\n",
							{ actor, target }, actor->getNameCapital(),
							target->getName(), actor->getPossessivePronoun(),
							weapon->getName(true), hitRoll, armorClass);
}

void BasicAttack::handleMeleeMiss(Character *target, MeleeWeaponItem *weapon,
								  unsigned int hitRoll, unsigned int armorClass)
{
	// Notify the actor, enemy and others.
	actor->sendMsg("You miss %s with %s (%s vs %s).\n\n", target->getName(),
				   weapon->getName(true), hitRoll, armorClass);
	target->sendMsg("%s misses you with %s (%s vs %s).\n\n",
					actor->getNameCapital(), weapon->getName(true), hitRoll,
					armorClass);
	target->room->sendToAll("%s misses %s with %s (%s vs %s).\n",
							{ actor, target }, actor->getNameCapital(),
							target->getName(), weapon->getName(true), hitRoll,
							armorClass);
}

void BasicAttack::handleRangedMiss(Character *target, RangedWeaponItem *weapon,
								   unsigned int hitRoll,
								   unsigned int armorClass)
{
	// Notify the actor.
	actor->sendMsg("You fire and miss %s with %s (%s vs %s).\n\n",
				   target->getName(), weapon->getName(true), hitRoll,
				   armorClass);
	// Notify the other characters.
	if (actor->room == target->room) {
		target->sendMsg("%s fires an misses you (%s vs %s).\n\n",
						actor->getName(), hitRoll, armorClass);
		actor->room->funcSendToAll(
			"%s fires and misses %s with %s (%s vs %s).\n",
			[&](Character *character) {
				return !((character == actor) || (character == target));
			},
			actor->getNameCapital(), target->getName(), weapon->getName(true),
			hitRoll, armorClass);
		return;
	}
	// Notify the enemy.
	if (target->combatHandler.getAimedTarget() == actor) {
		target->sendMsg("%s fires an misses you (%s vs %s).\n\n",
						actor->getName(), hitRoll, armorClass);
	} else {
		target->sendMsg("Someone fired at you, but missed (%s vs %s).\n\n",
						hitRoll, armorClass);
	}
	actor->room->funcSendToAll(
		"%s fires and misses %s with %s (%s vs %s).\n",
		[&](Character *character) {
			if ((character == actor) || (character == target))
				return false;
			if (character->combatHandler.charactersInSight.containsCharacter(
					target))
				return true;
			return character->combatHandler.getAimedTarget() == target;
		},
		actor->getNameCapital(), target->getName(), weapon->getName(true),
		hitRoll, armorClass);

	actor->room->funcSendToAll(
		"%s fires at someone or something with %s (%s vs %s).\n",
		[&](Character *character) {
			if ((character == actor) || (character == target))
				return false;
			if (character->combatHandler.charactersInSight.containsCharacter(
					target))
				return false;
			return character->combatHandler.getAimedTarget() != target;
		},
		actor->getNameCapital(), weapon->getName(true), hitRoll, armorClass);

	target->room->funcSendToAll(
		"%s fires a projectile which whizzes nearby %s (%s vs %s).\n",
		[&](Character *character) {
			if ((character == actor) || (character == target))
				return false;
			if (character->combatHandler.charactersInSight.containsCharacter(
					actor))
				return true;
			return character->combatHandler.getAimedTarget() == actor;
		},
		actor->getNameCapital(), target->getName(), weapon->getName(true),
		hitRoll, armorClass);

	target->room->funcSendToAll(
		"Something whizzes nearby %s (%s vs %s).\n",
		[&](Character *character) {
			if ((character == actor) || (character == target))
				return false;
			if (character->combatHandler.charactersInSight.containsCharacter(
					actor))
				return false;
			return character->combatHandler.getAimedTarget() != actor;
		},
		target->getName(), hitRoll, armorClass);
}
