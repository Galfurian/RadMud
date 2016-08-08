/// @file   currencyModel.hpp
/// @brief  Define variables and methods of Currency.
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

#pragma once

#include "itemModel.hpp"

/// @brief Model of a currency.
class CurrencyModel: public ItemModel
{
    private:
        struct Price
        {
            public:
                int material;
                unsigned int price;
                Price(const int & _material, const unsigned int & _price) :
                        material(_material),
                        price(_price)
                {
                    // Nothing to do.
                }
                /// @brief Operator used to order the prices.
                bool operator>(const Price & rhs) const
                {
                    return price > rhs.price;
                }
                /// @brief Operator used to order the prices.
                bool operator==(const Price & rhs) const
                {
                    return price == rhs.price;
                }
                /// @brief Operator used to order the prices.
                bool operator==(const int & _rhs) const
                {
                    return material == _rhs;
                }
        };

    public:
        std::vector<Price> prices;

        CurrencyModel();

        virtual ~CurrencyModel();

        virtual ModelType getType() const;

        virtual std::string getTypeName() const;

        virtual bool setModel(const std::string & source);

        virtual void getSheet(Table & sheet) const;

        virtual Item * createItem(
            std::string maker,
            Material * composition,
            const ItemQuality & itemQuality);

        bool addPrice(const int & materialVnum, const unsigned int & price);

        bool findPrice(const int & materialVnum, unsigned int & price) const;

        bool generateCurrency(
            const std::string & maker,
            const unsigned int & value,
            std::vector<Item *> & coins);

    private:
        /// @brief Sort the list of prices.
        void sortList();
};
