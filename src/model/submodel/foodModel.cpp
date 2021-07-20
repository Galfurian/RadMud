/// @file   foodModel.cpp
/// @brief  Iplement the methods for Food.
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

#include "model/submodel/foodModel.hpp"

#include "utilities/logger.hpp"

FoodModel::FoodModel() : nurishment(), foodFlags()
{
	// Nothing to do.
}

FoodModel::~FoodModel()
{
	// Nothing to do.
}

ModelType FoodModel::getType() const
{
	return ModelType::Food;
}

std::string FoodModel::getTypeName() const
{
	return "Food";
}

bool FoodModel::setModel(const std::vector<std::string> &source)
{
	if (source.empty()) {
		MudLog(LogLevel::Error, "Function list is empty (%s).", name);
		return false;
	}
	if (source.size() != 2) {
		MudLog(LogLevel::Error, "Wrong number of parameters for Food Model (%s).", name);
		return false;
	}
	nurishment = ToNumber<unsigned int>(source[0]);
	foodFlags = ToNumber<unsigned int>(source[1]);
	return true;
}

void FoodModel::getSheet(Table &sheet) const
{
	// Call the function of the father class.
	ItemModel::getSheet(sheet);
	// Add a divider.
	sheet.addDivider();
	// Set the values.
	sheet.addRow({ "Nurishment", ToString(nurishment) });
	sheet.addRow({ "Flags", GetFoodFlagString(foodFlags) });
}

std::string GetFoodFlagString(unsigned int flags)
{
	std::string flagList;
	if (HasFlag(flags, FoodFlag::Poisoned))
		flagList += "|Poisoned";
	if (HasFlag(flags, FoodFlag::Raw))
		flagList += "|Raw";
	flagList += "|";
	return flagList;
}
