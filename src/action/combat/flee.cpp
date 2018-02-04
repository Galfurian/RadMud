/// @file   flee.cpp
/// @brief  Contais the implementation of the class for the action
///          of fleeing from combat.
/// @author Enrico Fraccaroli
/// @date   Jul 14 2016
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

#include "flee.hpp"

#include "structureUtils.hpp"
#include "characterUtilities.hpp"
#include "logger.hpp"
#include "room.hpp"
#include <cassert>

Flee::Flee(Character * _actor) :
    CombatAction(_actor)
{
    // Debugging message.
    Logger::log(LogLevel::Debug, "Created Flee.");
    // Reset the cooldown of the action.
    this->resetCooldown(this->getCooldown());
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
    if (this->getConsumedStamina(actor) > actor->stamina)
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
    // Check the values of the action.
    std::string error;
    if (!this->check(error))
    {
        actor->sendMsg(error + "\n\n");
        return ActionStatus::Error;
    }
    // Get the character chance of fleeing (D20).
    size_t fleeChance = TRand<size_t>(0, 20) +
                        actor->getAbilityModifier(Ability::Agility);
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
        // Get the list of available destinations.
        std::vector<Room *> destinations;
        for (auto it : StructUtils::getConnectedRooms(actor->room))
        {
            // Prepare the movement options.
            MovementOptions options;
            options.character = actor;
            if (StructUtils::checkConnection(options, actor->room, it, error))
            {
                destinations.emplace_back(it);
            }
        }
        // Pick a random direction, from the poll of the available ones.
        auto destination = destinations.at(
            TRand<size_t>(
                0, destinations.size() - 1));
        // Check that the picked destination is not a null pointer.
        if (destination == nullptr)
        {
            Logger::log(LogLevel::Error, "Flee selected null destination.");
            actor->sendMsg("You were not able to escape.\n\n");
        }
        else
        {
            // Consume the stamina.
            actor->remStamina(consumedStamina, true);
            // Stop the current action.
            actor->sendMsg(this->stop() + "\n\n");
            // Move the actor to the random direction.
            MoveCharacterTo(
                actor,
                destination,
                actor->getName() + " flees from the battlefield.\n\n",
                actor->getName() + " arives fleeing.\n\n",
                "You flee from the battlefield.\n");
            return ActionStatus::Finished;
        }
    }
    // Reset the cooldown.
    actor->getAction()->resetCooldown();
    // Return that the action is still running.
    return ActionStatus::Running;
}

unsigned int Flee::getCooldown()
{
    assert(actor && "Actor is nullptr");
    // BASE     [+5.0]
    // STRENGTH [-0.0 to -1.40]
    // AGILITY  [-0.0 to -1.40]
    // WEIGHT   [+1.6 to +2.51]
    // CARRIED  [+0.0 to +2.48]
    unsigned int cooldown = 5;
    cooldown = SafeSum(cooldown, -actor->getAbilityLog(Ability::Strength));
    cooldown = SafeSum(cooldown, -actor->getAbilityLog(Ability::Agility));
    cooldown = SafeSum(cooldown, SafeLog10(actor->weight));
    cooldown = SafeSum(cooldown, SafeLog10(actor->getCarryingWeight()));
    return cooldown;
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
    consumedStamina -= character->getAbilityLog(Ability::Strength);
    consumedStamina = SafeSum(consumedStamina,
                              SafeLog10(character->weight));
    consumedStamina = SafeSum(consumedStamina,
                              SafeLog10(character->getCarryingWeight()));
    return consumedStamina;
}
