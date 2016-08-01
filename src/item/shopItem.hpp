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

        ShopItem();

        virtual ~ShopItem();

        virtual ItemType getType() const;

        virtual std::string getTypeName() const;

        virtual bool check(bool complete = false);

        virtual bool destroy();

        virtual bool createOnDB();

        virtual bool updateOnDB();

        virtual bool removeOnDB();

        virtual bool loadFromDB();

        virtual std::string lookContent();

        void setNewShopKeeper(Mobile * _shopKeeper);

    private:
        unsigned int evaluateBuyPrice(const unsigned int & price);

        unsigned int evaluateSellPrice(const unsigned int & price);
};
