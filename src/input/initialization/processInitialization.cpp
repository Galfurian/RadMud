/// @file   processInitialization.cpp
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

#include "input/initialization/processInitialization.hpp"
#include "character/character.hpp"
#include "utilities/formatter.hpp"
#include "character/player.hpp"

static inline void print_ability(std::stringstream &ss, Player *player, Ability const &ability)
{
	auto value = player->getAbility(ability, false);
	ss << "# " << std::setw(13) << std::left << ability.toString() << ":";
	if (value > 0)
		ss << value;
	ss << "\n";
}

void ProcessInitialization::printChoices()
{
	auto player = character->toPlayer();
	std::stringstream ss;
	ss << Formatter::clearScreen();
	ss << "# ------------ Character Creation ------------ #\n";
	// == NAME ================================================================
	ss << "# Name         :" << player->name << "\n";

	// == PASSWORD ============================================================
	for (size_t i = 0; i < player->password.size(); ++i)
		ss << "*";
	ss << "\n";

	// == RACE ================================================================
	ss << "# Race         :";
	if (player->race != nullptr)
		ss << player->race->name;
	ss << "\n";

	// == ABILITY =============================================================
	print_ability(ss, player, Ability::Strength);
	print_ability(ss, player, Ability::Agility);
	print_ability(ss, player, Ability::Perception);
	print_ability(ss, player, Ability::Constitution);
	print_ability(ss, player, Ability::Intelligence);

	// == GENDER ==============================================================
	ss << "# Gender       :" + GetGenderTypeName(player->gender) + "\n";

	// == AGE =================================================================
	ss << "# Age          :";
	if (player->age > 0)
		ss << player->age;
	ss << "\n";

	// == DESCRIPTION =========================================================
	ss << "# Description  :" << player->description << "\n";

	// == WEIGHT ==============================================================
	ss << "# Weight       :";
	if (player->weight > 0)
		ss << ToString(player->weight);
	ss << "\n";

	ss << "# -------------------------------------------- #\n";
	player->sendMsg(ss.str());
}

void ProcessInitialization::printError()
{
	if (!error.empty()) {
		character->sendMsg("# " + error + "\n");
		error.clear();
	}
}
