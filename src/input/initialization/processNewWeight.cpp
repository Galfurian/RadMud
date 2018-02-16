/// @file   processNewWeight.cpp
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

#include "processNewWeight.hpp"
#include "processNewDescription.hpp"
#include "processNewConfirm.hpp"
#include "formatter.hpp"
#include "player.hpp"
#include "mud.hpp"

bool ProcessNewWeight::process(Character * character, ArgumentHandler & args)
{
    auto player = character->toPlayer();
    auto input = args.getOriginal();
    // Check if the player has typed BACK.
    if (ToLower(input) == "back")
    {
        // Create a shared pointer to the previous step.
        auto newStep = std::make_shared<ProcessNewDescription>();
        // Set the handler.
        player->inputProcessor = newStep;
        // Advance to the next step.
        newStep->rollBack(character);
        return true;
    }
    else if (!IsNumber(input))
    {
        this->advance(character, "Not a valid weight.");
    }
    else
    {
        int weight = ToNumber<int>(input);
        if (weight < 40)
        {
            this->advance(character, "You can't be a feather.");
        }
        else if (weight > 120)
        {
            this->advance(character, "Too much.");
        }
        else
        {
            player->weight = static_cast<unsigned int>(weight);
            // Create a shared pointer to the next step.
            auto newStep = std::make_shared<ProcessNewConfirm>();
            // Set the handler.
            player->inputProcessor = newStep;
            // Advance to the next step.
            newStep->advance(character);
            return true;
        }
    }
    return false;
}

void ProcessNewWeight::advance(Character * character,
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
    msg += "# " + Bold("Character's Weight.") + "\n";
    msg += "# Choose the weight of your character.\n";
    msg += "# Type [" + Magenta("back") +
           "] to return to the previous step.\n";
    if (!error.empty())
    {
        msg += "# " + error + "\n";
    }
    character->sendMsg(msg);
}

void ProcessNewWeight::rollBack(Character * character)
{
    auto player = character->toPlayer();
    player->weight = 0;
    this->advance(character);
}
