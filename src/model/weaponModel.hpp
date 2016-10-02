/// @file   weaponModel.hpp
/// @brief  Define variables and methods of Weapon.
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

#pragma once

#include "itemModel.hpp"

/// Types of weapons.
using WeaponType = enum class WeaponType_t
{
    None,
    Pistol,             ///< [1]  Pistols that uses bullets
    Rifle,              ///< [2]  Rifles that uses bullets
    Shotgun,            ///< [3]  Shotguns
    HeavyWeapon,        ///< [4]  Big guns, heavy weapons that uses bullets
    EnergyPistol,       ///< [5]  Pistols that uses energy
    EnergyRifle,        ///< [6]  Rifles that uses energy.
    EnergyHeavyWeapon,  ///< [7]  Big guns, heavy weapons that uses energy.
    Granade,            ///< [8]  Granades
    Placed,             ///< [9]  Placeble explosives
    Bladed,             ///< [10] All sort of blades
    Blunt,              ///< [11] All sort of blunt weapons
    Thrown,             ///< [12] Throwable weapons
    Unarmed,            ///< [13] Weapon to enanche unarmed fight
};

/// @brief Model of a weapon.
class WeaponModel :
    public ItemModel
{
public:
    /// The type of the weapon.
    WeaponType weaponType;
    /// The minimum damage of the weapon.
    unsigned int minDamage;
    /// The maximum damage of the weapon.
    unsigned int maxDamage;
    /// The range of the weapon.
    unsigned int range;

    WeaponModel();

    virtual ~WeaponModel();

    ModelType getType() const override;

    std::string getTypeName() const override;

    bool setModel(const std::string & source) override;

    void getSheet(Table & sheet) const override;
};

/// @addtogroup EnumToString
/// @{

/// Return the string describing the type of a Weapon.
std::string GetWeaponTypeName(WeaponType type);

///@}
