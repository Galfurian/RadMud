/// @file   processNewPassword.cpp
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

#include "processNewPassword.hpp"
#include "processNewPasswordConfirm.hpp"
#include "processNewName.hpp"
#include "formatter.hpp"
#include "player.hpp"

void ProcessNewPassword::process(Character * character, ArgumentHandler & args)
{
    auto player = character->toPlayer();
    auto input = args.getOriginal();
    // Player_password can't be blank.
    if (input.empty())
    {
        this->advance(character, "Invalid input.");
    }
    else if (ToLower(input) == "back")
    {
        // Create a shared pointer to the previous step.
        std::shared_ptr<ProcessNewName> newStep = std::make_shared<ProcessNewName>();
        // Set the handler.
        player->inputProcessor = newStep;
        // Advance to the next step.
        newStep->rollBack(character);
    }
    else if (input.find_first_not_of(VALID_CHARACTERS_NAME) != std::string::npos)
    {
        this->advance(character, "Password cannot contain disallowed characters.");
    }
    else
    {
        player->password = input;
        // Create a shared pointer to the next step.
        std::shared_ptr<ProcessNewPasswordConfirm> newStep = std::make_shared<ProcessNewPasswordConfirm>();
        // Set the handler.
        player->inputProcessor = newStep;
        // Advance to the next step.
        newStep->advance(character);
    }
}

void ProcessNewPassword::advance(Character * character, const std::string & error)
{
    // Print the choices.
    this->printChices(character);
    character->sendMsg("# %sCharacter's Password.%s\n", Formatter::bold(), Formatter::reset());
    character->sendMsg("# Choose a proper password, in order to protect the acces to your character.\n");
    character->sendMsg("# Type [%sback%s] to return to the previous step.\n",
                       Formatter::magenta(),
                       Formatter::reset());
    if (!error.empty())
    {
        character->sendMsg("# " + error + "\n");
    }
}

void ProcessNewPassword::rollBack(Character * character)
{
    auto player = character->toPlayer();
    // Reset the values.
    player->password = "";
    // Advance to the current step.
    this->advance(character);
}
