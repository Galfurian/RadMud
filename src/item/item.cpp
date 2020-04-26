/// @file   item.cpp
/// @brief  Implements item methods.
/// @author Enrico Fraccaroli
/// @date   Aug 25 2014
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

#include "item/item.hpp"

#include "utilities/formatter.hpp"
#include "mud.hpp"

Item::Item() :
	vnum(),
	model(),
	quantity(),
	maker(),
	condition(),
	composition(),
	quality(ItemQuality::Normal),
	flags(),
	room(),
	owner(),
	container(),
	occupiedBodyParts(),
	content(),
	itemMutex()
{
}

Item::~Item()
{
	MudLog(LogLevel::Debug, "Deleted item\t\t[%d]\t\t(%s)", vnum,
		   this->getNameCapital());
}

bool Item::check()
{
	bool safe = true;
	safe &= CorrectAssert(vnum > 0);
	safe &= CorrectAssert(model != nullptr);
	safe &= CorrectAssert(quantity > 0);
	safe &= CorrectAssert(!maker.empty());
	safe &= CorrectAssert(condition > 0);
	safe &= CorrectAssert(composition != nullptr);
	return safe;
}

bool Item::removeFromMud()
{
	MudLog(LogLevel::Debug, "Removing %s from MUD...", this->getName());
	// Remove the item from the game, this means: Room, Player, Container.
	if (room != nullptr) {
		if (!room->characters.empty()) {
			room->sendToAll("%s crumbles into pieces.", {},
							this->getNameCapital());
			if (!content.empty())
				room->sendToAll("All its contents are scattered to the ground.",
								{});
		}
		auto itemRoom = room;
		// Move the content to the room.
		for (auto it : content)
			room->addItem(it, true);
		// Remove the item.
		if (room->removeItem(this)) {
			MudLog(LogLevel::Debug, "Removing item '%s' from room '%s'.",
				   this->getName(), itemRoom->name);
		} else {
			MudLog(LogLevel::Error,
				   "Failed to remove item '%s' from room '%s'.",
				   this->getName(), itemRoom->name);
			return false;
		}
	}
	if (owner != nullptr) {
		owner->sendMsg("%s crumbles into pieces.", this->getNameCapital());
		if (!content.empty())
			owner->sendMsg("All its contents are scattered in your inventory.");
		auto itemOwner = owner;
		// Move the content to the owner's inventory.
		for (auto it : content)
			owner->addInventoryItem(it);
		// Remove the item from the owner.
		if (owner->equipment.removeItem(this)) {
			MudLog(LogLevel::Debug, "Removing item '%s' from '%s' equipment.",
				   this->getName(), itemOwner->getName());
		} else if (owner->inventory.removeItem(this)) {
			MudLog(LogLevel::Debug, "Removing item '%s' from '%s' inventory.",
				   this->getName(), itemOwner->getName());
		} else {
			MudLog(LogLevel::Debug, "Failed to remove item '%s' from '%s'.",
				   this->getName(), itemOwner->getName());
			return false;
		}
	}
	if (container != nullptr) {
		auto itemContainer = container;
		// Move the content to the container.
		for (auto it : content)
			container->putInside(it, true);
		// Remove the item from the container.
		if (container->content.removeItem(this)) {
			MudLog(LogLevel::Debug, "Removing item '%s' from container '%s'.",
				   this->getName(), itemContainer->getName());
		} else {
			MudLog(LogLevel::Error,
				   "Failed to remove item '%s' from container '%s'.",
				   this->getName(), itemContainer->getName());
			return false;
		}
	}
	if (this->getType() != ModelType::Corpse) {
		if (Mud::instance().remItem(this)) {
			MudLog(LogLevel::Debug, "Removing item '%s' from MUD items.",
				   this->getName());
		} else {
			MudLog(LogLevel::Error,
				   "Failed to remove item '%s' from MUD items.",
				   this->getName());
			return false;
		}
	}
	return true;
}

bool Item::updateOnDB()
{
	// Prepare the vector used to insert into the database.
	std::vector<std::string> arguments;
	arguments.emplace_back(ToString(this->vnum));
	arguments.emplace_back(ToString(model->vnum));
	arguments.emplace_back(ToString(this->quantity));
	arguments.emplace_back(this->maker);
	arguments.emplace_back(ToString(this->condition));
	arguments.emplace_back(ToString(this->composition->vnum));
	arguments.emplace_back(ToString(this->quality.toUInt()));
	arguments.emplace_back(ToString(this->flags));
	if (SQLiteDbms::instance().insertInto("Item", arguments, false, true))
		return true;
	MudLog(LogLevel::Error, "Failed INSERT Item '%s'", this->getName());
	return false;
}

bool Item::removeOnDB()
{
	static bool status;
	static WhereClause vnum_pair = std::make_pair("vnum", "-1");
	static WhereClause item_pair = std::make_pair("item", "-1");
	static WhereClause container_pair = std::make_pair("container", "-1");
	// Log.
	MudLog(LogLevel::Debug, "Removing '%s' from DB...", this->getName(true));
	// Update/Prepare variables.
	status = true;
	vnum_pair.second = ToString(vnum);
	item_pair.second = ToString(vnum);
	container_pair.second = ToString(vnum);
	// Remove from DB.
	if (!SQLiteDbms::instance().deleteFrom("Item", { vnum_pair })) {
		MudLog(LogLevel::Error, "Remove from table 'Item'.");
		SQLiteDbms::instance().showLastError();
		status = false;
	}
	if (!SQLiteDbms::instance().deleteFrom("ItemPlayer", { item_pair })) {
		MudLog(LogLevel::Error, "Remove from table 'ItemPlayer'.");
		SQLiteDbms::instance().showLastError();
		status = false;
	}
	if (!SQLiteDbms::instance().deleteFrom("ItemRoom", { item_pair })) {
		MudLog(LogLevel::Error, "Remove from table 'ItemRoom'.");
		SQLiteDbms::instance().showLastError();
		status = false;
	}
	if (!SQLiteDbms::instance().deleteFrom("ItemContent", { item_pair })) {
		MudLog(LogLevel::Error, "Remove from table 'ItemContent'.");
		SQLiteDbms::instance().showLastError();
		status = false;
	}
	if (!SQLiteDbms::instance().deleteFrom("ItemContent", { container_pair })) {
		MudLog(LogLevel::Error, "Remove from table 'ItemContent'.");
		SQLiteDbms::instance().showLastError();
		status = false;
	}
	if (!SQLiteDbms::instance().deleteFrom("ItemLiquidContent",
										   { container_pair })) {
		MudLog(LogLevel::Error, "Remove from table 'ItemLiquidContent'.");
		SQLiteDbms::instance().showLastError();
		status = false;
	}
	return status;
}

void Item::getSheet(Table &sheet) const
{
	// Add the columns.
	sheet.addColumn("Attribute", align::left);
	sheet.addColumn("Value", align::left);
	// Set the values.
	sheet.addRow({ "vnum", ToString(vnum) });
	sheet.addRow({ "type", this->getTypeName() });
	sheet.addRow({ "model", model->name });
	sheet.addRow({ "quantity", ToString(quantity) });
	sheet.addRow({ "maker", maker });
	sheet.addRow({ "condition", ToString(condition) + "/" +
									ToString(this->getMaxCondition()) });
	sheet.addRow({ "Material", composition->name });
	sheet.addRow({ "Quality", quality.toString() });
	sheet.addRow({ "Weight", ToString(this->getWeight(true)) });
	sheet.addRow({ "Price", ToString(this->getPrice(true)) });
	sheet.addRow({ "Flags", ToString(flags) });
	TableRow locationRow = { "Location" };
	if (room != nullptr) {
		locationRow.emplace_back(room->name);
	} else if (owner != nullptr) {
		locationRow.emplace_back(owner->getNameCapital());
	} else if (container != nullptr) {
		locationRow.emplace_back(container->getNameCapital() + " " +
								 ToString(container->vnum));
	} else {
		locationRow.emplace_back("Nowhere");
	}
	sheet.addRow(locationRow);
	for (auto bodyPart : occupiedBodyParts) {
		sheet.addRow({ "Body Part", bodyPart->getName() });
	}
	if (!content.empty()) {
		sheet.addDivider();
		sheet.addRow({ "Content", "Vnum" });
		for (auto iterator : content) {
			sheet.addRow(
				{ iterator->getNameCapital(), ToString(iterator->vnum) });
		}
		sheet.addDivider();
	}
	if (this->isAContainer() ||
		(model->getType() == ModelType::LiquidContainer)) {
		sheet.addRow({ "Free  Space", ToString(this->getFreeSpace()) });
		sheet.addRow({ "Used  Space", ToString(this->getUsedSpace()) });
		sheet.addRow({ "Total Space", ToString(this->getTotalSpace()) });
	}
}

bool Item::canDeconstruct(std::string &error) const
{
	if (!HasFlag(this->flags, ItemFlag::Built)) {
		error = "It is not built.";
		return false;
	}
	if (!this->isEmpty()) {
		error = "You must remove all the content first.";
		return false;
	}
	return true;
}

ModelType Item::getType() const
{
	return model->getType();
}

std::string Item::getTypeName() const
{
	return model->getTypeName();
}

bool Item::canStackWith(Item *item) const
{
	if (model == nullptr)
		return false;
	if (composition == nullptr)
		return false;
	if (item->model == nullptr)
		return false;
	if (item->composition == nullptr)
		return false;
	return HasFlag(model->modelFlags, ModelFlag::CanBeStacked) &&
		   (model->vnum == item->model->vnum) &&
		   (composition->vnum == item->composition->vnum);
}

Item *Item::removeFromStack(Character *actor, unsigned int &_quantity)
{
	if (this->quantity > _quantity) {
		// Generate a copty of this item with the given quantity.
		auto newStack = model->createItem(actor->getName(), composition, false,
										  quality, _quantity);
		if (newStack != nullptr) {
			// Actually reduce the quantity.
			this->quantity -= _quantity;
			// Update this item, since its quantity has changed.
			this->updateOnDB();
			// Return the new stack.
			return newStack;
		}
		return nullptr;
	}
	return this;
}

bool Item::hasKey(std::string key)
{
	for (auto iterator : model->keys) {
		// Get the name of the item.
		std::string name = iterator;
		// Replace the symbols inside name.
		model->replaceSymbols(name, composition, quality);
		if (BeginWith(name, key)) {
			return true;
		}
	}
	return false;
}

double Item::getDecayRate() const
{
	return ((1 / composition->hardness) / quality.getModifier());
}

void Item::triggerDecay()
{
	if (!HasFlag(model->modelFlags, ModelFlag::Unbreakable)) {
		condition -= this->getDecayRate();
		if (condition < 0) {
			// Take everything out from the item.
			if ((this->room != nullptr) && (!this->isEmpty())) {
				for (auto it : this->content) {
					this->room->addItem(it, true);
				}
			}
			// Add the item to the list of items that has to be destroyed.
			MudUpdater::instance().addItemToDestroy(this);
		}
	}
}

double Item::getConditionModifier() const
{
	auto percent = ((100 * condition) / this->getMaxCondition());
	if (percent >= 75)
		return 1.00;
	if (percent >= 50)
		return 0.75;
	if (percent >= 25)
		return 0.50;
	return 0.25;
}

std::string Item::getConditionDescription()
{
	auto percent = ((100 * condition) / this->getMaxCondition());
	if (percent >= 100)
		return "is in perfect condition";
	if (percent >= 75)
		return "is scratched";
	if (percent >= 50)
		return "is ruined";
	if (percent >= 25)
		return "is cracked";
	if (percent > 0)
		return "is almost broken";
	return "is broken";
}

unsigned int Item::getPrice(bool entireStack) const
{
	auto result = ((model->basePrice * quality.getModifier()) +
				   (model->basePrice * composition->worth) +
				   (model->basePrice * this->getConditionModifier())) /
				  3;
	// Evaluate for the entire stack.
	if (entireStack)
		result *= this->quantity;
	// Cast and return the result.
	return static_cast<unsigned int>(result);
}

double Item::getWeight(bool entireStack) const
{
	auto result = ((model->baseWeight * (1 / quality.getModifier())) +
				   (model->baseWeight * composition->lightness)) /
				  2;
	// Evaluate for the entire stack.
	if (entireStack)
		result *= this->quantity;
	// Get also the weight of the content.
	if (!this->isEmpty()) {
		for (auto iterator : content) {
			result += iterator->getWeight(true);
		}
	}
	// Cast and return the result.
	return RoundTo(result, 2);
}

unsigned int Item::getMaxCondition() const
{
	return static_cast<unsigned int>(
		(100 * (1 / quality.getModifier()) + 100 * composition->hardness) / 2);
}

std::string Item::getName(bool colored) const
{
	std::string itemName = model->getName(composition, quality);
	if (colored) {
		itemName = Formatter::cyan() + itemName + Formatter::reset();
	}
	return itemName;
}

std::string Item::getNameCapital(bool colored) const
{
	std::string itemName = model->getName(composition, quality);
	itemName[0] = static_cast<char>(toupper(itemName[0]));
	if (colored) {
		itemName = Formatter::cyan() + itemName + Formatter::reset();
	}
	return itemName;
}

std::string Item::getDescription()
{
	return model->getDescription(composition, quality);
}

std::string Item::getLook()
{
	auto Gray = [](const std::string &s) {
		return Formatter::gray() + s + Formatter::reset();
	};
	auto Yellow = [](const std::string &s) {
		return Formatter::yellow() + s + Formatter::reset();
	};
	std::string output;
	// Prepare : Name, Condition.
	//           Description.
	output = "You look at " + this->getName(true);
	output += ", it " + this->getConditionDescription() + ".\n";
	output += Gray(this->getDescription()) + "\n";
	// Print the content.
	output += this->lookContent();
	// Print the weight.
	if (this->quantity > 1) {
		output += this->getNameCapital(true) + " weights about ";
		output += Yellow(ToString(this->getWeight(false)));
		output += " " + Mud::instance().getWeightMeasure() + ".\n";
		output += "The stack weights about ";
	} else {
		output += "It weights about ";
	}
	output += Yellow(ToString(this->getWeight(true)));
	output += " " + Mud::instance().getWeightMeasure() + ".\n";
	return output;
}

std::string Item::lookContent()
{
	return "";
}

bool Item::isAContainer() const
{
	return false;
}

bool Item::isEmpty() const
{
	return (!this->isAContainer() || content.empty());
}

double Item::getTotalSpace() const
{
	return 0.0;
}

double Item::getUsedSpace() const
{
	auto used = 0.0;
	if (this->isAContainer()) {
		for (auto iterator : content) {
			used += iterator->getWeight(true);
		}
	}
	return used;
}

double Item::getFreeSpace() const
{
	if (this->isAContainer()) {
		return this->getTotalSpace() - this->getUsedSpace();
	}
	return 0.0;
}

bool Item::canContain(Item *item, const unsigned int &amount) const
{
	if (this->isAContainer()) {
		return (item->getWeight(false) * amount) <= this->getFreeSpace();
	}
	return false;
}

void Item::putInside(Item *&item, bool updateDB)
{
	static WhereClause prep_pair = std::make_pair("item", "-1");
	// Put the item inside the container.
	content.emplace_back_item(item);
	// Set the container value to the content item.
	item->container = this;
	// Update the database.
	if (updateDB && (this->getType() != ModelType::Corpse)) {
		SQLiteDbms::instance().insertInto(
			"ItemContent", { ToString(this->vnum), ToString(item->vnum) },
			false, true);
	}
	// Log it.
	MudLog(LogLevel::Debug, "Item '%s' added to '%s';", item->getName(),
		   this->getName());
}

bool Item::takeOut(Item *item, bool updateDB)
{
	if (!content.removeItem(item)) {
		return false;
	}
	// Set the container reference of the item to nullptr.
	item->container = nullptr;
	// Update the database.
	if (updateDB && (this->getType() != ModelType::Corpse)) {
		SQLiteDbms::instance().deleteFrom(
			"ItemContent", { std::make_pair("item", ToString(item->vnum)) });
	}
	// Log it.
	MudLog(LogLevel::Debug, "Item '%s' taken out from '%s';", item->getName(),
		   this->getName());
	return true;
}

Item *Item::findContent(std::string search_parameter, int &number)
{
	if (this->isAContainer()) {
		for (auto iterator : content) {
			if (iterator->hasKey(ToLower(search_parameter))) {
				if (number == 1) {
					return iterator;
				}
				number -= 1;
			}
		}
	}
	return nullptr;
}

void Item::setOccupiedBodyParts(
	std::vector<std::shared_ptr<BodyPart> > _occupiedBodyParts)
{
	// Set the new list of occupied body parts.
	occupiedBodyParts = _occupiedBodyParts;
}

bool Item::operator<(Item &rhs) const
{
	MudLog(LogLevel::Debug, "%s < %s", vnum, rhs.vnum);
	return getName() < rhs.getName();
}

void Item::updateTicImpl()
{
	// Nothing to do.
}

void Item::updateHourImpl()
{
	this->triggerDecay();
}
