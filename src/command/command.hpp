/// @file   command.hpp
/// @brief  Define the methods used by the character in order to execute commands.
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

#pragma once

#include "../character/character.hpp"
#include "argumentHandler.hpp"

/// @brief An action handler for the character.
using ActionHandler = std::function<void(Character * character, ArgumentHandler & args)>;

/// @brief Contains all the informations concerning a command, including its handler.
class Command
{
public:
    /// @brief Constructor.
    Command();

    /// @brief Create a complete structure for a command.
    /// @param _level The level necessary to execute the command.
    /// @param _name  The name of the command.
    /// @param _help  The help message of the command.
    /// @param _args  The arguments of the command.
    /// @param _hndl  The handler of the command.
    Command(
        int _level,
        std::string _name,
        std::string _help,
        std::string _args,
        ActionHandler _hndl);

    /// @brief Checks if the provided character can use the command.
    /// @return <b>True</b> if the character can use the command,<br>
    ///         <b>False</b> otherwise.
    bool canUse(Character * character) const;

    /// The level of the command.
    int level;
    /// The name of the command.
    std::string name;
    /// The help message of the command.
    std::string help;
    /// The arguemtns of the command.
    std::string args;
    /// The handler of the command.
    ActionHandler hndl;
};

/// @defgroup ProcessStates Player state processing.
/// @brief All the functions necessary to process players commands, from creation to gameplay.
/// @{

/// @brief Process commands when character is connected.
/// @param character The character that execute the command.
/// @param args  Command arguments.
void ProcessCommand(Character * character, ArgumentHandler & args);

/// Check player name.
void ProcessPlayerName(Character * character, ArgumentHandler & args);

/// Check if the player password is correct.
void ProcessPlayerPassword(Character * character, ArgumentHandler & args);

/// Step 1  - Choose the Name.
void ProcessNewName(Character * character, ArgumentHandler & args);

/// Step 2  - Choose the Password.
void ProcessNewPwd(Character * character, ArgumentHandler & args);

/// Step 3  - Confirm the Password.
void ProcessNewPwdCon(Character * character, ArgumentHandler & args);

/// Step 4  - Short story of the mud world.
void ProcessNewStory(Character * character, ArgumentHandler & args);

/// Step 5  - Choose the Race.
void ProcessNewRace(Character * character, ArgumentHandler & args);

/// Step 6  - Choose the Attributes.
void ProcessNewAttr(Character * character, ArgumentHandler & args);

/// Step 7  - Choose the Gender.
void ProcessNewGender(Character * character, ArgumentHandler & args);

/// Step 8  - Choose the Age.
void ProcessNewAge(Character * character, ArgumentHandler & args);

/// Step 9  - Choose the description (optional).
void ProcessNewDesc(Character * character, ArgumentHandler & args);

/// Step 10 - Choose the Weight.
void ProcessNewWeight(Character * character, ArgumentHandler & args);

/// Step 11 - Confirm the character.
void ProcessNewConfirm(Character * character, ArgumentHandler & args);
///@}

/// @brief Check if the executer of this command is a player.
void NoMobile(Character * character);

/// @brief Stop any action the character is executing.
void StopAction(Character * character);

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
void AdvanceCharacterCreation(
    Character * character,
    ConnectionState con_state,
    std::string message = "");

/// @brief Map all the command to the respective std::string that the character can type.
void LoadCommands();

