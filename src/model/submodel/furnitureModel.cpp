/// @file   furnitureModel.cpp
/// @brief  Iplement the methods for Furniture.
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

#include "model/submodel/furnitureModel.hpp"

#include "utilities/logger.hpp"

FurnitureModel::FurnitureModel()
{
    // Nothing to do.
}

FurnitureModel::~FurnitureModel()
{
    // Nothing to do.
}

ModelType FurnitureModel::getType() const
{
    return ModelType::Furniture;
}

std::string FurnitureModel::getTypeName() const
{
    return "Furniture";
}

bool FurnitureModel::setModel(const std::string & source)
{
    std::vector<std::string> functionList = SplitString(source, " ");
    if (functionList.size() != 0)
    {
        Logger::log(
            LogLevel::Error,
            "Wrong number of parameters for Furniture Model (%s).",
            this->name);
        return false;
    }
    return true;
}

void FurnitureModel::getSheet(Table & sheet) const
{
    // Call the function of the father class.
    ItemModel::getSheet(sheet);
    // Add a divider.
    //sheet.addDivider();
}
