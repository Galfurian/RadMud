/// @file   weaponModel.cpp
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

#include "weaponModel.hpp"

WeaponModel::WeaponModel() :
    weaponType(),
    minDamage(),
    maxDamage(),
    range()
{
    // Nothing to do.
}

WeaponModel::~WeaponModel()
{
    // Nothing to do.
}

ModelType WeaponModel::getType() const
{
    return ModelType::Weapon;
}

std::string WeaponModel::getTypeName() const
{
    return "Weapon";
}

bool WeaponModel::setModel(const std::string & source)
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
            "Wrong number of parameters for Weapon Model (%s).",
            this->name);
        return false;
    }
    this->weaponType = static_cast<WeaponType>(ToNumber<unsigned int>(functionList[0]));
    this->minDamage = ToNumber<unsigned int>(functionList[1]);
    this->maxDamage = ToNumber<unsigned int>(functionList[2]);
    this->range = ToNumber<unsigned int>(functionList[3]);
    return true;
}

void WeaponModel::getSheet(Table & sheet) const
{
    // Call the function of the father class.
    ItemModel::getSheet(sheet);
    // Add a divider.
    sheet.addDivider();
    // Set the values.
    sheet.addRow({"Weapon Type", GetWeaponTypeName(this->weaponType)});
    sheet.addRow({"Minimum Damage", ToString(this->minDamage)});
    sheet.addRow({"Maximum Damage", ToString(this->maxDamage)});
    sheet.addRow({"Range", ToString(this->range)});
}

std::string GetWeaponTypeName(WeaponType type)
{
    if (type == WeaponType::Pistol) return "Pistol";
    if (type == WeaponType::Rifle) return "Rifle";
    if (type == WeaponType::Shotgun) return "Shotgun";
    if (type == WeaponType::HeavyWeapon) return "Heavy Weapon";
    if (type == WeaponType::EnergyPistol) return "Energy Pistol";
    if (type == WeaponType::EnergyRifle) return "Energy Rifle";
    if (type == WeaponType::EnergyHeavyWeapon) return "Energy Heavy Weapon";
    if (type == WeaponType::Granade) return "Granade";
    if (type == WeaponType::Placed) return "Placed";
    if (type == WeaponType::Bladed) return "Bladed";
    if (type == WeaponType::Blunt) return "Blunt";
    if (type == WeaponType::Thrown) return "Thrown";
    if (type == WeaponType::Unarmed) return "Unarmed";
    return "No Weapon Type";
}
