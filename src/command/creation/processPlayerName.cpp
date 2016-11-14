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

#include "creationStep.hpp"
#include "player.hpp"
#include "mud.hpp"

void ProcessPlayerName(Character * character, ArgumentHandler & args)
{
    auto player = character->toPlayer();
    auto input = args.getOriginal();
    // Name can't be blank.
    if (input.empty())
    {
        AdvanceCharacterCreation(character, ConnectionState::AwaitingName, "Name cannot be blank.");
    }
        // Check if the player has typed new.
    else if (ToLower(input) == "new")
    {
        AdvanceCharacterCreation(player, ConnectionState::AwaitingNewName);
    }
        // Check if the user want to quit.
    else if (input == "quit")
    {
        player->closeConnection();
    }
        // Check if the give name contains valid characters.
    else if (input.find_first_not_of(VALID_CHARACTERS_NAME) != std::string::npos)
    {
        AdvanceCharacterCreation(
            character,
            ConnectionState::AwaitingName,
            "That player name contains disallowed characters.");
    }
        // Check if the player is already connected.
    else if (Mud::instance().findPlayer(input))
    {
        AdvanceCharacterCreation(
            character,
            ConnectionState::AwaitingName,
            input + " is already connected.");
    }
        // Check if the player exists in the Database.
    else if (!SQLiteDbms::instance().searchPlayer(ToCapitals(input)))
    {
        AdvanceCharacterCreation(
            character,
            ConnectionState::AwaitingName,
            "That player doesen't exist.");
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
            AdvanceCharacterCreation(character, ConnectionState::AwaitingPassword);
        }
        else
        {
            player->closeConnection();
        }
    }
}
