/// @file   scoutAction.cpp
/// @brief  Implementation of the class for scouting an area.
/// @author Enrico Fraccaroli
/// @date   Oct 9 2016
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

#include "scoutAction.hpp"
#include "room.hpp"
#include "area.hpp"
#include "effectFactory.hpp"

ScoutAction::ScoutAction(Character * _actor, unsigned int _cooldown) :
    GeneralAction(_actor, std::chrono::system_clock::now() + std::chrono::seconds(_cooldown))
{
    Logger::log(LogLevel::Debug, "Created scout action.");
}

ScoutAction::~ScoutAction()
{
    Logger::log(LogLevel::Debug, "Deleted scout action.");
}

bool ScoutAction::check(std::string & error) const
{
    if (!GeneralAction::check(error))
    {
        return false;
    }
    if (actor->room == nullptr)
    {
        Logger::log(LogLevel::Error, "The actor's room is a nullptr.");
        return false;
    }
    if (actor->room->area == nullptr)
    {
        Logger::log(LogLevel::Error, "The room's area is a nullptr.");
        return false;
    }
    if (this->getConsumedStamina(actor) > actor->getStamina())
    {
        error = "You are too tired to scout the area.";
        return false;
    }
    return true;
}

ActionType ScoutAction::getType() const
{
    return ActionType::Scout;
}

std::string ScoutAction::getDescription() const
{
    return "scouting";
}

std::string ScoutAction::stop()
{
    return "You stop scouting the area.";
}

ActionStatus ScoutAction::perform()
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
        return ActionStatus::Error;
    }
    // Get the amount of required stamina and try to consume it.
    actor->remStamina(this->getConsumedStamina(actor));
    // Get the characters in sight.
    CharacterContainer exceptions;
    exceptions.emplace_back(actor);
    actor->charactersInSight = actor->room->area->getCharactersInSight(exceptions,
                                                                       actor->room->coord,
                                                                       actor->getViewDistance());
    if (actor->charactersInSight.empty())
    {
        actor->sendMsg("You have found nothing...\n");
        return ActionStatus::Error;
    }
    actor->sendMsg("Nearby you can see...\n");
    for (auto it : actor->charactersInSight)
    {
        actor->sendMsg("    %s\n", it->getName());
    }
    // Add the effect.
    unsigned int modifier = actor->getAbilityModifier(Ability::Perception);
    actor->effects.forceAddEffect(EffectFactory::clearTargets(actor, 2 + modifier));
    return ActionStatus::Finished;
}

unsigned int ScoutAction::getConsumedStamina(Character * character)
{
    // BASE     [+1.0]
    // STRENGTH [-0.0 to -2.80]
    // WEIGHT   [+1.6 to +2.51]
    // CARRIED  [+0.0 to +2.48]
    auto consumed = 1.0
                    - character->getAbilityLog(Ability::Strength, 0.0, 1.0)
                    + SafeLog10(character->weight)
                    + SafeLog10(character->getCarryingWeight());
    return (consumed < 0) ? 0 : static_cast<unsigned int>(consumed);
}
