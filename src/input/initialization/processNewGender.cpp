/// @file   processNewGender.cpp
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

#include "input/initialization/processNewGender.hpp"
#include "character/player.hpp"
#include "mud.hpp"
#include "input/initialization/processNewAttributes.hpp"
#include "input/initialization/processNewAge.hpp"

bool ProcessNewGender::process(Character * character, ArgumentHandler & args)
{
    auto player = character->toPlayer();
    auto input = args.getOriginal();
    // Check if the player has typed BACK.
    if (ToLower(input) == "back")
    {
        // Create a shared pointer to the previous step.
        auto newStep = std::make_shared<ProcessNewAttributes>();
        // Set the handler.
        player->inputProcessor = newStep;
        // Advance to the next step.
        newStep->rollBack(character);
        return true;
    }
    else if (input == "1")
    {
        player->gender = GenderType::Male;
        // Create a shared pointer to the next step.
        auto newStep = std::make_shared<ProcessNewAge>();
        // Set the handler.
        player->inputProcessor = newStep;
        // Advance to the next step.
        newStep->advance(character);
        return true;
    }
    else if (input == "2")
    {
        player->gender = GenderType::Female;
        // Create a shared pointer to the next step.
        auto newStep = std::make_shared<ProcessNewAge>();
        // Set the handler.
        player->inputProcessor = newStep;
        // Advance to the next step.
        newStep->advance(character);
        return true;
    }
    this->advance(character, "Not a valid gender.");
    return false;
}

void ProcessNewGender::advance(Character * character,
                               const std::string & error)
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
    msg += "# " + Bold("Character's Gender.") + "\n";
    msg += "#    [1] Male.\n";
    msg += "#    [2] Female.\n";
    msg += "#\n";
    msg += "# Choose one of the above gender by typing the correspondent number.\n";
    msg += "#\n";
    msg += "# Type [" + Magenta("back") + "] to return to the previous step.\n";
    if (!error.empty())
    {
        msg += "# " + error + "\n";
    }
    character->sendMsg(msg);
}

void ProcessNewGender::rollBack(Character * character)
{
    auto player = character->toPlayer();
    player->gender = GenderType::None;
    this->advance(character);
}
