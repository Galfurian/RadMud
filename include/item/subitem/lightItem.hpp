/// @file   lightItem.hpp
/// @brief
/// @author Enrico Fraccaroli
/// @date   05/12/2016
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

#include "item/item.hpp"

/// @brief Holds details about lights.
class LightItem : public Item {
public:
	/// Activity status.
	bool active;

	LightItem();

	virtual ~LightItem();

	std::string getName(bool colored = false) const override;

	std::string getNameCapital(bool colored = false) const override;

	void getSheet(Table &sheet) const override;

	std::string lookContent() override;

	double getTotalSpace() const override;

	/// @brief Checks if the light source is active.
	bool isActive() const;

	/// @brief Determines if this can be refilled with the given item.
	/// @param item  The item, probably the fuel.
	/// @param error The error message in case it cannot be used as fuel.
	/// @return If this can be refilled with item.
	bool canRefillWith(Item *item, std::string &error) const;

	/// @brief Provides the amount of the given item is required to fill this.
	/// @param item   The item, probably the fuel.
	/// @param amount The amount of required fuel.
	/// @param error  The error message in case something goes wrong.
	/// @return If the value amount has been set correctly.
	bool getAmountToRefill(Item *item, unsigned int &amount, std::string &error) const;

	/// @brief Provides the list already loaded fuel items.
	ItemVector getAlreadyLoadedFuel() const;

	/// @brief Provides the remaining TIC, IF the light is turned on.
	double getAutonomy() const;

protected:
	void updateTicImpl() override;

	void updateHourImpl() override;
};
