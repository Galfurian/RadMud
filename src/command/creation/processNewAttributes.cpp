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

void ProcessNewAttributes::process(Character * character, ArgumentHandler & args)
{
    auto player = character->toPlayer();
    if ((args.size() != 1) && (args.size() != 2))
    {
        this->advance(character, "Invalid input.");
    }
    else if (ToLower(args[0].getContent()) == "back")
    {
        // Create a shared pointer to the previous step.
        std::shared_ptr<ProcessNewRace> newStep = std::make_shared<ProcessNewRace>();
        // Set the handler.
        player->inputProcessor = newStep;
        // Advance to the next step.
        newStep->rollBack(character);
    }
    else if (ToLower(args[0].getContent()) == "reset")
    {
        player->remaining_points = 0;
        player->setAbility(Ability::Strength, player->race->getAbility(Ability::Strength));
        player->setAbility(Ability::Agility, player->race->getAbility(Ability::Agility));
        player->setAbility(Ability::Perception, player->race->getAbility(Ability::Perception));
        player->setAbility(Ability::Constitution, player->race->getAbility(Ability::Constitution));
        player->setAbility(Ability::Intelligence, player->race->getAbility(Ability::Intelligence));
        this->advance(character, Formatter::cyan() + "Attribute has been set by default." + Formatter::reset() + "\n");
    }
    else if (BeginWith(ToLower(args[0].getContent()), "continue"))
    {
        // Create a shared pointer to the next step.
        std::shared_ptr<ProcessNewGender> newStep = std::make_shared<ProcessNewGender>();
        // Set the handler.
        player->inputProcessor = newStep;
        // Advance to the next step.
        newStep->advance(character);
    }
    else if (BeginWith(ToLower(args[0].getContent()), "help"))
    {
        if (args.size() == 2)
        {
            std::string helpMessage;
            if (args[1].getContent() == "1")
            {
                helpMessage = "Help about Strength.\n" + Formatter::italic();
                helpMessage += "Strength is important for increasing the Carrying Weight and ";
                helpMessage += "satisfying the minimum Strength requirements for some weapons and armors.";
                helpMessage += Formatter::reset() + "\n";
            }
            else if (args[1].getContent() == "2")
            {
                helpMessage = "Help about Agility.\n" + Formatter::italic();
                helpMessage += "Besides increasing mobility in combat, it increases the recharge ";
                helpMessage += "speed of all the weapons, as well as the ability to use light armor.";
                helpMessage += Formatter::reset() + "\n";
            }
            else if (args[1].getContent() == "3")
            {
                helpMessage = "Help about Perception.\n" + Formatter::italic();
                helpMessage += "The ability to see, hear, taste and notice unusual things. ";
                helpMessage += "A high Perception is important for a sharpshooter.";
                helpMessage += Formatter::reset() + "\n";
            }
            else if (args[1].getContent() == "4")
            {
                helpMessage = "Help about Constitution.\n" + Formatter::italic();
                helpMessage += "Stamina and physical toughness. A character with a high Endurance ";
                helpMessage += "will survive where others may not.";
                helpMessage += Formatter::reset() + "\n";
            }
            else if (args[1].getContent() == "5")
            {
                helpMessage = "Help about Intelligence.\n" + Formatter::italic();
                helpMessage += "Knowledge, wisdom and the ability to think quickly, ";
                helpMessage += "this attribute is important for any character.";
                helpMessage += Formatter::reset() + "\n";
            }
            else
            {
                helpMessage = "No help for that attribute.";
            }
            this->advance(character, helpMessage);
        }
        else
        {
            this->advance(character, "You have to specify the attribute number.");
        }
    }
    else
    {
        if (args.size() == 2)
        {
            std::string helpMessage;
            int modifier = ToNumber<int>(args[1].getContent());
            // Check for errors.
            if (player->remaining_points < modifier)
            {
                helpMessage = "You don't have enough points left.";
            }
            else if (args[0].getContent() == "1")
            {
                int result = static_cast<int>(player->getAbility(Ability::Strength, false)) + modifier;
                int upperBound = static_cast<int>(player->race->getAbility(Ability::Strength)) + 5;
                int lowerBound = static_cast<int>(player->race->getAbility(Ability::Strength)) - 5;
                if (lowerBound < 0)
                {
                    lowerBound = 0;
                }
                if (result < lowerBound)
                {
                    helpMessage = "Strength can't go below " + ToString(lowerBound) + ".";
                }
                else if (result > upperBound)
                {
                    helpMessage = "Strength can't go above " + ToString(upperBound) + ".";
                }
                else
                {
                    player->remaining_points -= modifier;
                    player->setAbility(Ability::Strength, static_cast<unsigned int>(result));
                }
            }
            else if (args[0].getContent() == "2")
            {
                int result = static_cast<int>(player->getAbility(Ability::Agility, false)) + modifier;
                int upperBound = static_cast<int>(player->race->getAbility(Ability::Agility)) + 5;
                int lowerBound = static_cast<int>(player->race->getAbility(Ability::Agility)) - 5;
                if (lowerBound < 0)
                {
                    lowerBound = 0;
                }
                if (result < lowerBound)
                {
                    helpMessage = "Agility can't go below " + ToString(lowerBound) + ".";
                }
                else if (result > upperBound)
                {
                    helpMessage = "Agility can't go above " + ToString(upperBound) + ".";
                }
                else
                {
                    player->remaining_points -= modifier;
                    player->setAbility(Ability::Agility, static_cast<unsigned int>(result));
                }
            }
            else if (args[0].getContent() == "3")
            {
                int result = static_cast<int>(player->getAbility(Ability::Perception, false)) + modifier;
                int upperBound = static_cast<int>(player->race->getAbility(Ability::Perception)) + 5;
                int lowerBound = static_cast<int>(player->race->getAbility(Ability::Perception)) - 5;
                if (lowerBound < 0)
                {
                    lowerBound = 0;
                }
                if (result < lowerBound)
                {
                    helpMessage = "Perception can't go below " + ToString(lowerBound) + ".";
                }
                else if (result > upperBound)
                {
                    helpMessage = "Perception can't go above " + ToString(upperBound) + ".";
                }
                else
                {
                    player->remaining_points -= modifier;
                    player->setAbility(Ability::Perception, static_cast<unsigned int>(result));
                }
            }
            else if (args[0].getContent() == "4")
            {
                int result = static_cast<int>(player->getAbility(Ability::Constitution, false)) + modifier;
                int upperBound = static_cast<int>(player->race->getAbility(Ability::Constitution)) + 5;
                int lowerBound = static_cast<int>(player->race->getAbility(Ability::Constitution)) - 5;
                if (lowerBound < 0)
                {
                    lowerBound = 0;
                }
                if (result < lowerBound)
                {
                    helpMessage = "Constitution can't go below " + ToString(lowerBound) + ".";
                }
                else if (result > upperBound)
                {
                    helpMessage = "Constitution can't go above " + ToString(upperBound) + ".";
                }
                else
                {
                    player->remaining_points -= modifier;
                    player->setAbility(Ability::Constitution, static_cast<unsigned int>(result));
                }
            }
            else if (args[0].getContent() == "5")
            {
                int result = static_cast<int>(player->getAbility(Ability::Intelligence, false)) + modifier;
                int upperBound = static_cast<int>(player->race->getAbility(Ability::Intelligence)) + 5;
                int lowerBound = static_cast<int>(player->race->getAbility(Ability::Intelligence)) - 5;
                if (lowerBound < 0)
                {
                    lowerBound = 0;
                }
                if (result < lowerBound)
                {
                    helpMessage = "Constitution can't go below " + ToString(lowerBound) + ".";
                }
                else if (result > upperBound)
                {
                    helpMessage = "Intelligence can't go above " + ToString(upperBound) + ".";
                }
                else
                {
                    player->remaining_points -= modifier;
                    player->setAbility(Ability::Intelligence, static_cast<unsigned int>(result));
                }
            }
            else
            {
                helpMessage = "Must select a valid attribute.";
            }
            this->advance(character);
        }
        else
        {
            this->advance(character, "Type [#attribute +/-(value)].");
        }
    }
}

void ProcessNewAttributes::advance(Character * character, const std::string & error)
{
    // Transform the character into player.
    auto player = character->toPlayer();
    // Change the connection state to awaiting age.
    player->connectionState = ConnectionState::AwaitingNewAttr;
    // Print the choices.
    this->printChices(character);
    std::string msg;
    msg += "# " + Formatter::bold() + "Character's Attributes." + Formatter::reset() + "\n";
    msg += "#    [1] Strength     :" + ToString(player->getAbility(Ability::Strength, false)) + "\n";
    msg += "#    [2] Agility      :" + ToString(player->getAbility(Ability::Agility, false)) + "\n";
    msg += "#    [3] Perception   :" + ToString(player->getAbility(Ability::Perception, false)) + "\n";
    msg += "#    [4] Constitution :" + ToString(player->getAbility(Ability::Constitution, false)) + "\n";
    msg += "#    [5] Intelligence :" + ToString(player->getAbility(Ability::Intelligence, false)) + "\n";
    msg += "#\n";
    msg += "# Remaining Points: " + Formatter::green() + ToString(player->remaining_points) + Formatter::reset() + "\n";
    msg += "#\n";
    msg += "# Type [" + Formatter::magenta() + "(number) +/-modifier" + Formatter::reset() + "]";
    msg += " to decrease or increase the value of an attribute.\n";
    msg += "# Type [" + Formatter::magenta() + "help (number)" + Formatter::reset() + "]";
    msg += " to read a brief description of the attribute.\n";
    msg += "# Type [" + Formatter::magenta() + "reset" + Formatter::reset() + "]";
    msg += " to reset the values as default.\n";
    msg += "# Type [" + Formatter::magenta() + "continue" + Formatter::reset() + "]";
    msg += " to continue character creation.\n";
    msg += "# Type [" + Formatter::magenta() + "back" + Formatter::reset() + "]";
    msg += " to return to the previus step.\n";
    character->sendMsg(msg);
    if (!error.empty())
    {
        character->sendMsg("# " + error + "\n");
    }
}

void ProcessNewAttributes::rollBack(Character * character)
{
    auto player = character->toPlayer();
    player->setAbility(Ability::Strength, player->race->getAbility(Ability::Strength));
    player->setAbility(Ability::Agility, player->race->getAbility(Ability::Agility));
    player->setAbility(Ability::Perception, player->race->getAbility(Ability::Perception));
    player->setAbility(Ability::Constitution, player->race->getAbility(Ability::Constitution));
    player->setAbility(Ability::Intelligence, player->race->getAbility(Ability::Intelligence));
    this->advance(character);
}
