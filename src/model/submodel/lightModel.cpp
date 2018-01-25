/// @file   lightModel.cpp
/// @brief  Iplement the methods for Light.
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

#include "lightModel.hpp"

#include "logger.hpp"

LightModel::LightModel() :
    fuelType(),
    radius(),
    maxWeight(),
    lightSourceFlags()
{
    // Nothing to do.
}

LightModel::~LightModel()
{
    // Nothing to do.
}

ModelType LightModel::getType() const
{
    return ModelType::Light;
}

std::string LightModel::getTypeName() const
{
    return "Light";
}

bool LightModel::setModel(const std::string & source)
{
    if (source.empty())
    {
        Logger::log(LogLevel::Error, "Function list is empty (%s).", name);
        return false;
    }
    auto functionList = SplitString(source, " ");
    if (functionList.size() != 4)
    {
        Logger::log(LogLevel::Error,
                    "Wrong number of parameters for Light Model (%s)[%s].",
                    name, source);
        return false;
    }
    fuelType = ResourceType(ToNumber<unsigned int>(functionList[0]));
    radius = ToNumber<int>(functionList[1]);
    maxWeight = ToNumber<double>(functionList[2]);
    lightSourceFlags = ToNumber<unsigned int>(functionList[3]);
    return true;
}

void LightModel::getSheet(Table & sheet) const
{
    // Call the function of the father class.
    ItemModel::getSheet(sheet);
    // Add a divider.
    sheet.addDivider();
    // Set the values.
    sheet.addRow({"Fuel Type", fuelType.toString()});
    sheet.addRow({"Radius", ToString(radius)});
    sheet.addRow({"Max Weight", ToString(maxWeight)});
    sheet.addRow({"Flags", ToString(lightSourceFlags)});
}
