/// @file   weaponModel.cpp
/// @brief  Iplement the methods for Weapon.
/// @author Enrico Fraccaroli
/// @date   Jul 6 2016
/// @copyright
/// Copyright (c) 2016 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
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

#include "shopModel.hpp"

#include "../mud.hpp"
#include "../item/shopItem.hpp"

ShopModel::ShopModel() :
        maxWeight()
{
    // Nothing to do.
}

ShopModel::~ShopModel()
{
    // Nothing to do.
}

ModelType ShopModel::getType() const
{
    return ModelType::Shop;
}

std::string ShopModel::getTypeName() const
{
    return "Shop";
}

bool ShopModel::setModel(const std::string & source)
{
    if (source.empty())
    {
        Logger::log(LogLevel::Error, "Function list is empty (%s).", this->name);
        return false;
    }
    std::vector<std::string> functionList = SplitString(source, " ");
    if (functionList.size() != 1)
    {
        Logger::log(LogLevel::Error, "Wrong number of parameters for Shop Model (%s).", this->name);
        return false;
    }
    this->maxWeight = ToNumber<unsigned int>(functionList[0]);
    return true;
}

void ShopModel::getSheet(Table & sheet) const
{
    // Call the function of the father class.
    ItemModel::getSheet(sheet);
    // Add a divider.
    sheet.addDivider();
    // Set the values.
    sheet.addRow( { "Max Weight", ToString(this->maxWeight) });
}

Item * ShopModel::createItem(std::string maker, Material * composition, ItemQuality itemQuality)
{
    // Instantiate the new item.
    ShopItem * newItem = new ShopItem();
    newItem->vnum = Mud::instance().getMaxVnumItem() + 1;
    newItem->model = this;
    newItem->maker = maker;
    newItem->condition = condition;
    newItem->composition = composition;
    newItem->quality = itemQuality;
    newItem->flags = 0;
    newItem->room = nullptr;
    newItem->owner = nullptr;
    newItem->container = nullptr;
    newItem->currentSlot = slot;
    newItem->content = std::vector<Item *>();
    newItem->contentLiq = LiquidContent();

    if (!newItem->check())
    {
        Logger::log(LogLevel::Error, "Cannot create the shop.");
        // Delete the item.
        delete (newItem);
        // Return pointer to nothing.
        return nullptr;
    }
    SQLiteDbms::instance().beginTransaction();
    if (newItem->createOnDB())
    {
        // Insert into the item_list the new item.
        Mud::instance().addItem(newItem);
    }
    else
    {
        Logger::log(LogLevel::Error, "Cannot save the shop on DB.");
        // Rollback the transation.
        SQLiteDbms::instance().rollbackTransection();
        // Delete the item.
        delete (newItem);
        // Return pointer to nothing.
        return nullptr;
    }
    SQLiteDbms::instance().endTransaction();
    Logger::log(LogLevel::Debug, "A new shop has been created.");
    return newItem;
}
