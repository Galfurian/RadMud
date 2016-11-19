/// @file   processNewAge.cpp
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

#include "processNewAge.hpp"
#include "player.hpp"
#include "mud.hpp"
#include "processNewDescription.hpp"
#include "processNewGender.hpp"

void ProcessNewAge::process(Character * character, ArgumentHandler & args)
{
    // Transform the character into player.
    auto player = character->toPlayer();
    // Get the original input.
    auto input = args.getOriginal();
    // Check if the player has typed BACK.
    if (ToLower(input) == "back")
    {
        // Create a shared pointer to the previous step.
        std::shared_ptr<ProcessNewGender> newStep = std::make_shared<ProcessNewGender>();
        // Set the handler.
        player->inputProcessor = newStep;
        // Advance to the next step.
        newStep->rollBack(character);
    }
    else if (!IsNumber(input))
    {
        this->advance(character, "Not a valid age.");
    }
    else
    {
        int age = ToNumber<int>(input);
        if (age < 18)
        {
            this->advance(character, "A creature so young is not suitable for a world so wicked.");
        }
        else if (50 < age)
        {
            this->advance(character,
                          "Life expectancy in this world is 70 years, in order to still be competitive you can choose 50 years at most.");
        }
        else
        {
            player->age = age;
            // Create a shared pointer to the next step.
            std::shared_ptr<ProcessNewDescription> newStep = std::make_shared<ProcessNewDescription>();
            // Set the handler.
            player->inputProcessor = newStep;
            // Advance to the next step.
            newStep->advance(character);
        }
    }
}

void ProcessNewAge::advance(Character * character, const std::string & error)
{
    // Print the choices.
    this->printChices(character);
    // The message that has to be sent.
    character->sendMsg("# %sCharacter's Age.%s\n", Formatter::bold(), Formatter::reset());
    character->sendMsg("# Choose the age of your character.\n");
    character->sendMsg("# Type [%sback%s] to return to the previus step.\n", Formatter::magenta(), Formatter::reset());
    if (!error.empty())
    {
        character->sendMsg("# " + error + "\n");
    }
}

void ProcessNewAge::rollBack(Character * character)
{
    auto player = character->toPlayer();
    player->age = 0;
    this->advance(character);
}
