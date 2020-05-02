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

#include "input/initialization/processNewRace.hpp"
#include "input/initialization/processNewAttributes.hpp"
#include "input/initialization/processNewStory.hpp"
#include "utilities/formatter.hpp"

bool ProcessNewRace::process(ArgumentHandler &args)
{
	auto player = character->toPlayer();
	// Player_password can't be blank.
	if ((args.size() != 1) && (args.size() != 2)) {
		error = "Invalid input.";
		this->advance();
		return false;
	}
	if (ToLower(args[0].getContent()) == "back") {
		// Create a shared pointer to the previous step.
		auto newStep = std::make_shared<ProcessNewStory>(character);
		// Set the handler.
		player->inputProcessor = newStep;
		// Advance to the next step.
		newStep->rollBack();
		return true;
	}
	if (BeginWith(ToLower(args[0].getContent()), "help")) {
		if (args.size() == 1) {
			error = "You have to specify the race number.";
			this->advance();
			return false;
		}
		// Retrieve the number of the race.
		auto raceVnum = ToNumber<unsigned int>(args[1].getContent());
		// Get the race.
		auto race = Mud::instance().findRace(raceVnum);
		if (race == nullptr) {
			error = "No help for that race.";
			this->advance();
			return false;
		}
		if (!race->player_allow) {
			error = "No help for that race.";
			this->advance();
			return false;
		}
		error += "Help about '" + race->name + "'.\n";
		for (auto const &it : TextWrap(race->description, 60)) {
			error += it + "\n";
		}
		error += "  Strength     " + ToString(race->getAbility(Ability::Strength)) + "\n";
		error += "  Agility      " + ToString(race->getAbility(Ability::Agility)) + "\n";
		error += "  Perception   " + ToString(race->getAbility(Ability::Perception)) + "\n";
		error += "  Constitution " + ToString(race->getAbility(Ability::Constitution)) + "\n";
		error += "  Intelligence " + ToString(race->getAbility(Ability::Intelligence)) + "\n";
		this->advance();
		return true;
	}
	// Get the race.
	auto race = Mud::instance().findRace(ToNumber<unsigned int>(args[0].getContent()));
	if (race == nullptr) {
		error = "Not a valid race.";
		this->advance();
		return false;
	}
	if (!race->player_allow) {
		error = "Not a valid race.";
		this->advance();
		return false;
	}
	// Set the race.
	player->race = race;
	// Set the attributes.
	player->setAbility(Ability::Strength, race->getAbility(Ability::Strength));
	player->setAbility(Ability::Agility, race->getAbility(Ability::Agility));
	player->setAbility(Ability::Perception, race->getAbility(Ability::Perception));
	player->setAbility(Ability::Constitution, race->getAbility(Ability::Constitution));
	player->setAbility(Ability::Intelligence, race->getAbility(Ability::Intelligence));
	// Set the health & stamina.
	player->setHealth(player->getMaxHealth(), true);
	player->setStamina(player->getMaxStamina(), true);
	// Set the remaining points.
	player->remaining_points = 0;
	// Create a shared pointer to the next step.
	auto newStep = std::make_shared<ProcessNewAttributes>(character);
	// Set the handler.
	player->inputProcessor = newStep;
	// Advance to the next step.
	newStep->advance();
	return true;
}

void ProcessNewRace::advance()
{
	// Print the choices.
	this->printChoices();

	std::stringstream ss;
	ss << "# " << Formatter::bold("Character's Race.") << "\n";
	for (auto const &it : Mud::instance().mudRaces)
		if (it.second->player_allow)
			ss << "# [" << it.second->vnum << "] " << it.second->name << ".\n";
	ss << "#\n";
	ss << "# Choose one of the above race by typing the correspondent number.\n";
	ss << "#\n";
	ss << "# Type [" << Formatter::magenta("help [Number]");
	ss << "] to read a brief description of the race.\n";
	ss << "# Type [" << Formatter::magenta("back") << "] to return to the previous step.\n";
	character->sendMsg(ss.str());

	this->printError();
}

void ProcessNewRace::rollBack()
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
	this->advance();
}
