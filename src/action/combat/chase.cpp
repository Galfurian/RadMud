/// @file   chase.cpp
/// @brief  Contais the implementation of the chase class.
/// @author Enrico Fraccaroli
/// @date   Nov 11 2016
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

#include "chase.hpp"

#include "effectFactory.hpp"
#include "moveAction.hpp"
#include "logger.hpp"
#include "aStar.hpp"
#include "area.hpp"
#include "room.hpp"
#include <queue>

Chase::Chase(Character * _actor, Character * _target) :
    CombatAction(_actor),
    target(_target),
    lastRoom(_target->room),
    valid(),
    checkFunction(
        [&](Room * from, Room * to)
        {
            // Get the direction.
            std::string error;
            auto direction = Area::getDirection(from->coord, to->coord);
            return MoveAction::canMoveTo(actor, direction, error, true);
        })
{
    // Debugging message.
    //Logger::log(LogLevel::Debug, "Created Chase.");
    // Reset the cooldown of the action.
    this->resetCooldown(Chase::getCooldown(_actor));
    // Find the path from the actor to the target.
    AStar<Room *> aStar;
    valid = aStar.findPath(actor->room, target->room, path, checkFunction);
    if (!valid)
    {
        Logger::log(LogLevel::Debug, "There is no path to the target.");
    }
}

Chase::~Chase()
{
    //Logger::log(LogLevel::Debug, "Deleted Chase.");
}

bool Chase::check(std::string & error) const
{
    if (!CombatAction::check(error))
    {
        return false;
    }
    if (!valid)
    {
        error = "You are not able to chase your target.";
        return false;
    }
    if (target == nullptr)
    {
        error = "You don't have a valid target.";
        return false;
    }
    if (target->room == nullptr)
    {
        error = "Your target is not in a valid room.";
        return false;
    }
    return true;
}

ActionType Chase::getType() const
{
    return ActionType::Combat;
}

std::string Chase::getDescription() const
{
    return "Chasing";
}

std::string Chase::stop()
{
    return "You stop chasing your target.";
}

ActionStatus Chase::perform()
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
    if (actor->room->vnum == target->room->vnum)
    {
        return ActionStatus::Finished;
    }
    // Check if the actor has enough stamina to execute the action.
    auto consumedStamina = Chase::getConsumedStamina(actor);
    if (consumedStamina > actor->getStamina())
    {
        actor->sendMsg("You are too tired to chase your target.\n\n");
    }
    else
    {
        // First check if the target has changed room.
        if (target->room->vnum == lastRoom->vnum)
        {
            Logger::log(LogLevel::Debug, "The target didn't move.");
            Room * nextRoom = path.front();
            if (nextRoom != nullptr)
            {
                // Get the direction of the next room.
                Direction direction = Area::getDirection(actor->room->coord,
                                                         nextRoom->coord);
                // Check if the actor was aiming.
                if (actor->combatHandler.getAimedTarget() != nullptr)
                {
                    actor->effects.forceAddEffect(
                        EffectFactory::disturbedAim(actor, 1, -3));
                }
                // Consume the stamina.
                actor->remStamina(consumedStamina, true);
                // Move character.
                actor->moveTo(
                    nextRoom,
                    actor->getNameCapital() + " goes " +
                    direction.toString() + ".\n",
                    actor->getNameCapital() + " arrives from " +
                    direction.getOpposite().toString() + ".\n");
                // Pop the room.
                path.erase(path.begin());
            }
            else
            {
                Logger::log(LogLevel::Debug, "The next room is not valid.");
            }
        }
        else
        {
            Logger::log(LogLevel::Debug, "The target moved.");
            // Find the path from the actor to the target.
            AStar<Room *> aStar;
            valid = aStar.findPath(actor->room,
                                   target->room,
                                   path,
                                   checkFunction);
            if (valid)
            {
                Room * nextRoom = path.front();
                if (nextRoom != nullptr)
                {
                    // Get the direction of the next room.
                    Direction direction = Area::getDirection(actor->room->coord,
                                                             nextRoom->coord);
                    // Check if the actor was aiming.
                    if (actor->combatHandler.getAimedTarget() != nullptr)
                    {
                        actor->effects.forceAddEffect(
                            EffectFactory::disturbedAim(actor, 1, -3));
                    }
                    // Consume the stamina.
                    actor->remStamina(consumedStamina, true);
                    // Move character.
                    actor->moveTo(
                        nextRoom,
                        actor->getNameCapital() + " goes " +
                        direction.toString() + ".\n",
                        actor->getNameCapital() + " arrives from " +
                        direction.getOpposite().toString() + ".\n");
                    // Pop the room.
                    path.erase(path.begin());
                }
                else
                {
                    Logger::log(LogLevel::Debug, "The next room is not valid.");
                }
            }
            else
            {
                actor->sendMsg("You are not able to chase your target.\n\n");
                return ActionStatus::Error;
            }
        }
    }
    // Reset the cooldown.
    actor->getAction()->resetCooldown(Chase::getCooldown(actor));
    // Return that the action is still running.
    return ActionStatus::Running;
}

CombatActionType Chase::getCombatActionType() const
{
    return CombatActionType::Chase;
}

unsigned int Chase::getConsumedStamina(Character * character)
{
    auto multiplier = 1.0;
    if (character->posture == CharacterPosture::Crouch)
    {
        multiplier = 0.75;
    }
    else if (character->posture == CharacterPosture::Prone)
    {
        multiplier = 0.50;
    }
    // BASE     [+1.0]
    // STRENGTH [-0.0 to -1.40]
    // WEIGHT   [+1.6 to +2.51]
    // CARRIED  [+0.0 to +2.48]
    unsigned int consumedStamina = 1;
    consumedStamina -= character->getAbilityLog(Ability::Strength, 0.0, 1.0);
    consumedStamina = SafeSum(consumedStamina, SafeLog10(character->weight));
    consumedStamina = SafeSum(consumedStamina,
                              SafeLog10(character->getCarryingWeight()));
    return static_cast<unsigned int>(consumedStamina * multiplier);
}

unsigned int Chase::getCooldown(Character * character)
{
    // BASE     [+4.0]
    // STRENGTH [-0.0 to -1.40]
    // AGILITY  [-0.0 to -1.40]
    // WEIGHT   [+1.6 to +2.51]
    // CARRIED  [+0.0 to +2.48]
    // WEAPON   [+0.0 to +1.60]
    unsigned int cooldown = 4;
    cooldown = SafeSum(cooldown,
                       -character->getAbilityLog(Ability::Strength, 0.0, 1.0));
    cooldown = SafeSum(cooldown,
                       -character->getAbilityLog(Ability::Agility, 0.0, 1.0));
    cooldown = SafeSum(cooldown, SafeLog10(character->weight));
    cooldown = SafeSum(cooldown, SafeLog10(character->getCarryingWeight()));
    return cooldown;
}
