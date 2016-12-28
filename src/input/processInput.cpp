/// @file   processInput.cpp
/// @author Enrico Fraccaroli
/// @date   Nov 14, 2016
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

#include "processInput.hpp"
#include "character.hpp"
#include "crafting.hpp"
#include "general.hpp"
#include "mud.hpp"

ProcessInput::ProcessInput()
{
    // Nothing to do.
}

ProcessInput::~ProcessInput()
{
    // Nothing to do.
}

bool ProcessInput::process(Character * character, ArgumentHandler & args)
{
    if (args.empty())
    {
        character->sendMsg("Huh?\n");
        return false;
    }
    if (character->room == nullptr)
    {
        character->sendMsg("You are in a wrong room.\n");
        return false;
    }
    bool executionStatus = false;
    // Get the command.
    auto command = args[0].getContent();
    // Erase the first element which is the command.
    args.erase(0);
    // Check if it's a direction.
    auto direction = Mud::instance().findDirection(command, false);
    if (direction != Direction::None)
    {
        DoDirection(character, direction);
        executionStatus = true;
    }
    else
    {
        // Check if it's a command.
        bool done = false;
        for (auto iterator : Mud::instance().mudCommands)
        {
            // Skip the commands which do not start with the given command.
            if (!BeginWith(iterator->name, command))
            {
                continue;
            }
            // If the command is the right one, check if the character
            //  can execute the command.
            if (!iterator->canUse(character))
            {
                continue;
            }
            // Check if the command can be used in combat and if the
            //  character is actually in combat.
            if ((!iterator->canUseInCombat) &&
                (character->getAction()->getType() == ActionType::Combat))
            {
                character->sendMsg("You cannot do that in combat.\n");
                break;
            }
            else if (iterator->typedCompletely && (command != iterator->name))
            {
                character->sendMsg("You have to type completely"
                                       "\"" + iterator->name + "\".\n");
                break;
            }
            else
            {
                executionStatus = iterator->handler(character, args);
                done = true;
                break;
            }
        }
        if (!done)
        {
            // Check if the command is instead a profession.
            auto profession = Mud::instance().findProfession(command);
            if (profession != nullptr)
            {
                executionStatus = DoProfession(character, profession, args);
            }
            else
            {
                character->sendMsg("Huh?\n");
            }
        }
    }
    character->sendMsg("\n");
    return executionStatus;
}
