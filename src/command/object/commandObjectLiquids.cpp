/// @file   commandObjectLiquids.cpp
/// @author Enrico Fraccaroli
/// @date   29/12/2016
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

#include "commandObjectLiquids.hpp"
#include "liquidContainerModel.hpp"
#include "liquidContainerItem.hpp"
#include "lightModel.hpp"
#include "lightItem.hpp"
#include "formatter.hpp"
#include "command.hpp"
#include "room.hpp"

bool DoDrink(Character *character, ArgumentHandler &args)
{
	// Check if the character is sleeping.
	if (character->posture == CharacterPosture::Sleep) {
		character->sendMsg("Not while you're sleeping.\n");
		return false;
	}
	// Stop any action the character is executing.
	StopAction(character);
	if (args.empty()) {
		character->sendMsg("Drink from what?\n");
		return false;
	}
	Item *source = nullptr;
	// If the room is lit.
	if (character->room->isLit()) {
		source =
			character->findNearbyItem(args[0].getContent(), args[0].getIndex());
	} else if (character->inventoryIsLit()) {
		// If the room is not lit but the inventory is.
		source = character->findInventoryItem(args[0].getContent(),
											  args[0].getIndex());
	} else {
		// If the inventory is NOT empty, pick a random item.
		if (!character->inventory.empty()) {
			auto it = TRand<size_t>(0, character->inventory.size() - 1);
			source = character->inventory[it];
		}
	}
	// Check the source container.
	if (source == nullptr) {
		character->sendMsg("You don't see '%s'.\n", args[0].getContent());
		return false;
	}
	if (HasFlag(source->flags, ItemFlag::Locked)) {
		character->sendMsg("You have first to unlock %s.\n",
						   source->getName(true));
		return false;
	}
	if (HasFlag(source->flags, ItemFlag::Closed)) {
		character->sendMsg("You have first to open %s.\n",
						   source->getName(true));
		return false;
	}
	if (source->model->getType() != ModelType::LiquidContainer) {
		character->sendMsg("%s is not a source for liquids.\n",
						   source->getNameCapital(true));
		return false;
	}
	if (character->thirst == 100) {
		character->sendMsg("You are full.\n");
		return false;
	}
	// Cast the source item to liquid container.
	auto liqConSrc = static_cast<LiquidContainerItem *>(source);
	if (source->isEmpty()) {
		character->sendMsg("%s is empty.\n", source->getNameCapital(true));
		return false;
	}
	// Before managing the liquid, store the pointer to it.
	auto liquidContent = liqConSrc->liquidContent;
	// Evaluate the amount needed to quench the thirst of the character.
	auto quantity = (100 - character->thirst) / liquidContent->quench;
	// Get the model of the liquid container.
	auto liquidModelSource = source->model->to<LiquidContainerModel>();
	if (!HasFlag(liquidModelSource->liquidFlags, LiqContainerFlag::Endless)) {
		if (liqConSrc->liquidQuantity < quantity) {
			quantity = liqConSrc->liquidQuantity;
		}
	}
	std::cout << "Quantity : " << quantity << "\n";
	// Now pour out the liquid.
	if (!liqConSrc->pourOut(quantity)) {
		character->sendMsg("You were not able to drink some %s from %s.\n",
						   liquidContent->getName(), source->getName(true));
		return false;
	}
	std::cout << "Added Thirst : " << quantity * liquidContent->quench << "\n";
	// Evaluate the new thirst level.
	std::cout << "Old Thirst   : " << character->thirst << "\n";
	character->addThirst(static_cast<int>(quantity * liquidContent->quench));
	std::cout << "New Thirst   : " << character->thirst << "\n";
	// Send the message to the character.
	character->sendMsg("You drink some %s from %s.\n", liquidContent->getName(),
					   source->getName(true));
	// Send the message inside the room.
	character->room->sendToAll("%s drinks some %s from %s.\n", { character },
							   character->getNameCapital(),
							   liquidContent->getName(), source->getName(true));
	return true;
}

bool DoFill(Character *character, ArgumentHandler &args)
{
	// Check if the character is sleeping.
	if (character->posture == CharacterPosture::Sleep) {
		character->sendMsg("Not while you're sleeping.\n");
		return false;
	}
	// Stop any action the character is executing.
	StopAction(character);
	// Check the arguments.
	if (args.size() != 2) {
		character->sendMsg("You have to fill something from a source.\n");
		return false;
	}
	// Prepare variables for the destination and source liquid containers.
	Item *source = nullptr, *destination = nullptr;
	// If the room is lit.
	if (character->room->isLit()) {
		destination =
			character->findNearbyItem(args[0].getContent(), args[0].getIndex());
		source =
			character->findNearbyItem(args[1].getContent(), args[1].getIndex());
	} else if (character->inventoryIsLit()) {
		// If the room is not lit but the inventory is.
		destination = character->findInventoryItem(args[0].getContent(),
												   args[0].getIndex());
		source = character->findInventoryItem(args[1].getContent(),
											  args[1].getIndex());
	} else {
		character->sendMsg("You can't do that without seeing,"
						   "you could waste most of the liquid.\n");
		return false;
	}
	// Check the source item.
	if (source == nullptr) {
		character->sendMsg("You don't see any '%s'.\n", args[1].getContent());
		return false;
	}
	if (HasFlag(source->flags, ItemFlag::Locked)) {
		character->sendMsg("You have first to unlock %s.\n",
						   source->getName(true));
		return false;
	}
	if (HasFlag(source->flags, ItemFlag::Closed)) {
		character->sendMsg("You have first to open %s.\n",
						   source->getName(true));
		return false;
	}
	if (source->model->getType() != ModelType::LiquidContainer) {
		character->sendMsg("%s is not a suitable source of liquids.\n",
						   source->getNameCapital());
		return false;
	}
	// Check the destination item.
	if (destination == nullptr) {
		character->sendMsg("You don't have any '%s' with you.\n",
						   args[0].getContent());
		return false;
	}
	if (HasFlag(destination->flags, ItemFlag::Locked)) {
		character->sendMsg("You have first to unlock %s.\n",
						   destination->getName(true));
		return false;
	}
	if (HasFlag(destination->flags, ItemFlag::Closed)) {
		character->sendMsg("You have first to open %s.\n",
						   destination->getName(true));
		return false;
	}
	if (destination->model->getType() != ModelType::LiquidContainer) {
		character->sendMsg("%s is not a suitable destination.\n",
						   destination->getNameCapital());
		return false;
	}
	// Cast the source item to liquid container.
	auto liqConSrc = static_cast<LiquidContainerItem *>(source);
	// Cast the destination item to liquid container.
	auto liqConDst = static_cast<LiquidContainerItem *>(destination);
	// Check if source is empty.
	if (source->isEmpty()) {
		character->sendMsg("%s is empty.\n", source->getNameCapital());
		return false;
	}
	// Check if the destination container already contains a liquid.
	if (!liqConDst->isEmpty()) {
		// Check compatibility between liquids.
		if (liqConSrc->liquidContent->vnum != liqConDst->liquidContent->vnum) {
			character->sendMsg("You can't mix those two liquids.\n");
			return false;
		}
	}
	// Get the model of the liquid container.
	auto liquidModelSource = source->model->to<LiquidContainerModel>();
	// Fill the destination from the source.
	auto quantity = destination->getFreeSpace();
	if (!HasFlag(liquidModelSource->liquidFlags, LiqContainerFlag::Endless)) {
		if (liqConSrc->liquidQuantity < quantity) {
			quantity = liqConSrc->liquidQuantity;
		}
	}
	// Before pouring out the liquid, store the pointer to it.
	auto pouredLiquid = liqConSrc->liquidContent;
	// Now pour out the liquid.
	if (!liqConSrc->pourOut(quantity)) {
		character->sendMsg("You failed to take out the liquid from a %s.\n",
						   source->getNameCapital());
		return false;
	}
	// And now pour in the liquid.
	if (!liqConDst->pourIn(pouredLiquid, quantity)) {
		character->sendMsg(
			"You failed to fill the destination with the liquid.\n");
		return false;
	}
	// Send the messages.
	character->sendMsg("You fill %s with %s from %s.\n",
					   destination->getName(true), pouredLiquid->getName(),
					   source->getName(true));
	character->room->sendToAll("%s fills %s with %s from %s.\n", { character },
							   character->getNameCapital(),
							   destination->getName(true),
							   pouredLiquid->getName(), source->getName(true));
	return true;
}

bool DoPour(Character *character, ArgumentHandler &args)
{
	// Check if the character is sleeping.
	if (character->posture == CharacterPosture::Sleep) {
		character->sendMsg("Not while you're sleeping.\n");
		return false;
	}
	// Stop any action the character is executing.
	StopAction(character);
	// Check the arguments.
	if ((args.size() != 1) && (args.size() != 2)) {
		character->sendMsg("You have to pour something into something else.\n");
		return false;
	}
	// Prepare variables for the destination and source liquid containers.
	Item *source = nullptr, *destination = nullptr;
	// If the room is lit.
	if (character->room->isLit()) {
		// Liquids can be poured inside another container only if from
		// a container inside the inventory.
		source = character->findInventoryItem(args[0].getContent(),
											  args[0].getIndex());
		if (args.size() == 2) {
			destination = character->findNearbyItem(args[1].getContent(),
													args[1].getIndex());
		}
	} else if (character->inventoryIsLit()) {
		// If the room is not lit but the inventory is.
		source = character->findInventoryItem(args[0].getContent(),
											  args[0].getIndex());
		if (args.size() == 2) {
			destination = character->findInventoryItem(args[1].getContent(),
													   args[1].getIndex());
		}
	} else {
		character->sendMsg("You can't do that without seeing,"
						   "you could waste most of the liquid.\n");
		return false;
	}
	// Check the source item.
	if (source == nullptr) {
		character->sendMsg("You don't see any '%s'.\n", args[1].getContent());
		return false;
	}
	if (HasFlag(source->flags, ItemFlag::Locked)) {
		character->sendMsg("You have first to unlock %s.\n",
						   source->getName(true));
		return false;
	}
	if (HasFlag(source->flags, ItemFlag::Closed)) {
		character->sendMsg("You have first to open %s.\n",
						   source->getName(true));
		return false;
	}
	if (source->model->getType() != ModelType::LiquidContainer) {
		character->sendMsg("%s is not a suitable source of liquids.\n",
						   source->getNameCapital());
		return false;
	}
	// Cast the source item to liquid container.
	auto liqConSrc = static_cast<LiquidContainerItem *>(source);
	// Check if source is empty.
	if (liqConSrc->isEmpty()) {
		character->sendMsg("%s is empty.\n", source->getNameCapital());
		return false;
	}
	// Before pouring out the liquid, store the pointer to it.
	auto pouredLiquid = liqConSrc->liquidContent;
	// If the number of arguments is only one, pour the content of the source
	// on the ground.
	if (args.size() == 1) {
		// Now pour out the liquid.
		if (!liqConSrc->pourOut(liqConSrc->liquidQuantity)) {
			character->sendMsg("You failed to pour out the liquid from %s.\n",
							   source->getName(true));
			return false;
		}
		// Send the messages.
		character->sendMsg("You pour %s from %s on the ground.\n",
						   pouredLiquid->getName(), liqConSrc->getName());
		character->room->sendToAll("%s pours %s from %s on the ground.\n",
								   { character }, character->getNameCapital(),
								   pouredLiquid->getName(),
								   liqConSrc->getName(true));
		return true;
	}
	// Check the destination item.
	if (destination == nullptr) {
		character->sendMsg("You don't see '%s' anywhere.\n",
						   args[0].getContent());
		return false;
	}
	// Check if the destination is a kindled light source.
	if (destination->getType() == ModelType::Light) {
		// Cast the destination to light source.
		auto lightItem = static_cast<LightItem *>(destination);
		// Cast the model of the destination to light source.
		auto lightModel = destination->model->to<LightModel>();
		if (!lightItem->isActive() || !HasFlag(lightModel->lightSourceFlags,
											   LightModelFlags::NeedToKindle)) {
			character->sendMsg("You cannot use %s on %s.\n",
							   source->getName(true),
							   destination->getName(true));
			return false;
		}
		// Now pour out the liquid.
		if (!liqConSrc->pourOut(liqConSrc->liquidQuantity)) {
			character->sendMsg("You failed to pour the liquid from %s on %s.\n",
							   source->getName(true),
							   destination->getName(true));
			return false;
		}
		// Send the messages.
		// If the liquid content is alcohol, make some explosions.
		if (HasFlag(pouredLiquid->flags, LiquidFlags::Inflammable)) {
			character->sendMsg("As soon as you pour %s from %s on %s, a blaze "
							   "of fire arises from %s and hits you.\n",
							   pouredLiquid->getName(),
							   liqConSrc->getName(true),
							   destination->getName(true),
							   destination->getName(true));
			character->room->sendToAll(
				"As soon as %s pours %s from %s on %s,"
				" a blaze of fire arises from %s "
				"and hits %s.\n",
				{ character }, character->getNameCapital(),
				pouredLiquid->getName(), liqConSrc->getName(true),
				destination->getName(true), destination->getName(true),
				character->getName());
			// Evaluate the damage as the 15% of the maximum health.
			auto maxHealth = static_cast<double>(character->getMaxHealth());
			auto damage = (maxHealth / 100) * 15;
			// Remove the damage from the health of the character.
			if (!character->remHealth(static_cast<unsigned int>(damage))) {
				// Notify the others.
				character->room->funcSendToAll(
					"%s%s screams in pain and then die!%s\n",
					[&](Character *other) { return character != other; },
					Formatter::red(), character->getNameCapital(),
					Formatter::reset());
				// The target has received the damage and now it is dead.
				character->kill();
				return false;
			}
		} else {
			character->sendMsg("You pour %s from %s on %s.\n",
							   pouredLiquid->getName(),
							   liqConSrc->getName(true),
							   destination->getName(true));
			character->room->sendToAll(
				"%s pours %s from %s on %s.\n", { character },
				character->getNameCapital(), pouredLiquid->getName(),
				liqConSrc->getName(true), destination->getName(true));
			// Turn off the light source.
			lightItem->active = false;
		}
		return true;
	}
	if (HasFlag(destination->flags, ItemFlag::Locked)) {
		character->sendMsg("You have first to unlock %s.\n",
						   destination->getName(true));
		return false;
	}
	if (HasFlag(destination->flags, ItemFlag::Closed)) {
		character->sendMsg("You have first to open %s.\n",
						   destination->getName(true));
		return false;
	}
	if (destination->model->getType() != ModelType::LiquidContainer) {
		character->sendMsg("%s is not a suitable destination.\n",
						   destination->getNameCapital());
		return false;
	}
	// Cast the destination item to liquid container.
	auto liqConDst = static_cast<LiquidContainerItem *>(destination);
	// Check if the destination container already contains a liquid.
	if (!liqConDst->isEmpty()) {
		// Check compatibility between liquids.
		if (liqConSrc->liquidContent->vnum != liqConDst->liquidContent->vnum) {
			character->sendMsg("You can't mix those two liquids.\n");
			return false;
		}
	}
	// Fill the destination from the source.
	auto liquidModelSource = source->model->to<LiquidContainerModel>();
	auto quantity = destination->getFreeSpace();
	if (!HasFlag(liquidModelSource->liquidFlags, LiqContainerFlag::Endless)) {
		if (liqConSrc->liquidQuantity < quantity) {
			quantity = liqConSrc->liquidQuantity;
		}
	}
	// Now pour out the liquid.
	if (!liqConSrc->pourOut(quantity)) {
		character->sendMsg("You failed to pour out the liquid from %s.\n",
						   source->getName(true));
		return false;
	}
	// And now pour in the liquid.
	if (!liqConDst->pourIn(pouredLiquid, quantity)) {
		character->sendMsg("You failed to pour the liquid into %s.\n",
						   source->getName(true));
		return false;
	}
	// Send the messages.
	character->sendMsg("You pour %s of %s into %s.\n", pouredLiquid->getName(),
					   source->getName(true), destination->getName(true));
	character->room->sendToAll("%s pour %s of %s into %s.\n", { character },
							   character->getNameCapital(),
							   pouredLiquid->getName(), source->getName(true),
							   destination->getName(true));
	return true;
}
