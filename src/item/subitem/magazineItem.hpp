/// @file   magazineItem.hpp
/// @author Enrico Fraccaroli
/// @date   Oct 11 2016
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

#include "containerItem.hpp"

/// @brief Holds details about a magazine.
class MagazineItem :
    public ContainerItem
{
public:
    MagazineItem();

    virtual ~MagazineItem();

    void getSheet(Table & sheet) const override;

    std::string lookContent() override;

    bool isAContainer() const override;

    /// @brief Checks if the current magazine can be loaded with the given item.
    /// @param _projectile The projectile to load.
    /// @param error       An error string, set when the projectile
    ///                    cannot be loaded.
    /// @return <b>True</b> if the projectile can be loaded,<br>
    ///         <b>False</b> otherwise.
    bool canLoadWith(Item * _projectile, std::string & error) const;

    /// @brief Evaluates the amount of projectiles which can be loaded.
    /// @param _projectile The projectile to load.
    /// @param amount      The amount to load.
    /// @param error       An error string, set when the projectile cannot
    ///                    be loaded.
    /// @return <b>True</b> if the projectile can be loaded,<br>
    ///         <b>False</b> otherwise.
    bool getAmountToLoad(Item * _projectile,
                         unsigned int & amount,
                         std::string & error) const;

    /// @brief Provides the list of already loaded projectiles.
    /// @return The contained projectiles.
    Item * getAlreadyLoadedProjectile() const;
};
