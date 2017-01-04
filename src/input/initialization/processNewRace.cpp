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

bool ProcessNewRace::process(Character * character, ArgumentHandler & args)
{
    auto player = character->toPlayer();
    // Player_password can't be blank.
    if ((args.size() != 1) && (args.size() != 2))
    {
        this->advance(character, "Invalid input.");
        return false;
    }
    if (ToLower(args[0].getContent()) == "back")
    {
        // Create a shared pointer to the previous step.
        auto newStep = std::make_shared<ProcessNewStory>();
        // Set the handler.
        player->inputProcessor = newStep;
        // Advance to the next step.
        newStep->rollBack(character);
        return true;
    }
    if (BeginWith(ToLower(args[0].getContent()), "help"))
    {
        if (args.size() == 1)
        {
            this->advance(character, "You have to specify the race number.");
            return false;
        }
        // Retrieve the number of the race.
        auto raceVnum = ToNumber<int>(args[1].getContent());
        // Get the race.
        auto race = Mud::instance().findRace(raceVnum);
        if (race == nullptr)
        {
            this->advance(character, "No help for that race.");
            return false;
        }
        if (!race->player_allow)
        {
            this->advance(character, "No help for that race.");
            return false;
        }
        std::string help;
        help += "Help about '" + race->name + "'.\n";
        help += race->description + "\n";
        help += "  Strength     " +
                ToString(race->getAbility(Ability::Strength)) + "\n";
        help += "  Agility      " +
                ToString(race->getAbility(Ability::Agility)) + "\n";
        help += "  Perception   " +
                ToString(race->getAbility(Ability::Perception)) + "\n";
        help += "  Constitution " +
                ToString(race->getAbility(Ability::Constitution)) + "\n";
        help += "  Intelligence " +
                ToString(race->getAbility(Ability::Intelligence)) + "\n";
        this->advance(character, help);
        return true;
    }
    // Get the race.
    auto race = Mud::instance().findRace(ToNumber<int>(args[0].getContent()));
    if (race == nullptr)
    {
        this->advance(character, "Not a valid race.");
        return false;
    }
    if (!race->player_allow)
    {
        this->advance(character, "Not a valid race.");
        return false;
    }
    // Set the race.
    player->race = race;
    // Set the attributes.
    player->setAbility(Ability::Strength,
                       race->getAbility(Ability::Strength));
    player->setAbility(Ability::Agility,
                       race->getAbility(Ability::Agility));
    player->setAbility(Ability::Perception,
                       race->getAbility(Ability::Perception));
    player->setAbility(Ability::Constitution,
                       race->getAbility(Ability::Constitution));
    player->setAbility(Ability::Intelligence,
                       race->getAbility(Ability::Intelligence));
    // Set the health & stamina.
    player->setHealth(player->getMaxHealth(), true);
    player->setStamina(player->getMaxStamina(), true);
    // Set the remaining points.
    player->remaining_points = 0;
    // Create a shared pointer to the next step.
    auto newStep = std::make_shared<ProcessNewAttributes>();
    // Set the handler.
    player->inputProcessor = newStep;
    // Advance to the next step.
    newStep->advance(character);
    return true;
}

void ProcessNewRace::advance(Character * character,
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
    msg += "# " + Bold("Character's Race.") + "\n";
    for (auto iterator : Mud::instance().mudRaces)
    {
        if (iterator.second->player_allow)
        {
            msg += "#    [" + ToString(iterator.second->vnum) + "] ";
            msg += iterator.second->name + ".\n";
        }
    }
    msg += "#\n";
    msg += "# Choose one of the above race by typing the correspondent number.\n";
    msg += "#\n";
    msg += "# Type [" + Magenta("help [Number]");
    msg += "] to read a brief description of the race.\n";
    msg += "# Type [" + Magenta("back") + "] to return to the previous step.\n";
    if (!error.empty())
    {
        msg += "# " + error + "\n";
    }
    character->sendMsg(msg);
}

void ProcessNewRace::rollBack(Character * character)
{
    auto player = character->toPlayer();
    // Reset the values.
    player->race = nullptr;
    player->setHealth(0);
    player->setStamina(0);
    player->setAbility(Ability::Strength, 0);
    player->setAbility(Ability::Agility, 0);
    player->setAbility(Ability::Perception, 0);
    player->setAbility(Ability::Constitution, 0);
    player->setAbility(Ability::Intelligence, 0);
    player->remaining_points = 0;
    // Advance to the current step.
    this->advance(character);
}
