/// @file   vehicleModel.cpp
/// @brief  Iplement the methods for Vehicle.
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

#include "model/submodel/vehicleModel.hpp"

#include "utilities/logger.hpp"

VehicleModel::VehicleModel()
{
	// Nothing to do.
}

VehicleModel::~VehicleModel()
{
	// Nothing to do.
}

ModelType VehicleModel::getType() const
{
	return ModelType::Vehicle;
}

std::string VehicleModel::getTypeName() const
{
	return "Vehicle";
}

void VehicleModel::getSheet(Table &sheet) const
{
	// Call the function of the father class.
	ItemModel::getSheet(sheet);
	// Add a divider.
	//sheet.addDivider();
}

bool VehicleModel::setModel(const std::string &source)
{
	if (source.empty()) {
		MudLog(LogLevel::Error, "Function list is empty (%s).", name);
		return false;
	}
	std::vector<std::string> functionList = SplitString(source, " ");
	if (functionList.size() != 0) {
		MudLog(LogLevel::Error,
			   "Wrong number of parameters for Vehicle Model (%s).", name);
		return false;
	}
	return true;
}
