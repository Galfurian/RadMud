/// @file   moveAction.cpp
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

#include "moveAction.hpp"
#include "character.hpp"
#include "room.hpp"
#include "effectFactory.hpp"

using namespace std::chrono;

MoveAction::MoveAction(
    Character * _actor,
    Room * _destination,
    Direction _direction,
    unsigned int _cooldown) :
    GeneralAction(_actor, system_clock::now() + seconds(_cooldown)),
    destination(_destination),
    direction(_direction)
{
    Logger::log(LogLevel::Debug, "Created move action.");
}

MoveAction::~MoveAction()
{
    Logger::log(LogLevel::Debug, "Deleted move action.");
}

bool MoveAction::check(std::string & error) const
{
    if (!GeneralAction::check(error))
    {
        return false;
    }
    if (this->destination == nullptr)
    {
        Logger::log(LogLevel::Error, "No destination has been set.");
        error = "You cannot reach the destination.";
        return false;
    }
    if (this->direction == Direction::None)
    {
        Logger::log(LogLevel::Error, "No direction has been set.");
        error = "You have lost your direction.";
        return false;
    }
    return true;
}

ActionType MoveAction::getType() const
{
    return ActionType::Move;
}

std::string MoveAction::getDescription() const
{
    return "moving";
}

std::string MoveAction::stop()
{
    return "You stop moving.";
}

ActionStatus MoveAction::perform()
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
    if (!MoveAction::canMoveTo(actor, direction, error))
    {
        // Notify that the actor can't move because too tired.
        actor->sendMsg(error + "\n");
        return ActionStatus::Error;
    }
    // Get the amount of required stamina.
    auto consumedStamina = this->getConsumedStamina(actor, actor->posture);
    // Consume the stamina.
    actor->remStamina(consumedStamina, true);
    // Check if the actor was aiming.
    if (actor->aimedCharacter)
    {
        actor->effects.forceAddEffect(EffectFactory::disturbedAim(actor, 1, -3));
    }
    // Move character.
    actor->moveTo(
        destination,
        actor->getNameCapital() + " goes " + direction.toString() + ".\n",
        actor->getNameCapital() + " arrives from " + direction.getOpposite().toString() + ".\n");
    return ActionStatus::Finished;
}

unsigned int MoveAction::getConsumedStamina(const Character * character, const CharacterPosture & posture)
{
    auto multiplier = 1.0;
    if (posture == CharacterPosture::Crouch)
    {
        multiplier = 0.75;
    }
    else if (posture == CharacterPosture::Prone)
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
    consumedStamina = SafeSum(consumedStamina, SafeLog10(character->getCarryingWeight()));
    return static_cast<unsigned int>(consumedStamina * multiplier);
}

unsigned int MoveAction::getCooldown(const Character * character)
{
    unsigned int cooldown = 2;
    if (character->posture == CharacterPosture::Crouch)
    {
        cooldown = 4;
    }
    else if (character->posture == CharacterPosture::Prone)
    {
        cooldown = 6;
    }
    return cooldown;
}

bool MoveAction::canMoveTo(Character * character, const Direction & direction, std::string & error)
{
    if (character->getAction()->getType() == ActionType::Combat)
    {
        // Check if the character is locked into close combat.
        bool lockedInCombat = false;
        // Check if he is in the same room of one of its aggressors.
        for (auto iterator : character->aggressionList)
        {
            if (iterator->aggressor != nullptr)
            {
                if (iterator->aggressor->room == character->room)
                {
                    lockedInCombat = true;
                    break;
                }
            }
        }
        // Check even the aimed character.
        if (character->aimedCharacter)
        {
            if (character->aimedCharacter->room == character->room) lockedInCombat = true;
        }
        if (lockedInCombat)
        {
            error = "You cannot move while fighting in close combat.";
        }
        return !lockedInCombat;
    }
    // Check if the character is in a no-walk position.
    if (character->posture == CharacterPosture::Rest || character->posture == CharacterPosture::Sit)
    {
        error = "You first need to stand up.";
        return false;
    }
    // Find the exit to the destination.
    auto destExit = character->room->findExit(direction);
    if (destExit == nullptr)
    {
        error = "You cannot go that way.";
        return false;
    }
    // Check if the actor has enough stamina to execute the action.
    if (MoveAction::getConsumedStamina(character, character->posture) > character->getStamina())
    {
        error = "You are too tired to move.\n";
        return false;
    }
    // If the direction is upstairs, check if there is a stair.
    if (direction == Direction::Up)
    {
        if (!HasFlag(destExit->flags, ExitFlag::Stairs))
        {
            error = "You can't go upstairs, there are no stairs.";
            return false;
        }
    }
    // Check if the destination is correct.
    if (destExit->destination == nullptr)
    {
        error = "That direction can't take you anywhere.";
        return false;
    }
    // Check if the destination is bocked by a door.
    auto door = destExit->destination->findDoor();
    if (door != nullptr)
    {
        if (HasFlag(door->flags, ItemFlag::Closed))
        {
            error = "Maybe you have to open that door first.";
            return false;
        }
    }
    // Check if the destination has a floor.
    std::shared_ptr<Exit> destDown = destExit->destination->findExit(Direction::Down);
    if (destDown != nullptr)
    {
        if (!HasFlag(destDown->flags, ExitFlag::Stairs))
        {
            error = "Do you really want to fall in that pit?";
            return false;
        }
    }
    // Check if the destination is forbidden for mobiles.
    return !(character->isMobile() && HasFlag(destExit->flags, ExitFlag::NoMob));
}
