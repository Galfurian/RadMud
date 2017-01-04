/// @file   shopItem.hpp
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

#pragma once

#include "item.hpp"
#include "mobile.hpp"

/// @brief Holds details about shops.
class ShopItem :
    public Item
{
public:
    /// The name of the shop.
    std::string shopName;
    /// The tax applied to each good which is bought.
    unsigned int shopBuyTax;
    /// The tax applied to each good which is sold.
    unsigned int shopSellTax;
    /// The shop balance.
    unsigned int balance;
    /// The mobile which is managing the shop.
    Mobile * shopKeeper;

    /// @brief Constructor.
    ShopItem();

    virtual ~ShopItem();

    bool check(bool complete = false) override;

    bool updateOnDB() override;

    bool removeOnDB() override;

    void getSheet(Table & sheet) const override;

    bool canDeconstruct(std::string & error) const override;

    std::string lookContent() override;

    bool isAContainer() const override;

    double getTotalSpace() const override;

    /// @brief Allows to set the new shop keeper.
    /// @param _shopKeeper The new shop keeper.
    void setNewShopKeeper(Mobile * _shopKeeper);

    /// @brief Provides the balance of the shop.
    unsigned int getBalance() const;

    /// Check if the shop can be used.
    /// @param error In case it cannot be used, the reason is stored
    ///               in this variable.
    /// @return <b>True</b> if it can be used,<br>
    ///         <b>False</b> otherwise.
    bool canUse(std::string & error);

    /// @brief Applies the buy tax to the item's price.
    /// @param item The item that has to be priced.
    /// @return The buy price with applied the taxes.
    unsigned int evaluateBuyPrice(Item * item);

    /// @brief Applies the sell tax to the item's price.
    /// @param item The item that has to be priced.
    /// @return The sell price with applied the taxes.
    unsigned int evaluateSellPrice(Item * item);
};
