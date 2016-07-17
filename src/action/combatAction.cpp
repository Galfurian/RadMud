/// @file   combatAction.cpp
/// @brief  Implementation of the class for a move action.
/// @author Enrico Fraccaroli
/// @date   Jul 14 2016
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

#include "combatAction.hpp"
#include "../logger.hpp"
#include "../room.hpp"
#include "../character.hpp"
#include "../model/weaponModel.hpp"

using namespace std::chrono;

CombatAction::CombatAction(Character * _actor) :
        GeneralAction(_actor),
        combatAction(CombatActionType::NoAction)
{
    Logger::log(LogLevel::Debug, "Created combat action.");
}

CombatAction::~CombatAction()
{
    Logger::log(LogLevel::Debug, "Deleted combat action.");
}

bool CombatAction::check() const
{
    bool correct = GeneralAction::check();
    return correct;
}

ActionType CombatAction::getType() const
{
    return ActionType::Combat;
}

std::string CombatAction::getDescription() const
{
    return "fighting";
}

std::string CombatAction::stop()
{
    return "You stop fighting.";
}

bool CombatAction::perform()
{
    // Check if the cooldown is ended.
    if (this->checkElapsed())
    {
        // The sequence is the following:
        //  1. Check the list of opponents.
        //  2. Get the target. It could eighter be the top aggro or an allied.
        //  3. Check if the target is at range (depending on the action).
        //  4. If not return to 2.
        //  5. Otherwise perform the action.

        // Check the list of opponents.
        actor->opponents.checkList();
        if (combatAction != CombatActionType::NoAction)
        {
            // Perform the combat action.
            return this->performCombatAction(combatAction);
        }
        else
        {
            actor->sendMsg(this->stop());
            return true;
        }
    }
    return false;
}

CombatActionType CombatAction::getCombatActionType() const
{
    return this->combatAction;
}

bool CombatAction::setNextCombatAction(const CombatActionType & nextAction)
{
    if (!actor->opponents.hasOpponents())
    {
        Logger::log(LogLevel::Error, "The list of opponents is empty.");
        return false;
    }
    // Set the next combat action.
    combatAction = nextAction;
    // Set the action cooldown.
    unsigned int cooldown = actor->getCooldown(combatAction);
    actionCooldown = std::chrono::system_clock::now() + std::chrono::seconds(cooldown);
    Logger::log(
        LogLevel::Debug,
        "[%s] Next action in %s.",
        actor->getNameCapital(),
        ToString(cooldown));
    return true;
}

bool CombatAction::performCombatAction(const CombatActionType & move)
{
    // Retrive once and for all the name of the actor.
    std::string nam = actor->getNameCapital();
    if (move == CombatActionType::BasicAttack)
    {
        Logger::log(LogLevel::Debug, "[%s] Perform a BasicAttack.", nam);
        ItemVector activeWeapons = actor->getActiveWeapons();
        if (activeWeapons.empty())
        {
            actor->sendMsg("You do not have a valid weapon equipped.\n");
        }
        else
        {
            for (auto iterator : activeWeapons)
            {
                WeaponModel * weapon = iterator->model->toWeapon();
                // Get the top aggro enemy at range.
                Character * enemy = actor->getNextOpponentAtRange(weapon->range);
                if (enemy == nullptr)
                {
                    actor->sendMsg(
                        "You do not have opponents at reange for %s.\n",
                        iterator->getName());
                    continue;
                }
                // Will contain the required stamina.
                unsigned int consumedStamina;
                // Check if the actor has enough stamina to execute the action.
                if (!actor->hasStaminaFor(
                    consumedStamina,
                    ActionType::Combat,
                    CombatActionType::BasicAttack,
                    iterator->getCurrentSlot()))
                {
                    actor->sendMsg("You are too tired to attack with %s.\n", iterator->getName());
                    Logger::log(
                        LogLevel::Debug,
                        "[%s] Has %s stamina and needs %s.",
                        nam,
                        ToString(actor->getStamina()),
                        ToString(consumedStamina));
                    continue;
                }
                // Consume the stamina.
                actor->remStamina(consumedStamina, true);
                // Natural roll for the attack.
                unsigned int ATK = TRandInteger<unsigned int>(1, 20);
                // Log the rolled value.
                Logger::log(LogLevel::Debug, "[%s] Natural roll of %s.", nam, ToString(ATK));
                // Check if:
                //  1. The number of active weapons is more than 1, then we have to apply
                //      a penality to the attack roll.
                //  2. The value is NOT a natural 20 (hit).
                if ((activeWeapons.size() > 1) && (ATK != 20))
                {
                    // Evaluate the penality to the attack roll.
                    unsigned int penality = 0;
                    // On the main hand the penality is 6.
                    if (iterator->currentSlot == EquipmentSlot::RightHand)
                    {
                        penality = 6;
                    }
                    // On the off hand the penality is 10.
                    else if (iterator->currentSlot == EquipmentSlot::LeftHand)
                    {
                        penality = 10;
                    }
                    // Log that we have applied a penality.
                    Logger::log(
                        LogLevel::Debug,
                        "[%s] Suffer a %s penalty with its %s.",
                        nam,
                        ToString(penality),
                        GetEquipmentSlotName(iterator->currentSlot));
                    // Safely apply the penality.
                    if (ATK < penality)
                    {
                        ATK = 0;
                    }
                    else
                    {
                        ATK -= penality;
                    }
                }
                // Evaluate the armor class of the enemy.
                unsigned int AC = enemy->getArmorClass();
                // Log the overall attack roll.
                Logger::log(
                    LogLevel::Debug,
                    "[%s] Attack roll %s vs %s.",
                    nam,
                    ToString(ATK),
                    ToString(AC));
                // Check if:
                //  1. The attack roll is lesser than the armor class of the opponent.
                //  2. The attack roll is not a natural 20.
                if ((ATK < AC) && (ATK != 20))
                {
                    // Notify the actor.
                    actor->sendMsg(
                        "You miss %s with %s.\n\n",
                        enemy->getName(),
                        iterator->getName());
                    // Notify the enemy.
                    enemy->sendMsg("%s misses you with %s.\n\n", nam, iterator->getName());
                    // Notify the others.
                    CharacterVector exceptions;
                    exceptions.push_back(actor);
                    exceptions.push_back(enemy);
                    enemy->room->sendToAll(
                        "%s miss %s with %s.\n",
                        exceptions,
                        nam,
                        enemy->getName(),
                        iterator->getName());
                }
                else
                {
                    // Store the type of attack.
                    bool isCritical;
                    // Natural roll for the damage.
                    unsigned int DMG = TRandInteger<unsigned int>(
                        weapon->minDamage,
                        weapon->maxDamage);
                    // Log the damage roll.
                    Logger::log(LogLevel::Debug, "[%s] Rolls a damage of %s.", nam, ToString(DMG));
                    // Check if the character is wielding a two-handed weapon.
                    if (activeWeapons.size() == 1)
                    {
                        if (HasFlag(iterator->model->modelFlags, ModelFlag::TwoHand))
                        {
                            // Get the strenth modifier.
                            unsigned int STR = actor->getAbilityModifier(Ability::Strength);
                            // Add to the damage rool one and half the strenth value.
                            DMG += STR + (STR / 2);
                            // Log the additional damage.
                            Logger::log(LogLevel::Debug, "[%s] Add 1-1/2 times its Strength.", nam);
                        }
                    }
                    // If the character has rolled a natural 20, then multiply the damage by two.
                    if (ATK == 20)
                    {
                        DMG *= 2;
                        isCritical = true;
                    }
                    // Log the damage.
                    Logger::log(
                        LogLevel::Debug,
                        "[%s] Hits %s for with %s.",
                        nam,
                        enemy->getName(),
                        ToString(DMG),
                        iterator->getName());
                    // Procede and remove the damage from the health of the target.
                    if (!enemy->remHealth(DMG))
                    {
                        actor->sendMsg(
                            "You %s hit %s with %s and kill %s.\n\n",
                            (isCritical ? "critically" : ""),
                            enemy->getName(),
                            iterator->getName(),
                            enemy->getObjectPronoun());
                        // Notify the enemy.
                        enemy->sendMsg(
                            "%s %s hits you with %s and kill you.\n\n",
                            nam,
                            (isCritical ? "critically" : ""),
                            iterator->getName());
                        // Notify the others.
                        CharacterVector exceptions;
                        exceptions.push_back(actor);
                        exceptions.push_back(enemy);
                        enemy->room->sendToAll(
                            "%s %s hits %s with %s and kill %s.\n",
                            exceptions,
                            nam,
                            (isCritical ? "critically" : ""),
                            enemy->getName(),
                            iterator->getName(),
                            enemy->getObjectPronoun());
                        // The enemy has received the damage and now it is dead.
                        enemy->kill();
                        continue;
                    }
                    else
                    {
                        // The enemy has received the damage but it is still alive.
                        actor->sendMsg(
                            "You %s hit %s with %s for %s.\n\n",
                            (isCritical ? "critically" : ""),
                            enemy->getName(),
                            iterator->getName(),
                            ToString(DMG));
                        // Notify the enemy.
                        enemy->sendMsg(
                            "%s %s hits you with %s for %s.\n\n",
                            nam,
                            (isCritical ? "critically" : ""),
                            iterator->getName(),
                            ToString(DMG));
                        // Notify the others.
                        CharacterVector exceptions;
                        exceptions.push_back(actor);
                        exceptions.push_back(enemy);
                        enemy->room->sendToAll(
                            "%s %s hits %s with %s for %s.\n",
                            exceptions,
                            nam,
                            (isCritical ? "critically" : ""),
                            enemy->getName(),
                            iterator->getName(),
                            ToString(DMG));
                    }
                }
            }
        }
    }
    else if (move == CombatActionType::Flee)
    {
        // Get the character chance of fleeing (D20).
        unsigned int fleeChance = TRandInteger<unsigned int>(0, 20);
        fleeChance += actor->getAbilityModifier(Ability::Agility);
        // Will contain the required stamina.
        unsigned int consumedStamina;
        // Base the escape level on how many enemies are surrounding the character.
        if (fleeChance < static_cast<unsigned int>(actor->opponents.getSize()))
        {
            actor->sendMsg("You were not able to escape from your attackers.\n");
        }
        // Check if the actor has enough stamina to execute the action.
        else if (!actor->hasStaminaFor(consumedStamina, ActionType::Combat, CombatActionType::Flee))
        {
            actor->sendMsg("You are too tired to flee.\n");
            Logger::log(
                LogLevel::Debug,
                "[%s] Has %s stamina and needs %s.",
                nam,
                ToString(actor->getStamina()),
                ToString(consumedStamina));
        }
        else
        {
            // Consume the stamina.
            actor->remStamina(consumedStamina, true);
            // Get the list of available directions.
            std::vector<Direction> directions = actor->room->getAvailableDirections();
            // Check if there are some directions.
            if (!directions.empty())
            {
                // Pick a random direction, from the poll of the available ones.
                Direction randomDirection = directions.at(
                    TRandInteger<size_t>(0, directions.size() - 1));
                // Get the selected exit.
                std::shared_ptr<Exit> selected = actor->room->findExit(randomDirection);
                // Check that the picked exit is not a null pointer.
                if (selected == nullptr)
                {
                    Logger::log(LogLevel::Error, "Selected null exit during action Flee.");
                    actor->sendMsg("You were not able to escape from your attackers.\n");
                }
                else
                {
                    // Stop the current action.
                    actor->sendMsg(this->stop());
                    // Move the actor to the random direction.
                    actor->moveTo(
                        selected->destination,
                        nam + " flees from the battlefield.\n\n",
                        nam + " arives fleeing.\n\n",
                        "You flee from the battlefield.\n");
                    return true;
                }
            }
        }
    }
    // By default set the next combat action to basic attack.
    if (!this->setNextCombatAction(CombatActionType::BasicAttack))
    {
        actor->sendMsg(this->stop() + "\n\n");
        return true;
    }
    return false;
}
