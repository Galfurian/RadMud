/// @file   commandObjectShop.cpp
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

#include "command/object/commandObjectShop.hpp"

#include "character/characterUtilities.hpp"
#include "model/submodel/currencyModel.hpp"
#include "database/sqliteDbms.hpp"
#include "item/subitem/shopItem.hpp"
#include "command/command.hpp"
#include "updater/updater.hpp"
#include "structure/room.hpp"

bool DoDeposit(Character *character, ArgumentHandler &args)
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
		character->sendMsg("What do you want to deposit?\n");
		return false;
	}
	// Check if the room is NOT lit.
	if (!character->room->isLit()) {
		character->sendMsg("You cannot do that without light.\n");
		return false;
	}
	auto item =
		character->findInventoryItem(args[0].getContent(), args[0].getIndex());
	auto building =
		character->room->findBuilding(args[1].getContent(), args[1].getIndex());
	// Check the item.
	if (item == nullptr) {
		character->sendMsg("You don't have that item.\n");
		return false;
	}
	if (item->getType() != ModelType::Currency) {
		character->sendMsg("You can't deposit %s.\n", item->getName(true));
		return false;
	}
	// Check the building.
	if (building == nullptr) {
		character->sendMsg("You don't see that building.\n");
		return false;
	}
	if (building->getType() != ModelType::Shop) {
		character->sendMsg("You can't deposit %s in %s.\n", item->getName(true),
						   building->getName(true));
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
	// Cast the building to shop.
	auto shopBuilding = static_cast<ShopItem *>(building);
	if (item->quantity == quantity) {
		shopBuilding->balance += item->getPrice(true);
		MudUpdater::instance().addItemToDestroy(item);
	} else {
		item->quantity -= quantity;
		shopBuilding->balance += item->getPrice(false) * quantity;
	}
	return true;
}

bool DoBuy(Character *character, ArgumentHandler &args)
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
		character->sendMsg("Buy what from which shop?\n");
		return false;
	}
	// Check if the room is NOT lit.
	if (!character->room->isLit()) {
		character->sendMsg("You cannot do that without light.\n");
		return false;
	}
	// Get the target.
	auto target =
		character->room->findItem(args[1].getContent(), args[1].getIndex());
	if (target == nullptr) {
		character->sendMsg("You don't see '%s' here.\n", args[1].getContent());
		return false;
	}
	if (target->getType() != ModelType::Shop) {
		character->sendMsg("%s is not a shop.\n", target->getNameCapital(true));
		return false;
	}

	// Get the shop.
	auto shop = static_cast<ShopItem *>(target);
	std::string error;
	if (!shop->canUse(error)) {
		character->sendMsg(error + "\n");
		return false;
	}
	auto shopKeeper = shop->shopKeeper;

	// Get the item.
	auto item = shop->findContent(args[0].getContent(), args[0].getIndex());
	// Check the item.
	if (item == nullptr) {
		auto phrase = "There is no " + args[0].getContent() + " on sale.\n";
		shopKeeper->doCommand("say " + character->getName() + " " + phrase);
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
	if (!character->canCarry(item, quantity)) {
		auto phrase =
			"It seems that you can't carry " + item->getName(true) + ".\n";
		shopKeeper->doCommand("say " + character->getName() + " " + phrase);
		return false;
	}
	// Check if the character has enough coins.
	auto availableCoins = FindPosessedCoins(character);
	ItemVector changedCoins;
	std::vector<std::pair<Item *, unsigned int> > givenCoins;
	unsigned int requiredValue = shop->evaluateBuyPrice(item) * quantity;
	unsigned int providedValue = 0;
	for (auto coin : availableCoins) {
		unsigned int coinValue = coin->getPrice(false);
		unsigned int coinQuantity = 0;
		while ((coinQuantity < coin->quantity) &&
			   (providedValue < requiredValue)) {
			providedValue += coinValue;
			++coinQuantity;
		}
		givenCoins.push_back(std::make_pair(coin, coinQuantity));
		if (providedValue >= requiredValue) {
			break;
		}
	}
	if (givenCoins.empty()) {
		auto phrase = "You can't afford to buy " + item->getName(true) + ".\n";
		shopKeeper->doCommand("say " + character->getName() + " " + phrase);
		return false;
	}
	if (providedValue > requiredValue) {
		auto change = providedValue - requiredValue;
		auto currency = shopKeeper->faction->currency;
		changedCoins =
			currency->generateCurrency(shopKeeper->getName(), change);
		if (changedCoins.empty()) {
			auto phrase =
				"Sorry but I cannot sell " + item->getName(true) + " to you.\n";
			shopKeeper->doCommand("say " + character->getName() + " " + phrase);
			return false;
		}
	}
	SQLiteDbms::instance().beginTransaction();
	if (item->quantity <= quantity) {
		shop->balance += requiredValue;

		shop->takeOut(item);
		character->addInventoryItem(item);
		character->sendMsg("You buy %s from %s.\n", item->getName(true),
						   shop->getName(true));
	} else {
		// Remove from the stack.
		auto newStack = item->removeFromStack(character, quantity);
		if (newStack == nullptr) {
			// Rollback the transaction.
			SQLiteDbms::instance().rollbackTransection();
			return false;
		}
		character->addInventoryItem(newStack);
		character->sendMsg("You buy part of %s from %s.\n", item->getName(true),
						   shop->getName(true));
	}
	for (auto coin : changedCoins) {
		character->addInventoryItem(coin);
	}
	for (auto iterator : givenCoins) {
		auto coin = iterator.first;
		if (iterator.second == coin->quantity) {
			MudUpdater::instance().addItemToDestroy(coin);
		} else {
			coin->quantity -= iterator.second;
			coin->updateOnDB();
		}
	}
	SQLiteDbms::instance().endTransaction();
	return true;
}

bool DoSell(Character *character, ArgumentHandler &args)
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
		character->sendMsg("Sell what to whom?\n");
		return false;
	}
	// Check if the room is NOT lit.
	if (!character->room->isLit()) {
		character->sendMsg("You cannot do that without light.\n");
		return false;
	}
	// Get the item and the target.
	auto item =
		character->findInventoryItem(args[0].getContent(), args[0].getIndex());
	// Check the item.
	if (item == nullptr) {
		character->sendMsg("You don't have that item.\n");
		return false;
	}
	if (item->getType() == ModelType::Currency) {
		character->sendMsg("You can't sell %s.\n", item->getName(true));
		return false;
	}

	// Get the target.
	auto target =
		character->room->findItem(args[1].getContent(), args[1].getIndex());
	if (target == nullptr) {
		character->sendMsg("You don't see '%s' here.\n", args[1].getContent());
		return false;
	}
	if (target->getType() != ModelType::Shop) {
		character->sendMsg("%s is not a shop.\n", target->getNameCapital());
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
	// Cast the item to shop.
	auto shop = static_cast<ShopItem *>(target);
	// Check if the shop can be used.
	std::string error;
	if (!shop->canUse(error)) {
		character->sendMsg(error + "\n");
		return false;
	}

	auto shopKeeper = shop->shopKeeper;
	if (!shop->canContain(item, quantity)) {
		auto phrase = "The shop is full, come back another day.";
		shopKeeper->doCommand("say " + character->getName() + " " + phrase);
		return false;
	}

	auto price = shop->evaluateSellPrice(item) * quantity;
	if (shop->balance < price) {
		auto phrase = "I can't afford to buy your goods.";
		shopKeeper->doCommand("say " + character->getName() + " " + phrase);
		return false;
	}

	auto currency = shopKeeper->faction->currency;

	// Give the coins to the character.
	auto coins = currency->generateCurrency(shopKeeper->getName(), price);
	if (coins.empty()) {
		character->sendMsg("You failed to sell %s.\n", item->getName(true));
		return false;
	}

	SQLiteDbms::instance().beginTransaction();
	if (item->quantity <= quantity) {
		// Remove the item from the player's inventory.
		character->remInventoryItem(item);
		// Put the item inside the container.
		shop->putInside(item);
		// Decrese the shop balance.
		shop->balance -= price;
		// Send a message.
		character->sendMsg("You sell %s to %s.\n", item->getName(true),
						   shop->getName(true));
	} else {
		// Remove from the stack.
		auto newStack = item->removeFromStack(character, quantity);
		if (newStack == nullptr) {
			character->sendMsg("You failed sell part of %s to %s.\n",
							   item->getName(true), shop->getName(true));
			for (auto coin : coins) {
				MudUpdater::instance().addItemToDestroy(coin);
			}
			// Rollback the transaction.
			SQLiteDbms::instance().rollbackTransection();
			return false;
		}
		// Put the item inside the container.
		shop->putInside(newStack);
		// Decrese the shop balance.
		shop->balance -= price;
		// Send a message.
		character->sendMsg("You sell part of %s to %s.\n", item->getName(true),
						   shop->getName(true));
	}
	for (auto coin : coins) {
		character->addInventoryItem(coin);
	}
	SQLiteDbms::instance().endTransaction();
	return true;
}

bool DoBalance(Character *character, ArgumentHandler &args)
{
	// Check if the character is sleeping.
	if (character->posture == CharacterPosture::Sleep) {
		character->sendMsg("Not while you're sleeping.\n");
		return false;
	}
	// Check the number of arguments.
	if (!args.empty()) {
		character->sendMsg("You have provided unnecessary arguments.\n");
		return false;
	}
	// Check if the room is NOT lit.
	if (!character->room->isLit()) {
		character->sendMsg("You cannot do that without light.\n");
		return false;
	}
	unsigned int balance = 0;
	for (auto coin : FindPosessedCoins(character)) {
		balance += coin->getPrice(true);
	}
	character->sendMsg("Your balance is: %s.\n", balance);
	return true;
}
