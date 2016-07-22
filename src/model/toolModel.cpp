/// @file   toolModel.cpp
/// @brief  Iplement the methods for Tool.
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

#include "toolModel.hpp"

ToolModel::ToolModel() :
        toolType()
{
    // Nothing to do.
}

ToolModel::~ToolModel()
{
    // Nothing to do.
}

ModelType ToolModel::getType() const
{
    return ModelType::Tool;
}

std::string ToolModel::getTypeName() const
{
    return "Tool";
}

bool ToolModel::setModel(const std::string & source)
{
    if (source.empty())
    {
        Logger::log(LogLevel::Error, "Function list is empty (%s).", this->name);
        return false;
    }
    std::vector<std::string> functionList = SplitString(source, " ");
    if (functionList.size() != 1)
    {
        Logger::log(LogLevel::Error, "Wrong number of parameters for Tool Model (%s).", this->name);
        return false;
    }
    this->toolType = static_cast<ToolType>(ToNumber<unsigned int>(functionList[0]));
    return true;
}

void ToolModel::getSheet(Table & sheet)
{
    // Call the function of the father class.
    ItemModel::getSheet(sheet);
    // Add a divider.
    sheet.addDivider();
    // Set the values.
    sheet.addRow( { "Tool Type", GetToolTypeName(this->toolType) });
}

std::string GetToolTypeName(ToolType type)
{
    if (type == ToolType::Pickaxe) return "Pickaxe";
    if (type == ToolType::WoodcutterAxe) return "Woodcutter's Axe";
    if (type == ToolType::Saw) return "Saw";
    if (type == ToolType::PrecisionChisel) return "Precision Chisel";
    if (type == ToolType::PlaneChisel) return "PickaPlane Chiselxe";
    if (type == ToolType::Hammer) return "Hammer";
    if (type == ToolType::Forge) return "Forge";
    if (type == ToolType::Anvil) return "Anvil";
    if (type == ToolType::BlacksmithHammer) return "Blacksmith Hammer";
    if (type == ToolType::Bellows) return "Bellows";
    if (type == ToolType::Crucible) return "Crucible";
    if (type == ToolType::Tinderbox) return "Tinderbox";
    return "No Tool Type";
}
