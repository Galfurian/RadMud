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

#include "creationStep.hpp"
#include "player.hpp"
#include "mud.hpp"

void ProcessNewName(Character * character, ArgumentHandler & args)
{
    Player * player = character->toPlayer();
    auto input = args.getOriginal();
    // Player_password can't be blank.
    if (input.empty())
    {
        AdvanceCharacterCreation(character, ConnectionState::AwaitingNewName, "Invalid input.");
    }
        // Check if the player has typed BACK.
    else if (ToLower(input) == "back")
    {
        RollbackCharacterCreation(player, ConnectionState::AwaitingName);
    }
        // Check if the player has given bad characters.
    else if (input.find_first_not_of(VALID_CHARACTERS_NAME) != std::string::npos)
    {
        AdvanceCharacterCreation(
            character,
            ConnectionState::AwaitingNewName,
            "That player name contains disallowed characters.");
    }
        // Check for bad names here.
    else if (Mud::instance().badNames.find(input) != Mud::instance().badNames.end())
    {
        AdvanceCharacterCreation(
            character,
            ConnectionState::AwaitingNewName,
            "That name is not permitted.");
    }
        // Check if the player name has already been used.
    else if (SQLiteDbms::instance().searchPlayer(ToCapitals(input)))
    {
        AdvanceCharacterCreation(
            character,
            ConnectionState::AwaitingNewName,
            "That player already exists, please choose another name.");
    }
    else
    {
        Room * spawnRoom = Mud::instance().findRoom(player->rent_room);
        if (spawnRoom == nullptr)
        {
            spawnRoom = Mud::instance().findRoom(1000);
        }
        player->name = ToCapitals(input);
        player->room = spawnRoom;
        player->faction = Mud::instance().findFaction(1);
        player->prompt_save = "[" + player->name + "]";
        player->password_attempts = 0;
        AdvanceCharacterCreation(player, ConnectionState::AwaitingNewPwd);
    }
}
