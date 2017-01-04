/// @file   shopItem.cpp
/// @author Enrico Fraccaroli
/// @date   Aug 01 2016
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

#include "shopItem.hpp"

#include "currencyItem.hpp"
#include "shopModel.hpp"
#include "logger.hpp"
#include "mud.hpp"

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
        return SQLiteDbms::instance().insertInto("Shop", arguments, false,
                                                 true);
    }
    return false;
}

bool ShopItem::removeOnDB()
{
    if (Item::removeOnDB())
    {
        if (SQLiteDbms::instance().deleteFrom("Shop",
                                              {
                                                  std::make_pair("vnum",
                                                                 ToString(vnum))
                                              }))
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
    output += this->shopKeeper->getNameCapital() +
              " is currently managing the shop.\n";
    if (content.empty())
    {
        return Formatter::italic() + "There is nothing on sale.\n" +
               Formatter::reset();
    }
    else
    {
        Table saleTable(shopName);
        saleTable.addColumn("Good", StringAlign::Left);
        saleTable.addColumn("Quantity", StringAlign::Center);
        saleTable.addColumn("Weight (Single)", StringAlign::Right);
        saleTable.addColumn("Weight (stack)", StringAlign::Right);
        saleTable.addColumn("Buy", StringAlign::Right);
        saleTable.addColumn("Sell (Single)", StringAlign::Right);
        saleTable.addColumn("Sell (stack)", StringAlign::Right);
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
                row.push_back(ToString(
                    this->evaluateSellPrice(iterator) * iterator->quantity));
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
        output += Formatter::yellow() + ToString(this->getUsedSpace()) +
                  Formatter::reset();
        output += " out of ";
        output += Formatter::yellow() + ToString(this->getTotalSpace()) +
                  Formatter::reset();
        output += " " + Mud::instance().getWeightMeasure() + ".\n";
    }
    output += "\n";
    return output;
}

bool ShopItem::isAContainer() const
{
    // The shop can be considered a container only if built.
    return HasFlag(this->flags, ItemFlag::Built);
}

double ShopItem::getTotalSpace() const
{
    // The base space.
    double spaceBase = model->toShop()->maxWeight;
    // Evaluate the result.
    return ((spaceBase + (spaceBase * quality.getModifier())) / 2);
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
