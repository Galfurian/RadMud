/// @file   processNewStory.cpp
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

#include "input/initialization/processNewStory.hpp"
#include "input/initialization/processNewPassword.hpp"
#include "input/initialization/processNewRace.hpp"
#include "utilities/formatter.hpp"

bool ProcessNewStory::process(ArgumentHandler &args)
{
	auto player = character->toPlayer();
	auto input = args.getOriginal();
	// Player_password can't be blank.
	if (input.empty()) {
		error = "Invalid input.";
		this->advance();
	} else if (ToLower(input) == "back") {
		// Create a shared pointer to the previous step.
		auto newStep = std::make_shared<ProcessNewPassword>(character);
		// Set the handler.
		player->inputProcessor = newStep;
		// Advance to the next step.
		newStep->rollBack();
		return true;
	} else if (input != "continue") {
		error = "Write 'continue' after you have reade the story.";
		this->advance();
	} else {
		// Create a shared pointer to the next step.
		auto newStep = std::make_shared<ProcessNewRace>(character);
		// Set the handler.
		player->inputProcessor = newStep;
		// Advance to the next step.
		newStep->advance();
		return true;
	}
	return false;
}

void ProcessNewStory::advance()
{
	// Print the choices.
	this->printChoices();

	std::stringstream ss;
	ss << "# The story of the Wasteland.\n";
	ss << "#\n";
	ss << "Year 374\n";
	ss << "#\n";
	ss << "# Type [" << Formatter::magenta("continue")
	   << "] to continue to the next step.\n";
	ss << "# Type [" << Formatter::magenta("back")
	   << "] to return to the previous step.\n";
	character->sendMsg(ss.str());

	this->printError();
}

void ProcessNewStory::rollBack()
{
	this->advance();
}
