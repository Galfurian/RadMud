/// @file   armorModel.cpp
/// @brief  Iplement the methods for Armor.
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

#include "armorModel.hpp"

ArmorModel::ArmorModel() :
        size(),
        damageAbs(),
        allowedAnatomy()
{
    // Nothing to do.
}

ArmorModel::~ArmorModel()
{
    // Nothing to do.
}

ModelType ArmorModel::getType() const
{
    return ModelType::Armor;
}

std::string ArmorModel::getTypeName() const
{
    return "Armor";
}

bool ArmorModel::setModel(const std::string & source)
{
    if (source.empty())
    {
        Logger::log(LogLevel::Error, "Function list is empty (%s).", this->name);
        return false;
    }
    std::vector<std::string> functionList = SplitString(source, " ");
    if (functionList.size() != 3)
    {
        Logger::log(
            LogLevel::Error,
            "Wrong number of parameters for Armor Model (%s).",
            this->name);
        return false;
    }
    this->size = static_cast<ArmorSize>(ToNumber<unsigned int>(functionList[0]));
    this->damageAbs = ToNumber<unsigned int>(functionList[1]);
    this->allowedAnatomy = ToNumber<unsigned int>(functionList[2]);
    return true;
}

void ArmorModel::getSheet(Table & sheet)
{
    // Call the function of the father class.
    ItemModel::getSheet(sheet);
    // Add a divider.
    sheet.addDivider();
    // Set the values.
    sheet.addRow( { "Size", GetArmorSizeName(this->size) });
    sheet.addRow( { "Damage Absorption", ToString(this->damageAbs) });
    sheet.addRow( { "Allowed Anatomies", ToString(this->allowedAnatomy) });
}

std::string GetArmorSizeName(ArmorSize armorSize)
{
    if (armorSize == ArmorSize::All) return "Universal Armor";
    if (armorSize == ArmorSize::Light) return "Light Armor";
    if (armorSize == ArmorSize::Medium) return "Medium Armor";
    if (armorSize == ArmorSize::Heavy) return "Heavy Armor";
    return "No Armor Size";
}
