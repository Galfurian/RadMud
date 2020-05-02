/// @file   communication.cpp
/// @brief  Implements the methods used by the player in order to <b>communicate</b>.
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

#include "command/communication.hpp"

#include "utilities/formatter.hpp"
#include "command/command.hpp"
#include "mud.hpp"

void LoadCommunicationCommands()
{
	Mud::instance().addCommand(std::make_shared<Command>(DoSay, "say", "[someone] (something)",
														 "Talk to people in the current room.",
														 false, true, false));
	Mud::instance().addCommand(
		std::make_shared<Command>(DoWhisper, "whisper", "(someone) (something)",
								  "Whisper secretly to a single character.", false, true, false));
	Mud::instance().addCommand(std::make_shared<Command>(
		DoEmote, "emote", "(emotion)", "Express an emotion.", false, true, false));
	Mud::instance().addCommand(std::make_shared<Command>(
		DoBug, "bug", "(message)",
		"Report a bug, your character's name, location and date will be saved.", false, true,
		false));
	Mud::instance().addCommand(std::make_shared<Command>(
		DoIdea, "idea", "(message)", "Send an idea, try to be as clear as possible.", false, true,
		false));
	Mud::instance().addCommand(std::make_shared<Command>(DoTypo, "typo", "(message)",
														 "Report a typo.", false, true, false));
}

bool DoSay(Character *character, ArgumentHandler &args)
{
	// Check if the character is sleeping.
	if (character->posture == CharacterPosture::Sleep) {
		character->sendMsg("Not while you're sleeping.\n");
		return false;
	}
	if (args.empty()) {
		character->sendMsg("My dear friend, say what?\n");
		return false;
	}
	// Check if the character are talking to another character.
	auto receiver = character->room->findCharacter(args.get(0).getContent(), args.get(0).getIndex(),
												   nullptr, { character });
	if (receiver != nullptr) {
		// Get the rest of the message, minus the first word.
		auto message = args.substr(1);
		if (message.empty()) {
			character->sendMsg("My dear friend, say what to %s?\n", receiver->getName());
			return false;
		}
		// Eat the space between the name and the message.
		message = Trim(message, " ");
		// Player send.
		character->sendMsg("You say to %s, \"%s\"\n", receiver->getName(),
						   Formatter::cyan() + Formatter::italic() + message + Formatter::reset());
		// Target receive.
		receiver->sendMsg("%s say to you, \"%s\"\n\n", character->getNameCapital(),
						  Formatter::cyan() + Formatter::italic() + message + Formatter::reset());
		// Send the message inside the room.
		character->room->sendToAll("%s says to %s, \"%s\".\n", { character, receiver },
								   character->getNameCapital(), receiver->getName(),
								   Formatter::cyan() + Formatter::italic() + message +
									   Formatter::reset());
		// If it's a mobile, activate the trigger.
		if (receiver->isMobile()) {
			receiver->toMobile()->triggerEventMessage(character, message);
		}
	} else {
		character->sendMsg("You say \"%s\".\n", Formatter::cyan() + Formatter::italic() +
													args.getOriginal() + Formatter::reset());
		// Send the message inside the room.
		character->room->sendToAll("%s says \"%s\".\n", { character }, character->getNameCapital(),
								   Formatter::cyan() + Formatter::italic() + args.getOriginal() +
									   Formatter::reset());
	}
	return true;
}

bool DoWhisper(Character *character, ArgumentHandler &args)
{
	// Check if the character is sleeping.
	if (character->posture == CharacterPosture::Sleep) {
		character->sendMsg("Not while you're sleeping.\n");
		return false;
	}
	if (args.empty()) {
		character->sendMsg("Whisper to whom?\n");
		return false;
	}
	// Check the existence of the target character.
	auto receiver = character->room->findCharacter(args[0].getContent(), args[0].getIndex(),
												   nullptr, { character });
	if (receiver == nullptr) {
		character->sendMsg("You don't see %s here.\n", args[0].getContent());
		return false;
	}
	// Retrieve the message.
	auto message = args.substr(1);
	if (message.empty()) {
		character->sendMsg("What do you want to whisper to %s?\n", receiver->getName());
		return false;
	}
	// Check if the sender is invisible.
	auto sender = (receiver->canSee(character)) ? "Someone" : character->getNameCapital();
	// Send the message.
	character->sendMsg("%sYou whisper to %s, %s\"%s\".\n", Formatter::magenta(),
					   receiver->getName(), Formatter::reset(), message);
	receiver->sendMsg("%s whisper to you, %s\"%s\"\n\n", Formatter::magenta() + sender,
					  Formatter::reset(), message);
	return true;
}

bool DoEmote(Character *character, ArgumentHandler &args)
{
	// Check if the character is sleeping.
	if (character->posture == CharacterPosture::Sleep) {
		character->sendMsg("Not while you're sleeping.\n");
		return false;
	}
	if (args.empty()) {
		character->sendMsg("My dear friend, emote what?\n");
		return false;
	}
	// Send the messages.
	character->sendMsg("%sYou %s\n", Formatter::yellow(), args.getOriginal() + Formatter::reset());
	character->room->sendToAll("%s %s\n", { character }, Formatter::yellow() + character->getName(),
							   args.getOriginal() + Formatter::reset());
	return true;
}

bool DoBug(Character *character, ArgumentHandler &args)
{
	if (args.empty()) {
		character->sendMsg("Writing nothing won't help us!\n");
		return false;
	}
	std::vector<std::string> arguments;
	arguments.emplace_back(character->getName()); // Author
	arguments.emplace_back(GetDate()); // Date.
	arguments.emplace_back("0"); // Type
	arguments.emplace_back(ToString(character->room->vnum)); // Location
	arguments.emplace_back(args.getOriginal());
	SQLiteDbms::instance().beginTransaction();
	if (!SQLiteDbms::instance().insertInto("Board", arguments)) {
		character->sendMsg("Something gone wrong while reporting your bug.\n");
		SQLiteDbms::instance().rollbackTransection();
		return false;
	}
	SQLiteDbms::instance().endTransaction();
	character->sendMsg("Bug posted on Board correctly.\n");
	character->sendMsg("# Author   :%s\n", character->getName());
	character->sendMsg("# Date     :%s\n", GetDate());
	character->sendMsg("# Location :%s\n", character->room->name);
	character->sendMsg("# Message  :%s\n", args.getOriginal());
	return true;
}

bool DoIdea(Character *character, ArgumentHandler &args)
{
	if (args.empty()) {
		character->sendMsg("Writing nothing won't help us!\n");
		return false;
	}
	std::vector<std::string> arguments;
	arguments.emplace_back(character->getName()); // Author
	arguments.emplace_back(GetDate()); // Date.
	arguments.emplace_back("1"); // Type
	arguments.emplace_back(ToString(character->room->vnum)); // Location
	arguments.emplace_back(args.getOriginal());
	SQLiteDbms::instance().beginTransaction();
	if (!SQLiteDbms::instance().insertInto("Board", arguments)) {
		character->sendMsg("Something gone wrong while reporting your idea.\n");
		SQLiteDbms::instance().rollbackTransection();
		return false;
	}
	SQLiteDbms::instance().endTransaction();
	character->sendMsg("Idea posted on Board correctly.\n");
	character->sendMsg("# Author   :%s\n", character->getName());
	character->sendMsg("# Message  :%s\n", args.getOriginal());
	return true;
}

bool DoTypo(Character *character, ArgumentHandler &args)
{
	if (args.empty()) {
		character->sendMsg("Writing nothing won't help us!\n");
		return false;
	}
	std::vector<std::string> arguments;
	arguments.emplace_back(character->getName()); // Author
	arguments.emplace_back(GetDate()); // Date.
	arguments.emplace_back("2"); // Type
	arguments.emplace_back(ToString(character->room->vnum)); // Location
	arguments.emplace_back(args.getOriginal());

	SQLiteDbms::instance().beginTransaction();
	if (!SQLiteDbms::instance().insertInto("Board", arguments)) {
		character->sendMsg("Something gone wrong while reporting your Typo.\n");
		SQLiteDbms::instance().rollbackTransection();
		return false;
	}
	SQLiteDbms::instance().endTransaction();

	character->sendMsg("Typo posted on Board correctly.\n");
	character->sendMsg("# Author   :%s\n", character->getName());
	character->sendMsg("# Date     :%s\n", GetDate());
	character->sendMsg("# Location :%s\n", character->room->name);
	character->sendMsg("# Message  :%s\n", args.getOriginal());
	return true;
}
