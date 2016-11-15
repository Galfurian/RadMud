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

#include "processNewRace.hpp"
#include "player.hpp"
#include "mud.hpp"
#include "processNewStory.hpp"
#include "processNewAttributes.hpp"

void ProcessNewRace::process(Character * character, ArgumentHandler & args)
{
    auto player = character->toPlayer();
    // Player_password can't be blank.
    if ((args.size() != 1) && (args.size() != 2))
    {
        this->advance(character, "Invalid input.");
    }
    else if (ToLower(args[0].getContent()) == "back")
    {
        // Create a shared pointer to the previous step.
        std::shared_ptr<ProcessNewStory> newStep = std::make_shared<ProcessNewStory>();
        // Set the handler.
        player->inputHandler = newStep;
        // Advance to the next step.
        newStep->rollBack(character);
    }
    else if (BeginWith(ToLower(args[0].getContent()), "help"))
    {
        if (args.size() == 2)
        {
            // Get the race.
            Race * race = Mud::instance().findRace(ToNumber<int>(args[1].getContent()));
            if (race == nullptr)
            {
                this->advance(character, "No help for that race.");
            }
            else if (!race->player_allow)
            {
                this->advance(character, "No help for that race.");
            }
            else
            {
                std::string helpMessage;
                helpMessage += "Help about " + race->name + ".\n";
                helpMessage += "Strength     " + ToString(race->getAbility(Ability::Strength)) + ".\n";
                helpMessage += "Agility      " + ToString(race->getAbility(Ability::Agility)) + ".\n";
                helpMessage += "Perception   " + ToString(race->getAbility(Ability::Perception)) + ".\n";
                helpMessage += "Constitution " + ToString(race->getAbility(Ability::Constitution)) + ".\n";
                helpMessage += "Intelligence " + ToString(race->getAbility(Ability::Intelligence)) + ".\n";
                helpMessage += Formatter::italic() + race->description + Formatter::reset() + "\n";
                this->advance(character, helpMessage);
            }
        }
        else
        {
            this->advance(character, "You have to specify the race number.");
        }
    }
    else if (IsNumber(args[0].getContent()))
    {
        // Get the race.
        Race * race = Mud::instance().findRace(ToNumber<int>(args[0].getContent()));
        if (race == nullptr)
        {
            this->advance(character, "Not a valid race.");
        }
        else if (!race->player_allow)
        {
            this->advance(character, "Not a valid race.");
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
            // Create a shared pointer to the next step.
            std::shared_ptr<ProcessNewAttributes> newStep = std::make_shared<ProcessNewAttributes>();
            // Set the handler.
            player->inputHandler = newStep;
            // Advance to the next step.
            newStep->advance(character);
        }
    }
}

void ProcessNewRace::advance(Character * character, const std::string & error)
{
    // Change the connection state to awaiting age.
    character->toPlayer()->connectionState = ConnectionState::AwaitingNewRace;
    // Print the choices.
    this->printChices(character);
    std::string msg;
    msg += "# " + Formatter::bold() + "Character's Race." + Formatter::reset() + "\n";
    for (auto iterator : Mud::instance().mudRaces)
    {
        Race * race = iterator.second;
        if (race->player_allow)
        {
            msg += "#    [" + ToString(race->vnum) + "] " + race->name + ".\n";
        }
    }
    msg += "#\n";
    msg += "# Choose one of the above race by typing the correspondent number.\n";
    msg += "#\n";
    msg += "# Type [" + Formatter::magenta() + "help [Number]" + Formatter::reset() + "]";
    msg += " to read a brief description of the race.\n";
    msg += "# Type [" + Formatter::magenta() + "back" + Formatter::reset() + "]";
    msg += " to return to the previus step.\n";
    character->sendMsg(msg);
    if (!error.empty())
    {
        character->sendMsg("# " + error + "\n");
    }
}

void ProcessNewRace::rollBack(Character * character)
{
    auto player = character->toPlayer();
    player->race = nullptr;
    player->setHealth(0);
    player->setStamina(0);
    player->setAbility(Ability::Strength, 0);
    player->setAbility(Ability::Agility, 0);
    player->setAbility(Ability::Perception, 0);
    player->setAbility(Ability::Constitution, 0);
    player->setAbility(Ability::Intelligence, 0);
    player->remaining_points = 0;
    this->advance(character);
}
