/// @file   rangedWeaponItem.hpp
/// @author Enrico Fraccaroli
/// @date   Oct 09 2016
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

#include "item.hpp"

/// @brief Holds details about a ranged weapon.
class RangedWeaponItem :
    public Item
{
public:
    RangedWeaponItem();

    virtual ~RangedWeaponItem();

    void getSheet(Table & sheet) const override;

    /// Randomly roll the weapon damage.
    unsigned int rollDamage() const;

    /// Provides the minimum damage.
    unsigned int getMinDamage() const;

    /// Provides the maximum damage.
    unsigned int getMaxDamage() const;

    /// Provides the range of this weapon.
    unsigned int getRange() const;

    /// @brief Checks if the current weapon can be reloaded with the given item.
    /// @param magazine The magazine to check.
    /// @return <b>True</b> if the ranged weapon can be reloaded with the given magazine,<br>
    ///         <b>False</b> otherwise.
    bool canBeReloadedWith(Item * magazine) const;

    /// @brief Search for an already loaded magazine.
    /// @return A magazine already loaded if there is one.
    Item * getAlreadyLoadedMagazine() const;
};
