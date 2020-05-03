/// @file   general.cpp
/// @brief  Implements <b>general commands</b> used by the player.
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

#include "command/general.hpp"

#include "character/characterUtilities.hpp"
#include "utilities/formatter.hpp"
#include "utilities/namegen.hpp"
#include "mud.hpp"

bool DoQuit(Character *character, ArgumentHandler & /*args*/)
{
	// Check if the character is a mobile.
	if (character->isMobile()) {
		character->sendMsg("Npcs are not allowed to execute this command.\n");
		return false;
	}
	// Stop any ongoing action.
	StopAction(character);
	auto player = character->toPlayer();
	// If s/he finished connecting, tell others s/he has left.
	if (player->connectionState == ConnectionState::Playing) {
		// Say goodbye to player.
		player->sendMsg("See you next time!\n");
		if (player->room != nullptr) {
			// Send the message inside the room.
			player->room->sendToAll("Player %s disappear in a puff of smoke.\n", { character },
									player->getName());
		}
		MudLog(LogLevel::Global, "Player %s has left the game.", player->getName());
		// End of properly connected.
		player->closeConnection();
		return true;
	}
	return false;
}

bool DoWho(Character *character, ArgumentHandler & /*args*/)
{
	Table table = Table();
	table.addColumn("Player", align::left);
	table.addColumn("Location", align::left);
	std::string output;
	size_t total = 0;
	for (auto iterator : Mud::instance().mudPlayers) {
		// If the player is not playing, continue.
		if (!iterator->isPlaying())
			continue;
		table.addRow({ iterator->getName(),
					   (iterator->room != nullptr) ? iterator->room->name : "Nowhere" });
		++total;
	}
	character->sendMsg(table.getTable());
	character->sendMsg("# Total %s: %s\n", Formatter::yellow("Players"), total);
	return true;
}

bool DoSet(Character *character, ArgumentHandler &args)
{
	if (args.empty()) {
		character->sendMsg("What do you want to set?");
	} else {
		if (args[0].getContent() == "des") {
			auto newDescription = args.substr(1);
			if (newDescription.empty()) {
				character->sendMsg("You can't set an empty description.\n");
			} else {
				character->description = newDescription;
				character->sendMsg("You had set your description:\n");
				character->sendMsg(character->description + "\n");
				return true;
			}
		}
	}
	character->sendMsg("Usage: set <opt> <arguments>  \n");
	character->sendMsg("       ---------------------  \n");
	character->sendMsg("            des  [description]\n");
	return false;
}

bool DoLook(Character *character, ArgumentHandler &args)
{
	// Check if the character is sleeping.
	if (character->posture == CharacterPosture::Sleep) {
		character->sendMsg("You can't see anything, you're sleeping.\n");
		return false;
	}
	// If there are no arguments, show the room.
	if (args.empty()) {
		character->sendMsg(character->room->getLook(character));
		return true;
	}
	// The accepted number of arguemnts is 0, 1 or 2.
	if (args.size() > 2) {
		character->sendMsg("You don't remember how to look?\n");
		return false;
	}
	// If only one argument is provided, it wants to look at something.
	if (args.size() == 1) {
		// Store the index inside a variable.
		auto target_index = args[0].getIndex();
		// Check if the room is lit by a light. In that case, maybe the
		// player is looking at someone.
		if (character->room->isLit()) {
			// Check if the character is looking at another character.
			auto target = character->room->findCharacter(args[0].getContent(), target_index,
														 &target_index, { character });
			if (target) {
				if (character->canSee(target)) {
					// Show the target.
					character->sendMsg(target->getLook());
					// Notify the target.
					if (target->canSee(character)) {
						// Notify to other character, that this one are
						// looking at him.
						target->sendMsg("%s looks at you.\n\n", character->getNameCapital());
					}
					return true;
				}
			}
		}
		// Otherwise, check if the character is looking at an item.
		auto options = SearchOptionsCharacter::allOptions();
		options.randomIfNoLight = false;
		auto item =
			FindNearbyItem(character, args[0].getContent(), target_index, &target_index, options);
		if (item) {
			character->sendMsg(item->getLook());
			return true;
		}
		character->sendMsg("You don't see '%s' anywhere.\n", args[0].getContent());
	} else if (args.size() == 2) {
		// Check if the room is lit by a light. In that case, maybe the
		// player is looking at the equipment of someone.
		if (character->room->isLit()) {
			auto target = character->room->findCharacter(args[1].getContent(), args[1].getIndex(),
														 nullptr, { character });
			if (target != nullptr) {
				if (character->canSee(target)) {
					auto item = target->findEquipmentItem(args[0].getContent(), args[0].getIndex());
					if (item != nullptr) {
						character->sendMsg(item->getLook());
						return true;
					}
					character->sendMsg("You don't see %s on %s.\n", args[0].getContent(),
									   target->getName());
					return false;
				}
				character->sendMsg("You don't see the container '%s' anywhere.\n",
								   args[1].getContent());
				return false;
			}
		}
		// Otherwise, check if the character is looking at an item inside a
		// container.
		auto options = SearchOptionsCharacter::allOptions();
		options.randomIfNoLight = false;
		auto container =
			FindNearbyItem(character, args[1].getContent(), args[1].getIndex(), nullptr, options);
		if (container == nullptr) {
			character->sendMsg("You don't see the container '%s' anywhere.\n",
							   args[1].getContent());
			return false;
		}
		if (!container->isAContainer()) {
			character->sendMsg("%s is not a container.\n", container->getNameCapital(true));
			return false;
		}
		auto item = container->findContent(args[0].getContent(), args[0].getIndex());
		if (item) {
			character->sendMsg(item->getLook());
			return true;
		}
		character->sendMsg("It's not inside %s.\n", container->getName(true));
	}
	return false;
}

bool DoHelp(Character *character, ArgumentHandler &args)
{
	if (args.empty()) {
		size_t numColumns = 4;
		TableRow baseRow, godsRow;
		Table baseCommands, godsCommands;
		baseCommands.addHeader("Commands");
		godsCommands.addHeader("Management");
		for (size_t it = 0; it < numColumns; ++it) {
			baseCommands.addColumn("", align::left, 15);
			godsCommands.addColumn("", align::left, 15);
		}
		for (auto const &it : Mud::instance().mudCommands) {
			if (it->canUse(character)) {
				if (it->gods) {
					godsRow.emplace_back(it->name);
					if (godsRow.size() == numColumns) {
						godsCommands.addRow(godsRow);
						godsRow.clear();
					}
				} else {
					baseRow.emplace_back(it->name);
					if (baseRow.size() == numColumns) {
						baseCommands.addRow(baseRow);
						baseRow.clear();
					}
				}
			}
		}
		if (!godsRow.empty()) {
			for (auto it = godsRow.size(); it < 4; ++it) {
				godsRow.emplace_back("");
			}
			godsCommands.addRow(godsRow);
		}
		if (!baseRow.empty()) {
			for (auto it = baseRow.size(); it < 4; ++it) {
				baseRow.emplace_back("");
			}
			baseCommands.addRow(baseRow);
		}
		character->sendMsg(baseCommands.getTable());
		character->sendMsg(godsCommands.getTable());
	} else if (args.size() == 1) {
		auto command = args[0].getContent();
		for (auto it : Mud::instance().mudCommands) {
			if (it->canUse(character)) {
				if (BeginWith(ToLower(it->name), command)) {
					std::string msg;
					msg += "Showing help for command :" + it->name + "\n";
					msg += Formatter::yellow() + " Command   : " + Formatter::reset() + it->name +
						   "\n";
					msg += Formatter::yellow() + " Level     : " + Formatter::reset() +
						   ToString(it->gods) + "\n";
					msg += Formatter::yellow() + " Arguments : " + Formatter::reset() +
						   it->arguments + "\n";
					msg += Formatter::yellow() + " Help      : " + Formatter::reset() + it->help +
						   "\n";
					character->sendMsg(msg);
					return true;
				}
			}
		}
		character->sendMsg("There is no help for '%s'.\n", args.getOriginal());
		return false;
	}
	return true;
}

bool DoPrompt(Character *character, ArgumentHandler &args)
{
	// Check if the character is a mobile.
	if (character->isMobile()) {
		character->sendMsg("Npcs are not allowed to execute this command.\n");
		return false;
	}
	auto player = character->toPlayer();
	if (args.empty()) {
		character->sendMsg("Current prompt:\n");
		character->sendMsg("    %s\n", player->prompt);
		character->sendMsg("Type %sprompt help %sto read the guide.\n", Formatter::yellow(),
						   Formatter::reset());
	} else {
		if (args[0].getContent() == "help") {
			auto yellow = [](std::string s) {
				return Formatter::yellow() + s + Formatter::reset();
			};
			auto italic = [](std::string s) {
				return Formatter::italic() + s + Formatter::reset();
			};
			std::string msg;
			msg += yellow("Prompt Help") + "\n";
			msg += "You can set the prompt you prefer, respectfully to this constraints:\n";
			msg += " - Not more than 15 characters.\n";
			msg += "\n";
			msg += "You can use the following shortcuts in you prompt:\n";
			msg += "    " + italic("&n") + " - Player name.\n";
			msg += "    " + italic("&N") + " - Player name capitalized.\n";
			msg += "    " + italic("&h") + " - Player current health.\n";
			msg += "    " + italic("&H") + " - Player maximum health.\n";
			msg += "    " + italic("&s") + " - Player current stamina.\n";
			msg += "    " + italic("&S") + " - Player maximum stamina.\n";
			msg += "    " + italic("&T") + " - Currently aimed character.\n";
			player->sendMsg(msg);
		} else {
			player->prompt = args.substr(0);
		}
	}
	return true;
}

bool DoTime(Character *character, ArgumentHandler & /*args*/)
{
	// Check if the character is sleeping.
	if (character->posture == CharacterPosture::Sleep) {
		character->sendMsg("You can't see anything, you're sleeping.\n");
		return false;
	}
	if (MudUpdater::instance().getDayPhase() == DayPhase::Morning) {
		character->sendMsg("%sThe sun has just risen.%s\n", Formatter::yellow(),
						   Formatter::reset());
	} else if (MudUpdater::instance().getDayPhase() == DayPhase::Day) {
		character->sendMsg("%sThe sun is high in the sky.%s\n", Formatter::yellow(),
						   Formatter::reset());
	} else if (MudUpdater::instance().getDayPhase() == DayPhase::Dusk) {
		character->sendMsg("%sThe sun is setting, the shadows begin to prevail.%s\n",
						   Formatter::cyan(), Formatter::reset());
	} else if (MudUpdater::instance().getDayPhase() == DayPhase::Night) {
		character->sendMsg("%sThe darkness surrounds you.%s\n", Formatter::blue(),
						   Formatter::reset());
	}
	return true;
}

bool DoStatistics(Character *character, ArgumentHandler & /*args*/)
{
	// Check if the character is a mobile.
	if (character->isMobile()) {
		character->sendMsg("Npcs are not allowed to execute this command.\n");
		return false;
	}
	auto player = character->toPlayer();
	std::stringstream ss;
	ss << Formatter::magenta("Name        : ") << player->getName() << " ";
	ss << Formatter::magenta("Gender : ") << GetGenderTypeName(player->gender) << " ";
	ss << Formatter::magenta("Race : ") << player->race->name << "\n";
	ss << Formatter::magenta("Weight      : ") << player->weight << " ";
	ss << Mud::instance().getWeightMeasure() << "\n";
	ss << Formatter::magenta("Affiliation : ") << player->faction->name << "\n";
	ss << Formatter::magenta("Experience  : ") << player->experience << " px\n";
	// Add the abilities.
	// STRENGTH
	for (auto const &ability : player->abilities) {
		ss << "    " << Formatter::magenta(ability.first.getAbbreviation()) << "    " << std::right
		   << std::setw(5) << player->getAbility(ability.first) << "(" << std::right << std::setw(3)
		   << player->getAbilityModifier(ability.first) << ")\n";
	}
	// HEALTH
	ss << "    " << Formatter::magenta("Health  ");
	ss << player->health << "/" << player->getMaxHealth() << "\n";
	ss << "    " << Formatter::magenta("Stamina ");
	ss << player->stamina << "/" << player->getMaxStamina() << "\n";
	// Add the Armor Class.
	ss << "    " << Formatter::magenta("Armor Class ") << player->getArmorClass() << "\n";
	ss << "    " << Formatter::magenta("Parry Chance ") << player->getParryChace() << "\n";
	// Add the health and stamina conditions.
	ss << "You " << Formatter::bold(player->getHealthCondition(true)) << ".\n";
	ss << "You " << Formatter::bold(player->getStaminaCondition()) << ".\n";
	// Add the hunger and thirst conditions.
	ss << "You " << Formatter::bold(player->getHungerCondition()) << ".\n";
	ss << "You " << Formatter::bold(player->getThirstCondition()) << ".\n";
	// Add the posture.
	ss << "You are " << Formatter::bold(player->posture.getAction()) << ".\n\n";
	// [IF EXIST] Add the current action.
	if (player->getAction() != ActionType::Wait) {
		ss << "You are " << Formatter::bold(player->getAction()->getDescription()) << ".\n";
	}
	// [IF EXIST] Add the current opponent.
	if (player->combatHandler.getPredefinedTarget() != nullptr) {
		ss << "You are fighting with "
		   << Formatter::bold(player->combatHandler.getPredefinedTarget()->getName()) << ".\n";
	}
	// [IF EXIST] Add the current aimed opponent.
	if (player->combatHandler.getAimedTarget() != nullptr) {
		ss << "You are aiming at "
		   << Formatter::bold(player->combatHandler.getAimedTarget()->getName()) << ".\n";
	}
	player->sendMsg(ss.str());
	return true;
}

bool DoEffects(Character *character, ArgumentHandler &)
{
	std::string msg;
	msg += "Active Effects\n";
	for (auto const &it : character->effectManager.getActiveEffects()) {
		msg += "\t" + Align(it.name, align::left, 30) + "\n";
	}
	msg += "Ability Modifiers\n";
	for (auto const &it : character->effectManager.getAbilityMod()) {
		msg += "\t";
		msg += Align(it.first.getAbbreviation(), align::left, 30);
		msg += Align(it.second, align::right, 5) + "\n";
	}
	msg += "Combat Modifiers\n";
	for (auto const &it : character->effectManager.getCombatMod()) {
		msg += "\t";
		msg += Align(it.first.toString(), align::left, 30);
		msg += Align(it.second, align::right, 5) + "\n";
	}
	msg += "Status Modifiers\n";
	for (auto const &it : character->effectManager.getStatusMod()) {
		msg += "\t";
		msg += Align(it.first.toString(), align::left, 30);
		msg += Align(it.second, align::right, 5) + "\n";
	}
	msg += "Knowledges\n";
	for (auto const &it : character->effectManager.getKnowledge()) {
		msg += "\t";
		msg += Align(it.first.toString(), align::left, 30);
		msg += Align(it.second, align::right, 5) + "\n";
	}
	character->sendMsg(msg);
	return true;
}

bool DoRent(Character *character, ArgumentHandler & /*args*/)
{
	// Check if the character is a mobile.
	if (character->isMobile()) {
		character->sendMsg("Npcs are not allowed to execute this command.\n");
		return false;
	}
	// Check if the character is sleeping.
	if (character->posture == CharacterPosture::Sleep) {
		character->sendMsg("You can't see anything, you're sleeping.\n");
		return false;
	}
	auto player = character->toPlayer();
	// Check if the room allow to rent.
	if (!HasFlag(player->room->flags, RoomFlag::Rent)) {
		character->sendMsg("You can't rent here.\n");
		return false;
	} else {
		player->rent_room = player->room->vnum;
		player->doCommand("quit");
	}
	return true;
}

bool DoSkills(Character *character, ArgumentHandler & /*args*/)
{
	// Check if the character is a mobile.
	if (character->isMobile()) {
		character->sendMsg("Npcs are not allowed to execute this command.\n");
		return false;
	}
	auto player = character->toPlayer();
	Table table = Table();
	table.addColumn("Skill", align::left);
	table.addColumn("Level", align::left);
	for (const auto &skillData : player->skillManager.skills) {
		table.addRow({ skillData->skill->name, skillData->getSkillRank().toString() });
	}
	character->sendMsg(table.getTable());
	return true;
}

bool DoActions(Character *character, ArgumentHandler & /*args*/)
{
	std::string msg = "You are planning to...\n";
	character->actionQueueMutex.lock();
	bool first = true;
	for (auto it : character->actionQueue) {
		if (first) {
			msg += "  [Performing] ";
			first = false;
		} else
			msg += "  [Queued    ] ";
		msg += it->getDescription() + "\n";
	}
	character->actionQueueMutex.unlock();
	character->sendMsg(msg);
	return true;
}

bool DoServer(Character *character, ArgumentHandler & /*args*/)
{
	std::string msg;
	msg += "Mud         : RadMud.\n";
	msg += "Version     : ";
	msg += ToString(RADMUD_MAJOR_VERSION) + ".";
	msg += ToString(RADMUD_MINOR_VERSION) + ".";
	msg += ToString(RADMUD_VERSION) + "\n";
	msg += "    Uptime      : ";
	msg += ToString(Mud::instance().getUpTime()) + " s\n";
	msg += "    Players     : ";
	msg += ToString(Mud::instance().mudPlayers.size()) + "\n";
	msg += "    Mobiles     : ";
	msg += ToString(Mud::instance().mudMobiles.size()) + "\n";
	msg += "    Models      : ";
	msg += ToString(Mud::instance().mudItemModels.size()) + "\n";
	msg += "    Items       : ";
	msg += ToString(Mud::instance().mudItems.size()) + "\n";
	msg += "    Corpses     : ";
	msg += ToString(Mud::instance().mudCorpses.size()) + "\n";
	msg += "    Areas       : ";
	msg += ToString(Mud::instance().mudAreas.size()) + "\n";
	msg += "    Rooms       : ";
	msg += ToString(Mud::instance().mudRooms.size()) + "\n";
	msg += "    Races       : ";
	msg += ToString(Mud::instance().mudRaces.size()) + "\n";
	msg += "    Factions    : ";
	msg += ToString(Mud::instance().mudFactions.size()) + "\n";
	msg += "    Skills      : ";
	msg += ToString(Mud::instance().mudSkills.size()) + "\n";
	msg += "    Writings    : ";
	msg += ToString(Mud::instance().mudWritings.size()) + "\n";
	msg += "    Materials   : ";
	msg += ToString(Mud::instance().mudMaterials.size()) + "\n";
	msg += "    Professions : ";
	msg += ToString(Mud::instance().mudProfessions.size()) + "\n";
	msg += "    Productions : ";
	msg += ToString(Mud::instance().mudProductions.size()) + "\n";
	msg += "    Schematics  : ";
	msg += ToString(Mud::instance().mudBuildings.size()) + "\n";
	msg += "    Liquids     : ";
	msg += ToString(Mud::instance().mudLiquids.size()) + "\n";
	msg += "    News        : ";
	msg += ToString(Mud::instance().mudNews.size()) + "\n";
	msg += "    Commands    : ";
	msg += ToString(Mud::instance().mudCommands.size()) + "\n";
	character->sendMsg(msg);
	return true;
}

bool DoGenerateName(Character *character, ArgumentHandler &args)
{
	if (args.size() != 1) {
		std::string help;
		help += "You must provide a pattern.\n";
		help += "Guide\n";
		help += "\ts - generic syllable\n";
		help += "\tv - vowel\n";
		help += "\tV - vowel or vowel combination\n";
		help += "\tc - consonant\n";
		help += "\tB - consonant or consonant combination suitable for beginning a word\n";
		help += "\tC - consonant or consonant combination suitable anywhere in a word\n";
		help += "\ti - insult\n";
		help += "\tm - mushy name\n";
		help += "\tM - mushy name ending\n";
		help += "\tD - consonant suited for a stupid person's name\n";
		help += "\td - syllable suited for a stupid person's name (begins with a vowel)\n";
		help += "\n";
		help += "All characters between parenthesis () are emitted literally.\n";
		help += "  For example, the pattern 's(dim)', emits a random generic\n";
		help += "  syllable followed by 'dim'.\n";
		help += "Characters between angle brackets <> emit patterns from\n";
		help += "  the table above. Imagine the entire pattern is wrapped in\n";
		help += "  one of these.\n";
		help += "In both types of groupings, a vertical bar | denotes a\n";
		help += "  random choice. Empty groups are allowed.\n";
		help += "  For example, '(foo|bar)' emits either 'foo' or 'bar'.\n";
		help += "  The pattern '<c|v|>' emits a constant, vowel, or\n";
		help += "  nothing at all.\n";
		help += "An exclamation point ! means to capitalize the component \n";
		help += "  that follows it. For example, '!(foo)' will emit 'Foo'\n";
		help += "  and 'v!s' will emit a lowercase vowel followed by a\n";
		help += "  capitalized syllable, like 'eRod'.\n";
		help += "A tilde ~ means to reverse the letters of the component\n";
		help += "  that follows it. For example, '~(foo)' will emit 'oof'.\n";
		help += "  To reverse an entire template, wrap it in brackets.\n";
		help += "  For example, to reverse 'sV'i' as a whole use '~<sV'i>'.\n";
		help += "  The template '~sV'i' will only reverse the initial\n";
		help += "  syllable.\n";
		character->sendMsg(help + "\n");
		return false;
	}
	namegen::NameGenerator nameGenerator(args[0].getContent());
	character->sendMsg("Combinations : %s\n", nameGenerator.combinations());
	character->sendMsg("Names : %s\n\n", nameGenerator.toString());
	return true;
}
