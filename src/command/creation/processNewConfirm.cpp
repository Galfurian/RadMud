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

#include "creationStep.hpp"
#include "player.hpp"
#include "mud.hpp"

void ProcessNewConfirm(Character * character, ArgumentHandler & args)
{
    Player * player = character->toPlayer();
    auto input = args.getOriginal();
    // Check if the player has typed BACK.
    if (ToLower(input) == "back")
    {
        RollbackCharacterCreation(player, ConnectionState::AwaitingNewWeight);
    }
        // Check if the player has typed CONFIRM.
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
            AdvanceCharacterCreation(player, ConnectionState::Playing);
        }
        else
        {
            player->closeConnection();
        }
        SQLiteDbms::instance().endTransaction();
    }
    else
    {
        AdvanceCharacterCreation(
            character,
            ConnectionState::AwaitingNewConfirm,
            "You must write 'confirm' if you agree.");
    }
}
