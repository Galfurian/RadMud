/// @file   rangedWeaponModel.hpp
/// @brief  Define variables and methods of Ranged Weapons.
/// @author Enrico Fraccaroli
/// @date   Oct 9 2016
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
    unsigned int range;

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
