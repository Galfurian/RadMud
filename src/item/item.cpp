/// @file   item.cpp
/// @brief  Implements item methods.
/// @author Enrico Fraccaroli
/// @date   Aug 25 2014
/// @copyright
/// Copyright (c) 2014, 2015, 2016 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
/// Permission to use, copy, modify, and distribute this software for any
/// purpose with or without fee is hereby granted, provided that the above
/// copyright notice and this permission notice appear in all copies.
///
/// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
/// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
/// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
/// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
/// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
/// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
/// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

// Basic Include.
#include "item.hpp"

#include <algorithm>
#include <sstream>

// Other Include.
#include "../mud.hpp"
#include "../defines.hpp"
#include "../material.hpp"
#include "../constants.hpp"
#include "../luabridge/LuaBridge.h"
#include "../model/nodeModel.hpp"
#include "../model/containerModel.hpp"
#include "../model/liquidContainerModel.hpp"
#include "../model/shopModel.hpp"
#include "../utilities/logger.hpp"

#include "shopItem.hpp"
#include "armorItem.hpp"
#include "weaponItem.hpp"
#include "currencyItem.hpp"

Item::Item() :
        vnum(),
        type(),
        model(),
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
    Logger::log(
        LogLevel::Debug,
        "Deleted item\t\t[%s]\t\t(%s)",
        ToString(this->vnum),
        this->getNameCapital());
}

bool Item::check(bool complete)
{
    bool safe = true;
    safe &= CorrectAssert(vnum > 0);
    safe &= CorrectAssert(model != nullptr);
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

bool Item::removeFromMud()
{
    // Remove the item from the game, this means: Room, Player, Container.
    if (room != nullptr)
    {
        Logger::log(LogLevel::Debug, "Removing item from room.");
        if (!room->removeItem(this))
        {
            return false;
        }
    }
    else if (owner != nullptr)
    {
        Logger::log(LogLevel::Debug, "Removing item from owner.");
        if (!owner->remInventoryItem(this))
        {
            if (!owner->remEquipmentItem(this))
            {
                return false;
            }
        }
    }
    else if (container != nullptr)
    {
        Logger::log(LogLevel::Debug, "Removing item from container.");
        if (!container->takeOut(this))
        {
            return false;
        }
    }
    else
    {
        return false;
    }

    if (this->model->getType() == ModelType::Corpse)
    {
        Logger::log(LogLevel::Error, "Removing corpse '" + this->getName() + "' from MUD;");
        // Remove the item from the list of corpses.
        if (!Mud::instance().remCorpse(this))
        {
            Logger::log(LogLevel::Error, "Something gone wrong during corpse removal from mud.");
            return false;
        }
    }
    else
    {
        Logger::log(LogLevel::Error, "Removing item '" + this->getName() + "' from MUD;");
        // Remove the item from the mud.
        if (!Mud::instance().remItem(this))
        {
            Logger::log(LogLevel::Error, "Something gone wrong during item removal from mud.");
            return false;
        }
    }
    return true;
}

bool Item::destroy()
{
    if (this->removeFromMud())
    {
        if (this->model->getType() != ModelType::Corpse)
        {
            Logger::log(LogLevel::Error, "Removing item '" + this->getName() + "' from DB;");
            // Remove the item from the database.
            if (!this->removeOnDB())
            {
                Logger::log(LogLevel::Error, "Something gone wrong during item removal from DB.");
                return false;
            }
        }
    }
    return true;
}

bool Item::createOnDB()
{
    // Check if the Item can be saved on the database.
    if (model->getType() == ModelType::Corpse)
    {
        return true;
    }
    // Prepare the vector used to insert into the database.
    std::vector<std::string> arguments;
    arguments.push_back(ToString(this->vnum));
    arguments.push_back(ToString(model->vnum));
    arguments.push_back(this->maker);
    arguments.push_back(ToString(this->getPrice()));
    arguments.push_back(ToString(this->getWeight()));
    arguments.push_back(ToString(this->condition));
    arguments.push_back(ToString(this->maxCondition));
    arguments.push_back(ToString(this->composition->vnum));
    arguments.push_back(ToString(this->quality.toUInt()));
    arguments.push_back(ToString(this->flags));
    if (SQLiteDbms::instance().insertInto("Item", arguments))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Item::updateOnDB()
{
    // Check if the Item can be saved on the database.
    if (model->getType() == ModelType::Corpse)
    {
        return true;
    }

    // Save the item's Information.
    {
        QueryList value;
        value.push_back(std::make_pair("model", ToString(model->vnum)));
        value.push_back(std::make_pair("maker", maker));
        value.push_back(std::make_pair("condition", ToString(condition)));
        value.push_back(std::make_pair("composition", ToString(composition->vnum)));
        value.push_back(std::make_pair("quality", ToString(quality.toUInt())));
        value.push_back(std::make_pair("flag", ToString(flags)));
        QueryList where;
        where.push_back(std::make_pair("vnum", ToString(vnum)));
        if (!SQLiteDbms::instance().updateInto("Item", value, where))
        {
            Logger::log(LogLevel::Error, "Can't save the item!");
            return false;
        }
    }

    if ((room != nullptr) || (owner != nullptr) || (container != nullptr))
    {
        // Remove the item from any auxiliary table.
        QueryList where;
        where.push_back(std::make_pair("item", ToString(vnum)));
        SQLiteDbms::instance().deleteFrom("ItemPlayer", where);
        SQLiteDbms::instance().deleteFrom("ItemRoom", where);
        SQLiteDbms::instance().deleteFrom("ItemContent", where);
    }

    // Save the item's position.
    if (room != nullptr)
    {
        std::vector<std::string> arguments;
        arguments.push_back(ToString(room->vnum));
        arguments.push_back(ToString(vnum));
        if (!SQLiteDbms::instance().insertInto("ItemRoom", arguments))
        {
            Logger::log(LogLevel::Error, "Can't save the item inside the room!");
            return false;
        }
    }
    else if (owner != nullptr)
    {
        if (owner->isPlayer())
        {
            std::vector<std::string> arguments;
            arguments.push_back(owner->name);
            arguments.push_back(ToString(vnum));
            if (owner->hasEquipmentItem(this))
            {
                arguments.push_back(EnumToString(this->getCurrentSlot()));
            }
            else
            {
                arguments.push_back("0");
            }
            if (!SQLiteDbms::instance().insertInto("ItemPlayer", arguments, false, true))
            {
                Logger::log(LogLevel::Error, "Can't save the item possesed by the Player!");
                return false;
            }
        }
    }
    else if (container != nullptr)
    {
        if (container->model->getType() != ModelType::Corpse)
        {
            std::vector<std::string> arguments;
            arguments.push_back(ToString(container->vnum));
            arguments.push_back(ToString(vnum));
            if (!SQLiteDbms::instance().insertInto("ItemContent", arguments))
            {
                Logger::log(LogLevel::Error, "Can't save the item inside the container!");
                return false;
            }
        }
    }

    if (!content.empty())
    {
        for (auto iterator : content)
        {
            std::vector<std::string> arguments;
            // Prepare the query arguments.
            arguments.push_back(ToString(vnum));
            arguments.push_back(ToString(iterator->vnum));
            if (!SQLiteDbms::instance().insertInto("ItemContent", arguments))
            {
                Logger::log(LogLevel::Error, "Can't save the contained item!");
                return false;
            }
        }
    }

    if (contentLiq.first != nullptr)
    {
        std::vector<std::string> arguments;
        // Prepare the query arguments.
        arguments.push_back(ToString(vnum));
        arguments.push_back(ToString(contentLiq.first->vnum));
        arguments.push_back(ToString(contentLiq.second));
        if (!SQLiteDbms::instance().insertInto("ItemContentLiq", arguments, false, true))
        {
            Logger::log(LogLevel::Error, "Can't save the contained liquid!");
            return false;
        }
    }
    return true;
}

bool Item::removeOnDB()
{
    // Prepare the where clause.
    QueryList main = { std::make_pair("vnum", ToString(vnum)) };
    auto result = SQLiteDbms::instance().deleteFrom("Item", main);
    // Remove the item from everywhere.
    QueryList other = { std::make_pair("item", ToString(vnum)) };
    SQLiteDbms::instance().deleteFrom("ItemPlayer", other);
    SQLiteDbms::instance().deleteFrom("ItemRoom", other);
    SQLiteDbms::instance().deleteFrom("ItemContent", other);
    SQLiteDbms::instance().deleteFrom("Shop", other);
    if (!result)
    {
        Logger::log(LogLevel::Error, "Error during item removal from table Item.");
    }
    return result;
}

void Item::getSheet(Table & sheet) const
{
    // Add the columns.
    sheet.addColumn("Attribute", StringAlign::Left);
    sheet.addColumn("Value", StringAlign::Left);
    // Set the values.
    sheet.addRow( { "vnum", ToString(vnum) });
    sheet.addRow( { "type", this->getTypeName() });
    sheet.addRow( { "model", model->name });
    sheet.addRow( { "maker", maker });
    sheet.addRow( { "condition", ToString(condition) + "/" + ToString(this->getMaxCondition()) });
    sheet.addRow( { "Material", composition->name });
    sheet.addRow( { "Quality", quality.toString() });
    sheet.addRow( { "Weight", ToString(this->getWeight()) });
    sheet.addRow( { "Price", ToString(this->getPrice()) });
    sheet.addRow( { "Flags", ToString(flags) });
    TableRow locationRow = { "Location" };
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
    sheet.addRow( { "Equipment Slot", GetEquipmentSlotName(currentSlot) });
    if (!content.empty())
    {
        sheet.addDivider();
        sheet.addRow( { "Content", "Vnum" });
        for (auto iterator : content)
        {
            sheet.addRow( { iterator->getNameCapital(), ToString(iterator->vnum) });
        }
        sheet.addDivider();
    }
    if (model->getType() == ModelType::LiquidContainer)
    {
        sheet.addDivider();
        sheet.addRow( { "Liquid", "Quantity" });
        sheet.addRow( { contentLiq.first->getNameCapital(), ToString(contentLiq.second) });
        sheet.addDivider();
    }
    if (this->isAContainer() || (model->getType() == ModelType::LiquidContainer))
    {
        sheet.addRow( { "Free  Space", ToString(this->getFreeSpace()) });
        sheet.addRow( { "Used  Space", ToString(this->getUsedSpace()) });
        sheet.addRow( { "Total Space", ToString(this->getTotalSpace()) });
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

bool Item::triggerDecay()
{
    if (condition < model->decay)
    {
        return true;
    }
    else
    {
        condition -= model->decay;
        return false;
    }
}

double Item::getConditionModifier() const
{
    auto percent = ((100 * this->condition) / this->getMaxCondition());
    if (percent >= 75) return 1.00;
    else if (percent >= 50) return 0.75;
    else if (percent >= 25) return 0.50;
    else return 0.25;
}

std::string Item::getConditionDescription()
{
    auto percent = ((100 * this->condition) / this->getMaxCondition());
    if (percent >= 100) return "is in perfect condition";
    else if (percent >= 75) return "is scratched";
    else if (percent >= 50) return "is ruined";
    else if (percent >= 25) return "is cracked";
    else if (percent > 0) return "is almost broken";
    else return "is broken";
}

unsigned int Item::getPrice() const
{
    // Add the base price.
    auto pcBase = this->price;
    // Evaluate the modifier due to item's condition.
    auto pcCondition = static_cast<unsigned int>(pcBase * this->getConditionModifier());
    // The resulting price.
    return ((pcBase + pcCondition) / 2);
}

unsigned int Item::getWeight() const
{
    // Add the default weight of the model.
    unsigned int totalWeight = this->weight;
    if (!this->isEmpty())
    {
        for (auto iterator : content)
        {
            totalWeight += iterator->getWeight();
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

std::string Item::getName() const
{
    return model->getName(composition, quality);
}

std::string Item::getNameCapital() const
{
    std::string itemName = this->getName();
    itemName[0] = static_cast<char>(toupper(itemName[0]));
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
    output = "You look at " + Formatter::cyan() + this->getName() + Formatter::reset();
    output += ", it " + this->getConditionDescription() + ".\n";
    output += Formatter::gray() + this->getDescription() + Formatter::reset() + "\n";
    output += "\n";
    // Print the content.
    output += this->lookContent();
    output += "It weights about ";
    output += Formatter::yellow() + ToString(this->getWeight()) + Formatter::reset();
    output += " " + mud_measure + ".\n";
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
    if (model->toNode()->nodeType != nodeType)
    {
        return false;
    }
    return true;
}

bool Item::isAContainer() const
{
    if (model->getType() == ModelType::Container)
    {
        return true;
    }
    else if (model->getType() == ModelType::Corpse)
    {
        return true;
    }
    else if (model->getType() == ModelType::Shop)
    {
        if (HasFlag(this->flags, ItemFlag::Built))
        {
            return true;
        }
    }
    return false;
}

bool Item::isEmpty() const
{
    if (this->isAContainer())
    {
        return (content.size() == 0);
    }
    else if (model->getType() == ModelType::LiquidContainer)
    {
        return (contentLiq.first == nullptr);
    }
    else
    {
        return true;
    }
}

unsigned int Item::getTotalSpace() const
{
    if (model->getType() == ModelType::Container)
    {
        // Evaluate the base space.
        auto spBase = model->toContainer()->maxWeight;
        // Evaluate the modifier due to item's quality.
        auto spQuality = static_cast<unsigned int>(spBase * quality.getModifier());
        // Evaluate the result.
        return ((spBase + spQuality) / 2);
    }
    else if (model->getType() == ModelType::LiquidContainer)
    {
        // Evaluate the base space.
        auto spBase = model->toLiquidContainer()->maxWeight;
        // Evaluate the modifier due to item's quality.
        auto spQuality = static_cast<unsigned int>(spBase * quality.getModifier());
        // Evaluate the result.
        return ((spBase + spQuality) / 2);
    }
    else if (model->getType() == ModelType::Shop)
    {
        // Evaluate the base space.
        auto spBase = model->toShop()->maxWeight;
        // Evaluate the modifier due to item's quality.
        auto spQuality = static_cast<unsigned int>(spBase * quality.getModifier());
        // Evaluate the result.
        return ((spBase + spQuality) / 2);
    }
    else
    {
        return 0;
    }
}

unsigned int Item::getUsedSpace() const
{
    unsigned int used = 0;
    if (this->isAContainer())
    {
        for (auto iterator : content)
        {
            used += iterator->getWeight();
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

unsigned int Item::getFreeSpace() const
{
    unsigned int totalSpace = this->getTotalSpace();
    unsigned int usedSpace = this->getUsedSpace();
    if (totalSpace < usedSpace)
    {
        return 0;
    }
    else
    {
        return totalSpace - usedSpace;
    }
}

bool Item::canContain(Item * item) const
{
    return (item->getWeight() <= this->getFreeSpace());
}

bool Item::putInside(Item * item)
{
    if (this->canContain(item))
    {
        // Put the item into the container.
        content.push_back(item);
        // Set the container value to the content item.
        item->container = this;
        return true;
    }
    return false;
}

bool Item::takeOut(Item * item)
{
    bool removed = false;
    for (auto it = content.begin(); it != content.end(); ++it)
    {
        Item * containedItem = (*it);
        if (containedItem->vnum == item->vnum)
        {
            Logger::log(
                LogLevel::Debug,
                "Item '%s' taken out from '%s';",
                item->getName(),
                this->getName());
            item->container = nullptr;
            content.erase(it);
            removed = true;
            break;
        }
    }
    return removed;
}

bool Item::canContain(Liquid * liquid, const unsigned int & quantity) const
{
    if (quantity > this->getFreeSpace())
    {
        return false;
    }
    else if (contentLiq.first != liquid)
    {
        return false;
    }
    return true;
}

bool Item::pourIn(Liquid * liquid, const unsigned int & quantity)
{
    if (this->canContain(liquid, quantity))
    {
        if (contentLiq.first == nullptr)
        {
            // Set the liquid quantity.
            contentLiq.first = liquid;
            contentLiq.second = quantity;
        }
        else if (contentLiq.first == liquid)
        {
            QueryList value;
            value.push_back(std::make_pair("quantity", ToString(contentLiq.second)));
            QueryList where;
            where.push_back(std::make_pair("container", ToString(vnum)));
            where.push_back(std::make_pair("content", ToString(liquid->vnum)));
            // Update value inside the database.
            if (!SQLiteDbms::instance().updateInto("ItemContentLiq", value, where))
            {
                return false;
            }
            // Increment the liquid quantity.
            contentLiq.second += quantity;
        }
        return true;
    }
    return false;
}

bool Item::pourOut(const unsigned int & quantity)
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
    // Check if the container has the necessary quantity of liquid.
    if (contentLiq.second >= quantity)
    {
        // Decrement the liquid quantity.
        contentLiq.second -= quantity;
        if (contentLiq.second == 0)
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
            QueryList value;
            value.push_back(std::make_pair("quantity", ToString(contentLiq.second)));
            QueryList where;
            where.push_back(std::make_pair("container", ToString(vnum)));
            where.push_back(std::make_pair("content", ToString(contentLiq.first->vnum)));
            // Update value inside the database.
            // TODO: Check.
            SQLiteDbms::instance().updateInto("ItemContentLiq", value, where);
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
            for (auto it : GroupItems(content))
            {
                std::string contentName = it.first->getNameCapital();
                if (it.second > 1)
                {
                    output += " - " + Formatter::cyan() + contentName + Formatter::reset() + " ["
                        + ToString(it.second) + "].\n";
                }
                else
                {
                    output += " - " + Formatter::cyan() + contentName + Formatter::reset() + ".\n";
                }
            }
            output += "Has been used " + Formatter::yellow() + ToString(getUsedSpace())
                + Formatter::reset();
            output += " out of " + Formatter::yellow() + ToString(getTotalSpace())
                + Formatter::reset() + " " + mud_measure + ".\n";
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
                percent = (int) ((100.0 * (double) (getUsedSpace())) / (double) (getTotalSpace()));
            }

            if (percent >= 100) output += "It's full of ";
            else if (percent >= 75) output += "It's half full of ";
            else if (percent >= 50) output += "Contains a discrete quantity of ";
            else if (percent >= 25) output += "It contains a little bit of ";
            else if (percent >= 0) output += "It contains some drops of ";
            else output += "It's empty, but you can see some ";
            output += Formatter::cyan() + contentLiq.first->getName() + Formatter::reset() + ".\n";
        }
    }
    output += "\n";
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
    else
    {
        return currentSlot;
    }
}

std::string Item::getCurrentSlotName()
{
    return GetEquipmentSlotName(getCurrentSlot());
}

ShopItem * Item::toShopItem()
{
    return static_cast<ShopItem *>(this);
}
ArmorItem * Item::toArmorItem()
{
    return static_cast<ArmorItem *>(this);
}
WeaponItem * Item::toWeaponItem()
{
    return static_cast<WeaponItem *>(this);
}
CurrencyItem * Item::toCurrencyItem()
{
    return static_cast<CurrencyItem *>(this);
}

void Item::luaRegister(lua_State * L)
{
    luabridge::getGlobalNamespace(L) //
    .beginClass<Item>("Item") //
    .addData("vnum", &Item::vnum) //
    .addData("model", &Item::model) //
    .addFunction("getName", &Item::getName) //
    .addData("maker", &Item::maker) //
    .addData("condition", &Item::condition) //
    .addData("weight", &Item::weight) //
    .addData("price", &Item::price) //
    .addData("composition", &Item::composition) //
    .addData("room", &Item::room) //
    .addData("owner", &Item::owner) //
    .addData("container", &Item::container) //
    .addData("container", &Item::container) //
    .addData("content", &Item::content) //
    .endClass();
}

bool Item::operator<(Item & rhs) const
{
    Logger::log(LogLevel::Debug, "%s < %s", ToString(this->vnum), ToString(rhs.vnum));
    return getName() < rhs.getName();
}

Item * GenerateItem(const ModelType & type)
{
    switch (type)
    {
        case ModelType::Armor:
            return new ArmorItem();
        case ModelType::Shop:
            return new ShopItem();
        case ModelType::Weapon:
            return new WeaponItem();
        case ModelType::Currency:
            return new CurrencyItem();
        case ModelType::Corpse:
        case ModelType::Book:
        case ModelType::Container:
        case ModelType::Food:
        case ModelType::Furniture:
        case ModelType::Key:
        case ModelType::Light:
        case ModelType::LiquidContainer:
        case ModelType::Mechanism:
        case ModelType::Node:
        case ModelType::Projectile:
        case ModelType::Resource:
        case ModelType::Rope:
        case ModelType::Seed:
        case ModelType::Shield:
        case ModelType::Tool:
        case ModelType::Vehicle:
            return new Item();
        case ModelType::NoType:
            return nullptr;
        default:
            return nullptr;
    }
}

ItemVectorNumbered GroupItems(const ItemVector & items)
{
    ItemVectorNumbered numberedItems;
    for (auto it : items)
    {
        bool missing = true;
        for (auto it2 = numberedItems.begin(); it2 != numberedItems.end(); ++it2)
        {
            if (it2->first->getName() == it->getName())
            {
                if (HasFlag(it2->first->flags, ItemFlag::Built))
                {
                    if (HasFlag(it->flags, ItemFlag::Built))
                    {
                        missing = false;
                        it2->second++;
                        break;
                    }
                }
                else
                {
                    missing = false;
                    it2->second++;
                    break;
                }
            }
        }
        if (missing)
        {
            numberedItems.push_back(std::make_pair(it, 1));
        }
    }
    return numberedItems;
}

bool OrderItemByName(Item * first, Item * second)
{
    return first->getName() < second->getName();
}

bool OrderItemByWeight(Item * first, Item * second)
{
    return first->getWeight() < second->getWeight();
}
