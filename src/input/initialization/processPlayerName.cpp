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
#include "player.hpp"
#include "mud.hpp"
#include "processNewName.hpp"
#include "processPlayerPassword.hpp"

void ProcessPlayerName::process(Character * character, ArgumentHandler & args)
{
    Logger::log(LogLevel::Debug, "ProcessPlayerName");
    auto player = character->toPlayer();
    auto input = args.getOriginal();
    // Name can't be blank.
    if (input.empty())
    {
        this->advance(character, "Name cannot be blank.");
    }
    else if (ToLower(input) == "new")
    {
        // Create a shared pointer to the next step.
        std::shared_ptr<ProcessNewName> newStep = std::make_shared<ProcessNewName>();
        // Set the handler.
        player->inputProcessor = newStep;
        // Advance to the next step.
        newStep->advance(character);
    }
    else if (input == "quit")
    {
        player->closeConnection();
    }
    else if (input.find_first_not_of(VALID_CHARACTERS_NAME) != std::string::npos)
    {
        this->advance(character, "That player name contains disallowed characters.");
    }
    else if (Mud::instance().findPlayer(input))
    {
        this->advance(character, input + " is already connected.");
    }
    else if (!SQLiteDbms::instance().searchPlayer(ToCapitals(input)))
    {
        this->advance(character, "That player doesen't exist.");
    }
    else
    {
        // Save the name of the player.
        player->name = ToCapitals(input);
        // Load player so we know the player_password etc.
        if (SQLiteDbms::instance().loadPlayer(player))
        {
            // Delete the loaded prompt, otherwise it will be shown.
            player->prompt = "";
            // Set to 0 the current password attempts.
            player->password_attempts = 0;
            player->sendMsg("Username is correct, now insert the password.\n");
            // Create a shared pointer to the next step.
            std::shared_ptr<ProcessPlayerPassword> newStep = std::make_shared<ProcessPlayerPassword>();
            // Set the handler.
            player->inputProcessor = newStep;
            // Advance to the next step.
            newStep->advance(character);
        }
        else
        {
            player->closeConnection();
        }
    }
}

void ProcessPlayerName::advance(Character * character, const std::string & error)
{
    // Change the connection state to awaiting age.
    character->toPlayer()->connectionState = ConnectionState::AwaitingName;
    std::string msg;
    msg += Formatter::clearScreen();
    msg += "\nWelcome to RadMud!\n";
    msg += Formatter::red();
    msg += "#--------------------------------------------#\n";
    msg += "                 XXXXXXXXXXXXX                \n";
    msg += "      /'--_###XXXXXXXXXXXXXXXXXXX###_--'\\    \n";
    msg += "      \\##/#/#XXXXXXXX /O\\ XXXXXXXX#\\'\\##/ \n";
    msg += "       \\/#/#XXXXXXXXX \\O/ XXXXXXXXX#\\#\\/  \n";
    msg += "        \\/##XXXXXXXXXX   XXXXXXXXXX##\\/     \n";
    msg += "         ###XXXX  ''-.XXX.-''  XXXX###        \n";
    msg += "           \\XXXX               XXXX/         \n";
    msg += "             XXXXXXXXXXXXXXXXXXXXX            \n";
    msg += "             XXXX XXXX X XXXX XXXX            \n";
    msg += "             XXX # XXX X XXX # XXX            \n";
    msg += "            /XXXX XXX XXX XXX XXXX\\          \n";
    msg += "           ##XXXXXXX X   X XXXXXXX##          \n";
    msg += "          ##   XOXXX X   X XXXOX   ##         \n";
    msg += "          ##    #XXXX XXX XXX #    ##         \n";
    msg += "           ##..##  XXXXXXXXX  ##..##          \n";
    msg += "            ###      XXXXX     ####           \n";
    msg += "#--------------------------------------------#\n";
    msg += "| Created by : Enrico Fraccaroli.            |\n";
    msg += "| Date       : 21 Agosto 2014                |\n";
    msg += "#--------------------------------------------#\n";
    msg += Formatter::reset();
    msg += "# Enter your name, or type '" + Formatter::magenta() + "new" + Formatter::reset();
    msg += "' in order to create a new character!\n";
    character->sendMsg(msg);
    if (!error.empty())
    {
        character->sendMsg("# " + error + "\n");
    }
}

void ProcessPlayerName::rollBack(Character * character)
{
    this->advance(character);
}
