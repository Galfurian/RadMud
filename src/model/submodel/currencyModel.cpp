/// @file   currencyModel.cpp
/// @brief  Iplement the methods for Currency.
/// @author Enrico Fraccaroli
/// @date   Jul 6 2016
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

#include "currencyModel.hpp"

#include "logger.hpp"
#include "mud.hpp"

CurrencyModel::Price::Price() :
    material(),
    price()
{
    // Nothing to do.
}

CurrencyModel::Price::Price(const int & _material,
                            const unsigned int & _price) :
    material(_material),
    price(_price)
{
    // Nothing to do.
}

bool CurrencyModel::Price::operator>(const Price & rhs) const
{
    return price > rhs.price;
}

bool CurrencyModel::Price::operator==(const Price & rhs) const
{
    return price == rhs.price;
}

bool CurrencyModel::Price::operator==(const int & _rhs) const
{
    return material == _rhs;
}

CurrencyModel::CurrencyModel()
{
// Nothing to do.
}

CurrencyModel::~CurrencyModel()
{
// Nothing to do.
}

ModelType CurrencyModel::getType() const
{
    return ModelType::Currency;
}

std::string CurrencyModel::getTypeName() const
{
    return "Currency";
}

bool CurrencyModel::setModel(const std::string & source)
{
    std::vector<std::string> functionList = SplitString(source, " ");
    if (functionList.size() != 0)
    {
        Logger::log(
            LogLevel::Error,
            "Wrong number of parameters for Currency Model (%s).",
            this->name);
        return false;
    }
    return true;
}

void CurrencyModel::getSheet(Table & sheet) const
{
// Call the function of the father class.
    ItemModel::getSheet(sheet);
// Add a divider.
//sheet.addDivider();
}

Item * CurrencyModel::createItem(
    std::string maker,
    Material * composition,
    bool isForMobile,
    const ItemQuality & itemQuality,
    const unsigned int & quantity)
{
    auto it = std::find(prices.begin(), prices.end(), composition->vnum);
    if (it != prices.end())
    {
        return ItemModel::createItem(maker,
                                     composition,
                                     isForMobile,
                                     itemQuality,
                                     quantity);
    }
    else
    {
        Logger::log(LogLevel::Error, "Material is not allowed.");
        return nullptr;
    }
}

bool
CurrencyModel::addPrice(const int & materialVnum, const unsigned int & price)
{
    auto it = std::find(prices.begin(), prices.end(), materialVnum);
    if (it == prices.end())
    {
        prices.push_back(Price(materialVnum, price));
        this->sortList();
        return true;
    }
    return false;
}

bool
CurrencyModel::findPrice(const int & materialVnum, unsigned int & price) const
{
    auto it = std::find(prices.begin(), prices.end(), materialVnum);
    if (it != prices.end())
    {
        price = it->price;
        return true;
    }
    return false;
}

ItemVector CurrencyModel::generateCurrency(
    const std::string & maker,
    const unsigned int & value)
{
    auto status = true;
    auto currentValue = value;
    ItemVector coins;
    SQLiteDbms::instance().beginTransaction();
    for (auto it : prices)
    {
        auto coinMaterial = Mud::instance().findMaterial(it.material);
        auto coinQuantity = (currentValue / it.price);
        if (coinQuantity == 0)
        {
            continue;
        }
        auto coin = this->createItem(maker, coinMaterial, false,
                                     ItemQuality::Normal, coinQuantity);
        if (coin != nullptr)
        {
            coins.push_back(coin);
        }
        else
        {
            status = false;
            break;
        }
        currentValue -= it.price * coinQuantity;
    }
    if (!status)
    {
        for (auto generated : coins)
        {
            MudUpdater::instance().addItemToDestroy(generated);
        }
        coins.clear();
        SQLiteDbms::instance().rollbackTransection();
    }
    SQLiteDbms::instance().endTransaction();
    return coins;
}

void CurrencyModel::sortList()
{
    std::sort(prices.begin(), prices.end(), std::greater<Price>());
}
