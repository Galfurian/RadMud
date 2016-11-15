/// @file   InputHandler.cpp
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

#include "inputHandler.hpp"
#include "character.hpp"
#include "crafting.hpp"
#include "general.hpp"
#include "mud.hpp"

InputHandler::InputHandler()
{
    // Nothing to do.
}

InputHandler::~InputHandler()
{
    // Nothing to do.
}

void InputHandler::process(Character * character, ArgumentHandler & args)
{
    if (args.empty())
    {
        character->sendMsg("Huh?\n");
        return;
    }
    auto command = args[0].getContent();
    args.erase(0);

    // Check if it's a direction.
    Direction direction = Mud::instance().findDirection(command, false);

    if (direction != Direction::None)
    {
        DoDirection(character, direction);
    }
    else
    {
        bool found = false;
        // Check if it's a command.
        for (auto iterator : Mud::instance().mudCommands)
        {
            if (!BeginWith(iterator.name, command))
            {
                continue;
            }
            if (!iterator.canUse(character))
            {
                continue;
            }
            if ((!iterator.cuic) && (character->getAction()->getType() == ActionType::Combat) && !iterator.gods)
            {
                character->sendMsg("You cannot do that in combat.\n");
                found = true;
                break;
            }
            if (iterator.name == "shutdown" && command != "shutdown")
            {
                character->sendMsg("You have to type completly \"shutdown\".\n");
            }
            else if (iterator.name == "quit" && command != "quit")
            {
                character->sendMsg("You have to type completly \"quit\".\n");
            }
            else
            {
                iterator.hndl(character, args);
                found = true;
                break;
            }
        }
        if (!found)
        {
            Profession * profession = Mud::instance().findProfession(command);
            if (profession != nullptr)
            {
                DoProfession(character, profession, args);
            }
            else
            {
                throw std::runtime_error("Huh?\n");
            }
        }
    }
    character->sendMsg("\n");
}
