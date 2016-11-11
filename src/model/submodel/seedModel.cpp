/// @file   seedModel.cpp
/// @brief  Iplement the methods for Seed.
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

#include "seedModel.hpp"

SeedModel::SeedModel() :
    seedType()
{
    // Nothing to do.
}

SeedModel::~SeedModel()
{
    // Nothing to do.
}

ModelType SeedModel::getType() const
{
    return ModelType::Seed;
}

std::string SeedModel::getTypeName() const
{
    return "Seed";
}

bool SeedModel::setModel(const std::string & source)
{
    if (source.empty())
    {
        Logger::log(LogLevel::Error, "Function list is empty (%s).", this->name);
        return false;
    }
    std::vector<std::string> functionList = SplitString(source, " ");
    if (functionList.size() != 1)
    {
        Logger::log(LogLevel::Error, "Wrong number of parameters for Seed Model (%s).", this->name);
        return false;
    }
    this->seedType = static_cast<SeedType>(ToNumber<unsigned int>(functionList[0]));
    return true;
}

void SeedModel::getSheet(Table & sheet) const
{
    // Call the function of the father class.
    ItemModel::getSheet(sheet);
    // Add a divider.
    sheet.addDivider();
    // Set the values.
    sheet.addRow({"Seed Type", GetSeedTypeName(this->seedType)});
}

std::string GetSeedTypeName(SeedType type)
{
    if (type == SeedType::Plant) return "Plant Seed";
    if (type == SeedType::Tree) return "Tree Seed";
    return "No Seed Type";
}
