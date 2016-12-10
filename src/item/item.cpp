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

#include "item.hpp"

#include "mud.hpp"
#include "logger.hpp"
#include "shopItem.hpp"
#include "lightItem.hpp"
#include "shopModel.hpp"
#include "armorItem.hpp"
#include "corpseItem.hpp"
#include "currencyItem.hpp"
#include "containerModel.hpp"
#include "meleeWeaponItem.hpp"
#include "rangedWeaponItem.hpp"
#include "liquidContainerModel.hpp"

Item::Item() :
    vnum(),
    model(),
    quantity(),
    maker(),
    price(),
    weight(),
    condition(),
    maxCondition(),
    composition(),
    quality(ItemQuality::Normal),
    flags(),
    room(),
    owner(),
    container(),
    currentSlot(EquipmentSlot::None),
    content(),
    contentLiq()
{
}

Item::~Item()
{
    //Logger::log(LogLevel::Debug, "Deleted item\t\t[%s]\t\t(%s)", ToString(this->vnum), this->getNameCapital());
}

bool Item::check(bool complete)
{
    bool safe = true;
    safe &= CorrectAssert(vnum > 0);
    safe &= CorrectAssert(model != nullptr);
    safe &= CorrectAssert(quantity > 0);
    safe &= CorrectAssert(!maker.empty());
    safe &= CorrectAssert(condition > 0);
    safe &= CorrectAssert(composition != nullptr);
    if (complete)
    {
        safe &= CorrectAssert(!((room != nullptr) && (owner != nullptr)));
        safe &= CorrectAssert(!((room != nullptr) && (container != nullptr)));
        safe &= CorrectAssert(!((owner != nullptr) && (container != nullptr)));
        safe &= CorrectAssert((room != nullptr) || (owner != nullptr) || (container != nullptr));
    }
    //safe &= CorrectAssert(currentSlot != EquipmentSlot::kNoEquipSlot);
    if (!safe)
    {
        Logger::log(LogLevel::Error, "Item :" + ToString(vnum));
    }
    return safe;
}

void Item::removeFromMud()
{
    Logger::log(LogLevel::Debug, "Removing %s from MUD...", this->getName());
    // Remove the item from the game, this means: Room, Player, Container.
    if (room != nullptr)
    {
        auto itemRoom = room;
        if (room->removeItem(this))
        {
            Logger::log(LogLevel::Debug, "Removing item '%s' from room '%s'.", this->getName(), itemRoom->name);
        }
    }
    if (owner != nullptr)
    {
        auto itemOwner = owner;
        if (owner->equipment.removeItem(this))
        {
            Logger::log(LogLevel::Debug, "Removing item '%s' from '%s' equipment.",
                        this->getName(), itemOwner->getName());
        }
        if (owner->inventory.removeItem(this))
        {
            Logger::log(LogLevel::Debug, "Removing item '%s' from '%s' inventory.", this->getName(),
                        itemOwner->getName());
        }
    }
    if (container != nullptr)
    {
        auto itemContainer = container;
        if (container->content.removeItem(this))
        {
            Logger::log(LogLevel::Debug, "Removing item '%s' from container '%s'.", this->getName(),
                        itemContainer->getName());
        }
    }
    if (this->model->getType() != ModelType::Corpse)
    {
        if (Mud::instance().remItem(this))
        {
            Logger::log(LogLevel::Debug, "Removing item '%s' from MUD items.", this->getName());
        }
    }
}

bool Item::updateOnDB()
{
    // Prepare the vector used to insert into the database.
    std::vector<std::string> arguments;
    arguments.push_back(ToString(this->vnum));
    arguments.push_back(ToString(model->vnum));
    arguments.push_back(ToString(this->quantity));
    arguments.push_back(this->maker);
    arguments.push_back(ToString(this->price));  // Save the basic value of price.
    arguments.push_back(ToString(this->weight)); // Save the weight of just the item.
    arguments.push_back(ToString(this->condition));
    arguments.push_back(ToString(this->maxCondition));
    arguments.push_back(ToString(this->composition->vnum));
    arguments.push_back(ToString(this->quality.toUInt()));
    arguments.push_back(ToString(this->flags));
    return SQLiteDbms::instance().insertInto("Item", arguments, false, true);
}

bool Item::removeOnDB()
{
    Logger::log(LogLevel::Debug, "Removing %s from DB...", this->getName());
    auto strVnum = ToString(vnum);
    // Prepare the where clause.
    if (SQLiteDbms::instance().deleteFrom("Item", {std::make_pair("vnum", strVnum)}))
    {
        // Remove the item from everywhere.
        SQLiteDbms::instance().deleteFrom("ItemPlayer", {std::make_pair("item", strVnum)});
        SQLiteDbms::instance().deleteFrom("ItemRoom", {std::make_pair("item", strVnum)});
        SQLiteDbms::instance().deleteFrom("ItemContent", {std::make_pair("item", strVnum)});
        return true;
    }
    Logger::log(LogLevel::Error, "Can't delete the item from the database!");
    SQLiteDbms::instance().showLastError();
    return false;
}

void Item::getSheet(Table & sheet) const
{
    // Add the columns.
    sheet.addColumn("Attribute", StringAlign::Left);
    sheet.addColumn("Value", StringAlign::Left);
    // Set the values.
    sheet.addRow({"vnum", ToString(vnum)});
    sheet.addRow({"type", this->getTypeName()});
    sheet.addRow({"model", model->name});
    sheet.addRow({"quantity", ToString(quantity)});
    sheet.addRow({"maker", maker});
    sheet.addRow({"condition", ToString(condition) + "/" + ToString(this->getMaxCondition())});
    sheet.addRow({"Material", composition->name});
    sheet.addRow({"Quality", quality.toString()});
    sheet.addRow({"Weight", ToString(this->getWeight(true))});
    sheet.addRow({"Price", ToString(this->getPrice(true))});
    sheet.addRow({"Flags", ToString(flags)});
    TableRow locationRow = {"Location"};
    if (room != nullptr)
    {
        locationRow.push_back(room->name);
    }
    else if (owner != nullptr)
    {
        locationRow.push_back(owner->getNameCapital());
    }
    else if (container != nullptr)
    {
        locationRow.push_back(container->getNameCapital() + " " + ToString(container->vnum));
    }
    else
    {
        locationRow.push_back("Nowhere");
    }
    sheet.addRow(locationRow);
    sheet.addRow({"Equipment Slot", currentSlot.toString()});
    if (!content.empty())
    {
        sheet.addDivider();
        sheet.addRow({"Content", "Vnum"});
        for (auto iterator : content)
        {
            sheet.addRow({iterator->getNameCapital(), ToString(iterator->vnum)});
        }
        sheet.addDivider();
    }
    if (model->getType() == ModelType::LiquidContainer)
    {
        sheet.addDivider();
        sheet.addRow({"Liquid", "Quantity"});
        sheet.addRow({contentLiq.first->getNameCapital(), ToString(contentLiq.second)});
        sheet.addDivider();
    }
    if (this->isAContainer() || (model->getType() == ModelType::LiquidContainer))
    {
        sheet.addRow({"Free  Space", ToString(this->getFreeSpace())});
        sheet.addRow({"Used  Space", ToString(this->getUsedSpace())});
        sheet.addRow({"Total Space", ToString(this->getTotalSpace())});
    }
}

bool Item::canDeconstruct(std::string & error) const
{
    if (!HasFlag(this->flags, ItemFlag::Built))
    {
        error = "It is not built.";
        return false;
    }
    if (!this->isEmpty())
    {
        error = "You must remove all the content first.";
        return false;
    }
    return true;
}

ModelType Item::getType() const
{
    return this->model->getType();
}

std::string Item::getTypeName() const
{
    return this->model->getTypeName();
}

bool Item::canStackWith(Item * item) const
{
    return HasFlag(this->model->modelFlags, ModelFlag::CanBeStacked)
           && (this->model->vnum == item->model->vnum)
           && (this->composition->vnum == item->composition->vnum);
}

Item * Item::removeFromStack(Character * actor, unsigned int & _quantity)
{
    if (this->quantity > _quantity)
    {
        // Generate a copty of this item with the given quantity.
        auto newStack = this->model->createItem(actor->getName(), this->composition, false, this->quality, _quantity);
        if (newStack != nullptr)
        {
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
    for (auto iterator : model->keys)
    {
        // Get the name of the item.
        std::string name = iterator;
        // Replace the symbols inside name.
        model->replaceSymbols(name, composition, quality);
        if (BeginWith(name, key))
        {
            return true;
        }
    }
    return false;
}

unsigned int Item::getMaxCondition() const
{
    return this->maxCondition;
}

void Item::triggerDecay()
{
    if (!HasFlag(model->modelFlags, ModelFlag::Unbreakable))
    {
        if (condition < model->decay)
        {
            condition = 0;
            // Take everything out from the item.
            if ((this->room != nullptr) && (!this->isEmpty()))
            {
                for (auto it: this->content)
                {
                    this->room->addItem(it, true);
                }
            }
            // Add the item to the list of items that has to be destroyed.
            MudUpdater::instance().addItemToDestroy(this);
        }
        else
        {
            condition -= model->decay;
        }
    }
}

double Item::getConditionModifier() const
{
    auto percent = ((100 * this->condition) / this->getMaxCondition());
    if (percent >= 75) return 1.00;
    if (percent >= 50) return 0.75;
    if (percent >= 25) return 0.50;
    return 0.25;
}

std::string Item::getConditionDescription()
{
    auto percent = ((100 * this->condition) / this->getMaxCondition());
    if (percent >= 100) return "is in perfect condition";
    if (percent >= 75) return "is scratched";
    if (percent >= 50) return "is ruined";
    if (percent >= 25) return "is cracked";
    if (percent > 0) return "is almost broken";
    return "is broken";
}

unsigned int Item::getPrice(bool entireStack) const
{
    // Add the base price.
    auto pcBase = this->price;
    // Evaluate the modifier due to item's condition.
    auto pcCondition = static_cast<unsigned int>(pcBase * this->getConditionModifier());
    // The resulting price.
    auto itemPrice = ((pcBase + pcCondition) / 2);
    // Evaluate for the entire stack.
    if (entireStack)
    {
        itemPrice *= this->quantity;
    }
    return itemPrice;
}

double Item::getWeight(bool entireStack) const
{
    // Add the default weight of the model.
    auto totalWeight = this->weight;
    // If not single, multiply for the quantity.
    if (entireStack)
    {
        totalWeight *= this->quantity;
    }
    if (!this->isEmpty())
    {
        for (auto iterator : content)
        {
            totalWeight += iterator->getWeight(true);
        }
    }
    else if (model->getType() == ModelType::LiquidContainer)
    {
        if (!this->isEmpty())
        {
            totalWeight += contentLiq.second;
        }
    }
    return totalWeight;
}

std::string Item::getName(bool colored) const
{
    std::string itemName = model->getName(composition, quality);
    if (colored)
    {
        itemName = Formatter::cyan() + itemName + Formatter::reset();
    }
    return itemName;
}

std::string Item::getNameCapital(bool colored) const
{
    std::string itemName = model->getName(composition, quality);
    itemName[0] = static_cast<char>(toupper(itemName[0]));
    if (colored)
    {
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
    std::string output;
    // Prepare : Name, Condition.
    //           Description.
    output = "You look at " + this->getName(true);
    output += ", it " + this->getConditionDescription() + ".\n";
    output += Formatter::gray() + this->getDescription() + Formatter::reset() + "\n";
    // Print the content.
    output += this->lookContent();
    if (this->quantity > 1)
    {
        output += this->getNameCapital(true) + " weights about ";
        output += Formatter::yellow() + ToString(this->getWeight(false)) + Formatter::reset();
        output += " " + Mud::instance().getWeightMeasure() + ".\n";
        output += "The stack weights about ";
        output += Formatter::yellow() + ToString(this->getWeight(true)) + Formatter::reset();
        output += " " + Mud::instance().getWeightMeasure() + ".\n";
    }
    else
    {
        output += "It weights about ";
        output += Formatter::yellow() + ToString(this->getWeight(true)) + Formatter::reset();
        output += " " + Mud::instance().getWeightMeasure() + ".\n";
    }
    return output;
}

bool Item::hasNodeType(NodeType nodeType)
{
    if (model == nullptr)
    {
        return false;
    }
    if (model->getType() != ModelType::Node)
    {
        return false;
    }
    return (model->toNode()->nodeType == nodeType);
}

bool Item::isAContainer() const
{
    if (model->getType() == ModelType::Container)
    {
        return true;
    }
    if (model->getType() == ModelType::Corpse)
    {
        return true;
    }
    return ((model->getType() == ModelType::Shop) && HasFlag(this->flags, ItemFlag::Built));
}

bool Item::isEmpty() const
{
    if (this->isAContainer() ||
        (model->getType() == ModelType::Magazine) ||
        (model->getType() == ModelType::RangedWeapon))
    {
        return content.empty();
    }
    if (model->getType() == ModelType::LiquidContainer)
    {
        return contentLiq.first == nullptr;
    }
    return true;
}

double Item::getTotalSpace() const
{
    if (model->getType() == ModelType::Container)
    {
        // Evaluate the base space.
        auto spBase = model->toContainer()->maxWeight;
        // Evaluate the modifier due to item's quality.
        auto spQuality = spBase * quality.getModifier();
        // Evaluate the result.
        return ((spBase + spQuality) / 2);
    }
    if (model->getType() == ModelType::LiquidContainer)
    {
        // Evaluate the base space.
        auto spBase = model->toLiquidContainer()->maxWeight;
        // Evaluate the modifier due to item's quality.
        auto spQuality = spBase * quality.getModifier();
        // Evaluate the result.
        return ((spBase + spQuality) / 2);
    }
    if (model->getType() == ModelType::Shop)
    {
        // Evaluate the base space.
        auto spBase = model->toShop()->maxWeight;
        // Evaluate the modifier due to item's quality.
        auto spQuality = spBase * quality.getModifier();
        // Evaluate the result.
        return ((spBase + spQuality) / 2);
    }
    return 0.0;
}

double Item::getUsedSpace() const
{
    auto used = 0.0;
    if (this->isAContainer())
    {
        for (auto iterator : content)
        {
            used += iterator->getWeight(true);
        }
    }
    else if (model->getType() == ModelType::LiquidContainer)
    {
        if (contentLiq.first != nullptr)
        {
            used += contentLiq.second;
        }
    }
    return used;
}

double Item::getFreeSpace() const
{
    auto totalSpace = this->getTotalSpace();
    auto usedSpace = this->getUsedSpace();
    if (totalSpace < usedSpace)
    {
        return 0.0;
    }
    return totalSpace - usedSpace;
}

bool Item::canContain(Item * item, const unsigned int & ammount) const
{
    return (item->getWeight(false) * ammount) <= this->getFreeSpace();
}

void Item::putInside(Item *& item, bool updateDB)
{
    // Put the item inside the container.
    content.push_back_item(item);
    // Set the container value to the content item.
    item->container = this;
    // Update the database.
    if (updateDB)
    {
        SQLiteDbms::instance().insertInto("ItemContent", {ToString(this->vnum), ToString(item->vnum)}, false, true);
    }
    // Log it.
    //Logger::log(LogLevel::Debug, "Item '%s' added to '%s';", item->getName(), this->getName());
}

bool Item::takeOut(Item * item, bool updateDB)
{
    if (!content.removeItem(item))
    {
        return false;
    }
    // Set the container reference of the item to nullptr.
    item->container = nullptr;
    // Update the database.
    if (updateDB)
    {
        SQLiteDbms::instance().deleteFrom("ItemContent", {std::make_pair("item", ToString(item->vnum))});
    }
    // Log it.
    //Logger::log(LogLevel::Debug, "Item '%s' taken out from '%s';", item->getName(), this->getName());
    return true;
}

bool Item::canContainLiquid(Liquid * liquid, const double & ammount) const
{
    if (ammount > this->getFreeSpace())
    {
        return false;
    }
    if (contentLiq.first != liquid)
    {
        return false;
    }
    return true;
}

bool Item::pourIn(Liquid * liquid, const double & ammount, bool updateDB)
{
    if (this->canContainLiquid(liquid, ammount))
    {
        if (contentLiq.first == nullptr)
        {
            // Set the liquid ammount.
            contentLiq.first = liquid;
            contentLiq.second = ammount;
        }
        else if (contentLiq.first == liquid)
        {
            // Increment the liquid ammount.
            contentLiq.second += ammount;
        }
        // Prepare the query arguments.
        if (updateDB)
        {
            SQLiteDbms::instance().insertInto("ItemContentLiq",
                                              {ToString(vnum), ToString(contentLiq.first->vnum),
                                               ToString(contentLiq.second)},
                                              false,
                                              true);
        }
        return true;
    }
    return false;
}

bool Item::pourOut(const double & ammount, bool updateDB)
{
    if (model->getType() != ModelType::LiquidContainer)
    {
        return false;
    }
    // If the item has an Endless provision of liquid, don't do any check.
    if (HasFlag(model->toLiquidContainer()->liquidFlags, LiqContainerFlag::Endless))
    {
        return true;
    }
    // Check if the container has the necessary ammount of liquid.
    if (contentLiq.second >= ammount)
    {
        // Decrement the liquid ammount.
        contentLiq.second -= ammount;
        if (updateDB)
        {
            // Check if the quantity has dropped to zero.
            if (contentLiq.second < 0.1)
            {
                QueryList where;
                where.push_back(std::make_pair("container", ToString(vnum)));
                where.push_back(std::make_pair("content", ToString(contentLiq.first->vnum)));
                // If the container is empty, remove the entry from the liquid contained table.
                SQLiteDbms::instance().deleteFrom("ItemContentLiq", where);
                // Erase the key of the liquid.
                contentLiq.first = nullptr;
            }
            else
            {
                SQLiteDbms::instance().insertInto(
                    "ItemContentLiq",
                    {ToString(vnum), ToString(contentLiq.first->vnum), ToString(contentLiq.second)},
                    false,
                    true);
            }
        }
        return true;
    }
    return false;
}

Item * Item::findContent(std::string search_parameter, int & number)
{
    if (this->isEmpty())
    {
        return nullptr;
    }
    for (auto iterator : content)
    {
        if (iterator->hasKey(ToLower(search_parameter)))
        {
            if (number == 1)
            {
                return iterator;
            }
            number -= 1;
        }
    }
    return nullptr;
}

std::string Item::lookContent()
{
    std::string output;
    if (this->isAContainer())
    {
        if (HasFlag(this->flags, ItemFlag::Closed))
        {
            output += Formatter::italic() + "It is closed.\n" + Formatter::reset();
            if (!HasFlag(this->model->modelFlags, ModelFlag::CanSeeThrough))
            {
                return output + "\n";
            }
        }
        if (content.empty())
        {
            output += Formatter::italic() + "It's empty.\n" + Formatter::reset();
        }
        else
        {
            output += "Looking inside you see:\n";
            Table table = Table();
            table.addColumn("Item", StringAlign::Left);
            table.addColumn("Quantity", StringAlign::Right);
            table.addColumn("Weight", StringAlign::Right);
            // List all the items in inventory
            for (auto it : content)
            {
                table.addRow(
                    {it->getNameCapital(), ToString(it->quantity), ToString(it->getWeight(true))});
            }
            output += table.getTable();
            output += "Has been used " + Formatter::yellow() + ToString(getUsedSpace())
                      + Formatter::reset();
            output += " out of " + Formatter::yellow() + ToString(getTotalSpace())
                      + Formatter::reset() + " " + Mud::instance().getWeightMeasure() + ".\n";
        }
    }
    else if (model->getType() == ModelType::Magazine)
    {
        if (content.empty())
        {
            output += Formatter::italic() + "It does not contain any projectiles.\n" + Formatter::reset();
        }
        else
        {
            auto loadedProjectiles = content.front();
            if (loadedProjectiles != nullptr)
            {
                output += Formatter::italic();
                output += "It contains " + loadedProjectiles->getName(true) + "[";
                output += ToString(loadedProjectiles->quantity) + "].\n";
                output += Formatter::reset();
            }
        }
    }
    else if (model->getType() == ModelType::RangedWeapon)
    {
        if (content.empty())
        {
            output += Formatter::italic() + "It does not contain any magazine.\n" + Formatter::reset();
        }
        else
        {
            auto containedMagazine = content.front();
            if (containedMagazine != nullptr)
            {
                output += Formatter::italic();
                output += "It is loaded with " + containedMagazine->getName(true) + "\n";
                output += Formatter::reset();
            }
        }
    }
    else if (model->getType() == ModelType::LiquidContainer)
    {
        if (HasFlag(this->flags, ItemFlag::Closed))
        {
            output += Formatter::italic() + "It is closed.\n" + Formatter::reset();
            if (!HasFlag(this->model->modelFlags, ModelFlag::CanSeeThrough))
            {
                return output + "\n";
            }
        }
        if (contentLiq.first == nullptr)
        {
            output += Formatter::italic() + "It does not contain any liquid.\n"
                      + Formatter::reset();
        }
        else
        {
            int percent = 0;
            if (HasFlag(this->model->toLiquidContainer()->liquidFlags, LiqContainerFlag::Endless))
            {
                percent = 100;
            }
            else if (getUsedSpace() > 0)
            {
                percent = static_cast<int>((100.0 * this->getUsedSpace()) / this->getTotalSpace());
            }

            if (percent >= 100) output += "It's full of ";
            else if (percent >= 75) output += "It's half full of ";
            else if (percent >= 50) output += "Contains a discrete ammount of ";
            else if (percent >= 25) output += "It contains a little bit of ";
            else if (percent >= 0) output += "It contains some drops of ";
            else output += "It's empty, but you can see some ";
            output += Formatter::cyan() + contentLiq.first->getName() + Formatter::reset() + ".\n";
        }
    }
    return output;
}

void Item::setCurrentSlot(EquipmentSlot _currentSlot)
{
    currentSlot = _currentSlot;
}

EquipmentSlot Item::getCurrentSlot()
{
    if (currentSlot == EquipmentSlot::None)
    {
        return model->slot;
    }
    return currentSlot;
}

std::string Item::getCurrentSlotName()
{
    return getCurrentSlot().toString();
}

ShopItem * Item::toShopItem()
{
    return static_cast<ShopItem *>(this);
}

ArmorItem * Item::toArmorItem()
{
    return static_cast<ArmorItem *>(this);
}

MeleeWeaponItem * Item::toMeleeWeaponItem()
{
    return static_cast<MeleeWeaponItem *>(this);
}

RangedWeaponItem * Item::toRangedWeaponItem()
{
    return static_cast<RangedWeaponItem *>(this);
}

CurrencyItem * Item::toCurrencyItem()
{
    return static_cast<CurrencyItem *>(this);
}

CorpseItem * Item::toCorpseItem()
{
    return static_cast<CorpseItem *>(this);
}

MagazineItem * Item::toMagazineItem()
{
    return static_cast<MagazineItem *>(this);
}

LightItem * Item::toLightItem()
{
    return static_cast<LightItem *>(this);
}

void Item::luaRegister(lua_State * L)
{
    luabridge::getGlobalNamespace(L)
        .beginClass<Item>("Item")
        .addData("vnum", &Item::vnum)
        .addData("model", &Item::model)
        .addFunction("getName", &Item::getName)
        .addFunction("hasKey", &Item::hasKey)
        .addFunction("getType", &Item::getType)
        .addFunction("getTypeName", &Item::getTypeName)
        .addData("maker", &Item::maker)
        .addData("condition", &Item::condition)
        .addData("weight", &Item::weight)
        .addData("price", &Item::price)
        .addData("composition", &Item::composition)
        .addData("room", &Item::room)
        .addData("owner", &Item::owner)
        .addData("container", &Item::container)
        .addData("container", &Item::container)
        .endClass()
        .deriveClass<ArmorItem, Item>("ArmorItem")
        .addFunction("getAC", &ArmorItem::getArmorClass)
        .endClass()
        .deriveClass<CorpseItem, Item>("CorpseItem")
        .endClass()
        .deriveClass<CurrencyItem, Item>("CurrencyItem")
        .endClass()
        .deriveClass<ShopItem, Item>("ShopItem")
        .endClass()
        .deriveClass<MeleeWeaponItem, Item>("MeleeWeaponItem")
        .endClass()
        .deriveClass<RangedWeaponItem, Item>("RangedWeaponItem")
        .endClass();
}

bool Item::operator<(Item & rhs) const
{
    Logger::log(LogLevel::Debug, "%s < %s", ToString(this->vnum), ToString(rhs.vnum));
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
