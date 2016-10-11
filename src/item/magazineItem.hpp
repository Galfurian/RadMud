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

    /// Checks if the current magazine can be loaded with the given item.
    bool canLoadWith(Item * projectileToLoad, std::string & error) const;

    /// Evaluates the ammount of projectiles which can be loaded.
    bool getAmountToLoad(Item * projectileToLoad, unsigned int & ammountToLoad, std::string & error) const;

private:
    Item * getAlreadyLoadedProjectile() const;
};
