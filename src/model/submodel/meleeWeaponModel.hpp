/// @file   meleeWeaponModel.hpp
/// @brief  Define variables and methods of Weapon.
/// @author Enrico Fraccaroli
/// @date   Jul 6 2016
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

/// Types of weapons.
using MeleeWeaponType = enum class MeleeWeaponType_t
{
    None,
    Unarmed, ///< [1] Weapon to enanche unarmed fight
    Bladed,  ///< [2] All sort of blades
    Blunt,   ///< [3] All sort of blunt weapons
    Placed,  ///< [4]  Placeble explosives
};

/// @brief Model of a weapon.
class MeleeWeaponModel :
    public ItemModel
{
public:
    /// The type of the melee weapon.
    MeleeWeaponType meleeWeaponType;
    /// The minimum damage of the weapon.
    unsigned int minDamage;
    /// The maximum damage of the weapon.
    unsigned int maxDamage;

    MeleeWeaponModel();

    virtual ~MeleeWeaponModel();

    ModelType getType() const override;

    std::string getTypeName() const override;

    bool setModel(const std::string & source) override;

    void getSheet(Table & sheet) const override;
};

/// @addtogroup EnumToString
/// @{

/// Return the string describing the type of a Melee Weapon.
std::string GetMeleeWeaponTypeName(MeleeWeaponType type);

///@}
