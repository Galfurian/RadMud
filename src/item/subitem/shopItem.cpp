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

#include "shopModel.hpp"
#include "logger.hpp"
#include "mud.hpp"

ShopItem::ShopItem() :
    shopName(),
    shopBuyTax(1),
    shopSellTax(1),
    balance(),
    shopKeeper(),
    openingHour(6),
    closingHour(18)
{
    // Nothing to do.
}

ShopItem::~ShopItem()
{
    // Nothing to do.
}

bool ShopItem::updateOnDB()
{
    if (!Item::updateOnDB())
    {
        return false;
    }
    // Prepare the vector used to insert into the database.
    std::vector<std::string> arguments;
    arguments.emplace_back(ToString(vnum));
    arguments.emplace_back(shopName);
    arguments.emplace_back(ToString(shopBuyTax));
    arguments.emplace_back(ToString(shopSellTax));
    arguments.emplace_back(ToString(balance));
    arguments.emplace_back((shopKeeper != nullptr) ? shopKeeper->id : "");
    arguments.emplace_back(ToString(openingHour));
    arguments.emplace_back(ToString(closingHour));
    return SQLiteDbms::instance().insertInto("Shop", arguments, false, true);
}

bool ShopItem::removeOnDB()
{
    if (Item::removeOnDB())
    {
        if (SQLiteDbms::instance().deleteFrom(
            "Shop", {std::make_pair("vnum", ToString(vnum))}))
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
    sheet.addRow({"ShopKeeper",
                  (shopKeeper) ? shopKeeper->getNameCapital() : "None"});
    sheet.addRow({"Opening Hour", ToString(openingHour)});
    sheet.addRow({"Closing Hour", ToString(closingHour)});
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
    // If the shop is not built, show the item normal content.
    if (!HasFlag(flags, ItemFlag::Built))
    {
        return "";
    }
    std::stringstream ss;
    if (HasFlag(flags, ItemFlag::Closed))
    {
        ss << Formatter::italic("It is closed.\n");
    }
    std::string error;
    if (!this->canUse(error))
    {
        // Show the error.
        ss << Formatter::italic(error) + "\n\n";
        // Show the content.
        ss << "Looking inside you see:\n";
        for (auto it : content)
        {
            ss << " [" << std::right << std::setw(3) << it->quantity << "] ";
            ss << it->getNameCapital() << "\n";
        }
    }
    else
    {
        // Show who is managing the shop.
        ss << shopKeeper->getNameCapital();
        ss << " is currently managing the shop.\n";
        if (content.empty())
        {
            ss << Formatter::italic("There is nothing on sale.\n\n");
            return ss.str();
        }
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
            row.emplace_back(iterator->getNameCapital());
            row.emplace_back(ToString(iterator->quantity));
            row.emplace_back(ToString(iterator->getWeight(false)));
            if (iterator->quantity > 1)
            {
                row.emplace_back(ToString(iterator->getWeight(true)));
            }
            else
            {
                row.emplace_back("");
            }
            row.emplace_back(ToString(this->evaluateBuyPrice(iterator)));
            row.emplace_back(ToString(this->evaluateSellPrice(iterator)));
            if (iterator->quantity > 1)
            {
                row.emplace_back(ToString(
                    this->evaluateSellPrice(iterator) *
                    iterator->quantity));
            }
            else
            {
                row.emplace_back("");
            }
            // Add the row to the table.
            saleTable.addRow(row);
        }
        ss << saleTable.getTable();
    }
    ss << "Has been used " << Formatter::yellow(ToString(getUsedSpace()));
    ss << " out of " << Formatter::yellow(ToString(getTotalSpace())) << ' ';
    ss << Mud::instance().getWeightMeasure() << ".\n";
    return ss.str();
}

bool ShopItem::isAContainer() const
{
    // The shop can be considered a container only if built.
    return HasFlag(flags, ItemFlag::Built);
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
    if (shopKeeper == nullptr)
    {
        _shopKeeper->doCommand("say let's get to work!");
    }
    else if (shopKeeper != _shopKeeper)
    {
        _shopKeeper->doCommand("say let's get to work!");
        shopKeeper->doCommand("say let's get some rest.");
    }
    shopKeeper = _shopKeeper;
}

unsigned int ShopItem::getBalance() const
{
    return balance;
}

bool ShopItem::canUse(std::string & error)
{
    if (shopKeeper == nullptr)
    {
        error = "Nobody is managing the shop.";
        return false;
    }
    if (!shopKeeper->isAlive())
    {
        error = "The shopkeeper's probably ill.";
        return false;
    }
    if (room->vnum != shopKeeper->room->vnum)
    {
        error = "The shopkeeper's is elsewhere.";
        return false;
    }
    auto hour = MudUpdater::instance().getMudHour();
    if (((openingHour < closingHour) && ((hour < openingHour) ||
                                         (hour > closingHour))) ||
        ((closingHour < openingHour) && ((hour < closingHour) ||
                                         (hour > openingHour))))
    {
        error = "The shopkeeper's probably taking a nap.";
        return false;
    }
    return true;
}

unsigned int ShopItem::evaluateBuyPrice(Item * item)
{
    return shopBuyTax * item->getPrice(false);
}

unsigned int ShopItem::evaluateSellPrice(Item * item)
{
    return shopSellTax * item->getPrice(false);
}
