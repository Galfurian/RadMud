/// @file   processNewPasswordConfirm.cpp
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

#include "processNewPasswordConfirm.hpp"
#include "processNewPassword.hpp"
#include "processNewStory.hpp"
#include "formatter.hpp"

bool ProcessNewPasswordConfirm::process(ArgumentHandler &args)
{
	auto player = character->toPlayer();
	auto input = args.getOriginal();
	// Check if the player has typed BACK.
	if (ToLower(input) == "back") {
		// Create a shared pointer to the previous step.
		auto newStep = std::make_shared<ProcessNewPassword>(character);
		// Set the handler.
		player->inputProcessor = newStep;
		// Advance to the next step.
		newStep->rollBack();
		return true;
	} else if (input != player->password) {
		error = "Password and confirmation do not agree.";
		this->advance();
	} else {
		// Create a shared pointer to the next step.
		auto newStep = std::make_shared<ProcessNewStory>(character);
		// Set the handler.
		player->inputProcessor = newStep;
		// Advance to the next step.
		newStep->advance();
		return true;
	}
	return false;
}

void ProcessNewPasswordConfirm::advance()
{
	// Print the choices.
	this->printChoices();
	character->sendMsg(Formatter::green("Re-enter the password...\n"));
	this->printError();
}

void ProcessNewPasswordConfirm::rollBack()
{
	// Do not stop this step, just go back to ProcessNewPassword.
	auto newStep = std::make_shared<ProcessNewPassword>(character);
	character->inputProcessor = newStep;
	newStep->rollBack();
}
