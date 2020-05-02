/// @file   processNewGender.cpp
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

#include "input/initialization/processNewGender.hpp"
#include "input/initialization/processNewAttributes.hpp"
#include "input/initialization/processNewAge.hpp"
#include "utilities/formatter.hpp"
#include "character/player.hpp"
#include "mud.hpp"

bool ProcessNewGender::process(ArgumentHandler &args)
{
	auto player = character->toPlayer();
	auto input = args.getOriginal();
	// Check if the player has typed BACK.
	if (ToLower(input) == "back") {
		// Create a shared pointer to the previous step.
		auto newStep = std::make_shared<ProcessNewAttributes>(character);
		// Set the handler.
		player->inputProcessor = newStep;
		// Advance to the next step.
		newStep->rollBack();
		return true;
	} else if (input == "1") {
		player->gender = GenderType::Male;
		// Create a shared pointer to the next step.
		auto newStep = std::make_shared<ProcessNewAge>(character);
		// Set the handler.
		player->inputProcessor = newStep;
		// Advance to the next step.
		newStep->advance();
		return true;
	} else if (input == "2") {
		player->gender = GenderType::Female;
		// Create a shared pointer to the next step.
		auto newStep = std::make_shared<ProcessNewAge>(character);
		// Set the handler.
		player->inputProcessor = newStep;
		// Advance to the next step.
		newStep->advance();
		return true;
	}
	error = "Not a valid gender.";
	this->advance();
	return false;
}

void ProcessNewGender::advance()
{
	// Print the choices.
	this->printChoices();
	std::stringstream ss;
	ss << "# " + Formatter::bold("Character's Gender.") << "\n";
	ss << "#    [1] Male.\n";
	ss << "#    [2] Female.\n";
	ss << "#\n";
	ss << "# Choose one of the above gender by typing the correspondent number.\n";
	ss << "#\n";
	ss << "# Type [" + Formatter::magenta("back") << "] to return to the previous step.\n";
	character->sendMsg(ss.str());
	this->printError();
}

void ProcessNewGender::rollBack()
{
	auto player = character->toPlayer();
	player->gender = GenderType::None;
	this->advance();
}
