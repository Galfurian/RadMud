/// @file   liquidContainerItem.hpp
/// @author Enrico Fraccaroli
/// @date   Jan 03 2017
/// @copyright
/// Copyright (c) 2017 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
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

/// @brief Holds details about a container of liquids.
class LiquidContainerItem : public Item {
public:
	/// The liquid inside the container.
	Liquid *liquidContent;
	/// The quantity.
	double liquidQuantity;

	LiquidContainerItem();

	virtual ~LiquidContainerItem();

	void getSheet(Table &sheet) const override;

	double getWeight(bool entireStack) const override;

	std::string lookContent() override;

	bool isAContainer() const override;

	bool isEmpty() const override;

	double getTotalSpace() const override;

	double getUsedSpace() const override;

	/// @brief Check if this item can contain the passed one.
	/// @param newLiquidContent The liquid to pour in.
	/// @param newLiquidAmount  The amount of liquid.
	/// @return <b>True</b> if it can be contained,<br>
	///         <b>False</b> otherwise.
	bool canContainLiquid(Liquid *newLiquidContent,
						  const double &newLiquidAmount) const;

	/// @brief Load some liquid inside the container and update the database.
	/// @param newLiquidContent The liquid to pour in.
	/// @param quantityToPourIn The amount of liquid.
	/// @param updateDB         If the action has to be updated on the database.
	/// @return <b>True</b> if the operation is a success,<br>
	///         <b>False</b> otherwise.
	bool pourIn(Liquid *newLiquidContent, const double &quantityToPourIn,
				bool updateDB = true);

	/// @brief Extract some liquid from the container and update the database.
	/// @param amount   The amount of liquid.
	/// @param updateDB If the action has to be updated on the database.
	/// @return <b>True</b> if the operation is a success,<br>
	///         <b>False</b> otherwise.
	bool pourOut(const double &amount, bool updateDB = true);

protected:
	void updateTicImpl() override;

	void updateHourImpl() override;
};
