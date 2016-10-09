/// @file   meleeWeaponModel.cpp
/// @brief  Iplement the methods for Weapon.
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

#include "meleeWeaponModel.hpp"

MeleeWeaponModel::MeleeWeaponModel() :
    meleeWeaponType(),
    minDamage(),
    maxDamage()
{
    // Nothing to do.
}

MeleeWeaponModel::~MeleeWeaponModel()
{
    // Nothing to do.
}

ModelType MeleeWeaponModel::getType() const
{
    return ModelType::MeleeWeapon;
}

std::string MeleeWeaponModel::getTypeName() const
{
    return "Melee Weapon";
}

bool MeleeWeaponModel::setModel(const std::string & source)
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
            "Wrong number of parameters for Weapon Model (%s).",
            this->name);
        return false;
    }
    this->meleeWeaponType = static_cast<MeleeWeaponType>(ToNumber<unsigned int>(functionList[0]));
    this->minDamage = ToNumber<unsigned int>(functionList[1]);
    this->maxDamage = ToNumber<unsigned int>(functionList[2]);
    return true;
}

void MeleeWeaponModel::getSheet(Table & sheet) const
{
    // Call the function of the father class.
    ItemModel::getSheet(sheet);
    // Add a divider.
    sheet.addDivider();
    // Set the values.
    sheet.addRow({"Melee Weapon Type", GetMeleeWeaponTypeName(this->meleeWeaponType)});
    sheet.addRow({"Minimum Damage", ToString(this->minDamage)});
    sheet.addRow({"Maximum Damage", ToString(this->maxDamage)});
}

std::string GetMeleeWeaponTypeName(MeleeWeaponType type)
{
    if (type == MeleeWeaponType::Unarmed) return "Unarmed";
    if (type == MeleeWeaponType::Bladed) return "Bladed";
    if (type == MeleeWeaponType::Blunt) return "Blunt";
    if (type == MeleeWeaponType::Placed) return "Placed";
    return "No Weapon Type";
}
