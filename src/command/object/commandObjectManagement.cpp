/// @file   commandObjectManagement.cpp
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

#include "command/object/commandObjectManagement.hpp"
#include "command/command.hpp"
#include "structure/room.hpp"

bool DoTake(Character *character, ArgumentHandler &args)
{
	// Check if the character is sleeping.
	if (character->posture == CharacterPosture::Sleep) {
		character->sendMsg("Not while you're sleeping.\n");
		return false;
	}
	// Stop any action the character is executing.
	StopAction(character);
	if ((args.size() != 1) && (args.size() != 2)) {
		character->sendMsg("Wrong number of parameters.\n");
		return false;
	}
	// Take to room.
	auto room = character->room;
	// Check if the room is lit by a light.
	auto roomIsLit = room->isLit();
	// Try to take the item inside the room.
	if (args.size() == 1) {
		// Check if the character wants to take everything.
		if (ToLower(args[0].getContent()) == "all") {
			if (room->items.empty()) {
				character->sendMsg("There is nothing to pick up.\n");
				return false;
			}
			// Make a temporary copy of the character's inventory.
			auto originalList = room->items;
			// Used to determine if the character has picked up something.
			auto pickedUpSomething = false;
			for (auto iterator : originalList) {
				// Check if the item is static.
				if (HasFlag(iterator->model->modelFlags, ModelFlag::Static)) {
					continue;
				}
				// Check if the item is built
				if (HasFlag(iterator->flags, ItemFlag::Built)) {
					continue;
				}
				// Check if the player can carry the item.
				if (!character->canCarry(iterator, iterator->quantity)) {
					continue;
				}
				// Remove the item from the room.
				room->removeItem(iterator);
				// Add the item to the player's inventory.
				character->addInventoryItem(iterator);
				// If the item was a temporary item, make it permanent.
				ClearFlag(iterator->flags, ItemFlag::Temporary);
				// Set that he has picked up something.
				pickedUpSomething = true;
			}
			// Handle output only if the player has really taken something.
			if (pickedUpSomething) {
				// Send the messages.
				character->sendMsg("You've picked up everything you could.\n");
				room->sendToAll("%s has picked up everything %s could.\n",
								{ character }, character->getNameCapital(),
								character->getSubjectPronoun());
				return true;
			}
			character->sendMsg("You've picked up nothing.\n");
			return false;
		}
		// Check if the character wants to take a specific item.
		Item *item = nullptr;
		// If the room is NOT lit and NOT empty, pick a random item.
		if (!roomIsLit && !room->items.empty()) {
			// Pick from the items.
			item = room->items[TRand<size_t>(0, room->items.size() - 1)];
		} else {
			item = room->findItem(args[0].getContent(), args[0].getIndex());
		}
		// If the item is null.
		if (item == nullptr) {
			// Try to check if the character is a mobile, since mobiles can take
			//  items by providing the specific vnum.
			if (character->isMobile() && IsNumber(args[0].getContent())) {
				for (auto it : room->items) {
					if (it->vnum ==
						ToNumber<unsigned int>(args[0].getContent())) {
						item = it;
					}
				}
			}
			// Check if the item is still null.
			if (item == nullptr) {
				character->sendMsg("You don't see that item.\n");
				return false;
			}
		}
		// Check if the item has the flag Static.
		if (HasFlag(item->model->modelFlags, ModelFlag::Static)) {
			character->sendMsg("You can't pick up %s!\n", item->getName(true));
			return false;
		}
		if (HasFlag(item->flags, ItemFlag::Built)) {
			character->sendMsg("You can't pick up something which is built!\n");
			return false;
		}
		// Set the quantity.
		auto multiplier = args[0].getMultiplier();
		if ((multiplier <= 0) && (multiplier != -1)) {
			character->sendMsg("You must provide a valid amount!\n");
			return false;
		}
		auto quantity = item->quantity;
		if (multiplier)
			quantity = static_cast<unsigned int>(multiplier);
		if (item->quantity < quantity)
			quantity = item->quantity;
		// Check if the player can carry the item.
		if (!character->canCarry(item, quantity)) {
			character->sendMsg("You can't carry %s!\n", item->getName(true));
			return false;
		}
		if (item->quantity == quantity) {
			// Remove the item from the room.
			room->removeItem(item);
			// Add the item to the player's inventory.
			character->addInventoryItem(item);
			// If the item was a temporary item, make it permanent.
			ClearFlag(item->flags, ItemFlag::Temporary);
			// Send the messages.
			character->sendMsg("You take %s.\n", item->getName(true));
			room->sendToAll("%s has picked up %s.\n", { character },
							character->getNameCapital(), item->getName(true));
		} else {
			// Remove a stack from item.
			auto newStack = item->removeFromStack(character, quantity);
			if (newStack == nullptr) {
				character->sendMsg("You failed to drop %s.\n",
								   item->getName(true));
				return false;
			}
			// Add the item to the player's inventory.
			character->addInventoryItem(newStack);
			// If the item was a temporary item, make it permanent.
			ClearFlag(newStack->flags, ItemFlag::Temporary);
			// Send the messages.
			character->sendMsg("You take a part of %s.\n", item->getName(true));
			room->sendToAll("%s has picked up part of %s.\n", { character },
							character->getNameCapital(), item->getName(true));
		}
	} else {
		// If the character wants to take an item from a container,
		// the container MUST be visible.
		Item *container = nullptr;
		// If the room is lit.
		if (roomIsLit) {
			container = character->findNearbyItem(args[1].getContent(),
												  args[1].getIndex());
		} else if (character->inventoryIsLit()) {
			// If the room is not lit but the inventory is.
			container = character->findInventoryItem(args[1].getContent(),
													 args[1].getIndex());
		} else {
			character->sendMsg("You can't see.\n");
			return false;
		}
		// Check if a container has been found.
		if (container == nullptr) {
			character->sendMsg("You don't see that container.\n");
			return false;
		}
		// Check if the item is a container.
		if (!container->isAContainer()) {
			character->sendMsg("%s is not a container.\n",
							   container->getNameCapital(true));
			return false;
		}
		// Check if the item is a magazine.
		if ((container->getType() == ModelType::Magazine) ||
			(container->getType() == ModelType::RangedWeapon)) {
			character->sendMsg("%s is not a container, try unloading it.\n",
							   container->getNameCapital(true));
			return false;
		}
		// Check if the item is a liquid container.
		if (container->getType() == ModelType::LiquidContainer) {
			character->sendMsg("%s is not a container, try pouring.\n",
							   container->getNameCapital(true));
			return false;
		}
		// Check if it is locked.
		if (HasFlag(container->flags, ItemFlag::Locked)) {
			character->sendMsg("You have first to unlock %s first.\n",
							   container->getName(true));
			return false;
		}
		if (HasFlag(container->flags, ItemFlag::Closed)) {
			character->sendMsg("You have first to open %s first.\n",
							   container->getName(true));
			return false;
		}
		if (ToLower(args[0].getContent()) == "all") {
			if (container->content.empty()) {
				character->sendMsg("There is nothing inside %s.\n",
								   container->getName(true));
				return false;
			}
			// Make a temporary copy of the character's inventory.
			auto originalList = container->content;
			// Used to determine if the character has picked up something.
			auto takenSomething = false;
			for (auto iterator : originalList) {
				// Check if the item is static.
				if (HasFlag(iterator->model->modelFlags, ModelFlag::Static)) {
					continue;
				}
				// Check if the player can carry the item.
				if (!character->canCarry(iterator, iterator->quantity)) {
					continue;
				}
				// Remove the item from the container.
				container->takeOut(iterator);
				// Add the item to the player's inventory.
				character->addInventoryItem(iterator);
				// If the item was a temporary item, make it permanent.
				ClearFlag(iterator->flags, ItemFlag::Temporary);
				// Set that he has picked up something.
				takenSomething = true;
			}
			// Handle output only if the player has really taken something.
			if (takenSomething) {
				// Send the messages.
				character->sendMsg(
					"You've taken everything you could from %s.\n",
					container->getName(true));
				room->sendToAll("%s has taken everything %s could from %s.\n",
								{ character }, character->getNameCapital(),
								character->getSubjectPronoun(),
								container->getName(true));
				return true;
			}
			character->sendMsg("You've taken nothing from %s.\n",
							   container->getName(true));
			return false;
		}
		// Search the item inside the container.
		auto item =
			container->findContent(args[0].getContent(), args[0].getIndex());
		if (item == nullptr) {
			character->sendMsg(
				"You don't see that item inside the container.\n");
			return false;
		}
		// Check if the item has the flag kNoPick.
		if (HasFlag(item->model->modelFlags, ModelFlag::Static)) {
			character->sendMsg("You can't pick up this kind of items!\n");
			return false;
		}
		// Set the quantity.
		auto multiplier = args[0].getMultiplier();
		if ((multiplier <= 0) && (multiplier != -1)) {
			character->sendMsg("You must provide a valid amount!\n");
			return false;
		}
		auto quantity = item->quantity;
		if (multiplier)
			quantity = static_cast<unsigned int>(multiplier);
		if (item->quantity < quantity)
			quantity = item->quantity;
		// Check if the player can carry the item.
		if (!character->canCarry(item, quantity)) {
			character->sendMsg(
				"You are not strong enough to carry that object.\n");
			return false;
		}
		if (item->quantity == quantity) {
			// Remove the item from the container.
			container->takeOut(item);
			// Add the item to the player's inventory.
			character->addInventoryItem(item);
			// If the item was a temporary item, make it permanent.
			ClearFlag(item->flags, ItemFlag::Temporary);
			// Send the messages.
			character->sendMsg("You take out %s from %s.\n",
							   item->getName(true), container->getName(true));
			room->sendToAll("%s takes out %s from %s.\n", { character },
							character->getNameCapital(), item->getName(true),
							container->getName(true));
		} else {
			// Remove a stack from item.
			auto newStack = item->removeFromStack(character, quantity);
			if (newStack == nullptr) {
				character->sendMsg("You failed to take part of %s.\n",
								   item->getName(true));
				return false;
			}
			// Add the item to the player's inventory.
			character->addInventoryItem(newStack);
			// If the item was a temporary item, make it permanent.
			ClearFlag(newStack->flags, ItemFlag::Temporary);
			// Send the messages.
			character->sendMsg("You take out part of %s from %s.\n",
							   item->getName(true), container->getName(true));
			room->sendToAll("%s takes out %s from %s.\n", { character },
							character->getNameCapital(), item->getName(true),
							container->getName(true));
		}
	}
	return true;
}

bool DoDrop(Character *character, ArgumentHandler &args)
{
	// Check if the character is sleeping.
	if (character->posture == CharacterPosture::Sleep) {
		character->sendMsg("Not while you're sleeping.\n");
		return false;
	}
	// Stop any action the character is executing.
	StopAction(character);
	// Check the number of arguments.
	if (args.empty()) {
		character->sendMsg("Drop what?\n");
		return false;
	}
	if (args.size() > 1) {
		character->sendMsg("Too many arguments!\n");
		return false;
	}
	if (character->inventory.empty()) {
		character->sendMsg("You have nothing to drop.\n");
		return false;
	}
	if (ToLower(args[0].getContent()) == "all") {
		// Make a temporary copy of the character's inventory.
		auto originalList = character->inventory;
		for (auto iterator : originalList) {
			// Remove the item from the player's inventory.
			character->remInventoryItem(iterator);
			// Add the item to the room.
			character->room->addItem(iterator);
		}
		// Send the messages.
		character->sendMsg("You dropped all.\n");
		character->room->sendToAll("%s has dropped all %s items.\n",
								   { character }, character->getNameCapital(),
								   character->getPossessivePronoun());
		return true;
	}
	// Get the item.
	Item *item =
		character->findInventoryItem(args[0].getContent(), args[0].getIndex());
	// If the room is not lit, check if the inventory contains a light.
	if (!character->room->isLit()) {
		// If the inventory is NOT lit and NOT empty, pick a random item.
		if (!character->inventoryIsLit() && !character->inventory.empty()) {
			auto it = TRand<size_t>(0, character->inventory.size() - 1);
			item = character->inventory[it];
		}
	}
	// Check if the player has the item that he want to drop.
	if (item == nullptr) {
		character->sendMsg("You don't have that item.\n");
		return false;
	}
	// Set the quantity.
	auto multiplier = args[0].getMultiplier();
	if ((multiplier <= 0) && (multiplier != -1)) {
		character->sendMsg("You must provide a valid amount!\n");
		return false;
	}
	auto quantity = item->quantity;
	if (multiplier)
		quantity = static_cast<unsigned int>(multiplier);
	if (item->quantity < quantity)
		quantity = item->quantity;
	if (item->quantity == quantity) {
		// Remove the item from the player's inventory.
		character->remInventoryItem(item);
		// Put the item inside the room.
		character->room->addItem(item);
		// Send the messages.
		character->sendMsg("You drop %s.\n", item->getName(true));
		character->room->sendToAll("%s has dropped %s.\n", { character },
								   character->getNameCapital(),
								   item->getName(true));
	} else {
		// Remove from the stack.
		auto newStack = item->removeFromStack(character, quantity);
		if (newStack == nullptr) {
			character->sendMsg("You failed to drop %s.\n", item->getName(true));
			return false;
		}
		// Put the item inside the room.
		character->room->addItem(newStack);
		// Send the messages.
		character->sendMsg("You drop part of %s.\n", item->getName(true));
		character->room->sendToAll("%s has dropped %s.\n", { character },
								   character->getNameCapital(),
								   item->getName(true));
	}
	return true;
}

bool DoPut(Character *character, ArgumentHandler &args)
{
	// Check if the character is sleeping.
	if (character->posture == CharacterPosture::Sleep) {
		character->sendMsg("Not while you're sleeping.\n");
		return false;
	}
	// Stop any action the character is executing.
	StopAction(character);
	if (args.size() != 2) {
		character->sendMsg("Put what inside what?\n");
		return false;
	}
	auto container =
		character->findNearbyItem(args[1].getContent(), args[1].getIndex());
	if (container == nullptr) {
		character->sendMsg("You don't see any container named '%s' here.\n",
						   args[1].getContent());
		return false;
	}
	if (!container->isAContainer()) {
		character->sendMsg("%s is not a valid container.\n",
						   container->getNameCapital());
		return false;
	}
	// Check if the item is a magazine.
	if (container->getType() == ModelType::Magazine) {
		character->sendMsg("%s is not a container, try to load it with ammo.\n",
						   container->getNameCapital(true));
		return false;
	}
	// Check if the item is a ranged weapon.
	if (container->getType() == ModelType::RangedWeapon) {
		character->sendMsg("%s is not a container, try to reload it.\n",
						   container->getNameCapital(true));
		return false;
	}
	// Check if the item is a liquid container.
	if (container->getType() == ModelType::LiquidContainer) {
		character->sendMsg("%s is not a container, try fill it with liquids.\n",
						   container->getNameCapital(true));
		return false;
	}
	if (container->getType() == ModelType::Corpse) {
		character->sendMsg(
			"You don't really want to put something inside that body...\n");
		return false;
	}
	if (HasFlag(container->flags, ItemFlag::Locked)) {
		character->sendMsg("You have first to unlock %s.\n",
						   container->getName(true));
		return false;
	}
	if (HasFlag(container->flags, ItemFlag::Closed)) {
		character->sendMsg("You have first to open %s.\n",
						   container->getName(true));
		return false;
	}
	if (character->inventory.empty()) {
		character->sendMsg("You don't have anything to put in a container.");
		return false;
	}
	// Check if the player wants to put all in the container.
	if (args[0].getContent() == "all") {
		// Make a temporary copy of the character's inventory.
		auto originalList = character->inventory;
		// Move all the items inside the container.
		for (auto iterator : originalList) {
			// Skip the item if it cannot be contained inside the destination.
			if (!container->canContain(iterator, iterator->quantity))
				continue;
			// Skip the item if it is the destination.
			if (iterator->vnum == container->vnum)
				continue;
			// Remove the item from the player's inventory.
			character->remInventoryItem(iterator);
			// Put the item inside the container.
			container->putInside(iterator);
		}
		// Send the messages.
		character->sendMsg("You put everything you could in %s.\n",
						   container->getName(true));
		character->room->sendToAll("%s puts everything %s could inside %s.\n",
								   { character }, character->getNameCapital(),
								   character->getSubjectPronoun(),
								   container->getName(true));
		return true;
	}
	// Find the specific item inside the inventory.
	auto item =
		character->findInventoryItem(args[0].getContent(), args[0].getIndex());
	// If the room is not lit, check if the inventory contains a light.
	if (!character->room->isLit()) {
		// If the inventory is NOT lit and NOT empty, pick a random item.
		if (!character->inventoryIsLit() && !character->inventory.empty()) {
			auto it = TRand<size_t>(0, character->inventory.size() - 1);
			item = character->inventory[it];
		}
	}
	if (item == nullptr) {
		character->sendMsg("You don't have any '%s'.\n", args[0].getContent());
		return false;
	}
	// Set the quantity.
	auto multiplier = args[0].getMultiplier();
	if ((multiplier <= 0) && (multiplier != -1)) {
		character->sendMsg("You must provide a valid amount!\n");
		return false;
	}
	auto quantity = item->quantity;
	if (multiplier)
		quantity = static_cast<unsigned int>(multiplier);
	if (item->quantity < quantity)
		quantity = item->quantity;
	// Check if the item can be contained inside the destination.
	if (!container->canContain(item, quantity)) {
		character->sendMsg("%s can't contain any more items.\n",
						   container->getNameCapital());
		return false;
	}
	if (item->quantity == quantity) {
		// Remove the item from the player's inventory.
		character->remInventoryItem(item);
		// Put the item inside the container.
		container->putInside(item);
		// Send the messages.
		character->sendMsg("You put %s inside %s.\n", item->getName(true),
						   container->getName(true));
		character->room->sendToAll("%s puts %s inside %s.\n", { character },
								   character->getNameCapital(),
								   item->getName(true),
								   container->getName(true));
	} else {
		// Remove from the stack.
		auto newStack = item->removeFromStack(character, quantity);
		if (newStack == nullptr) {
			character->sendMsg("You failed to put part of %s inside %s.\n",
							   item->getName(true), container->getName(true));
			return false;
		}
		// Put the stack inside the container.
		container->putInside(newStack);
		// Send the messages.
		character->sendMsg("You put part of %s inside %s.\n",
						   item->getName(true), container->getName(true));
		character->room->sendToAll("%s puts %s inside %s.\n", { character },
								   character->getNameCapital(),
								   item->getName(true),
								   container->getName(true));
	}
	return true;
}

bool DoGive(Character *character, ArgumentHandler &args)
{
	// Check if the character is sleeping.
	if (character->posture == CharacterPosture::Sleep) {
		character->sendMsg("Not while you're sleeping.\n");
		return false;
	}
	// Stop any action the character is executing.
	StopAction(character);
	// Check the number of arguments.
	if (args.size() != 2) {
		character->sendMsg("Give what to whom?\n");
		return false;
	}
	// Get the item.
	auto item =
		character->findInventoryItem(args[0].getContent(), args[0].getIndex());
	// If the room is not lit, check if the inventory contains a light.
	if (!character->room->isLit()) {
		// If the inventory is NOT lit and NOT empty, pick a random item.
		if (!character->inventoryIsLit() && !character->inventory.empty()) {
			auto it = TRand<size_t>(0, character->inventory.size() - 1);
			item = character->inventory[it];
		}
	}
	if (item == nullptr) {
		character->sendMsg("You don't have that item.\n");
		return false;
	}
	auto target = character->room->findCharacter(
		args[1].getContent(), args[1].getIndex(), { character });
	if (target == nullptr) {
		character->sendMsg("You don't see that person.\n");
		return false;
	}
	// Check if the target player can carry the item.
	if (!target->canCarry(item, item->quantity)) {
		character->sendMsg("%s can't carry anymore items.\n",
						   target->getNameCapital());
		return false;
	}
	// Set the quantity.
	auto multiplier = args[0].getMultiplier();
	if ((multiplier <= 0) && (multiplier != -1)) {
		character->sendMsg("You must provide a valid amount!\n");
		return false;
	}
	auto quantity = item->quantity;
	if (multiplier)
		quantity = static_cast<unsigned int>(multiplier);
	if (item->quantity < quantity)
		quantity = item->quantity;
	if (item->quantity == quantity) {
		// Remove the item from the character inventory.
		character->remInventoryItem(item);
		// Add the item to the target inventory.
		target->addInventoryItem(item);
		// Send all the messages.
		character->sendMsg("You give %s to %s.\n", item->getName(true),
						   target->getName());
		target->sendMsg("%s gives you %s.\n\n", character->getNameCapital(),
						item->getName(true));
		character->room->sendToAll("%s gives %s to %s.\n",
								   { character, target },
								   character->getNameCapital(),
								   item->getName(true), target->getName());
	} else {
		// Remove from the stack.
		auto newStack = item->removeFromStack(character, quantity);
		if (newStack == nullptr) {
			character->sendMsg("You failed to give part of %s to %s.\n",
							   item->getName(true), target->getName());
			return false;
		}
		// Add the stack to the target inventory.
		target->addInventoryItem(newStack);
		// Send all the messages.
		character->sendMsg("You give part of %s to %s.\n", item->getName(true),
						   target->getName());
		target->sendMsg("%s gives you %s.\n\n", character->getNameCapital(),
						item->getName(true));
		character->room->sendToAll("%s gives %s to %s.\n",
								   { character, target },
								   character->getNameCapital(),
								   item->getName(true), target->getName());
	}
	return true;
}
