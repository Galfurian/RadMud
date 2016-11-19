/// @file   processNewStory.cpp
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

#include "processNewStory.hpp"
#include "processNewPassword.hpp"
#include "processNewRace.hpp"

void ProcessNewStory::process(Character * character, ArgumentHandler & args)
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
        std::shared_ptr<ProcessNewPassword> newStep = std::make_shared<ProcessNewPassword>();
        // Set the handler.
        player->inputProcessor = newStep;
        // Advance to the next step.
        newStep->rollBack(character);
    }
    else if (input != "continue")
    {
        this->advance(character, "Write 'continue' after you have readed the story.");
    }
    else
    {
        // Create a shared pointer to the next step.
        std::shared_ptr<ProcessNewRace> newStep = std::make_shared<ProcessNewRace>();
        // Set the handler.
        player->inputProcessor = newStep;
        // Advance to the next step.
        newStep->advance(character);
    }
}

void ProcessNewStory::advance(Character * character, const std::string & error)
{
    // Print the choices.
    this->printChices(character);
    std::string msg;
    msg += "# The story of the Wasteland.\n";
    msg += "#\n";
    msg += "Year 374\n";
    msg += "#\n";
    msg += "# Type [" + Formatter::magenta() + "continue" + Formatter::reset()
           + "] to continue character creation.\n";
    msg += "# Type [" + Formatter::magenta() + "back" + Formatter::reset()
           + "] to return to the previus step.\n";
    character->sendMsg(msg);
    if (!error.empty())
    {
        character->sendMsg("# " + error + "\n");
    }
}

void ProcessNewStory::rollBack(Character * character)
{
    this->advance(character);
}
