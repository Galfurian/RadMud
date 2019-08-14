/// @file   command.hpp
/// @brief  Define the methods used by the character in order to execute
///          commands.
/// @author Enrico Fraccaroli
/// @date   Aug 23 2014
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

#pragma once

#include "character.hpp"
#include "argumentHandler.hpp"

/// @brief Contains all the informations concerning a command,
///         including its handler.
class Command {
public:
	/// The handler of the command.
	std::function<bool(Character *character, ArgumentHandler &args)> handler;
	/// The name of the command.
	std::string name;
	/// The arguemtns of the command.
	std::string arguments;
	/// The help message of the command.
	std::string help;
	/// Flag which identifies if the command is for gods.
	bool gods;
	/// Flag which determines if the command can be used in combat.
	bool canUseInCombat;
	/// Flag which determines if the command must be typed completely.
	bool typedCompletely;

	/// @brief Constructor.
	Command();

	/// @brief Create a complete structure for a command.
	/// @param _handler         The handler of the command.
	/// @param _name            The name of the command.
	/// @param _arguments       The arguments of the command.
	/// @param _help            The help message of the command.
	/// @param _gods            The level necessary to execute the command.
	/// @param _canUseInCombat  The can_use_in_combat flag.
	/// @param _typedCompletely If the command must be typed completely.
	Command(const std::function<bool(Character *character,
									 ArgumentHandler &args)> &_handler,
			const std::string &_name, const std::string &_arguments,
			const std::string &_help, const bool &_gods,
			const bool &_canUseInCombat, const bool &_typedCompletely);

	/// @brief Checks if the provided character can use the command.
	/// @param character The character to check.
	/// @return <b>True</b> if the character can use the command,<br>
	///         <b>False</b> otherwise.
	bool canUse(Character *character) const;
};

/// @brief Stop any action the character is executing.
void StopAction(Character *character);

/// @brief Map all the command to the respective std::string that the
///         character can type.
void LoadCommands();
