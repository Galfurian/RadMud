/// @file   containerModel.cpp
/// @brief  Iplement the methods for Container.
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

#include "model/submodel/containerModel.hpp"

#include "utilities/logger.hpp"

ContainerModel::ContainerModel() : maxWeight(), containerFlags(), keyVnum(), difficulty()
{
	// Nothing to do.
}

ContainerModel::~ContainerModel()
{
	// Nothing to do.
}

ModelType ContainerModel::getType() const
{
	return ModelType::Container;
}

std::string ContainerModel::getTypeName() const
{
	return "Container";
}

bool ContainerModel::setModel(const std::string &source)
{
	if (source.empty()) {
		MudLog(LogLevel::Error, "Function list is empty (%s).", this->name);
		return false;
	}
	std::vector<std::string> functionList = SplitString(source, " ");
	if (functionList.size() != 4) {
		MudLog(LogLevel::Error, "Wrong number of parameters for Container Model (%s).", this->name);
		return false;
	}
	this->maxWeight = ToNumber<unsigned int>(functionList[0]);
	this->containerFlags = ToNumber<unsigned int>(functionList[1]);
	this->keyVnum = ToNumber<unsigned int>(functionList[2]);
	this->difficulty = ToNumber<unsigned int>(functionList[3]);
	return true;
}

void ContainerModel::getSheet(Table &sheet) const
{
	// Call the function of the father class.
	ItemModel::getSheet(sheet);
	// Add a divider.
	sheet.addDivider();
	// Set the values.
	sheet.addRow({ "Max Weight", ToString(this->maxWeight) });
	sheet.addRow({ "Flags", GetContainerFlagString(this->containerFlags) });
	sheet.addRow({ "Key Vnum", ToString(this->keyVnum) });
	sheet.addRow({ "Picklock Difficulty", ToString(this->difficulty) });
}

std::string GetContainerFlagString(unsigned int flags)
{
	std::string flagList;
	if (HasFlag(flags, ContainerFlag::CanClose))
		flagList += "|CanClose";
	if (HasFlag(flags, ContainerFlag::CanBurgle))
		flagList += "|CanBurgle";
	if (HasFlag(flags, ContainerFlag::CanSee))
		flagList += "|CanSee";
	flagList += "|";
	return flagList;
}
