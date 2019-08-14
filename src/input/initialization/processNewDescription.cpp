/// @file   processNewDescription.cpp
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

#include "processNewDescription.hpp"
#include "processNewWeight.hpp"
#include "processNewAge.hpp"
#include "formatter.hpp"
#include "player.hpp"
#include "mud.hpp"

bool ProcessNewDescription::process(Character *character, ArgumentHandler &args)
{
	auto player = character->toPlayer();
	auto input = args.getOriginal();
	// Player_password can't be blank.
	if (input.empty()) {
		this->advance(character, "Invalid input.");
	} else if (ToLower(input) == "back") {
		// Create a shared pointer to the previous step.
		auto newStep = std::make_shared<ProcessNewAge>();
		// Set the handler.
		player->inputProcessor = newStep;
		// Advance to the next step.
		newStep->rollBack(character);
		return true;
	} else if (ToLower(input) == "skip") {
		// Check if the player has typed SKIP.
		player->description = "You see " + ToLower(player->name) + " here.";
		// Create a shared pointer to the next step.
		auto newStep = std::make_shared<ProcessNewWeight>();
		// Set the handler.
		player->inputProcessor = newStep;
		// Advance to the next step.
		newStep->advance(character);
		return true;
	} else if (input.find_first_not_of(VALID_CHARACTERS_DESC) !=
			   std::string::npos) {
		this->advance(character,
					  "Description cannot contain disallowed characters.");
	} else {
		player->description = input;
		// Create a shared pointer to the next step.
		auto newStep = std::make_shared<ProcessNewWeight>();
		// Set the handler.
		player->inputProcessor = newStep;
		// Advance to the next step.
		newStep->advance(character);
		return true;
	}
	return false;
}

void ProcessNewDescription::advance(Character *character,
									const std::string &error)
{
	// Print the choices.
	this->printChoices(character);
	auto Bold = [](const std::string &s) {
		return Formatter::magenta() + s + Formatter::reset();
	};
	auto Magenta = [](const std::string &s) {
		return Formatter::magenta() + s + Formatter::reset();
	};
	std::string msg;
	msg += "# " + Bold("Character's Description.") + "\n";
	msg += "# Insert a brief description of your character, its optional.\n";
	msg += "# Type [" + Magenta("back") + "] to return to the previous step.\n";
	msg += "# Type [" + Magenta("skip") + "] to just pass to the next step.\n";
	if (!error.empty()) {
		msg += "# " + error + "\n";
	}
	character->sendMsg(msg);
}

void ProcessNewDescription::rollBack(Character *character)
{
	auto player = character->toPlayer();
	player->description = "";
	this->advance(character);
}
