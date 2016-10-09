/// @file   rangedWeaponModel.cpp
/// @brief  Iplement the methods for Ranged Weapons.
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

#include "rangedWeaponModel.hpp"

RangedWeaponModel::RangedWeaponModel() :
    rangedWeaponType(),
    minDamage(),
    maxDamage(),
    range()
{
    // Nothing to do.
}

RangedWeaponModel::~RangedWeaponModel()
{
    // Nothing to do.
}

ModelType RangedWeaponModel::getType() const
{
    return ModelType::RangedWeapon;
}

std::string RangedWeaponModel::getTypeName() const
{
    return "Ranged Weapon";
}

bool RangedWeaponModel::setModel(const std::string & source)
{
    if (source.empty())
    {
        Logger::log(LogLevel::Error, "Function list is empty (%s).", this->name);
        return false;
    }
    std::vector<std::string> functionList = SplitString(source, " ");
    if (functionList.size() != 4)
    {
        Logger::log(
            LogLevel::Error,
            "Wrong number of parameters for Ranged Weapon Model (%s).",
            this->name);
        return false;
    }
    this->rangedWeaponType = static_cast<RangedWeaponType>(ToNumber<unsigned int>(functionList[0]));
    this->minDamage = ToNumber<unsigned int>(functionList[1]);
    this->maxDamage = ToNumber<unsigned int>(functionList[2]);
    this->range = ToNumber<unsigned int>(functionList[3]);
    return true;
}

void RangedWeaponModel::getSheet(Table & sheet) const
{
    // Call the function of the father class.
    ItemModel::getSheet(sheet);
    // Add a divider.
    sheet.addDivider();
    // Set the values.
    sheet.addRow({"Ranged Weapon Type", GetRangedWeaponTypeName(this->rangedWeaponType)});
    sheet.addRow({"Minimum Damage", ToString(this->minDamage)});
    sheet.addRow({"Maximum Damage", ToString(this->maxDamage)});
    sheet.addRow({"Range", ToString(this->range)});
}

std::string GetRangedWeaponTypeName(RangedWeaponType type)
{
    if (type == RangedWeaponType::Pistol) return "Pistol";
    if (type == RangedWeaponType::Rifle) return "Rifle";
    if (type == RangedWeaponType::Shotgun) return "Shotgun";
    if (type == RangedWeaponType::HeavyWeapon) return "Heavy Weapon";
    if (type == RangedWeaponType::EnergyPistol) return "Energy Pistol";
    if (type == RangedWeaponType::EnergyRifle) return "Energy Rifle";
    if (type == RangedWeaponType::EnergyHeavyWeapon) return "Energy Heavy Weapon";
    if (type == RangedWeaponType::Granade) return "Granade";
    if (type == RangedWeaponType::Thrown) return "Thrown";
    return "No Weapon Type";
}
