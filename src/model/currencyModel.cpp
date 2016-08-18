/// @file   currencyModel.cpp
/// @brief  Iplement the methods for Currency.
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

#include "currencyModel.hpp"

#include "../material.hpp"
#include "../mud.hpp"

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
    const ItemQuality & itemQuality)
{
    auto it = std::find(prices.begin(), prices.end(), composition->vnum);
    if (it != prices.end())
    {
        return ItemModel::createItem(maker, composition, itemQuality);
    }
    else
    {
        Logger::log(LogLevel::Error, "Material is not allowed.");
        return nullptr;
    }
}

bool CurrencyModel::addPrice(const int & materialVnum, const unsigned int & price)
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

bool CurrencyModel::findPrice(const int & materialVnum, unsigned int & price) const
{
    auto it = std::find(prices.begin(), prices.end(), materialVnum);
    if (it != prices.end())
    {
        price = it->price;
        return true;
    }
    return false;
}

std::vector<Item *> CurrencyModel::generateCurrency(
    const std::string & maker,
    const unsigned int & value)
{
    auto status = true;
    auto currentValue = value;
    std::vector<Item *> coins;
    for (auto it : prices)
    {
        auto coinMaterial = Mud::instance().findMaterial(it.material);
        auto coinQuantity = (currentValue / it.price);
        auto coin = this->createItem(maker, coinMaterial, ItemQuality::Normal);
        if (coin != nullptr)
        {
            coin->quantity = coinQuantity;
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
            delete (generated);
        }
        coins.clear();
    }
    return coins;
}

void CurrencyModel::sortList()
{
    std::sort(prices.begin(), prices.end(), std::greater<Price>());
}
