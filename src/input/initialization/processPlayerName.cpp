/// @file   processPlayerName.cpp
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

#include "processPlayerName.hpp"
#include "processPlayerPassword.hpp"
#include "processNewName.hpp"
#include "formatter.hpp"
#include "player.hpp"
#include "mud.hpp"

bool ProcessPlayerName::process(ArgumentHandler &args)
{
	auto player = character->toPlayer();
	auto input = args.getOriginal();
	// Name can't be blank.
	if (input.empty()) {
		error = "Name cannot be blank.";
		this->advance();
	} else if (ToLower(input) == "new") {
		// Create a shared pointer to the next step.
		auto newStep = std::make_shared<ProcessNewName>(character);
		// Set the handler.
		player->inputProcessor = newStep;
		// Advance to the next step.
		newStep->advance();
		return true;
	} else if (ToLower(input) == "quit") {
		player->closeConnection();
	} else if (input.find_first_not_of(VALID_CHARACTERS_NAME) !=
			   std::string::npos) {
		error = "That player name contains disallowed characters.";
		this->advance();
	} else if (Mud::instance().findPlayer(input)) {
		error = input + " is already connected.";
		this->advance();
	} else if (!SQLiteDbms::instance().searchPlayer(ToCapitals(input))) {
		error = "That player doesen't exist.";
		this->advance();
	} else {
		// Save the name of the player.
		player->name = ToCapitals(input);
		// Load player so we know the player_password etc.
		if (SQLiteDbms::instance().loadPlayer(player)) {
			player->sendMsg("Username is correct, now insert the password.\n");
			// Set to 0 the current password attempts.
			player->password_attempts = 0;
			// Create a shared pointer to the next step.
			auto newStep = std::make_shared<ProcessPlayerPassword>(character);
			// Set the handler.
			player->inputProcessor = newStep;
			// Advance to the next step.
			newStep->advance();
			return true;
		} else {
			player->closeConnection();
		}
	}
	return false;
}

void ProcessPlayerName::advance()
{
	std::stringstream ss;
	ss << Formatter::clearScreen();
	ss << "\nWelcome to RadMud!\n";
	ss << Formatter::red();
	ss << "#--------------------------------------------#\n";
	ss << "                 XXXXXXXXXXXXX                \n";
	ss << "      /'--_###XXXXXXXXXXXXXXXXXXX###_--'\\    \n";
	ss << "      \\##/#/#XXXXXXXX /O\\ XXXXXXXX#\\'\\##/ \n";
	ss << "       \\/#/#XXXXXXXXX \\O/ XXXXXXXXX#\\#\\/  \n";
	ss << "        \\/##XXXXXXXXXX   XXXXXXXXXX##\\/     \n";
	ss << "         ###XXXX  ''-.XXX.-''  XXXX###        \n";
	ss << "           \\XXXX               XXXX/         \n";
	ss << "             XXXXXXXXXXXXXXXXXXXXX            \n";
	ss << "             XXXX XXXX X XXXX XXXX            \n";
	ss << "             XXX # XXX X XXX # XXX            \n";
	ss << "            /XXXX XXX XXX XXX XXXX\\          \n";
	ss << "           ##XXXXXXX X   X XXXXXXX##          \n";
	ss << "          ##   XOXXX X   X XXXOX   ##         \n";
	ss << "          ##    #XXXX XXX XXX #    ##         \n";
	ss << "           ##..##  XXXXXXXXX  ##..##          \n";
	ss << "            ###      XXXXX     ####           \n";
	ss << "#--------------------------------------------#\n";
	ss << "| Created by : Enrico Fraccaroli.            |\n";
	ss << "| In Date    : 21 Agosto 2014                |\n";
	ss << "#--------------------------------------------#\n";
	ss << Formatter::reset();
	ss << "# Enter your name, or type '" << Formatter::magenta("new")
	   << "' in order to create a new character!\n";
	character->sendMsg(ss.str());
	this->printError();
}

void ProcessPlayerName::rollBack()
{
	this->advance();
}
