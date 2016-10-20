/// @file   rangedWeaponItem.hpp
/// @author Enrico Fraccaroli
/// @date   Oct 09 2016
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
