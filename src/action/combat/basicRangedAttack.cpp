/// @file   basicRangedAttack.cpp
/// @brief  Contais the implementation of the class for the basic ranged attacks.
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

#include "basicRangedAttack.hpp"
#include "room.hpp"
#include "rangedWeaponItem.hpp"
#include "rangedWeaponModel.hpp"

BasicRangedAttack::BasicRangedAttack(Character * _actor) :
    CombatAction(_actor)
{
    Logger::log(LogLevel::Debug, "Created BasicRangedAttack.");
}

BasicRangedAttack::~BasicRangedAttack()
{
    Logger::log(LogLevel::Debug, "Deleted BasicRangedAttack.");
}

bool BasicRangedAttack::check(std::string & error) const
{
    if (!CombatAction::check(error))
    {
        return false;
    }
    return true;
}

ActionType BasicRangedAttack::getType() const
{
    return ActionType::Combat;
}

std::string BasicRangedAttack::getDescription() const
{
    return "fighting";
}

std::string BasicRangedAttack::stop()
{
    return "You stop fighting.";
}

ActionStatus BasicRangedAttack::perform()
{
    // Check if the cooldown is ended.
    if (!this->checkElapsed())
    {
        return ActionStatus::Running;
    }
    std::string error;
    if (!this->check(error))
    {
        actor->sendMsg(error + "\n\n");
        return ActionStatus::Finished;
    }
    // Iterate trough all the ranged weapons.
    auto activeWeapons = actor->getActiveRangedWeapons();
    bool canAttackTarget = false;
    for (auto iterator : activeWeapons)
    {
        Character * enemy = actor->aimedCharacter;
        // Check if the target is at range of the weapon.
        if (!actor->isAtRange(enemy, iterator->getRange()))
        {
            // If the aimed character is not at range, switch to a closer target.
            enemy = actor->getNextOpponentAtRange(iterator->getRange());
            if (enemy == nullptr)
            {
                actor->sendMsg("%s is not at range for %s.\n", actor->getNameCapital(), iterator->getName(true));
                continue;
            }
        }
        canAttackTarget = true;
        // Get the required stamina.
        auto consumedStamina = this->getConsumedStamina(actor, iterator);
        // Check if the actor has enough stamina to execute the action.
        if (consumedStamina > actor->getStamina())
        {
            actor->sendMsg("You are too tired to fire with %s.\n", iterator->getName(true));
            Logger::log(LogLevel::Debug, "[%s] Has %s stamina and needs %s.",
                        actor->getName(), actor->getStamina(), consumedStamina);
            continue;
        }
        // Natural roll for the attack.
        auto ATK = TRandInteger<unsigned int>(1, 20);
        Logger::log(LogLevel::Debug, "[%s] Natural roll of %s.", actor->getName(), ATK);
        // Check if:
        //  1. The number of active weapons is more than 1, then we have to apply
        //      a penalty to the attack roll.
        //  2. The value is NOT a natural 20 (hit).
        if ((activeWeapons.size() > 1) && (ATK != 20))
        {
            // Evaluate the penalty to the attack roll.
            unsigned int penalty = 0;
            // On the main hand the penalty is 6.
            if (iterator->currentSlot == EquipmentSlot::RightHand)
            {
                penalty = 6;
            }
                // On the off hand the penalty is 10.
            else if (iterator->currentSlot == EquipmentSlot::LeftHand)
            {
                penalty = 10;
            }
            // Log that we have applied a penalty.
            Logger::log(LogLevel::Debug, "[%s] Suffer a %s penalty with its %s.",
                        actor->getName(), penalty, GetEquipmentSlotName(iterator->currentSlot));
            // Safely apply the penalty.
            ATK = (ATK < penalty) ? 0 : (ATK - penalty);
        }
        // Evaluate the armor class of the aimed character.
        auto AC = actor->aimedCharacter->getArmorClass();
        // Log the overall attack roll.
        Logger::log(LogLevel::Debug, "[%s] Attack roll %s vs %s.", actor->getName(), ATK, AC);
        // Check if:
        //  1. The attack roll is lesser than the armor class of the opponent.
        //  2. The attack roll is not a natural 20.
        if ((ATK < AC) && (ATK != 20))
        {
            // Notify the actor.
            actor->sendMsg("You miss %s with %s.\n\n", enemy->getName(), iterator->getName(true));
            // Notify the enemy.
            enemy->sendMsg("%s misses you with %s.\n\n", actor->getName(), iterator->getName(true));
            // Notify the others.
            enemy->room->sendToAll("%s miss %s with %s.\n", {actor, enemy},
                                   actor->getName(), enemy->getName(), iterator->getName(true));
            // Consume half the stamina.
            actor->remStamina(consumedStamina / 2, true);
        }
        else
        {
            // Consume the stamina.
            actor->remStamina(consumedStamina, true);
            // Natural roll for the damage.
            auto DMG = iterator->rollDamage();
            // Log the damage roll.
            Logger::log(LogLevel::Debug, "[%s] Rolls a damage of %s.", actor->getName(), DMG);
            // If the character has rolled a natural 20, then multiply the damage by two.
            std::string criticalMsg;
            if (ATK == 20)
            {
                DMG *= 2;
                criticalMsg = "critically ";
            }
            // Log the damage.
            Logger::log(LogLevel::Debug, "[%s] Hits %s for with %s.",
                        actor->getName(), enemy->getName(), DMG, iterator->getName(true));
            // Procede and remove the damage from the health of the target.
            if (!enemy->remHealth(DMG))
            {
                actor->sendMsg("You %shit %s with %s and kill %s.\n\n",
                               criticalMsg, enemy->getName(), iterator->getName(true), enemy->getObjectPronoun());
                // Notify the enemy.
                enemy->sendMsg("%s %shits you with %s and kill you.\n\n",
                               actor->getName(), criticalMsg, iterator->getName(true));
                // Notify the others.
                enemy->room->sendToAll("%s %shits %s with %s and kill %s.\n", {actor, enemy},
                                       actor->getName(), criticalMsg, enemy->getName(),
                                       iterator->getName(true), enemy->getObjectPronoun());
                // The enemy has received the damage and now it is dead.
                if (actor->aimedCharacter != nullptr)
                {
                    if (enemy->getName() == actor->aimedCharacter->getName())
                    {
                        actor->aimedCharacter = nullptr;
                    }
                }
                enemy->kill();
                continue;
            }
            else
            {
                // The enemy has received the damage but it is still alive.
                actor->sendMsg("You %shit %s with %s for %s.\n\n",
                               criticalMsg, enemy->getName(), iterator->getName(true), DMG);
                // Notify the enemy.
                enemy->sendMsg("%s %shits you with %s for %s.\n\n",
                               actor->getName(), criticalMsg, iterator->getName(true), DMG);
                // Notify the others.
                enemy->room->sendToAll("%s %shits %s with %s for %s.\n", {actor, enemy},
                                       actor->getName(), criticalMsg, enemy->getName(),
                                       iterator->getName(true), DMG);
            }
        }
    }
    if ((actor->aimedCharacter == nullptr) && (actor->aggressionList.getSize() == 0))
    {
        actor->sendMsg(this->stop() + "\n\n");
        return ActionStatus::Finished;
    }
    if (!canAttackTarget)
    {
        actor->sendMsg(this->stop() + "\n\n");
        return ActionStatus::Finished;
    }
    // By default set the next combat action to basic attack.
    if (!actor->setNextCombatAction(CombatActionType::BasicRangedAttack))
    {
        actor->sendMsg(this->stop() + "\n\n");
        return ActionStatus::Finished;
    }
    return ActionStatus::Running;
}

CombatActionType BasicRangedAttack::getCombatActionType() const
{
    return CombatActionType::BasicRangedAttack;
}

unsigned int BasicRangedAttack::getConsumedStamina(Character * character, RangedWeaponItem * weapon)
{
    // BASE     [+1.0]
    // STRENGTH [-0.0 to -2.80]
    // WEIGHT   [+1.6 to +2.51]
    // CARRIED  [+0.0 to +2.48]
    // WEAPON   [+0.0 to +1.60]
    if (weapon->model->toRangedWeapon()->rangedWeaponType == RangedWeaponType::Thrown)
    {
        return static_cast<unsigned int>(1.0
                                         - character->getAbilityLog(Ability::Strength, 0.0, 1.0)
                                         + SafeLog10(character->weight)
                                         + SafeLog10(character->getCarryingWeight())
                                         + SafeLog10(weapon->getWeight(true)));
    }
    return 0;
}
