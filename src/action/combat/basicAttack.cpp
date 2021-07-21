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

#include "action/combat/basicAttack.hpp"

#include "character/characterUtilities.hpp"
#include "model/submodel/rangedWeaponModel.hpp"
#include "utilities/formatter.hpp"
#include "updater/updater.hpp"
#include "action/combat/chase.hpp"
#include "structure/room.hpp"
#include "structure/area.hpp"

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
	MudLog(LogLevel::Debug, "%s performs a BasicAttack.", actor->getNameCapital());
	if (!this->check(error)) {
		actor->sendMsg(error + "\n\n");
		return ActionStatus::Error;
	}
	// If there are no enemies, just stop fighting.
	if (actor->combatHandler.empty()) {
		MudLog(LogLevel::Debug, "%s has no opponents.", actor->getNameCapital());
		// Stop the combat.
		this->handleStop();
		// Return that the action is finished.
		return ActionStatus::Finished;
	}

	// Set the progressive number of the attack to 0.
	int attacks = 0;

	// Retrieve all the melee, ranged, and natural weapons.
	auto meleeWeapons = GetActiveWeapons<MeleeWeaponItem>(actor);
	auto rangedWeapons = GetActiveWeapons<RangedWeaponItem>(actor);
	std::vector<std::shared_ptr<BodyPart::BodyWeapon> > naturalWeapons;
	if (meleeWeapons.empty() && rangedWeapons.empty())
		naturalWeapons = GetActiveNaturalWeapons(actor);

	// Prepare iterators for each category of weapons.
	auto melee_it = meleeWeapons.begin();
	auto rangd_it = rangedWeapons.begin();
	auto natrl_it = naturalWeapons.begin();

	// Iterate until we have used all the weapons.
	while ((melee_it != meleeWeapons.end()) || (rangd_it != rangedWeapons.end()) ||
		   (natrl_it != naturalWeapons.end())) {
		// If we cannot find a valid target, stop.
		if (!this->setPredefinedTarget()) {
			MudLog(LogLevel::Debug, "%s cannot set predefined target, break.", actor->getNameCapital());
			break;
		}
		// Get the predefined target.
		Character *pred = actor->combatHandler.getPredefinedTarget();
		// If the actor and the pred-target are in the same room,
		//  and there are still melee weapons to use, attack.
		if (melee_it != meleeWeapons.end()) {
			MudLog(LogLevel::Debug, "%s can MELEE attack with %s.", actor->getNameCapital(), (*melee_it)->getName());
			// Check if the target is at range of the weapon.
			if (!actor->isAtRange(pred, (*melee_it)->getRange())) {
				actor->sendMsg("You can't reach %s with %s.\n\n", pred->getName(), (*melee_it)->getName());
				++melee_it;
				continue;
			}
			// Increment the number of executed attacks.
			++attacks;
			// Perform the attack, if we kill the target, continue.
			if (this->performAttack(pred, *melee_it++, nullptr, nullptr, attacks) == AttackStatus ::Killed) {
				continue;
			}
		}
		if (rangd_it != rangedWeapons.end()) {
			MudLog(LogLevel::Debug, "%s can RANGED attack with %s.", actor->getNameCapital(), (*rangd_it)->getName());
			// Check if the target is at range of the weapon.
			if (!actor->isAtRange(pred, (*rangd_it)->getRange())) {
				actor->sendMsg("You can't reach %s with %s.\n\n", pred->getName(), (*rangd_it)->getName());
				++rangd_it;
				continue;
			}
			// Increment the number of executed attacks.
			++attacks;
			// Perform the attack, if we kill the target, continue.
			if (this->performAttack(pred, nullptr, *rangd_it++, nullptr, attacks) == AttackStatus ::Killed) {
				continue;
			}
		}
		// Perform the attack for each natural weapon.
		if (natrl_it != naturalWeapons.end()) {
			MudLog(LogLevel::Debug, "%s can NAT attack with %s.", actor->getNameCapital(), (*natrl_it)->getName());
			// Check if the target is at range of the weapon.
			if (!actor->isAtRange(pred, (*natrl_it)->range)) {
				actor->sendMsg("You can't reach %s with %s.\n\n", pred->getName(), (*natrl_it)->getName());
				++natrl_it;
				continue;
			}
			// Increment the number of executed attacks.
			++attacks;
			// Perform the attack, if we kill the target, continue.
			if (this->performAttack(pred, nullptr, nullptr, *natrl_it++, attacks) == AttackStatus ::Killed) {
				continue;
			}
		}
	}
	// Check if all the opponents are dead.
	if (actor->combatHandler.empty()) {
		MudLog(LogLevel::Debug, "%s has no more opponents.", actor->getNameCapital());
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
	consumedStamina = SafeSum(consumedStamina, SafeLog10(character->getCarryingWeight()));
	if (weapon == nullptr) {
		return consumedStamina;
	}
	if (weapon->getType() == ModelType::MeleeWeapon) {
		consumedStamina = SafeSum(consumedStamina, SafeLog10(weapon->getWeight(true)));
	} else if (weapon->getType() == ModelType::RangedWeapon) {
		if (weapon->model->to<RangedWeaponModel>()->rangedWeaponType != RangedWeaponType::Thrown) {
			return 0;
		}
		consumedStamina = SafeSum(consumedStamina, SafeLog10(weapon->getWeight(true)));
	}
	return consumedStamina;
}

unsigned int BasicAttack::getConsumedStamina(Character *character, const std::shared_ptr<BodyPart::BodyWeapon> &)
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

unsigned int BasicAttack::Attack::hit_compute() const
{
	unsigned int result = hit.base;
	result = SafeSum(result, hit.multi_attack, 1U);
	result = SafeSum(result, hit.effects, 1U);
	return result;
}

unsigned int BasicAttack::Attack::dmg_compute() const
{
	unsigned int result = dmg.base;
	result = SafeSum(result, dmg.ability, 0U);
	result = SafeSum(result, dmg.effects, 0U);
	// If the character has rolled a critical: multiply the damage by two.
	result *= 2U - !this->is_critical();
	return result;
}

bool BasicAttack::Attack::is_hit() const
{
	//  1. The hit roll is lesser than the armor class of the opponent.
	//  2. The hit roll is not a critical.
	if (this->is_critical())
		return true;
	return this->hit_compute() >= enemy.ac;
}

bool BasicAttack::setPredefinedTarget()
{
	// If there is a predefined target, check if it is a valid target.
	if (actor->combatHandler.getPredefinedTarget() != nullptr) {
		MudLog(LogLevel::Debug, "[%s] Has a predefined target.", actor->getNameCapital());
		if (this->checkTarget(actor->combatHandler.getPredefinedTarget())) {
			MudLog(LogLevel::Debug, "[%s] Predefined target is a valid target.", actor->getNameCapital());
			return true;
		}
		MudLog(LogLevel::Debug, "[%s] Predefined target is NOT a valid target.", actor->getNameCapital());
	} else {
		MudLog(LogLevel::Debug, "[%s] Has no predefined target.", actor->getNameCapital());
	}
	// Take a valid target.
	for (auto const &it : actor->combatHandler) {
		if (this->checkTarget(it->aggressor)) {
			MudLog(LogLevel::Debug, "[%s] Has a new predefined target: %s", actor->getNameCapital(),
				   it->aggressor->getNameCapital());
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
		MudLog(LogLevel::Debug, "[%s] Either the actor or the target are in a nullptr room.", actor->getNameCapital());
		return false;
	}
	// Check if they are at close range.
	if (actor->room->coord == target->room->coord) {
		MudLog(LogLevel::Debug, "[%s] The actor and the target are in the same room.", actor->getNameCapital());
		return true;
	}
	// Check if there is no aimed character.
	if (actor->combatHandler.getAimedTarget() == nullptr) {
		MudLog(LogLevel::Debug, "[%s] The actor has no aimed character, so the target cannot be attacked.",
			   actor->getNameCapital());
		return false;
	}
	// If at long range, check if the target is the aimed character.
	if (actor->combatHandler.getAimedTarget() == target) {
		MudLog(LogLevel::Debug, "[%s] The aimed character and the target are the same character.",
			   actor->getNameCapital());
		// Retrieve all the ranged weapons.
		auto rangedWeapons = GetActiveWeapons<RangedWeaponItem>(actor);
		// Check if the actor has no ranged weapon equipped.
		if (rangedWeapons.empty()) {
			MudLog(LogLevel::Debug, "[%s] The actor has no ranged weapon equipped.", actor->getNameCapital());
			return false;
		}
		// Just check if AT LEAST one of the equipped ranged weapons can be used
		//  to attack the target.
		// TODO: This does not check if the weapon is USABLE!
		for (auto weapon : rangedWeapons) {
			if (actor->isAtRange(target, weapon->getRange())) {
				MudLog(LogLevel::Debug, "[%s] The target is at range with %s.", actor->getNameCapital(),
					   weapon->getName(false));
				return true;
			}
		}
	}
	MudLog(LogLevel::Debug, "[%s] No valid target has been found.", actor->getNameCapital());
	return false;
}

BasicAttack::AttackStatus BasicAttack::performAttack(Character *target, MeleeWeaponItem *weapon_melee,
													 RangedWeaponItem *weapon_ranged,
													 const std::shared_ptr<BodyPart::BodyWeapon> &weapon_natural,
													 int attackNumber)
{
	// Prepare some variables, fro readability.
	auto &effects = actor->effectManager;
	auto &skills = actor->skillManager;
	// The specific combat modifiers.
	CombatModifier hitModType;
	CombatModifier damModType;
	// A string representing the weapon name.
	std::string weaponName;
	// Prepare the structures used to compute hit and damage rolls.
	Attack attack;

	// -------------------------------------------------------------------------
	// Preamble: Evaluate all the values which are required by the function.
	// -------------------------------------------------------------------------
	// Get the strength modifier.
	int str_mod = actor->getAbilityModifier(Ability::Strength);
	int per_mod = actor->getAbilityModifier(Ability::Perception);
	// Get the required stamina, and the type of modifier.
	unsigned int consumedStamina = 0;
	if (weapon_melee) {
		consumedStamina = this->getConsumedStamina(actor, weapon_melee);
		hitModType = CombatModifier::MeleeWeaponHitRoll;
		damModType = CombatModifier::MeleeWeaponDamage;
		weaponName = weapon_melee->getName(true);
	} else if (weapon_ranged) {
		consumedStamina = this->getConsumedStamina(actor, weapon_ranged);
		hitModType = CombatModifier::RangedWeaponHitRoll;
		damModType = CombatModifier::RangedWeaponDamage;
		weaponName = weapon_ranged->getName(true);
	} else {
		consumedStamina = this->getConsumedStamina(actor, weapon_natural);
		hitModType = CombatModifier::UnarmedHitRoll;
		damModType = CombatModifier::UnarmedDamage;
		weaponName = weapon_natural->getName(true);
	}

	// -------------------------------------------------------------------------
	// Phase 1: Check if the actor has enough stamina to execute the action.
	// -------------------------------------------------------------------------
	if (consumedStamina > actor->stamina) {
		actor->sendMsg("You are too tired to attack with %s.\n", weaponName);
		return AttackStatus::Failed;
	}

	// -------------------------------------------------------------------------
	// Phase 2: Roll the hit and damage.
	// -------------------------------------------------------------------------
	// Evaluate the natural roll for the attack.
	attack.hit.base = TRand<unsigned int>(1, 20);
	// Apply the penalty due to multi-wield, check if:
	//  1. The number of already executed attacks is greater-than 1.
	//  2. The attack is not a critical hit.
	if ((attackNumber > 1) && (!attack.is_critical())) {
		// Evaluate the penalty to the hit roll.
		attack.hit.multi_attack = -((attackNumber - 1) * 3);
	}
	// Get the hit modifier.
	attack.hit.effects = effects.getCombatMod(hitModType);
	// Improve the skill.
	skills.improveCombat(hitModType);

	// -------------------------------------------------------------------------
	// Phase 3: Check if the target is hit.
	// -------------------------------------------------------------------------
	// Evaluate the armor class of the enemy.
	attack.enemy.ac = target->getArmorClass();
	// Check if it is a hit.
	if (!attack.is_hit()) {
		// Show miss messages.
		this->showAtkMsg(target, weapon_melee, weapon_ranged, weapon_natural, attack);
		// Consume half the stamina.
		actor->remStamina(consumedStamina / 2, true);
	} else {
		// Roll the damage and apply specific modifiers.
		if (weapon_melee) {
			attack.dmg.base = weapon_melee->rollDamage();
			// Check if the character is wielding a two-handed melee weapon.
			bool two_handed = HasFlag(weapon_melee->model->modelFlags, ModelFlag::TwoHand);
			// Add the strength modifier and also half of it if it is a two-handed weapon.
			attack.dmg.ability = str_mod + two_handed * (str_mod / 2);
		} else if (weapon_ranged) {
			attack.dmg.base = weapon_ranged->rollDamage();
			// Add the perception modifier.
			attack.dmg.ability = per_mod;
		} else {
			attack.dmg.base = weapon_natural->rollDamage();
			// Add the strength modifier.
			attack.dmg.ability = str_mod;
		}
		// Apply the modifier to the damage roll when unarmed fighting.
		attack.dmg.effects = effects.getCombatMod(damModType);
		// Improve the skills which has contributed to the damage roll.
		actor->skillManager.improveCombat(damModType);
		// Show hit messages.
		this->showAtkMsg(target, weapon_melee, weapon_ranged, weapon_natural, attack);
		// Consume the stamina.
		actor->remStamina(consumedStamina, true);
		// The damage.
		unsigned int damage = attack.dmg_compute();
		// Proceed and remove the damage from the health of the target.
		if (!target->remHealth(damage)) {
			// Notify the others.
			target->room->sendToAll("%s%s screams in pain and then die!%s\n", { target }, Formatter::red(),
									target->getNameCapital(), Formatter::reset());
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
		aggro += ((damage * 100) / target->getMaxHealth(true));
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

void BasicAttack::showAtkMsg(Character *target, MeleeWeaponItem *weapon_melee, RangedWeaponItem *weapon_ranged,
							 const std::shared_ptr<BodyPart::BodyWeapon> &weapon_natural, const Attack &attack)
{
	const bool has_hit = attack.is_hit();
	// Get the name of the actor.
	std::string name_act = actor->getNameCapital();
	// Get the name of the target.
	std::string name_tar = target->getName();
	// Prepare the hit/miss messages.
	std::string hit_miss = (has_hit) ? Formatter::green("hit") : Formatter::red("miss");
	std::string hit_miss_3d = (has_hit) ? Formatter::green("hits") : Formatter::red("misses");
	// Prepare the string describing the rolls.
	std::string rolls = std::to_string(attack.hit.base);
	rolls += (attack.hit.multi_attack < 0 ? "" : "+") + std::to_string(attack.hit.multi_attack);
	rolls += (attack.hit.effects < 0 ? "" : "+") + std::to_string(attack.hit.effects);
	rolls += "=" + std::to_string(attack.hit_compute());
	if (has_hit) {
		rolls += " for " + std::to_string(attack.dmg.base);
		rolls += "+" + std::to_string(attack.dmg.ability);
		rolls += (attack.dmg.effects < 0 ? "" : "+") + std::to_string(attack.dmg.effects);
		rolls += "=" + std::to_string(attack.dmg_compute());
	}

	if (weapon_melee) {
		std::string name_wpn = weapon_melee->getName(true);

		actor->sendMsg("You %s %s with %s (%s).\n\n", hit_miss, name_tar, name_wpn, rolls);
		target->sendMsg("%s %s you with %s (%s).\n\n", name_act, hit_miss_3d, name_wpn, rolls);
		target->room->funcSendToAll(
			"%s %s %s with %s (%s).\n",
			[&](Character *character) { return !((character == actor) || (character == target)); }, name_act,
			hit_miss_3d, name_tar, name_wpn, rolls);

	} else if (weapon_natural) {
		std::string name_wpn = weapon_natural->getName(true);
		std::string pron = actor->getPossessivePronoun();

		actor->sendMsg("You %s %s with your %s (%s).\n\n", hit_miss, name_tar, name_wpn, rolls);
		target->sendMsg("%s %s you with %s %s (%s).\n\n", name_act, hit_miss_3d, pron, name_wpn, rolls);
		target->room->funcSendToAll(
			"%s %s %s with %s %s (%s).\n",
			[&](Character *character) { return !((character == actor) || (character == target)); }, name_act,
			hit_miss_3d, name_tar, pron, name_wpn, rolls);
	} else {
		std::string name_wpn = weapon_ranged->getName(true);
		// The actor receives the message.
		actor->sendMsg("You %s %s with %s (%s).\n\n", hit_miss, name_tar, name_wpn, rolls);
		// The target receives a message stating WHO shot, if he is in the same room, or he is
		//  currently focused on WHOM is shooting at him. Otherwise, he just know that something
		//  hit him.
		if ((actor->room == target->room) || (target->combatHandler.getAimedTarget() == actor)) {
			target->sendMsg("%s fires a projectile with %s which %s you (%s).\n\n", name_act, name_wpn, hit_miss_3d,
							rolls);
		} else {
			target->sendMsg("Someone fires a projectile that %s you (%s).\n\n", hit_miss_3d, rolls);
		}
		// Send a message in the same room of the ACTOR. If, WHO is in the same room with the
		//  ACTOR CAN see his TARGET, we show the TARGET.
		actor->room->funcSendToAll(
			"%s fires with %s and %s %s (%s).\n",
			[&](Character *character) {
				if ((character == actor) || (character == target))
					return false;
				if (character->combatHandler.charactersInSight.containsCharacter(target))
					return true;
				return character->combatHandler.getAimedTarget() == target;
			},
			name_act, name_wpn, hit_miss_3d, name_tar, rolls);
		// Send a message in the same room of the ACTOR. If, WHO is in the same room with the
		//  ACTOR CANNOT see his TARGET, we DO NOT show the TARGET.
		actor->room->funcSendToAll(
			"%s fires with %s at someone or something (%s).\n",
			[&](Character *character) {
				if ((character == actor) || (character == target))
					return false;
				if (character->combatHandler.charactersInSight.containsCharacter(target))
					return false;
				return character->combatHandler.getAimedTarget() != target;
			},
			name_act, name_wpn, rolls);

		// Send a message in the same room of the TARGET. If, WHO is in the same room with the
		//  TARGET CAN see the ACTOR, we show the ACTOR.
		target->room->funcSendToAll(
			"%s fires a projectile with %s which %s %s (%s).\n",
			[&](Character *character) {
				if ((character == actor) || (character == target))
					return false;
				if (character->combatHandler.charactersInSight.containsCharacter(actor))
					return true;
				return character->combatHandler.getAimedTarget() == actor;
			},
			name_act, name_wpn, hit_miss_3d, name_tar, rolls);

		// Send a message in the same room of the TARGET. If, WHO is in the same room with the
		//  TARGET CANNOT see the ACTOR, we DO NOT show the ACTOR.
		target->room->funcSendToAll(
			"Someone fires a projectile which %s %s (%s).\n",
			[&](Character *character) {
				if ((character == actor) || (character == target))
					return false;
				if (character->combatHandler.charactersInSight.containsCharacter(actor))
					return false;
				return character->combatHandler.getAimedTarget() != actor;
			},
			hit_miss_3d, name_tar, rolls);
	}
}
