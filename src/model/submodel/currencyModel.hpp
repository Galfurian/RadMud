/// @file   currencyModel.hpp
/// @brief  Define variables and methods of Currency.
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

#pragma once

#include "itemModel.hpp"
#include "itemVector.hpp"

/// @brief Model of a currency.
class CurrencyModel :
    public ItemModel
{
private:
    /// @brief Give a currency, this class is used to determine the value
    ///         of the currency based on material and a pre-defined price.
    class Price
    {
    public:
        /// Material vnum.
        unsigned int material;
        /// The associated price.
        unsigned int price;

        /// @brief Constructor.
        Price() :
            material(),
            price()
        {
            // Nothing to do.
        }

        /// @brief Constructor.
        /// @param _material The material.
        /// @param _price    The price associated to the material.
        Price(const unsigned int & _material, const unsigned int & _price) :
            material(_material),
            price(_price)
        {
            // Nothing to do.
        }

        /// @brief Operator used to order the prices.
        inline bool operator>(const Price & rhs) const
        {
            return price > rhs.price;
        }

        /// @brief Operator used to order the prices.
        inline bool operator==(const Price & rhs) const
        {
            return price == rhs.price;
        }

        /// @brief Operator used to order the prices.
        inline bool operator==(const unsigned int & _rhs) const
        {
            return material == _rhs;
        }
    };

public:
    /// All the prices defined for this currency.
    std::vector<Price> prices;

    CurrencyModel();

    virtual ~CurrencyModel();

    ModelType getType() const override;

    std::string getTypeName() const override;

    bool setModel(const std::string & source) override;

    void getSheet(Table & sheet) const override;

    Item * createItem(
        std::string maker,
        Material * composition,
        bool isForMobile = false,
        const ItemQuality & itemQuality = ItemQuality::Normal,
        const unsigned int & quantity = 1) override;

    /// @brief Add an entry to the prices.
    /// @param materialVnum The vnum of the material.
    /// @param price        The price associated to the material.
    /// @return <b>True</b> if the price has been added,<br>
    ///         <b>False</b> otherwise.
    bool addPrice(const unsigned int & materialVnum,
                  const unsigned int & price);

    /// @brief Given a specific material, this function returns the
    ///         associated price.
    /// @param materialVnum The vnum of the material.
    /// @param price        Variable where the found price is saved.
    /// @return <b>True</b> if the price has been found,<br>
    ///         <b>False</b> otherwise.
    bool findPrice(const unsigned int & materialVnum,
                   unsigned int & price) const;

    /// @brief Generates an amount of currency with a worth equal to
    ///         the variable value.
    /// @details <b>This function starts and ends a SQLITE3 Transaction.</b>
    /// @param maker Who has generated the currency.
    /// @param value The total worth that has to be generate.
    /// @return The list of generated currencies.
    ItemVector generateCurrency(const std::string & maker,
                                const unsigned int & value);

private:
    /// @brief Sort the list of prices.
    void sortList();
};
