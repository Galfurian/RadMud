/// @file   ropeModel.cpp
/// @brief  Iplement the methods for Rope.
/// @author Enrico Fraccaroli
/// @date   Jul 6 2016
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

#include "ropeModel.hpp"

RopeModel::RopeModel() :
        difficulty(),
        ropeType()
{
    // Nothing to do.
}

RopeModel::~RopeModel()
{
    // Nothing to do.
}

ModelType RopeModel::getType() const
{
    return ModelType::Rope;
}

std::string RopeModel::getTypeName() const
{
    return "Rope";
}

bool RopeModel::setModel(const std::string & source)
{
    if (source.empty())
    {
        Logger::log(LogLevel::Error, "Function list is empty (%s).", this->name);
        return false;
    }
    std::vector<std::string> functionList = SplitString(source, " ");
    if (functionList.size() != 2)
    {
        Logger::log(LogLevel::Error, "Wrong number of parameters for Rope Model (%s).", this->name);
        return false;
    }
    this->difficulty = ToNumber<unsigned int>(functionList[0]);
    this->ropeType = ToNumber<unsigned int>(functionList[1]);
    return true;
}

void RopeModel::getSheet(Table & sheet) const
{
    // Call the function of the father class.
    ItemModel::getSheet(sheet);
    // Add a divider.
    sheet.addDivider();
    // Set the values.
    sheet.addRow( { "Untie Difficulty", ToString(this->difficulty) });
    sheet.addRow( { "Rope Type", ToString(this->ropeType) });
}
