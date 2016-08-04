/// @file   shopItem.cpp
/// @author Enrico Fraccaroli
/// @date   Aug 01 2016
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

#include "shopItem.hpp"

#include "../mud.hpp"
#include "../logger.hpp"
#include "../formatter.hpp"
#include "../constants.hpp"

ShopItem::ShopItem() :
        shopName(),
        shopBuyTax(1),
        shopSellTax(1),
        shopKeeper()
{
    // Nothing to do.
}

ShopItem::~ShopItem()
{
    // Nothing to do.
}

ItemType ShopItem::getType() const
{
    return ItemType::Shop;
}

std::string ShopItem::getTypeName() const
{
    return "shop";
}

bool ShopItem::check(bool complete)
{
    return Item::check(complete);
}

bool ShopItem::destroy()
{
    return Item::destroy();
}

bool ShopItem::createOnDB()
{
    if (Item::createOnDB())
    {
        // Prepare the vector used to insert into the database.
        std::vector<std::string> arguments;
        arguments.push_back(ToString(this->vnum));
        arguments.push_back(this->shopName);
        arguments.push_back(ToString(this->shopBuyTax));
        arguments.push_back(ToString(this->shopSellTax));
        if (this->shopKeeper != nullptr)
        {
            arguments.push_back(this->shopKeeper->id);
        }
        else
        {
            arguments.push_back("");
        }
        if (SQLiteDbms::instance().insertInto("ItemShop", arguments))
        {
            return true;
        }
    }
    return false;
}

bool ShopItem::updateOnDB()
{
    if (Item::updateOnDB())
    {
        // Save the item's Information.
        {
            QueryList value;
            value.push_back(std::make_pair("vnum", ToString(this->vnum)));
            value.push_back(std::make_pair("name", this->shopName));
            value.push_back(std::make_pair("buyTax", ToString(this->shopBuyTax)));
            value.push_back(std::make_pair("sellTax", ToString(this->shopSellTax)));
            if (this->shopKeeper != nullptr)
            {
                value.push_back(std::make_pair("keeper", this->shopKeeper->id));
            }
            else
            {
                value.push_back(std::make_pair("keeper", ""));
            }
            QueryList where;
            where.push_back(std::make_pair("vnum", ToString(vnum)));
            if (SQLiteDbms::instance().updateInto("ItemShop", value, where))
            {
                return true;
            }
            else
            {
                Logger::log(LogLevel::Error, "Can't save the shop!");
            }
        }
    }
    return false;
}

bool ShopItem::removeOnDB()
{
    if (Item::removeOnDB())
    {
        QueryList where;
        where.push_back(std::make_pair("vnum", ToString(vnum)));
        if (SQLiteDbms::instance().deleteFrom("ItemShop", where))
        {
            return true;
        }
        else
        {
            Logger::log(LogLevel::Error, "Error during item removal from table Item.");
        }
    }
    return false;
}

void ShopItem::getSheet(Table & sheet) const
{
    // Call the function of the father class.
    Item::getSheet(sheet);
    // Add a divider.
    sheet.addDivider();
    // Set the values.
    sheet.addRow( { "Shop Name", shopName });
    sheet.addRow( { "Buy Tax", ToString(shopBuyTax) });
    sheet.addRow( { "Sell Tax", ToString(shopSellTax) });
    if (shopKeeper)
    {
        sheet.addRow( { "ShopKeeper", shopKeeper->getNameCapital() });
    }
    else
    {
        sheet.addRow( { "ShopKeeper", "Nobody" });
    }
}

std::string ShopItem::lookContent()
{
    std::string output;
    if (this->shopKeeper == nullptr)
    {
        output += Formatter::italic() + "Nobody is managing the shop.\n\n" + Formatter::reset();
        output += Item::lookContent();
        return output;
    }
    else if (!this->shopKeeper->alive)
    {

        output += Formatter::italic() + "The shopkeeper's probably taking a nap.\n\n"
            + Formatter::reset();
        output += Item::lookContent();
        return output;
    }

    output += this->shopKeeper->getNameCapital() + " is currently managing the shop.\n";
    if (content.empty())
    {
        return Formatter::italic() + "There is nothing on sale.\n" + Formatter::reset();
    }
    else
    {
        Table saleTable(shopName);
        saleTable.addColumn("Good", StringAlign::Left);
        saleTable.addColumn("Quantity", StringAlign::Center);
        saleTable.addColumn("Weight", StringAlign::Center);
        saleTable.addColumn("Buy", StringAlign::Left);
        saleTable.addColumn("Sell", StringAlign::Left);
        for (auto iterator : GroupItems(content))
        {
            // Retrieve the item.
            Item * item = iterator.first;
            // Prepare the row.
            TableRow row;
            row.push_back(item->getNameCapital());
            row.push_back(ToString(iterator.second));
            row.push_back(ToString(item->getWeight(true)));
            row.push_back(ToString(this->evaluateBuyPrice(item->getPrice())));
            row.push_back(ToString(this->evaluateSellPrice(item->getPrice())));
            // Add the row to the table.
            saleTable.addRow(row);
        }
        output += saleTable.getTable();
        output += "Has been used ";
        output += Formatter::yellow() + ToString(this->getUsedSpace()) + Formatter::reset();
        output += " out of ";
        output += Formatter::yellow() + ToString(this->getTotalSpace()) + Formatter::reset();
        output += " " + mud_measure + ".\n";
    }
    output += "\n";
    return output;
}

void ShopItem::setNewShopKeeper(Mobile * _shopKeeper)
{
    if (this->shopKeeper == nullptr)
    {
        _shopKeeper->doCommand("say let's get to work!");
    }
    else
    {
        if (this->shopKeeper != _shopKeeper)
        {
            _shopKeeper->doCommand("say let's get to work!");
            this->shopKeeper->doCommand("say let's get some rest.");
        }
    }
    this->shopKeeper = _shopKeeper;
}

unsigned int ShopItem::evaluateBuyPrice(const unsigned int & price)
{
    return this->shopBuyTax * price;
}

unsigned int ShopItem::evaluateSellPrice(const unsigned int & price)
{
    return this->shopSellTax * price;
}
