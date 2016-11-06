/// @file   flee.cpp
/// @brief  Contais the implementation of the class for the action of fleeing from combat.
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

#include "flee.hpp"
#include "room.hpp"
#include "basicAttack.hpp"

Flee::Flee(Character * _actor) :
    CombatAction(_actor)
{
    Logger::log(LogLevel::Debug, "Created Flee.");
}

Flee::~Flee()
{
    Logger::log(LogLevel::Debug, "Deleted Flee.");
}

bool Flee::check(std::string & error) const
{
    if (!CombatAction::check(error))
    {
        return false;
    }
    if (actor->room->exits.empty())
    {
        error = "There is no way of escape.";
        return false;
    }
    // Check if the actor has enough stamina to execute the action.
    if (this->getConsumedStamina(actor) > actor->getStamina())
    {
        error = "You are too tired to flee from the battle.";
        return false;
    }
    return true;
}

ActionType Flee::getType() const
{
    return ActionType::Combat;
}

std::string Flee::getDescription() const
{
    return "fleeing";
}

std::string Flee::stop()
{
    return "You stop fleeing.";
}

ActionStatus Flee::perform()
{
    // Check if the cooldown is ended.
    if (!this->checkElapsed())
    {
        return ActionStatus::Running;
    }
    // Check the values of the action.
    std::string error;
    if (!this->check(error))
    {
        actor->sendMsg(error + "\n\n");
        return ActionStatus::Error;
    }
    auto counter = actor->actionQueue.size();
    for (auto it : actor->actionQueue)
    {
        Logger::log(LogLevel::Debug, "[%s][%s] %s", actor->getName(), counter, it->getDescription());
        counter--;
    }
    // Get the character chance of fleeing (D20).
    auto fleeChance = TRandInteger<unsigned int>(0, 20) + actor->getAbilityModifier(Ability::Agility);
    // Get the required stamina.
    auto consumedStamina = this->getConsumedStamina(actor);
    // Base the escape level on how many enemies are surrounding the character.
    if (fleeChance < static_cast<unsigned int>(actor->combatHandler.getSize()))
    {
        actor->sendMsg("You were not able to escape from your attackers.\n");
        // Consume half the stamina.
        actor->remStamina(consumedStamina / 2, true);
    }
    else
    {
        // Get the list of available directions.
        auto directions = actor->room->getAvailableDirections();
        // Pick a random direction, from the poll of the available ones.
        auto randomDirValue = TRandInteger<size_t>(0, directions.size() - 1);
        auto randomDirection = directions.at(randomDirValue);
        // Get the selected exit.
        auto selected = actor->room->findExit(randomDirection);
        // Check that the picked exit is not a null pointer.
        if (selected == nullptr)
        {
            Logger::log(LogLevel::Error, "Selected null exit during action Flee.");
            actor->sendMsg("You were not able to escape from your attackers.\n");
        }
        else
        {
            // Consume the stamina.
            actor->remStamina(consumedStamina, true);
            // Stop the current action.
            actor->sendMsg(this->stop() + "\n\n");
            // Move the actor to the random direction.
            actor->moveTo(
                selected->destination,
                actor->getName() + " flees from the battlefield.\n\n",
                actor->getName() + " arives fleeing.\n\n",
                "You flee from the battlefield.\n");
            return ActionStatus::Finished;
        }
    }
    // Reset the cooldown.
    actor->getAction()->resetCooldown(BasicAttack::getCooldown(actor));
    // Return that the action is still running.
    return ActionStatus::Running;
}

CombatActionType Flee::getCombatActionType() const
{
    return CombatActionType::Flee;
}

unsigned int Flee::getConsumedStamina(Character * character)
{
    // BASE     [+1.0]
    // STRENGTH [-0.0 to -1.40]
    // WEIGHT   [+1.6 to +2.51]
    // CARRIED  [+0.0 to +2.48]
    unsigned int consumedStamina = 1;
    consumedStamina -= character->getAbilityLog(Ability::Strength, 0.0, 1.0);
    consumedStamina = SafeSum(consumedStamina, SafeLog10(character->weight));
    consumedStamina = SafeSum(consumedStamina, SafeLog10(character->getCarryingWeight()));
    return consumedStamina;
}

unsigned int Flee::getCooldown(Character * character)
{
    // BASE     [+5.0]
    // STRENGTH [-0.0 to -1.40]
    // AGILITY  [-0.0 to -1.40]
    // WEIGHT   [+1.6 to +2.51]
    // CARRIED  [+0.0 to +2.48]
    // WEAPON   [+0.0 to +1.60]
    unsigned int cooldown = 5;
    cooldown -= character->getAbilityLog(Ability::Strength, 0.0, 1.0);
    cooldown -= character->getAbilityLog(Ability::Agility, 0.0, 1.0);
    cooldown = SafeSum(cooldown, SafeLog10(character->weight));
    cooldown = SafeSum(cooldown, SafeLog10(character->getCarryingWeight()));
    if (character->canAttackWith(EquipmentSlot::RightHand))
    {
        auto weapon = character->findEquipmentSlotItem(EquipmentSlot::RightHand);
        cooldown = SafeSum(cooldown, SafeLog10(weapon->getWeight(true)));
    }
    if (character->canAttackWith(EquipmentSlot::LeftHand))
    {
        auto weapon = character->findEquipmentSlotItem(EquipmentSlot::RightHand);
        cooldown = SafeSum(cooldown, SafeLog10(weapon->getWeight(true)));
    }
    return cooldown;
}
