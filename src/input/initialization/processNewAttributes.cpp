/// @file   processNewAttributes.cpp
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

#include "processNewAttributes.hpp"
#include "processNewGender.hpp"
#include "processNewRace.hpp"
#include "formatter.hpp"
#include "player.hpp"
#include "mud.hpp"

bool ProcessNewAttributes::process(ArgumentHandler &args)
{
	auto player = character->toPlayer();
	if ((args.size() != 1) && (args.size() != 2)) {
		error = "Invalid input.";
		this->advance();
		return false;
	}
	if (ToLower(args[0].getContent()) == "back") {
		// Create a shared pointer to the previous step.
		auto newStep = std::make_shared<ProcessNewRace>(character);
		// Set the handler.
		player->inputProcessor = newStep;
		// Advance to the next step.
		newStep->rollBack();
		return true;
	}
	if (ToLower(args[0].getContent()) == "reset") {
		player->remaining_points = 0;
		for (auto &ability : player->abilities) {
			player->setAbility(ability.first,
							   player->race->getAbility(ability.first));
		}
		error = Formatter::cyan("Attribute has been set by default.\n");
		this->advance();
		return true;
	}
	if (BeginWith(ToLower(args[0].getContent()), "continue")) {
		// Create a shared pointer to the next step.
		auto newStep = std::make_shared<ProcessNewGender>(character);
		// Set the handler.
		player->inputProcessor = newStep;
		// Advance to the next step.
		newStep->advance();
		return true;
	}
	if (BeginWith(ToLower(args[0].getContent()), "help")) {
		if (args.size() != 2) {
			error = "Specify the attribute number.";
			this->advance();
		}
		// Get the ability number.
		auto abilityNumber = ToNumber<unsigned int>(args[1].getContent());
		// Get and check the ability.
		Ability ability = Ability(abilityNumber);
		if (ability == Ability::None) {
			error = "Must select a valid attribute.";
			this->advance();
			return false;
		}
		error = "Help about " + ability.toString() + ".\n";
		error += Formatter::italic() + ability.getDescription() + "\n";
		this->advance();
		return true;
	} else {
		if (args.size() == 2) {
			// Get the ability number.
			auto abilityNumber = ToNumber<unsigned int>(args[0].getContent());
			// Get and check the ability.
			Ability ability = Ability(abilityNumber);
			if (ability == Ability::None) {
				error = "Must select a valid attribute.";
				this->advance();
				return false;
			}
			// Get the modifier.
			int modifier = ToNumber<int>(args[1].getContent());
			// Check for errors.
			if (player->remaining_points < modifier) {
				error = "You don't have enough points left.";
				this->advance();
				return false;
			}
			// Create an help string.
			std::string help;
			// Get the base value.
			auto base = static_cast<int>(player->getAbility(ability, false));
			// Evaluate the result.
			auto result = base + modifier;
			// Evaluate the upper and lower bounds.
			int upperBound = base + 5;
			int lowerBound = ((base - 5) > 0) ? (base - 5) : 0;
			// Check if the result is inside the boundaries.
			if (result < lowerBound) {
				error = ability.toString() + " can't go below " +
						ToString(lowerBound) + ".";
				this->advance();
				return false;
			} else if (result > upperBound) {
				error = ability.toString() + " can't go above " +
						ToString(upperBound) + ".";
				this->advance();
			}
			// Decrease the remaining points.
			player->remaining_points -= modifier;
			// Set the new ability value.
			player->setAbility(ability, static_cast<unsigned int>(result));
			return true;
		} else {
			error = "Type [#attribute +/-(value)].";
			this->advance();
		}
	}
	return false;
}

void ProcessNewAttributes::advance()
{
	// Transform the character into player.
	auto player = character->toPlayer();
	// Print the choices.
	this->printChoices();

	std::stringstream ss;
	ss << "# " << Formatter::bold("Character's Attributes.") << "\n";
	for (auto const &ability : player->abilities) {
		ss << "#    [" << ToString(ability.first.toUInt()) << "]";
		ss << Align(ability.first.toString(), align::left, 15);
		ss << " : ";
		ss << ToString(player->getAbility(Ability::Strength, false));
		ss << "\n";
	}
	ss << "#\n";
	ss << "# Remaining Points: ";
	ss << ((player->remaining_points > 0) ? Formatter::green() :
											Formatter::red());
	ss << player->remaining_points << Formatter::reset() << "\n";
	ss << "#\n";
	ss << "# Type [" << Formatter::magenta("(number)") << "]"
	   << "[" << Formatter::magenta("+/-modifier") << "]"
	   << " to decrease or increase the value of an attribute.\n";

	ss << "# Type [" << Formatter::magenta("help (number)") << "]"
	   << " to read a brief description of the attribute.\n";

	ss << "# Type [" << Formatter::magenta("reset") << "]"
	   << " to reset the values as default.\n";

	ss << "# Type [" << Formatter::magenta("continue") << "]"
	   << " to continue character creation.\n";

	ss << "# Type [" << Formatter::magenta("back") << "]"
	   << " to return to the previous step.\n";
	character->sendMsg(ss.str());
	this->printError();
}

void ProcessNewAttributes::rollBack()
{
	auto player = character->toPlayer();
	for (auto &it : player->abilities)
		player->setAbility(it.first, player->race->getAbility(it.first));
	this->advance();
}
