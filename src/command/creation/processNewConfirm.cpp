/// @file   processNewConfirm.cpp
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

#include "processNewConfirm.hpp"
#include "player.hpp"
#include "mud.hpp"
#include "processNewWeight.hpp"

void ProcessNewConfirm::process(Character * character, ArgumentHandler & args)
{
    auto player = character->toPlayer();
    auto input = args.getOriginal();
    if (ToLower(input) == "back")
    {
        // Create a shared pointer to the previous step.
        std::shared_ptr<ProcessNewWeight> newStep = std::make_shared<ProcessNewWeight>();
        // Set the handler.
        player->inputHandler = newStep;
        // Advance to the next step.
        newStep->rollBack(character);
    }
    else if (ToLower(input) == "confirm")
    {
        // Set the last variables.
        player->level = 0;
        player->experience = 0;
        player->flags = 0;
        player->rent_room = 1000;
        SQLiteDbms::instance().beginTransaction();
        if (player->updateOnDB())
        {
            // Set the handler.
            player->inputHandler = std::make_shared<InputHandler>();
            // Retrieve the saved prompt.
            player->prompt = player->prompt_save;
            // Entered the MUD.
            player->enterGame();
            player->connectionState = ConnectionState::Playing;
        }
        else
        {
            player->closeConnection();
        }
        SQLiteDbms::instance().endTransaction();
    }
    else
    {
        this->advance(character, "You must write 'confirm' if you agree.");
    }
}

void ProcessNewConfirm::advance(Character * character, const std::string & error)
{
    // Change the connection state to awaiting age.
    character->toPlayer()->connectionState = ConnectionState::AwaitingNewConfirm;
    // Print the choices.
    this->printChices(character);
    std::string msg;
    msg += "# Give a look to the information you have provided, now it's the right time";
    msg += " to decide if you want to change something.\n";
    msg += "# Type [" + Formatter::magenta() + "confirm" + Formatter::reset()
           + "] to conclude the character creation.\n";
    msg += "# Type [" + Formatter::magenta() + "back" + Formatter::reset()
           + "]    to return to the previus step.\n";
    msg += Formatter::green() + "Do you confirm? " + Formatter::reset() + "\n";
    character->sendMsg(msg);
    if (!error.empty())
    {
        character->sendMsg("# " + error + "\n");
    }
}

void ProcessNewConfirm::rollBack(Character *)
{

}
