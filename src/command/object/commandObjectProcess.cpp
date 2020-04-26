/// @file   commandObjectProcess.cpp
/// @author Enrico Fraccaroli
/// @date   gen 12 2017
/// @copyright
/// Copyright (c) 2017 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
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

#include "command/object/commandObjectProcess.hpp"
#include "item/subitem/corpseItem.hpp"
#include "command/command.hpp"
#include "character/player.hpp"
#include "structure/room.hpp"
#include "action/object/dismemberAction.hpp"

bool DoDismember(Character *character, ArgumentHandler &args)
{
	// Check if the character is sleeping.
	if (character->posture == CharacterPosture::Sleep) {
		character->sendMsg("Not while you're sleeping.\n");
		return false;
	}
	// Stop any action the character is executing.
	StopAction(character);
	// If the character is a Player, check if the character has the required
	// knowledge.
	if (character->isPlayer()) {
		// Transform the character to player.
		auto player = character->toPlayer();
		// Check if the player can butcher animals.
		if (!player->effectManager.getKnowledge(Knowledge::Butchery)) {
			character->sendMsg("You don't know how to dismember corpses.\n");
			return false;
		}
	}
	// Check the arguments.
	if (args.size() != 1) {
		character->sendMsg("What do you want to dismember?\n");
		return false;
	}
	// Check if the room is NOT lit.
	if (!character->room->isLit()) {
		character->sendMsg("You cannot do that without light.\n");
		return false;
	}
	auto item =
		character->findNearbyItem(args[0].getContent(), args[0].getIndex());
	if (item == nullptr) {
		character->sendMsg("You cannot find '%s'.\n", args[0].getContent());
		return false;
	}
	if (item->getType() != ModelType::Corpse) {
		character->sendMsg("%s is not a corpse.\n", item->getNameCapital(true));
		return false;
	}
	// Cast the item to corpse.
	auto corpse = dynamic_cast<CorpseItem *>(item);
	// Get a valid body part.
	auto bodyPart = corpse->getAvailableBodyPart();
	if (bodyPart == nullptr) {
		character->sendMsg("%s does not contain anything useful.\n",
						   item->getNameCapital(true));
		return false;
	}
	auto act = std::make_shared<DismemberAction>(character, corpse, bodyPart);
	if (act->start()) {
		character->pushAction(act);
		return true;
	}
	return false;
}