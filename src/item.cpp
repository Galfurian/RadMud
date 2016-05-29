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
#include "mud.hpp"
#include "utils.hpp"
#include "logger.hpp"
#include "defines.hpp"
#include "material.hpp"
#include "constants.hpp"
#include "luabridge/LuaBridge.h"

using namespace std;

Item::Item() :
        vnum(),
        model(),
        maker(),
        condition(),
        composition(),
        quality(ItemQuality::None),
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
    Logger::log(LogLevel::Debug, "Deleted: Item.");
}

bool Item::check(bool complete)
{
    bool safe = true;
    safe &= SafeAssert(vnum > 0);
    safe &= SafeAssert(model != nullptr);
    safe &= SafeAssert(!maker.empty());
    safe &= SafeAssert(condition > 0);
    safe &= SafeAssert(composition != nullptr);
    safe &= SafeAssert(quality != ItemQuality::None);
    safe &= SafeAssert(flags >= 0);
    if (complete)
    {
        safe &= SafeAssert(!((room != nullptr) && (owner != nullptr)));
        safe &= SafeAssert(!((room != nullptr) && (container != nullptr)));
        safe &= SafeAssert(!((owner != nullptr) && (container != nullptr)));
        safe &= SafeAssert((room != nullptr) || (owner != nullptr) || (container != nullptr));
    }
    //safe &= SafeAssert(currentSlot != EquipmentSlot::kNoEquipSlot);
    if (!safe)
    {
        Logger::log(LogLevel::Error, "Item :" + ToString(vnum));
    }
    return safe;
}

void Item::destroy()
{
    Logger::log(LogLevel::Error, "Destroying item '" + this->getName() + "'");
    // Remove the item from the game, this means: Room, Player, Container.
    if (room != nullptr)
    {
        room->removeItem(this);
    }
    else if (owner != nullptr)
    {
        if (owner->hasInventoryItem(this))
        {
            owner->remInventoryItem(this);
        }
        else if (owner->hasEquipmentItem(this))
        {
            owner->remEquipmentItem(this);
        }
    }
    else if (container != nullptr)
    {
        container->takeOut(this);
    }

    if (this->model->type == ModelType::Corpse)
    {
        Logger::log(LogLevel::Error, "Removing corpse '" + this->getName() + "' from MUD;");
        // Remove the item from the list of corpses.
        if (!Mud::getInstance().remCorpse(this))
        {
            Logger::log(LogLevel::Error, "Something gone wrong during corpse removal from mud.");
        }
        // Delete the auto-generated model for this corpse.
        delete (this->model);
    }
    else
    {
        Logger::log(LogLevel::Error, "Removing item '" + this->getName() + "' from MUD;");
        // Remove the item from the mud.
        if (!Mud::getInstance().remItem(this))
        {
            Logger::log(LogLevel::Error, "Something gone wrong during item removal from mud.");
        }
        Logger::log(LogLevel::Error, "Removing item '" + this->getName() + "' from DB;");
        // Remove the item from the database.
        if (!this->removeOnDB())
        {
            Logger::log(LogLevel::Error, "Something gone wrong during item removal from DB.");
        }
    }
    // Delete the item.
    delete (this);
}

bool Item::updateOnDB()
{
    // Check if the Item can be saved on the database.
    if (model->type == ModelType::Corpse)
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
        if (!Mud::getInstance().getDbms().updateInto("Item", value, where))
        {
            Logger::log(LogLevel::Error, "Can't save the item!");
            return false;
        }
    }

    // Save the item's position.
    if (room != nullptr)
    {
        vector<string> arguments;
        arguments.push_back(ToString(room->vnum));
        arguments.push_back(ToString(vnum));
        if (!Mud::getInstance().getDbms().insertInto("ItemRoom", arguments))
        {
            Logger::log(LogLevel::Error, "Can't save the item inside the room!");
            return false;
        }

        // Remove from ItemPlayer and ItemContent.
        QueryList where;
        where.push_back(std::make_pair("item", ToString(vnum)));
        Mud::getInstance().getDbms().deleteFrom("ItemPlayer", where);
        Mud::getInstance().getDbms().deleteFrom("ItemContent", where);
    }
    else if (owner != nullptr)
    {
        if (owner->isMobile())
        {
            // Remove from ItemContent and ItemRoom.
            QueryList where;
            where.push_back(std::make_pair("item", ToString(vnum)));
            Mud::getInstance().getDbms().deleteFrom("ItemPlayer", where);
            Mud::getInstance().getDbms().deleteFrom("ItemContent", where);
            Mud::getInstance().getDbms().deleteFrom("ItemRoom", where);
        }
        else
        {
            vector<string> arguments;
            arguments.push_back(owner->name);
            arguments.push_back(ToString(vnum));
            if (owner->hasEquipmentItem(this))
            {
                arguments.push_back(EnumToString(this->getCurrentSlot()));
            }
            else
            {
                arguments.push_back(ToString(0));
            }
            if (!Mud::getInstance().getDbms().insertInto("ItemPlayer", arguments, false, true))
            {
                Logger::log(LogLevel::Error, "Can't save the item possesed by the Player!");
                return false;
            }

            // Remove from ItemContent and ItemRoom.
            QueryList where;
            where.push_back(std::make_pair("item", ToString(vnum)));
            Mud::getInstance().getDbms().deleteFrom("ItemContent", where);
            Mud::getInstance().getDbms().deleteFrom("ItemRoom", where);
        }
    }
    else if (container != nullptr)
    {
        vector<string> arguments;
        arguments.push_back(ToString(container->vnum));
        arguments.push_back(ToString(vnum));
        if (!Mud::getInstance().getDbms().insertInto("ItemContent", arguments))
        {
            Logger::log(LogLevel::Error, "Can't save the item inside the container!");
            return false;
        }

        // Remove from ItemPlayer and ItemRoom.
        QueryList where;
        where.push_back(std::make_pair("item", ToString(vnum)));
        Mud::getInstance().getDbms().deleteFrom("ItemPlayer", where);
        Mud::getInstance().getDbms().deleteFrom("ItemRoom", where);
    }
    if (!content.empty())
    {
        for (auto iterator : content)
        {
            vector<string> arguments;
            // Prepare the query arguments.
            arguments.push_back(ToString(vnum));
            arguments.push_back(ToString(iterator->vnum));
            if (!Mud::getInstance().getDbms().insertInto("ItemContent", arguments))
            {
                Logger::log(LogLevel::Error, "Can't save the contained item!");
                return false;
            }
        }
    }
    if (contentLiq.first != nullptr)
    {
        vector<string> arguments;
        // Prepare the query arguments.
        arguments.push_back(ToString(vnum));
        arguments.push_back(ToString(contentLiq.first->vnum));
        arguments.push_back(ToString(contentLiq.second));
        if (!Mud::getInstance().getDbms().insertInto("ItemContentLiq", arguments, false, true))
        {
            Logger::log(LogLevel::Error, "Can't save the contained liquid!");
            return false;
        }
    }
    return true;
}

bool Item::removeOnDB()
{
    bool result = Mud::getInstance().getDbms().deleteFrom("Item",
    { std::make_pair("vnum", ToString(vnum)) });
    if (!result)
    {
        Logger::log(LogLevel::Error, "Error during item removal from table Item.");
    }
    return result;
}

bool Item::hasKey(string key)
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

bool Item::editCondition(int degradation)
{
    condition += degradation;
    if (condition <= 0)
    {
        return true;
    }
    else if (condition > model->condition)
    {
        condition = model->condition;
    }
    return false;
}

void Item::decayCondition()
{
    condition -= model->decay;
}

string Item::getCondition()
{
    string output;
    int percent;

    if (condition > 0)
    {
        percent = (int) ((100.0 * (double) (condition)) / (double) (model->condition));
    }
    else
    {
        percent = -1;
    }

    if (percent >= 100) output = " is in perfect condition.\n";
    else if (percent >= 75) output = " is scratched.\n";
    else if (percent >= 50) output = " is ruined.\n";
    else if (percent >= 25) output = " is cracked.\n";
    else if (percent >= 0) output = " is almost broken.\n";
    else output = " is broken.\n";

    return output;
}

int Item::getWeight()
{
    // Add the default weight of the model.
    int totalWeight = model->weight;

    // Add the addition weight due to the material.
    totalWeight += static_cast<int>(static_cast<double>(model->weight / 100) * composition->lightness);

    if (model->type == ModelType::Container)
    {
        if (!this->isEmpty())
        {
            for (auto iterator : content)
            {
                totalWeight += iterator->getWeight();
            }
        }
    }
    else if (model->type == ModelType::LiqContainer)
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

string Item::getNameCapital()
{
    std::string itemName = getName();
    itemName[0] = static_cast<char>(toupper(itemName[0]));
    return itemName;
}

string Item::getDescription()
{
    return model->getDescription(composition, quality);
}

string Item::getLook()
{
    string output;

    // Prepare : Name, Condition.
    //           Description.
    output = "You look at " + Telnet::cyan() + getName() + Telnet::reset() + ", it" + getCondition();
    output += Telnet::gray() + getDescription() + Telnet::reset() + "\n";
    // Print the content.
    output += lookContent();
    output += "It weights about " + Telnet::yellow() + ToString(getWeight()) + Telnet::reset() + " " + mud_measure
        + ".\n";

    return output;
}

bool Item::hasNodeType(NodeType nodeType)
{
    if (model == nullptr)
    {
        return false;
    }
    if (model->type != ModelType::Node)
    {
        return false;
    }
    if (model->getNodeFunc().type != nodeType)
    {
        return false;
    }
    return true;
}

bool Item::isEmpty()
{
    if (model->type == ModelType::Container)
    {
        return (content.size() == 0);
    }
    else if (model->type == ModelType::LiqContainer)
    {
        return (contentLiq.first == nullptr);
    }
    else
    {
        return true;
    }
}

int Item::getTotalSpace()
{
    if (model->type == ModelType::Container)
    {
        return model->getContainerFunc().maxWeight;
    }
    else if (model->type == ModelType::LiqContainer)
    {
        return model->getLiqContainerFunc().maxWeight;
    }
    else
    {
        return 0;
    }
}

int Item::getUsedSpace()
{
    unsigned int used = 0;
    if (model->type == ModelType::Container)
    {
        for (auto iterator : content)
        {
            used += iterator->model->weight;
        }
    }
    else if (model->type == ModelType::LiqContainer)
    {
        if (contentLiq.first != nullptr)
        {
            used += contentLiq.second;
        }
    }
    return used;
}

int Item::getFreeSpace()
{
    int free = getTotalSpace() - getUsedSpace();
    if (free < 0)
    {
        return 0;
    }
    return static_cast<unsigned int>(free);
}

bool Item::putInside(Item * item)
{
    if (item->model->weight > getFreeSpace())
    {
        return false;
    }
    // Insert in the container table.
    std::vector<std::string> arguments;
    arguments.push_back(ToString(vnum));       // Container
    arguments.push_back(ToString(item->vnum)); // Content
    if (!Mud::getInstance().getDbms().insertInto("ItemContent", arguments))
    {
        return false;
    }
    // Put the item into the container.
    content.push_back(item);
    // Set the container value to the content item.
    item->container = this;
    return true;
}

void Item::takeOut(Item * item)
{
    if (FindErase(content, item))
    {
        Logger::log(LogLevel::Debug, "Item '" + item->getName() + "' taken out from '" + this->getName() + "';");
        item->container = nullptr;
    }
    else
    {
        Logger::log(LogLevel::Error, "Error during item removal from container.");
    }
}

bool Item::pourIn(Liquid * liquid, int quantity)
{
    if (quantity > getFreeSpace())
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
        if (!Mud::getInstance().getDbms().insertInto("ItemContentLiq", arguments))
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
        if (!Mud::getInstance().getDbms().updateInto("ItemContentLiq", value, where))
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

bool Item::pourOut(unsigned int quantity)
{
    // If the item has an Endless provision of liquid, don't do any check.
    int modelFlags = model->getLiqContainerFunc().flags;
    if (HasFlag(modelFlags, LiqContainerFlag::Endless))
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
            Mud::getInstance().getDbms().deleteFrom("ItemContentLiq", where);
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
            Mud::getInstance().getDbms().updateInto("ItemContentLiq", value, where);
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

string Item::lookContent()
{
    string output;
    if (model->type == ModelType::Container)
    {
        if (content.empty())
        {
            output += Telnet::italic() + "It's empty.\n" + Telnet::reset();
        }
        else
        {
            output += "Looking inside you see:\n";
            for (auto it : GroupItems(content))
            {
                std::string contentName = it.first->getNameCapital();
                if (it.second > 1)
                {
                    output += " - " + Telnet::cyan() + contentName + Telnet::reset() + " [" + ToString(it.second)
                        + "].\n";
                }
                else
                {
                    output += " - " + Telnet::cyan() + contentName + Telnet::reset() + ".\n";
                }
            }
            output += "Has been used " + Telnet::yellow() + ToString(getUsedSpace()) + Telnet::reset();
            output += " out of " + Telnet::yellow() + ToString(getTotalSpace()) + Telnet::reset() + " " + mud_measure
                + ".\n";
        }
    }
    else if (model->type == ModelType::LiqContainer)
    {
        if (contentLiq.first == nullptr)
        {
            output += Telnet::italic() + "It does not contain any liquid.\n" + Telnet::reset();
        }
        else
        {
            int percent = 0;

            int modelFlags = model->getLiqContainerFunc().flags;
            if (HasFlag(modelFlags, LiqContainerFlag::Endless))
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
            output += Telnet::cyan() + contentLiq.first->getName() + Telnet::reset() + ".\n";
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
    else
    {
        return currentSlot;
    }
}

std::string Item::getCurrentSlotName()
{
    return GetEquipmentSlotName(getCurrentSlot());
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
    return first->getWeight() < second->getWeight();
}
