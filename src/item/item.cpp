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
#include "../utils.hpp"
#include "../logger.hpp"
#include "../defines.hpp"
#include "../material.hpp"
#include "../constants.hpp"
#include "../luabridge/LuaBridge.h"
#include "../model/nodeModel.hpp"
#include "../model/containerModel.hpp"
#include "../model/liquidContainerModel.hpp"
#include "../model/shopModel.hpp"

#include "shopItem.hpp"

Item::Item() :
        vnum(),
        type(),
        model(),
        maker(),
        condition(),
        composition(),
        quality(ItemQuality::Normal),
        flags(),
        room(),
        owner(),
        container(),
        currentSlot(EquipmentSlot::None),
        content(),
        contentLiq(),
        customWeight()
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

ItemType Item::getType() const
{
    return ItemType::Generic;
}

std::string Item::getTypeName() const
{
    return "generic";
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

bool Item::destroy()
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
        Logger::log(LogLevel::Error, "Removing item '" + this->getName() + "' from DB;");
        // Remove the item from the database.
        if (!this->removeOnDB())
        {
            Logger::log(LogLevel::Error, "Something gone wrong during item removal from DB.");
            return false;
        }
    }
    // Delete the item.
    delete (this);
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
    arguments.push_back(ToString(this->condition));
    arguments.push_back(ToString(this->composition->vnum));
    arguments.push_back(EnumToString(this->quality));
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
        value.push_back(std::make_pair("quality", EnumToString(quality)));
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
    bool result = SQLiteDbms::instance().deleteFrom(
        "Item",
        { std::make_pair("vnum", ToString(vnum)) });
    if (!result)
    {
        Logger::log(LogLevel::Error, "Error during item removal from table Item.");
    }
    return result;
}

bool Item::loadFromDB()
{
    return true;
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

bool Item::triggerDecay()
{
    condition -= model->decay;
    if (condition <= 0)
    {
        return true;
    }
    return false;
}

std::string Item::getCondition()
{
    auto percent = (100 * this->condition) / model->condition;
    if (percent >= 100) return "is in perfect condition";
    else if (percent >= 75) return "is scratched";
    else if (percent >= 50) return "is ruined";
    else if (percent >= 25) return "is cracked";
    else if (percent >= 0) return "is almost broken";
    else return "is broken";
}

unsigned int Item::getWeight(bool withMaterial)
{
    unsigned int result = this->model->weight;
    if (this->customWeight != 0)
    {
        result = this->customWeight;
    }
    if (withMaterial)
    {
        // Add the addition weight due to the material.
        result += static_cast<unsigned int>(static_cast<double>(result / 100)
            * composition->lightness);
    }
    return result;
}

unsigned int Item::getTotalWeight()
{
    // Add the default weight of the model.
    unsigned int totalWeight = this->getWeight(true);
    if (!this->isEmpty())
    {
        for (auto iterator : content)
        {
            totalWeight += iterator->getTotalWeight();
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

std::string Item::getName()
{
    return model->getName(composition, quality);
}

std::string Item::getNameCapital()
{
    std::string itemName = getName();
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
    output += ", it " + this->getCondition() + ".\n";
    output += Formatter::gray() + this->getDescription() + Formatter::reset() + "\n";
    output += "\n";
    // Print the content.
    output += this->lookContent();
    output += "It weights about ";
    output += Formatter::yellow() + ToString(this->getTotalWeight()) + Formatter::reset();
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

bool Item::isAContainer()
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

bool Item::isEmpty()
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

unsigned int Item::getTotalSpace()
{
    if (model->getType() == ModelType::Container)
    {
        return model->toContainer()->maxWeight;
    }
    else if (model->getType() == ModelType::LiquidContainer)
    {
        return model->toLiquidContainer()->maxWeight;
    }
    else if (model->getType() == ModelType::Shop)
    {
        return model->toShop()->maxWeight;
    }
    else
    {
        return 0;
    }
}

unsigned int Item::getUsedSpace()
{
    unsigned int used = 0;
    if (this->isAContainer())
    {
        for (auto iterator : content)
        {
            used += iterator->model->weight;
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

unsigned int Item::getFreeSpace()
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

bool Item::putInside(Item * item)
{
    if (item->model->weight > this->getFreeSpace())
    {
        return false;
    }
    // Insert in the container table.
    std::vector<std::string> arguments;
    arguments.push_back(ToString(vnum));       // Container
    arguments.push_back(ToString(item->vnum)); // Content
    if (!SQLiteDbms::instance().insertInto("ItemContent", arguments))
    {
        return false;
    }
    // Put the item into the container.
    content.push_back(item);
    // Set the container value to the content item.
    item->container = this;
    return true;
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

bool Item::pourIn(Liquid * liquid, const unsigned int & quantity)
{
    if (quantity > this->getFreeSpace())
    {
        return false;
    }
    if (contentLiq.first == nullptr)
    {
        // Insert in the liquid table the value.
        std::vector<std::string> arguments;
        arguments.push_back(ToString(vnum));         // Container
        arguments.push_back(ToString(liquid->vnum)); // Content
        arguments.push_back(ToString(quantity));     // Quantity
        // Execute the insert.
        if (!SQLiteDbms::instance().insertInto("ItemContentLiq", arguments))
        {
            return false;
        }
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
    else
    {
        return false;
    }
    return true;
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

void Item::luaRegister(lua_State * L)
{
    luabridge::getGlobalNamespace(L) //
    .beginClass<Item>("Item") //
    .addData("vnum", &Item::vnum) //
    .addData("model", &Item::model) //
    .addFunction("getName", &Item::getName) //
    .addData("maker", &Item::maker) //
    .addData("condition", &Item::condition) //
    .addData("composition", &Item::composition) //
    .addData("quality", &Item::quality) //
    .addData("room", &Item::room) //
    .addData("owner", &Item::owner) //
    .addData("container", &Item::container) //
    .addData("container", &Item::container) //
    .addData("content", &Item::content) //
    .endClass();
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
    return first->getTotalWeight() < second->getTotalWeight();
}

std::string GetItemQualityName(ItemQuality quality)
{
    if (quality == ItemQuality::Disastrous) return "Disastrous";
    if (quality == ItemQuality::Poor) return "Poor";
    if (quality == ItemQuality::Normal) return "Normal";
    if (quality == ItemQuality::Fine) return "Fine";
    if (quality == ItemQuality::Masterful) return "Masterful";
    if (quality == ItemQuality::Fine) return "Fine";
    return "No Quality";
}
