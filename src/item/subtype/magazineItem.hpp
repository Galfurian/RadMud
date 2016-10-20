/// @file   magazineItem.hpp
/// @author Enrico Fraccaroli
/// @date   Oct 11 2016
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

/// @brief Holds details about a magazine.
class MagazineItem :
    public Item
{
public:
    MagazineItem();

    virtual ~MagazineItem();

    void getSheet(Table & sheet) const override;

    /// @brief Checks if the current magazine can be loaded with the given item.
    /// @param projectile The projectile to load.
    /// @param error      An error string, set when the projectile cannot be loaded.
    /// @return <b>True</b> if the projectile can be loaded,<br>
    ///         <b>False</b> otherwise.
    bool canLoadWith(Item * projectile, std::string & error) const;

    /// @brief Evaluates the ammount of projectiles which can be loaded.
    /// @param projectile The projectile to load.
    /// @param ammount    The ammount to load.
    /// @param error      An error string, set when the projectile cannot be loaded.
    /// @return <b>True</b> if the projectile can be loaded,<br>
    ///         <b>False</b> otherwise.
    bool getAmountToLoad(Item * projectile, unsigned int & ammount, std::string & error) const;

    /// @brief Provides the list of already loaded projectiles.
    /// @return The contained projectiles.
    Item * getAlreadyLoadedProjectile() const;
};
