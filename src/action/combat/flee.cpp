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
#include "../../structure/room.hpp"

Flee::Flee(Character * _actor) :
    CombatAction(_actor)
{
    Logger::log(LogLevel::Debug, "Created Flee.");
}

Flee::~Flee()
{
    Logger::log(LogLevel::Debug, "Deleted Flee.");
}

bool Flee::check() const
{
    bool correct = CombatAction::check();
    return correct;
}

ActionType Flee::getType() const
{
    return ActionType::Combat;
}

std::string Flee::getDescription() const
{
    return "fighting";
}

std::string Flee::stop()
{
    return "You stop fighting.";
}

ActionStatus Flee::perform()
{
    // Check if the cooldown is ended.
    if (!this->checkElapsed())
    {
        return ActionStatus::Running;
    }
    Logger::log(LogLevel::Debug, "[%s] Perform a Flee.", actor->getName());
    // Get the character chance of fleeing (D20).
    auto fleeChance = TRandInteger<unsigned int>(0, 20)
                      + actor->getAbilityModifier(Ability::Agility);
    // Get the required stamina.
    auto consumedStamina = this->getConsumedStamina(actor);
    // Base the escape level on how many enemies are surrounding the character.
    if (fleeChance < static_cast<unsigned int>(actor->opponents.getSize()))
    {
        actor->sendMsg("You were not able to escape from your attackers.\n");
        // Consume half the stamina.
        actor->remStamina(consumedStamina / 2, true);
    }
        // Check if the actor has enough stamina to execute the action.
    else if (consumedStamina > actor->getStamina())
    {
        actor->sendMsg("You are too tired to flee.\n");
    }
    else
    {
        // Get the list of available directions.
        std::vector<Direction> directions = actor->room->getAvailableDirections();
        // Check if there are some directions.
        if (!directions.empty())
        {
            // Pick a random direction, from the poll of the available ones.
            size_t randomDirValue = TRandInteger<size_t>(0, directions.size() - 1);
            Direction randomDirection = directions.at(randomDirValue);
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
        else
        {
            actor->sendMsg("You have no way out.\n");
        }
    }
    // By default set the next combat action to basic attack.
    if (!actor->setNextCombatAction(CombatActionType::BasicMeleeAttack))
    {
        actor->sendMsg(this->stop() + "\n\n");
        return ActionStatus::Finished;
    }
    return ActionStatus::Running;
}

CombatActionType Flee::getCombatActionType() const
{
    return CombatActionType::Flee;
}

unsigned int Flee::getConsumedStamina(Character * character)
{
    // BASE     [+1.0]
    // STRENGTH [-0.0 to -2.80]
    // WEIGHT   [+1.6 to +2.51]
    // CARRIED  [+0.0 to +2.48]
    return static_cast<unsigned int>(1.0
                                     - character->getAbilityLog(Ability::Strength, 0.0, 1.0)
                                     + SafeLog10(character->weight)
                                     + SafeLog10(character->getCarryingWeight()));
}
