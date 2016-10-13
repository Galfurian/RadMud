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
#include "../character/character.hpp"

ScoutAction::ScoutAction(Character * _actor, unsigned int _cooldown) :
    GeneralAction(_actor, std::chrono::system_clock::now() + std::chrono::seconds(_cooldown))
{
    Logger::log(LogLevel::Debug, "Created scout action.");
}

ScoutAction::~ScoutAction()
{
    Logger::log(LogLevel::Debug, "Deleted scout action.");
}

bool ScoutAction::check() const
{
    bool correct = GeneralAction::check();
    return correct;
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
    // Create a variable which will contain the ammount of consumed stamina.
    std::string error;
    // Get the amount of required stamina.
    auto consumedStamina = actor->getConsumedStaminaFor(ActionType::Scout);
    // Consume the stamina.
    actor->remStamina(consumedStamina, true);
    // Show the surrounding.
    actor->charactersInSight = actor->getCharactersInSight();
    if (actor->charactersInSight.empty())
    {
        actor->sendMsg("You have found nothing...\n");
    }
    else
    {
        actor->sendMsg("Nearby you see...\n");
        for (auto it : actor->charactersInSight)
        {
            actor->sendMsg("    %s\n", it->getName());
        }
    }
    return ActionStatus::Finished;
}
