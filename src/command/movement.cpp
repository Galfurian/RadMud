/// @file   movement.cpp
/// @author Enrico Fraccaroli
/// @date   gen 07 2017
/// @copyright
/// Copyright (c) 2017 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
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

#include "movement.hpp"
#include "moveAction.hpp"
#include "logger.hpp"
#include "mud.hpp"

void LoadMovementCommands()
{
    Mud::instance().addCommand(std::make_shared<Command>(
        DoTravel, "travel", "",
        "Allow the character to travel between areas.",
        false, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoStop, "stop", "",
        "Stop the current character action.",
        false, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoStand, "stand", "",
        "Stand up.",
        false, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoCrouch, "crouch", "",
        "The character crouches down himself, it's a good stance for hiding.",
        false, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoSit, "sit", "",
        "The player sits down, ideal for a quick break.",
        false, false, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoProne, "prone", "",
        "The player starts prone, a perfect position to shoot long distance.",
        false, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoRest, "rest", "",
        "The player lies down and begin to rest.",
        false, false, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoSleep, "sleep", "",
        "The player lies down, closes its eyes and sleep.",
        false, false, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoWake, "wake", "",
        "The player wake up.",
        false, false, false));
}


bool DoDirection(Character * character, Direction direction)
{
    // Stop any action the character is executing.
    StopAction(character);
    std::string error;
    if (!MoveAction::canMoveTo(character, direction, error, false))
    {
        character->sendMsg(error + "\n");
        return false;
    }
    // Get the destination.
    auto destination = character->room->findExit(direction)->destination;
    auto moveAction = std::make_shared<MoveAction>(character, destination,
                                                   direction);
    // Check the new action.
    error = std::string();
    if (moveAction->check(error))
    {
        // Set the new action.
        character->setAction(moveAction);
        // Calculate the time needed to move.
        if (character->posture == CharacterPosture::Stand)
        {
            character->sendMsg("You start to go %s...\n",
                               direction.toString());
        }
        else if (character->posture == CharacterPosture::Crouch)
        {
            character->sendMsg("You move crouching towards %s...\n",
                               direction.toString());
        }
        else if (character->posture == CharacterPosture::Prone)
        {
            character->sendMsg("You begin to crawl to %s...\n",
                               direction.toString());
        }
        return true;
    }
    character->sendMsg("%s\n", error);
    return false;
}

bool DoTravel(Character * character, ArgumentHandler & /*args*/)
{
    // Stop any ongoing action.
    StopAction(character);
    // Check if the room is a travel point.
    if (!HasFlag(character->room->flags, RoomFlag::TravelPoint))
    {
        character->sendMsg("You can't travel from here.\n");
        return false;
    }
    auto destination = Mud::instance().findTravelPoint(character->room);
    if (destination == nullptr)
    {
        character->sendMsg("You can't find an exit from here.\n");
        return false;
    }
    auto msgDepart = character->getNameCapital() + " goes outside.\n";
    auto msgArrive = character->getNameCapital() + " enter the room.\n";
    auto msgChar = "You begin to travel...";
    // Move character.
    character->moveTo(destination, msgDepart, msgArrive, msgChar);
    return true;
}

bool DoStop(Character * character, ArgumentHandler & /*args*/)
{
    if (character->getAction()->getType() != ActionType::Combat)
    {
        character->sendMsg(character->getAction()->stop() + "\n");
        return true;
    }
    return false;
}

bool DoStand(Character * character, ArgumentHandler & /*args*/)
{
    // Stop any action the character is executing.
    StopAction(character);
    if (character->posture == CharacterPosture::Stand)
    {
        character->sendMsg("You are already standing.\n");
        return false;
    }
    if (character->posture == CharacterPosture::Sleep)
    {
        character->sendMsg("You wake and stand up.\n");
    }
    else
    {
        character->sendMsg("You stand up.\n");
    }
    character->posture = CharacterPosture::Stand;
    return true;
}

bool DoCrouch(Character * character, ArgumentHandler & /*args*/)
{
    // Stop any action the character is executing.
    StopAction(character);
    if (character->posture == CharacterPosture::Crouch)
    {
        character->sendMsg("You are already crouched.\n");
        return false;
    }
    if (character->posture == CharacterPosture::Sleep)
    {
        character->sendMsg("You wake up and put yourself crouched.\n");
    }
    else
    {
        character->sendMsg("You put yourself crouched.\n");
    }
    character->posture = CharacterPosture::Crouch;
    return true;
}

bool DoSit(Character * character, ArgumentHandler & /*args*/)
{
    // Stop any action the character is executing.
    StopAction(character);
    if (character->posture == CharacterPosture::Sit)
    {
        character->sendMsg("You are already seated.\n");
        return false;
    }
    if (character->posture == CharacterPosture::Sleep)
    {
        character->sendMsg("You wake up and sit.\n");
    }
    else
    {
        character->sendMsg("You sit down.\n");
    }
    character->posture = CharacterPosture::Sit;
    return true;
}

bool DoProne(Character * character, ArgumentHandler & /*args*/)
{
    // Stop any action the character is executing.
    StopAction(character);
    if (character->posture == CharacterPosture::Prone)
    {
        character->sendMsg("You are already prone.\n");
        return false;
    }
    if (character->posture == CharacterPosture::Sleep)
    {
        character->sendMsg("You wake up and put yourself prone.\n");
    }
    else
    {
        character->sendMsg("You put yourself prone.\n");
    }
    character->posture = CharacterPosture::Prone;
    return true;
}

bool DoRest(Character * character, ArgumentHandler & /*args*/)
{
    // Stop any action the character is executing.
    StopAction(character);
    if (character->posture == CharacterPosture::Rest)
    {
        character->sendMsg("You are already resting.\n");
        return false;
    }
    if (character->posture == CharacterPosture::Sleep)
    {
        character->sendMsg("You wake up and begin to rest.\n");
    }
    else
    {
        character->sendMsg("You lie down.\n");
    }
    character->posture = CharacterPosture::Rest;
    return true;
}

bool DoSleep(Character * character, ArgumentHandler & /*args*/)
{
    // Stop any action the character is executing.
    StopAction(character);
    if (character->posture == CharacterPosture::Sleep)
    {
        character->sendMsg("You are already sleeping.\n");
        return false;
    }
    character->sendMsg("You lie down and begin to sleep.\n");
    character->posture = CharacterPosture::Sleep;
    return true;
}

bool DoWake(Character * character, ArgumentHandler & /*args*/)
{
    // Stop any action the character is executing.
    StopAction(character);
    if (character->posture != CharacterPosture::Sleep)
    {
        character->sendMsg("You are already awaken.\n");
        return false;
    }
    character->posture = CharacterPosture::Sit;
    character->sendMsg("You wake up and sit.\n");
    return true;
}
