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

#include "currencyItem.hpp"

ShopItem::ShopItem() :
    shopName(),
    shopBuyTax(1),
    shopSellTax(1),
    balance(),
    shopKeeper()
{
    // Nothing to do.
}

ShopItem::~ShopItem()
{
    // Nothing to do.
}

bool ShopItem::check(bool complete)
{
    return Item::check(complete);
}

bool ShopItem::updateOnDB()
{
    if (Item::updateOnDB())
    {
        // Prepare the vector used to insert into the database.
        std::vector<std::string> arguments;
        arguments.push_back(ToString(this->vnum));
        arguments.push_back(this->shopName);
        arguments.push_back(ToString(this->shopBuyTax));
        arguments.push_back(ToString(this->shopSellTax));
        arguments.push_back(ToString(this->balance));
        if (this->shopKeeper != nullptr)
        {
            arguments.push_back(this->shopKeeper->id);
        }
        else
        {
            arguments.push_back("");
        }
        return SQLiteDbms::instance().insertInto("Shop", arguments, false, true);
    }
    return false;
}

bool ShopItem::removeOnDB()
{
    if (Item::removeOnDB())
    {
        if (SQLiteDbms::instance().deleteFrom("Shop", {std::make_pair("vnum", ToString(vnum))}))
        {
            return true;
        }
    }
    Logger::log(LogLevel::Error, "Error during item removal from table Shop.");
    return false;
}

void ShopItem::getSheet(Table & sheet) const
{
    // Call the function of the father class.
    Item::getSheet(sheet);
    // Add a divider.
    sheet.addDivider();
    // Set the values.
    sheet.addRow({"Shop Name", shopName});
    sheet.addRow({"Buy Tax", ToString(shopBuyTax)});
    sheet.addRow({"Sell Tax", ToString(shopSellTax)});
    sheet.addRow({"Balance", ToString(this->getBalance())});
    if (shopKeeper)
    {
        sheet.addRow({"ShopKeeper", shopKeeper->getNameCapital()});
    }
    else
    {
        sheet.addRow({"ShopKeeper", "Nobody"});
    }
}

bool ShopItem::canDeconstruct(std::string & error) const
{
    if (!Item::canDeconstruct(error))
    {
        return false;
    }
    if (this->getBalance() != 0)
    {
        error = "You must withdraw all the coin first.";
        return false;
    }
    return true;
}

std::string ShopItem::lookContent()
{
    std::string output, error;
    if (!this->canUse(error))
    {
        output += Formatter::italic() + error + "\n\n" + Formatter::reset();
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
        saleTable.addColumn("Weight (Single)", StringAlign::Right);
        saleTable.addColumn("Weight (Stack)", StringAlign::Right);
        saleTable.addColumn("Buy", StringAlign::Right);
        saleTable.addColumn("Sell (Single)", StringAlign::Right);
        saleTable.addColumn("Sell (Stack)", StringAlign::Right);
        for (auto iterator : content)
        {
            // Prepare the row.
            TableRow row;
            row.push_back(iterator->getNameCapital());
            row.push_back(ToString(iterator->quantity));
            row.push_back(ToString(iterator->getWeight(false)));
            if (iterator->quantity > 1)
            {
                row.push_back(ToString(iterator->getWeight(true)));
            }
            else
            {
                row.push_back("");
            }
            row.push_back(ToString(this->evaluateBuyPrice(iterator)));
            row.push_back(ToString(this->evaluateSellPrice(iterator)));
            if (iterator->quantity > 1)
            {
                row.push_back(ToString(this->evaluateSellPrice(iterator) * iterator->quantity));
            }
            else
            {
                row.push_back("");
            }
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

unsigned int ShopItem::getBalance() const
{
    return this->balance;
}

bool ShopItem::canUse(std::string & error)
{
    if (this->shopKeeper == nullptr)
    {
        error = "Nobody is managing the shop.";
        return false;
    }
    if (!this->shopKeeper->isAlive())
    {
        error = "The shopkeeper's probably taking a nap.";
        return false;
    }
    if (this->room->vnum != this->shopKeeper->room->vnum)
    {
        error = "The shopkeeper's is elsewhere.";
        return false;
    }
    return true;
}

unsigned int ShopItem::evaluateBuyPrice(Item * item)
{
    return this->shopBuyTax * item->getPrice(false);
}

unsigned int ShopItem::evaluateSellPrice(Item * item)
{
    return this->shopSellTax * item->getPrice(false);
}
