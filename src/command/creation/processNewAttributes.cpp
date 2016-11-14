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

#include "creationStep.hpp"
#include "player.hpp"
#include "mud.hpp"

void ProcessNewAttributes(Character * character, ArgumentHandler & args)
{
    Player * player = character->toPlayer();
    // Player_password can't be blank.
    if ((args.size() != 1) && (args.size() != 2))
    {
        AdvanceCharacterCreation(character, ConnectionState::AwaitingNewAttr, "Invalid input.");
    }
        // Check if the player has typed BACK.
    else if (ToLower(args[0].getContent()) == "back")
    {
        RollbackCharacterCreation(player, ConnectionState::AwaitingNewRace);
    }
        // Check if the player has not yet typed RESET.
    else if (ToLower(args[0].getContent()) == "reset")
    {
        player->remaining_points = 0;
        player->setAbility(Ability::Strength, player->race->getAbility(Ability::Strength));
        player->setAbility(Ability::Agility, player->race->getAbility(Ability::Agility));
        player->setAbility(Ability::Perception, player->race->getAbility(Ability::Perception));
        player->setAbility(Ability::Constitution, player->race->getAbility(Ability::Constitution));
        player->setAbility(Ability::Intelligence, player->race->getAbility(Ability::Intelligence));
        AdvanceCharacterCreation(
            character,
            ConnectionState::AwaitingNewAttr,
            Formatter::cyan() + "Attribute has been set by default." + Formatter::reset() + "\n");
    }
        // If the player has insert help (attribute number), show its help.
    else if (BeginWith(ToLower(args[0].getContent()), "continue"))
    {
        AdvanceCharacterCreation(player, ConnectionState::AwaitingNewGender);
    }
        // If the player has insert help (attribute number), show its help.
    else if (BeginWith(ToLower(args[0].getContent()), "help"))
    {
        if (args.size() == 2)
        {
            std::string helpMessage;
            if (args[1].getContent() == "1")
            {
                helpMessage = "Help about Strength.\n" + Formatter::italic();
                helpMessage += "Strength is important for increasing the Carrying Weight and ";
                helpMessage +=
                    "satisfying the minimum Strength requirements for some weapons and armors.";
                helpMessage += Formatter::reset() + "\n";
            }
            else if (args[1].getContent() == "2")
            {
                helpMessage = "Help about Agility.\n" + Formatter::italic();
                helpMessage += "Besides increasing mobility in combat, it increases the recharge ";
                helpMessage +=
                    "speed of all the weapons, as well as the ability to use light armor.";
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
            AdvanceCharacterCreation(character, ConnectionState::AwaitingNewAttr, helpMessage);
        }
        else
        {
            AdvanceCharacterCreation(
                character,
                ConnectionState::AwaitingNewAttr,
                "You have to specify the attribute number.");
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
                int result = static_cast<int>(player->getAbility(Ability::Strength, false))
                             + modifier;
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
                int result = static_cast<int>(player->getAbility(Ability::Agility, false))
                             + modifier;
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
                int result = static_cast<int>(player->getAbility(Ability::Perception, false))
                             + modifier;
                int upperBound = static_cast<int>(player->race->getAbility(Ability::Perception))
                                 + 5;
                int lowerBound = static_cast<int>(player->race->getAbility(Ability::Perception))
                                 - 5;
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
                int result = static_cast<int>(player->getAbility(Ability::Constitution, false))
                             + modifier;
                int upperBound = static_cast<int>(player->race->getAbility(Ability::Constitution))
                                 + 5;
                int lowerBound = static_cast<int>(player->race->getAbility(Ability::Constitution))
                                 - 5;
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
                int result = static_cast<int>(player->getAbility(Ability::Intelligence, false))
                             + modifier;
                int upperBound = static_cast<int>(player->race->getAbility(Ability::Intelligence))
                                 + 5;
                int lowerBound = static_cast<int>(player->race->getAbility(Ability::Intelligence))
                                 - 5;
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
            AdvanceCharacterCreation(character, ConnectionState::AwaitingNewAttr, helpMessage);
        }
        else
        {
            AdvanceCharacterCreation(
                character,
                ConnectionState::AwaitingNewAttr,
                "Type [#attribute +/-(value)].");
        }
    }
}
