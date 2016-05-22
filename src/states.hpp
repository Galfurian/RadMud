/// @file   states.hpp
/// @brief  Defines methods that allow to handle player states.
/// @author Enrico Fraccaroli
/// @date   Aug 23 2014
/// @copyright
/// Copyright (c) 2014, 2015, 2016 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
/// Permission to use, copy, modify, and distribute this software for any
/// purpose with or without fee is hereby granted, provided that the above
/// copyright notice and this permission notice appear in all copies.
///
/// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
/// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
/// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
/// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
/// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
/// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
/// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

#ifndef STATES_HPP
#define STATES_HPP

#include <stdexcept>
#include <fstream>
#include <iostream>
#include "player.hpp"

/// Handle the message to send if an error has occurred.
#define HADLE_ERROR()\
if (!message.empty()) {\
    msg+="# " + Telnet::red() + message + Telnet::reset() + "\n";\
}\
else{\
    msg+="#\n";\
}\


/// @brief Load all the possible player states.
void LoadStates();

/// @brief Print the values inserted until now.
/// @param character The player whose creating a new character.
void PrintChoices(Character * character);

/// @brief Reset the informations inserted in the previous state.
/// @param character The player whose creating a new character.
/// @param new_state The step reached by this player.
void RollbackCharacterCreation(Character * character, ConnectionState new_state);

/// @brief Print the advancement in the character creation.
/// @details If you want to introduce a new step, you have to insert it's text here.
/// @param character The player whose creating a new character.
/// @param con_state The step reached by this player.
/// @param message   An optional message used only during error handling.
void AdvanceCharacterCreation(Character * character, ConnectionState con_state, std::string message = "");

/// @defgroup ProcessStates Player state processing.
/// @brief All the functions necessary to process players commands, from creation to gameplay.
/// @{

/// Check player name.
void ProcessPlayerName(Character * character, std::istream & sArgs);
/// Check if the player password is correct.
void ProcessPlayerPassword(Character * character, std::istream & sArgs);
/// Step 1  - Choose the Name.
void ProcessNewName(Character * character, std::istream & sArgs);
/// Step 2  - Choose the Password.
void ProcessNewPwd(Character * character, std::istream & sArgs);
/// Step 3  - Confirm the Password.
void ProcessNewPwdCon(Character * character, std::istream & sArgs);
/// Step 4  - Short story of the mud world.
void ProcessNewStory(Character * character, std::istream & sArgs);
/// Step 5  - Choose the Race.
void ProcessNewRace(Character * character, std::istream & sArgs);
/// Step 6  - Choose the Attributes.
void ProcessNewAttr(Character * character, std::istream & sArgs);
/// Step 7  - Choose the Gender.
void ProcessNewGender(Character * character, std::istream & sArgs);
/// Step 8  - Choose the Age.
void ProcessNewAge(Character * character, std::istream & sArgs);
/// Step 9  - Choose the description (optional).
void ProcessNewDesc(Character * character, std::istream & sArgs);
/// Step 10 - Choose the Weight.
void ProcessNewWeight(Character * character, std::istream & sArgs);
/// Step 11 - Confirm the character.
void ProcessNewConfirm(Character * character, std::istream & sArgs);
///@}

#endif
