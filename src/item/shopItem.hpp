/// @file   shopItem.hpp
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

#pragma once

#include "item.hpp"
#include "../mobile.hpp"

/// @brief Holds details about shops.
class ShopItem: public Item
{
    public:
        /// The name of the shop.
        std::string shopName;
        /// The tax applied to each good which is bought.
        unsigned int shopBuyTax;
        /// The tax applied to each good which is sold.
        unsigned int shopSellTax;
        /// The mobile which is managing the shop.
        Mobile * shopKeeper;

        /// @brief Constructor.
        ShopItem();

        virtual ~ShopItem();

        virtual bool check(bool complete = false);

        virtual bool destroy();

        virtual bool createOnDB();

        virtual bool updateOnDB();

        virtual bool removeOnDB();

        virtual void getSheet(Table & sheet) const;

        virtual std::string lookContent();

        /// @brief Allows to set the new shop keeper.
        /// @param _shopKeeper The new shop keeper.
        void setNewShopKeeper(Mobile * _shopKeeper);

    private:
        /// @brief Applies the buy tax to the item's price.
        /// @param item The item that has to be priced.
        /// @return The buy price with applied the taxes.
        unsigned int evaluateBuyPrice(Item * item);

        /// @brief Applies the sell tax to the item's price.
        /// @param item The item that has to be priced.
        /// @return The sell price with applied the taxes.
        unsigned int evaluateSellPrice(Item * item);
};
