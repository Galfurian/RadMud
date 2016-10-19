/// @file   loadAction.hpp
/// @brief  Class which manage loading activities.
/// @author Enrico Fraccaroli
/// @date   Oct 10 2016
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

#include "generalAction.hpp"
#include "rangedWeaponItem.hpp"

/// @brief Allows to load something.
class LoadAction :
    public GeneralAction
{
private:
    /// The item which has to be loaded.
    Item * itemToBeLoaded;
    /// The projectile used to load the item.
    Item * projectile;

public:
    /// @brief Constructor.
    LoadAction(Item * _itemToBeLoaded, Item * _projectile, Character * _actor, unsigned int _cooldown);

    /// @brief Destructor.
    virtual ~LoadAction();

    bool check(std::string & error) const override;

    ActionType getType() const override;

    std::string getDescription() const override;

    std::string stop() override;

    ActionStatus perform() override;

private:
    /// @brief Checks the item tha has to be loaded.
    /// @return <b>True</b> if the item is available,<br>
    ///         <b>False</b> otherwise.
    bool checkItem() const;

    /// @brief Checks the projectile.
    /// @return <b>True</b> if the projectile is available,<br>
    ///         <b>False</b> otherwise.
    bool checkProjectile() const;
};
