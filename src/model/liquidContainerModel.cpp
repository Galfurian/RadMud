/// @file   liquidContainerModel.cpp
/// @brief  Iplement the methods for LiquidContainer.
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

#include "liquidContainerModel.hpp"

LiquidContainerModel::LiquidContainerModel() :
    maxWeight(),
    liquidFlags()
{
    // Nothing to do.
}

LiquidContainerModel::~LiquidContainerModel()
{
    // Nothing to do.
}

ModelType LiquidContainerModel::getType() const
{
    return ModelType::LiquidContainer;
}

std::string LiquidContainerModel::getTypeName() const
{
    return "LiquidContainer";
}

bool LiquidContainerModel::setModel(const std::string & source)
{
    if (source.empty())
    {
        Logger::log(LogLevel::Error, "Function list is empty (%s).", this->name);
        return false;
    }
    std::vector<std::string> functionList = SplitString(source, " ");
    if (functionList.size() != 2)
    {
        Logger::log(
            LogLevel::Error,
            "Wrong number of parameters for Liquid Container Model (%s).",
            this->name);
        return false;
    }
    this->maxWeight = ToNumber<unsigned int>(functionList[0]);
    this->liquidFlags = ToNumber<unsigned int>(functionList[1]);
    return true;
}

void LiquidContainerModel::getSheet(Table & sheet) const
{
    // Call the function of the father class.
    ItemModel::getSheet(sheet);
    // Add a divider.
    sheet.addDivider();
    // Set the values.
    sheet.addRow({"Max Weight", ToString(this->maxWeight)});
    sheet.addRow({"Flags", GetLiqContainerFlagString(this->liquidFlags)});
}

std::string GetLiqContainerFlagString(unsigned int flags)
{
    std::string flagList;
    if (HasFlag(flags, LiqContainerFlag::Poisoned)) flagList += "|Poisoned";
    if (HasFlag(flags, LiqContainerFlag::Endless)) flagList += "|Endless";
    if (HasFlag(flags, LiqContainerFlag::Destroy)) flagList += "|Destroy";
    flagList += "|";
    return flagList;
}
