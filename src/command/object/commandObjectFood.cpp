/// @file   commandObjectFood.cpp
/// @author Enrico Fraccaroli
/// @date   Jan 12 2017
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

#include "commandObjectFood.hpp"
#include "foodModel.hpp"
#include "command.hpp"
#include "room.hpp"
#include "mud.hpp"

bool DoEat(Character *character, ArgumentHandler &args)
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
	if (source->model->getType() != ModelType::Food) {
		character->sendMsg("You cannot eat %s.\n", source->getName(true));
		return false;
	}
	auto foodModel = std::dynamic_pointer_cast<FoodModel>(source->model);
	auto finalHunger = character->hunger + foodModel->nurishment;
	if (finalHunger > 100) {
		character->sendMsg("You cannot eat more food.\n");
		return false;
	}
	character->sendMsg("You eat %s.\n", source->getName(true));
	// Update the hunger.
	character->hunger = finalHunger;
	// Destroy the food.
	MudUpdater::instance().addItemToDestroy(source);
	// Apply effects due to food.
	if (HasFlag(foodModel->foodFlags, FoodFlag::Raw)) {
		character->effectManager.addEffect(EffectFactory::poisonDamage(
			character, "Food Poisoning", 1, TRand<unsigned int>(1, 5),
			"Your stomach complains...",
			"The pain in your guts is excruciating!",
			"Your stomach is calm now.",
			"The pain in your guts is excruciating, your body collapses "
			"and our merciful friend, Death, finds you very quickly.",
			foodModel->nurishment));
	}

	return true;
}