/// @file   currencyItem.cpp
/// @author Enrico Fraccaroli
/// @date   Aug 04 2016
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

#include "currencyItem.hpp"

#include "currencyModel.hpp"
#include "logger.hpp"

CurrencyItem::CurrencyItem()
{
	// Nothing to do.
}

CurrencyItem::~CurrencyItem()
{
	// Nothing to do.
}

void CurrencyItem::getSheet(Table &sheet) const
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
	if (composition != nullptr) {
		if (model->to<CurrencyModel>()->findPrice(composition->vnum,
												  customPrice)) {
			return customPrice * quantity;
		}
		MudLog(LogLevel::Error,
					"The item (%s) has a wrong composition w.r.t its currency.",
					this->getName());
	}
	MudLog(LogLevel::Error, "The item %s has no composition.",
				this->getName());
	return customPrice;
}

double CurrencyItem::getWeight(bool) const
{
	return 0.0;
}

void CurrencyItem::triggerDecay()
{
	// Nothing to do.
}
