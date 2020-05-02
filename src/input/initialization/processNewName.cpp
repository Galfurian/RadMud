/// @file   processNewName.cpp
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

#include "input/initialization/processNewName.hpp"
#include "input/initialization/processNewPassword.hpp"
#include "input/initialization/processPlayerName.hpp"
#include "utilities/formatter.hpp"
#include "character/player.hpp"
#include "mud.hpp"

bool ProcessNewName::process(ArgumentHandler &args)
{
	auto player = character->toPlayer();
	auto input = args.getOriginal();
	// Player_password can't be blank.
	if (input.empty()) {
		error = "Invalid input.";
		this->advance();
	} else if (ToLower(input) == "back") {
		// Create a shared pointer to the previous step.
		auto newStep = std::make_shared<ProcessPlayerName>(character);
		// Set the handler.
		player->inputProcessor = newStep;
		// Advance to the next step.
		newStep->rollBack();
		return true;
	} else if (input.find_first_not_of(VALID_CHARACTERS_NAME) != std::string::npos) {
		error = "That player name contains disallowed characters.";
		this->advance();
	}
	// Check for bad names here.
	else if (Mud::instance().badNames.find(input) != Mud::instance().badNames.end()) {
		error = "That name is not permitted.";
		this->advance();
	}
	// Check if the player name has already been used.
	else if (SQLiteDbms::instance().searchPlayer(ToCapitals(input))) {
		error = "That player already exists, please choose another name.";
		this->advance();
	} else {
		player->name = ToCapitals(input);
		player->room = Mud::instance().findRoom(1000);
		player->faction = Mud::instance().findFaction(1);
		player->password_attempts = 0;
		// Create a shared pointer to the next step.
		auto newStep = std::make_shared<ProcessNewPassword>(character);
		// Set the handler.
		player->inputProcessor = newStep;
		// Advance to the next step.
		newStep->advance();
		return true;
	}
	return false;
}

void ProcessNewName::advance()
{
	// Print the choices.
	this->printChoices();
	std::stringstream ss;
	ss << "# " + Formatter::bold("Character's Name.") + "\n";
	ss << "# Choose carefully, because this it's the only chance you have";
	ss << " to pick a legendary name, maybe one day it will";
	ss << " be whispered all over the lands.\n";
	ss << "# Type [" + Formatter::magenta("back") << "] to return to the previous step.\n";
	character->sendMsg(ss.str());
	this->printError();
}

void ProcessNewName::rollBack()
{
	// Reset the values.
	character->name = "";
	// Advance to the current step.
	this->advance();
}
