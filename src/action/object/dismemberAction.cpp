/// @file   dismemberAction.cpp
/// @author Enrico Fraccaroli
/// @date   Jan 13 2017
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

#include "action/object/dismemberAction.hpp"
#include "model/submodel/resourceModel.hpp"
#include "item/subitem/corpseItem.hpp"
#include "character/character.hpp"
#include "structure/room.hpp"

#include <cassert>

DismemberAction::DismemberAction(Character *_actor, CorpseItem *_corpse,
								 const std::shared_ptr<BodyPart> &_bodyPart) :
	GeneralAction(_actor), corpse(_corpse), bodyPart(_bodyPart)
{
	// Debugging message.
	MudLog(LogLevel::Debug, "Created DismemberAction.");
	// Reset the cooldown of the action.
	this->resetCooldown(this->getCooldown());
}

DismemberAction::~DismemberAction()
{
	MudLog(LogLevel::Debug, "Deleted DismemberAction.");
}

bool DismemberAction::check(std::string &error) const
{
	if (!GeneralAction::check(error)) {
		return false;
	}
	if (corpse == nullptr) {
		error = "You cannot find the corpse you want to dismember.";
		return false;
	}
	if (bodyPart == nullptr) {
		error = "You were not able to dismember the corpse.";
		return false;
	}
	return true;
}

ActionType DismemberAction::getType() const
{
	return ActionType::Dismember;
}

std::string DismemberAction::getDescription() const
{
	return "dismembering";
}

bool DismemberAction::start()
{
	std::string error;
	if (!this->check(error)) {
		actor->sendMsg(error + "\n\n");
		return false;
	}
	actor->sendMsg("You start dismembering %s...\n", corpse->getName(true));
	actor->room->sendToAll("%s starts dismembering %s...\n", { actor }, actor->getNameCapital(),
						   corpse->getName(true));
	return true;
}

std::string DismemberAction::stop()
{
	return "You stop dismembering.";
}

ActionStatus DismemberAction::perform()
{
	std::string error;
	if (!this->check(error)) {
		actor->sendMsg(error + "\n\n");
		return ActionStatus::Error;
	}
	// First try to remove the body part.
	if (!corpse->removeBodyPart(bodyPart)) {
		actor->sendMsg("You fail to dismember %s.", corpse->getName(true));
		return ActionStatus::Error;
	}
	// Get the skill rank of the actor.
	auto butchery = actor->effectManager.getKnowledge(Knowledge::Butchery);
	// Create the resources of the given body part.
	bool dismemberedSomthing = false;
	for (auto const &resources : bodyPart->resources) {
		// Roll the DC and check if the action is a success. However, the
		// actor can keep all the things he has dismembered til now.
		auto DCRoll = TRand<int>(1, 20) + butchery;
		MudLog(LogLevel::Debug, "DC: %d vs %d", DCRoll, resources.difficulty);
		if (DCRoll < resources.difficulty) {
			actor->sendMsg("You stop to dismember %s.", corpse->getName(true));
			return ActionStatus::Error;
		}
		auto item = resources.resource->createItem(actor->getName(), resources.material, false,
												   ItemQuality::Normal,
												   static_cast<unsigned int>(resources.quantity));
		if (item == nullptr) {
			actor->sendMsg("You fail to dismember %s.", corpse->getName(true));
			return ActionStatus::Error;
		}
		actor->skillManager.improveKnowledge(Knowledge::Butchery);
		actor->sendMsg("You successfully butcher %s and produce %s.\n\n", corpse->getName(true),
					   item->getName(true));
		if (actor->canCarry(item, item->quantity)) {
			actor->addInventoryItem(item);
		} else if (actor->room != nullptr) {
			actor->sendMsg("%s has been placed on the ground.", corpse->getNameCapital(true));
			actor->room->addItem(item);
		} else {
			actor->sendMsg("You fail to dismember %s.", corpse->getName(true));
			return ActionStatus::Error;
		}
		dismemberedSomthing = true;
	}
	if (!dismemberedSomthing) {
		actor->sendMsg("You produced nothing from %s.", corpse->getName(true));
		return ActionStatus::Error;
	}
	return ActionStatus::Finished;
}

unsigned int DismemberAction::getCooldown()
{
	assert(actor && "Actor is nullptr");
	assert(corpse && "Corpse is nullptr");
	assert(bodyPart && "BodyPart is nullptr");
	double required = 6;
	MudLog(LogLevel::Debug, "Base time  : %f", required);
	required -= (required * actor->effectManager.getKnowledge(Knowledge::Butchery)) / 100;
	MudLog(LogLevel::Debug, "With skill : %f", required);
	return static_cast<unsigned int>(required);
}
