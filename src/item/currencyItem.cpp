/// @file   currencyItem.cpp
/// @author Enrico Fraccaroli
/// @date   Aug 04 2016
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

#include "currencyItem.hpp"

#include "../model/currencyModel.hpp"
#include "../material.hpp"

CurrencyItem::CurrencyItem()
{
    // Nothing to do.
}

CurrencyItem::~CurrencyItem()
{
    // Nothing to do.
}

void CurrencyItem::getSheet(Table & sheet) const
{
    // Call the function of the father class.
    Item::getSheet(sheet);
    // Add a divider.
    sheet.addDivider();
    // Set the values.
}

unsigned int CurrencyItem::getPrice(bool entireStack) const
{
    auto customPrice = Item::getPrice(entireStack);
    if (!model->toCurrency()->findPrice(this->composition->vnum, customPrice))
    {
        Logger::log(
            LogLevel::Error,
            "The item (%s) has a wrong composition w.r.t its currency.",
            this->getName());
    }
    return customPrice;
}

unsigned int CurrencyItem::getWeight(bool) const
{
    return 0;
}
