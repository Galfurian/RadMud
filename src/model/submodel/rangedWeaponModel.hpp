/// @file   rangedWeaponModel.hpp
/// @brief  Define variables and methods of Ranged Weapons.
/// @author Enrico Fraccaroli
/// @date   Oct 9 2016
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

#pragma once

#include "itemModel.hpp"

/// Types of ranged weapons.
using RangedWeaponType = enum class RangedWeaponType_t
{
    None,
    Pistol,             ///< [1] Normal Pistols
    Rifle,              ///< [2] Normal Rifles
    Shotgun,            ///< [3] Normal Shotguns
    HeavyWeapon,        ///< [4] Normal Heavy Weapons
    EnergyPistol,       ///< [5] Pistols that uses energy
    EnergyRifle,        ///< [6] Rifles that uses energy
    EnergyHeavyWeapon,  ///< [7] Heavy Weapons that uses energy
    Granade,            ///< [8] Normal Granades
    Thrown,             ///< [9] Throwable weapons
};

/// @brief Model of a weapon.
class RangedWeaponModel :
    public ItemModel
{
public:
    /// The type of the ranged weapon.
    RangedWeaponType rangedWeaponType;
    /// The minimum damage of the weapon.
    unsigned int minDamage;
    /// The maximum damage of the weapon.
    unsigned int maxDamage;
    /// The range of the weapon.
    int range;

    RangedWeaponModel();

    virtual ~RangedWeaponModel();

    ModelType getType() const override;

    std::string getTypeName() const override;

    bool setModel(const std::string & source) override;

    void getSheet(Table & sheet) const override;
};

/// @addtogroup EnumToString
/// @{

/// Return the string describing the type of a Ranged Weapon.
std::string GetRangedWeaponTypeName(RangedWeaponType type);

///@}
