/// @file   processNewRace.cpp
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

void ProcessNewRace(Character * character, ArgumentHandler & args)
{
    Player * player = character->toPlayer();
    // Player_password can't be blank.
    if ((args.size() != 1) && (args.size() != 2))
    {
        AdvanceCharacterCreation(character, ConnectionState::AwaitingNewRace, "Invalid input.");
    }
        // Check if the player has typed BACK.
    else if (ToLower(args[0].getContent()) == "back")
    {
        RollbackCharacterCreation(player, ConnectionState::AwaitingNewStory);
    }
        // If the player has insert help (race_number), show its help.
    else if (BeginWith(ToLower(args[0].getContent()), "help"))
    {
        if (args.size() == 2)
        {
            // Get the race.
            Race * race = Mud::instance().findRace(ToNumber<int>(args[1].getContent()));
            if (race == nullptr)
            {
                AdvanceCharacterCreation(
                    character,
                    ConnectionState::AwaitingNewRace,
                    "No help for that race.");
            }
            else if (!race->player_allow)
            {
                AdvanceCharacterCreation(
                    character,
                    ConnectionState::AwaitingNewRace,
                    "No help for that race.");
            }
            else
            {
                std::string helpMessage;
                helpMessage += "Help about " + race->name + ".\n";
                helpMessage += "Strength     " + ToString(race->getAbility(Ability::Strength))
                               + ".\n";
                helpMessage += "Agility      " + ToString(race->getAbility(Ability::Agility))
                               + ".\n";
                helpMessage += "Perception   " + ToString(race->getAbility(Ability::Perception))
                               + ".\n";
                helpMessage += "Constitution " + ToString(race->getAbility(Ability::Constitution))
                               + ".\n";
                helpMessage += "Intelligence " + ToString(race->getAbility(Ability::Intelligence))
                               + ".\n";
                helpMessage += Formatter::italic() + race->description + Formatter::reset() + "\n";
                AdvanceCharacterCreation(character, ConnectionState::AwaitingNewRace, helpMessage);
            }

        }
        else
        {
            AdvanceCharacterCreation(
                character,
                ConnectionState::AwaitingNewRace,
                "You have to specify the race number.");
        }
    }
    else if (IsNumber(args[0].getContent()))
    {
        // Get the race.
        Race * race = Mud::instance().findRace(ToNumber<int>(args[0].getContent()));
        if (race == nullptr)
        {
            AdvanceCharacterCreation(
                character,
                ConnectionState::AwaitingNewRace,
                "Not a valid race.");
        }
        else if (!race->player_allow)
        {
            AdvanceCharacterCreation(
                character,
                ConnectionState::AwaitingNewRace,
                "Not a valid race.");
        }
        else
        {
            player->race = race;
            player->setAbility(Ability::Strength, race->getAbility(Ability::Strength));
            player->setAbility(Ability::Agility, race->getAbility(Ability::Agility));
            player->setAbility(Ability::Perception, race->getAbility(Ability::Perception));
            player->setAbility(Ability::Constitution, race->getAbility(Ability::Constitution));
            player->setAbility(Ability::Intelligence, race->getAbility(Ability::Intelligence));
            player->setHealth(player->getMaxHealth(), true);
            player->setStamina(player->getMaxStamina(), true);
            AdvanceCharacterCreation(player, ConnectionState::AwaitingNewAttr);
        }
    }
}
