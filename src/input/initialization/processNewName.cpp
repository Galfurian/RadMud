/// @file   processNewName.cpp
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

#include "processNewName.hpp"
#include "processNewPassword.hpp"
#include "processPlayerName.hpp"
#include "formatter.hpp"
#include "player.hpp"
#include "mud.hpp"

bool ProcessNewName::process(Character * character, ArgumentHandler & args)
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
        auto newStep = std::make_shared<ProcessPlayerName>();
        // Set the handler.
        player->inputProcessor = newStep;
        // Advance to the next step.
        newStep->rollBack(character);
        return true;
    }
    else if (input.find_first_not_of(VALID_CHARACTERS_NAME) !=
             std::string::npos)
    {
        this->advance(character,
                      "That player name contains disallowed characters.");
    }
        // Check for bad names here.
    else if (Mud::instance().badNames.find(input) !=
             Mud::instance().badNames.end())
    {
        this->advance(character, "That name is not permitted.");
    }
        // Check if the player name has already been used.
    else if (SQLiteDbms::instance().searchPlayer(ToCapitals(input)))
    {
        this->advance(character,
                      "That player already exists, please choose another name.");
    }
    else
    {
        player->name = ToCapitals(input);
        player->room = Mud::instance().findRoom(1000);
        player->faction = Mud::instance().findFaction(1);
        player->password_attempts = 0;
        // Create a shared pointer to the next step.
        auto newStep = std::make_shared<ProcessNewPassword>();
        // Set the handler.
        player->inputProcessor = newStep;
        // Advance to the next step.
        newStep->advance(character);
        return true;
    }
    return false;
}

void ProcessNewName::advance(Character * character, const std::string & error)
{
    // Print the choices.
    this->printChoices(character);
    auto Bold = [](const std::string & s)
    {
        return Formatter::magenta() + s + Formatter::reset();
    };
    auto Magenta = [](const std::string & s)
    {
        return Formatter::magenta() + s + Formatter::reset();
    };
    std::string msg;
    msg += "# " + Bold("Character's Name.") + "\n";
    msg += "# Choose carefully, because this it's the only chance you have";
    msg += " to pick a legendary name, maybe one day it will";
    msg += " be whispered all over the lands.\n";
    msg += "# Type [" + Magenta("back") + "] to return to the previous step.\n";
    if (!error.empty())
    {
        msg += "# " + error + "\n";
    }
    character->sendMsg(msg);
}

void ProcessNewName::rollBack(Character * character)
{
    // Reset the values.
    character->name = "";
    // Advance to the current step.
    this->advance(character);
}
