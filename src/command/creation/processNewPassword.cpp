/// @file   processNewPassword.cpp
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

void ProcessNewPassword(Character * character, ArgumentHandler & args)
{
    Player * player = character->toPlayer();
    auto input = args.getOriginal();
    // Player_password can't be blank.
    if (input.empty())
    {
        AdvanceCharacterCreation(character, ConnectionState::AwaitingNewPwd, "Invalid input.");
    }
        // Check if the player has typed BACK.
    else if (ToLower(input) == "back")
    {
        RollbackCharacterCreation(player, ConnectionState::AwaitingNewName);
    }
        // Check if the player has given bad characters.
    else if (input.find_first_not_of(VALID_CHARACTERS_NAME) != std::string::npos)
    {
        AdvanceCharacterCreation(
            character,
            ConnectionState::AwaitingNewPwd,
            "Password cannot contain disallowed characters.");
    }
    else
    {
        player->password = input;
        AdvanceCharacterCreation(player, ConnectionState::AwaitingNewPwdCon);
    }
}
