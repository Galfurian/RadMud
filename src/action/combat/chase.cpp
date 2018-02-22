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

#include "structureUtils.hpp"
#include "characterUtilities.hpp"
#include "aStar.hpp"
#include "area.hpp"
#include <queue>
#include <cassert>

Chase::Chase(Character * _actor, Character * _target) :
    CombatAction(_actor),
    target(_target),
    lastRoom(_target->room),
    RoomCheckFunction()
{
    // Debugging message.
    Logger::log(LogLevel::Debug, "Created Chase.");
    // Reset the cooldown of the action.
    this->resetCooldown(this->getCooldown());
}

Chase::~Chase()
{
    Logger::log(LogLevel::Debug, "Deleted Chase.");
}

bool Chase::check(std::string & error) const
{
    if (!CombatAction::check(error))
    {
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
    if (consumedStamina > actor->stamina)
    {
        actor->sendMsg("You are too tired to chase your target.\n\n");
    }
    else
    {
        // If the path to the target is empty or the target has changed room.
        if (path.empty() || (target->room->vnum != lastRoom->vnum))
        {
            Logger::log(LogLevel::Debug, "Evaluating the path...");
            // Find the path from the actor to the target.
            if (!this->updatePath())
            {
                actor->sendMsg("There is no path to your target.\n\n");
                return ActionStatus::Error;
            }
        }
        // Move towards the target.
        if (this->moveTowardsTarget())
        {
            // Consume the stamina.
            actor->remStamina(consumedStamina, true);
        }
        else
        {
            actor->sendMsg("You are not able to chase your target.\n\n");
            return ActionStatus::Error;
        }
    }
    // Reset the cooldown.
    actor->getAction()->resetCooldown();
    // Return that the action is still running.
    return ActionStatus::Running;
}

unsigned int Chase::getCooldown()
{
    assert(actor && "Actor is nullptr");
    // BASE     [+4.0]
    // STRENGTH [-0.0 to -1.40]
    // AGILITY  [-0.0 to -1.40]
    // WEIGHT   [+1.6 to +2.51]
    // CARRIED  [+0.0 to +2.48]
    // WEAPON   [+0.0 to +1.60]
    unsigned int cooldown = 4;
    cooldown = SafeSum(cooldown, -actor->getAbilityLog(Ability::Strength));
    cooldown = SafeSum(cooldown, -actor->getAbilityLog(Ability::Agility));
    cooldown = SafeSum(cooldown, SafeLog10(actor->weight));
    cooldown = SafeSum(cooldown, SafeLog10(actor->getCarryingWeight()));
    return cooldown;
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
    consumedStamina -= character->getAbilityLog(Ability::Strength);
    consumedStamina = SafeSum(consumedStamina, SafeLog10(character->weight));
    consumedStamina = SafeSum(consumedStamina,
                              SafeLog10(character->getCarryingWeight()));
    return static_cast<unsigned int>(consumedStamina * multiplier);
}

bool Chase::updatePath()
{
    // Set the check function.
    if (RoomCheckFunction == nullptr)
    {
        RoomCheckFunction = [&](Room * from, Room * to)
        {
            // Prepare the movement options.
            MovementOptions options;
            options.character = actor;
            // Prepare the error string.
            std::string error;
            return StructUtils::checkConnection(options, from, to, error);
        };
    }
    // Find the path from the actor to the target.
    AStar<Room *> aStar(RoomCheckFunction,
                        StructUtils::getRoomDistance,
                        StructUtils::roomsAreEqual,
                        StructUtils::getNeighbours);
    return aStar.findPath(actor->room, target->room, path);
}

bool Chase::moveTowardsTarget()
{
    if (path.empty())
    {
        Logger::log(LogLevel::Debug, "Path is empty.");
        return false;
    }
    Room * nextRoom = path.front();
    if (nextRoom == nullptr)
    {
        Logger::log(LogLevel::Debug, "Next room is a nullptr.");
        return false;
    }
    // Get the direction of the next room.
    auto direction =
        StructUtils::getDirection(actor->room->coord, nextRoom->coord);
    // Move character.
    if (!MoveCharacterTo(
        actor,
        nextRoom,
        actor->getNameCapital() + " goes " +
        direction.toString() + ".\n",
        actor->getNameCapital() + " arrives from " +
        direction.getOpposite().toString() + ".\n"))
    {
        Logger::log(LogLevel::Debug, "Cannot move to the next room.");
        return false;
    }
    // Apply the disturbed aim effect.
    actor->effectManager.addEffect(
        EffectFactory::disturbedAim(actor, 1, -3), true);
    // Pop the room.
    path.erase(path.begin());
    return true;
}
