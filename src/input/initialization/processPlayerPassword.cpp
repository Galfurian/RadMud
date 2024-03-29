/// @file   processPlayerPassword.cpp
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

#include "input/initialization/processPlayerPassword.hpp"
#include "character/player.hpp"
#include "mud.hpp"

bool ProcessPlayerPassword::process(Character * character,
                                    ArgumentHandler & args)
{
    auto player = character->toPlayer();
    auto input = args.getOriginal();
    // Check the correctness of the password.
    if (input != player->password)
    {
        // Detect too many password attempts.
        if (++player->password_attempts >= 3)
        {
            player->closeConnection();
            player->sendMsg("Goodbye!\n");
            player->sendMsg("Too many attempts to guess the password!\n");
        }
        else
        {
            this->advance(character, "Incorrect password.");
        }
    }
    else
    {
        // Set the handler.
        player->inputProcessor = std::make_shared<ProcessInput>();
        // Entered the MUD.
        player->enterGame();
        // Set the connection state to playing.
        player->connectionState = ConnectionState::Playing;
        return true;
    }
    return false;
}

void ProcessPlayerPassword::advance(Character * character,
                                    const std::string & error)
{
    if (!error.empty())
    {
        character->sendMsg("# " + error + "\n");
    }
}

void ProcessPlayerPassword::rollBack(Character *)
{

}
