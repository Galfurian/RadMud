/// @file   processNewAttributes.cpp
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

#include "processNewAttributes.hpp"
#include "player.hpp"
#include "mud.hpp"
#include "processNewRace.hpp"
#include "processNewGender.hpp"

bool
ProcessNewAttributes::process(Character * character, ArgumentHandler & args)
{
    auto player = character->toPlayer();
    if ((args.size() != 1) && (args.size() != 2))
    {
        this->advance(character, "Invalid input.");
        return false;
    }
    if (ToLower(args[0].getContent()) == "back")
    {
        // Create a shared pointer to the previous step.
        auto newStep = std::make_shared<ProcessNewRace>();
        // Set the handler.
        player->inputProcessor = newStep;
        // Advance to the next step.
        newStep->rollBack(character);
        return true;
    }
    if (ToLower(args[0].getContent()) == "reset")
    {
        player->remaining_points = 0;
        for (auto & ability : player->abilities)
        {
            player->setAbility(ability.first,
                               player->race->getAbility(ability.first));
        }
        this->advance(character,
                      Formatter::cyan() +
                      "Attribute has been set by default." +
                      Formatter::reset() + "\n");
        return true;
    }
    if (BeginWith(ToLower(args[0].getContent()), "continue"))
    {
        // Create a shared pointer to the next step.
        auto newStep = std::make_shared<ProcessNewGender>();
        // Set the handler.
        player->inputProcessor = newStep;
        // Advance to the next step.
        newStep->advance(character);
        return true;
    }
    if (BeginWith(ToLower(args[0].getContent()), "help"))
    {
        if (args.size() != 2)
        {
            this->advance(character, "Specify the attribute number.");
        }
        // Get the ability number.
        auto abilityNumber = ToNumber<unsigned int>(args[1].getContent());
        // Get and check the ability.
        Ability ability = Ability(abilityNumber);
        if (ability == Ability::None)
        {
            this->advance(character, "Must select a valid attribute.");
            return false;
        }
        std::string help;
        help += "Help about " + ability.toString() + ".\n";
        help += Formatter::italic() + ability.getDescription() + "\n";
        this->advance(character, help);
        return true;
    }
    else
    {
        if (args.size() == 2)
        {
            // Get the ability number.
            auto abilityNumber = ToNumber<unsigned int>(args[0].getContent());
            // Get and check the ability.
            Ability ability = Ability(abilityNumber);
            if (ability == Ability::None)
            {
                this->advance(character, "Must select a valid attribute.");
                return false;
            }
            // Get the modifier.
            int modifier = ToNumber<int>(args[1].getContent());
            // Check for errors.
            if (player->remaining_points < modifier)
            {
                this->advance(character, "You don't have enough points left.");
                return false;
            }
            // Create an help string.
            std::string help;
            // Get the base value.
            auto base = static_cast<int>(player->getAbility(ability, false));
            // Evaluate the result.
            auto result = base + modifier;
            // Evaluate the upper and lower bounds.
            int upperBound = base + 5;
            int lowerBound = ((base - 5) > 0) ? (base - 5) : 0;
            // Check if the result is inside the boundaries.
            if (result < lowerBound)
            {
                this->advance(character,
                              ability.toString() + " can't go below " +
                              ToString(lowerBound) + ".");
                return false;
            }
            else if (result > upperBound)
            {
                this->advance(character,
                              ability.toString() + " can't go above " +
                              ToString(upperBound) + ".");
            }
            // Decrease the remaining points.
            player->remaining_points -= modifier;
            // Set the new ability value.
            player->setAbility(ability, static_cast<unsigned int>(result));
            return true;
        }
        else
        {
            this->advance(character, "Type [#attribute +/-(value)].");
        }
    }
    return false;
}

void
ProcessNewAttributes::advance(Character * character, const std::string & error)
{
    // Transform the character into player.
    auto player = character->toPlayer();
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
    msg += "# " + Bold("Character's Attributes.") + "\n";
    for (auto ability : player->abilities)
    {
        msg += "#    [" + ToString(ability.first.toUInt()) + "]";
        msg += Align(ability.first.toString(), align::left, 15);
        msg += " : ";
        msg += ToString(player->getAbility(Ability::Strength, false));
    }
    msg += "#\n";
    msg += "# Remaining Points: ";
    msg += (player->remaining_points > 0) ? Formatter::green()
                                          : Formatter::red();
    msg += ToString(player->remaining_points);
    msg += Formatter::reset();
    msg += "#\n";
    msg += "# Type [" + Magenta("(number)") + "]";
    msg += "[" + Magenta("+/-modifier") + "]";
    msg += " to decrease or increase the value of an attribute.\n";
    msg += "# Type [" + Magenta("help (number)") + "]";
    msg += " to read a brief description of the attribute.\n";
    msg += "# Type [" + Magenta("reset") + "]";
    msg += " to reset the values as default.\n";
    msg += "# Type [" + Magenta("continue") + "]";
    msg += " to continue character creation.\n";
    msg += "# Type [" + Magenta("back") + "]";
    msg += " to return to the previous step.\n";
    if (!error.empty())
    {
        msg += "# " + error + "\n";
    }
    character->sendMsg(msg);
}

void ProcessNewAttributes::rollBack(Character * character)
{
    auto player = character->toPlayer();
    for (auto & ability : player->abilities)
    {
        player->setAbility(ability.first,
                           player->race->getAbility(ability.first));
    }
    this->advance(character);
}
