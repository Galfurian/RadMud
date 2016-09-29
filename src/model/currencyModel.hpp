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
class CurrencyModel :
    public ItemModel
{
private:
    /// @brief Give a currency, this class is used to determine the value of the currency
    ///         based on material and a pre-defined price.
    class Price
    {
    public:
        /// Material vnum.
        int material;
        /// The associated price.
        unsigned int price;

        /// @brief Constructor.
        /// @param _material The material.
        /// @param _price    The price associated to the material.
        Price(const int & _material, const unsigned int & _price);

        /// @brief Operator used to order the prices.
        bool operator>(const Price & rhs) const;

        /// @brief Operator used to order the prices.
        bool operator==(const Price & rhs) const;

        /// @brief Operator used to order the prices.
        bool operator==(const int & _rhs) const;
    };

public:
    /// All the prices defined for this currency.
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
        const ItemQuality & itemQuality,
        const unsigned int & quantity);

    /// @brief Add an entry to the prices.
    /// @param materialVnum The vnum of the material.
    /// @param price        The price associated to the material.
    /// @return <b>True</b> if the price has been added,<br>
    ///         <b>False</b> otherwise.
    bool addPrice(const int & materialVnum, const unsigned int & price);

    /// @brief Given a specific material, this function returns the associated price.
    /// @param materialVnum The vnum of the material.
    /// @param price        Variable where the found price is saved.
    /// @return <b>True</b> if the price has been found,<br>
    ///         <b>False</b> otherwise.
    bool findPrice(const int & materialVnum, unsigned int & price) const;

    /// @brief Generates an ammount of currency with a worth equal to the variable value.
    /// @details
    ///     <b>This function starts and ends a SQLITE3 Transaction.</b>
    /// @param maker Who has generated the currency.
    /// @param value The total worth that has to be generate.
    /// @return The list of generated currencies.
    std::vector<Item *> generateCurrency(const std::string & maker, const unsigned int & value);

private:
    /// @brief Sort the list of prices.
    void sortList();
};
