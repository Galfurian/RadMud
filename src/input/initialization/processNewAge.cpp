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

#include "input/initialization/processNewAge.hpp"
#include "character/player.hpp"
#include "mud.hpp"
#include "input/initialization/processNewDescription.hpp"
#include "input/initialization/processNewGender.hpp"

bool ProcessNewAge::process(Character * character, ArgumentHandler & args)
{
    // Transform the character into player.
    auto player = character->toPlayer();
    // Get the original input.
    auto input = args.getOriginal();
    // Check if the player has typed BACK.
    if (ToLower(input) == "back")
    {
        // Create a shared pointer to the previous step.
        auto newStep = std::make_shared<ProcessNewGender>();
        // Set the handler.
        player->inputProcessor = newStep;
        // Advance to the next step.
        newStep->rollBack(character);
        return true;
    }
    // Check if the input is a number.
    if (!IsNumber(input))
    {
        this->advance(character, "Not a valid age.");
        return false;
    }
    // Get the value of age.
    int age = ToNumber<int>(input);
    if (age < 18)
    {
        this->advance(character,
                      "A creature so young is not suitable"
                          "for a world so wicked.");
        return false;
    }
    if (50 < age)
    {
        this->advance(character,
                      "Life expectancy in this world is 70 years,"
                          " in order to still be competitive you"
                          " can choose 50 years at most.");

        return false;
    }
    // Set the age.
    player->age = age;
    // Create a shared pointer to the next step.
    auto newStep = std::make_shared<ProcessNewDescription>();
    // Set the handler.
    player->inputProcessor = newStep;
    // Advance to the next step.
    newStep->advance(character);
    return true;
}

void ProcessNewAge::advance(Character * character, const std::string & error)
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
    msg += "# " + Bold("Character's Age.") + "\n";
    msg += "# Choose the age of your character.\n";
    msg += "# Type [" + Magenta("back") + "] to return to the previous step.\n";
    if (!error.empty())
    {
        msg += "# " + error + "\n";
    }
    character->sendMsg(msg);
}

void ProcessNewAge::rollBack(Character * character)
{
    auto player = character->toPlayer();
    player->age = 0;
    this->advance(character);
}
