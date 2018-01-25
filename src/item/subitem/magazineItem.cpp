/// @file   magazineItem.cpp
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

#include "magazineItem.hpp"
#include "magazineModel.hpp"
#include "formatter.hpp"

MagazineItem::MagazineItem()
{
    // Nothing to do.
}

MagazineItem::~MagazineItem()
{
    // Nothing to do.
}

void MagazineItem::getSheet(Table & sheet) const
{
    // Call the function of the father class.
    Item::getSheet(sheet);
    // Add a divider.
    sheet.addDivider();
    // Set the values.
    if (!this->isEmpty())
    {
        // Get the already loaded projectile.
        auto loadedProjectile = this->getAlreadyLoadedProjectile();
        if (loadedProjectile == nullptr)
        {
            sheet.addRow({"Projectile", "NULL"});
        }
        else
        {
            sheet.addRow({"Projectile", loadedProjectile->getName(false)});
        }
    }
}

std::string MagazineItem::lookContent()
{
    std::string output;
    if (content.empty())
    {
        output +=
            Formatter::italic() + "It does not contain any projectiles.\n" +
            Formatter::reset();
    }
    else
    {
        // Get the already loaded projectile.
        auto loadedProjectile = this->getAlreadyLoadedProjectile();
        if (loadedProjectile != nullptr)
        {
            output += Formatter::italic();
            output += "It contains " + loadedProjectile->getName(true) + "[";
            output += ToString(loadedProjectile->quantity) + "].\n";
            output += Formatter::reset();
        }
    }
    return output;
}

bool MagazineItem::isAContainer() const
{
    return true;
}

bool MagazineItem::canLoadWith(Item * _projectile, std::string & error) const
{
    if (_projectile->getType() != ModelType::Projectile)
    {
        error = "You can't load " + this->getName(true) + " with " +
                _projectile->getName(true);
        return false;
    }
    // Retrieve any already loaded projectiles.
    auto loadedProjectile = this->getAlreadyLoadedProjectile();
    if (loadedProjectile != nullptr)
    {
        // If there are projectiles inside, check if the two types of
        //  projectiles are compatible.
        if (!loadedProjectile->canStackWith(_projectile))
        {
            error = "The magazine already contains a different"
                " type of projectiles";
            return false;
        }
    }
    return true;
}

bool MagazineItem::getAmountToLoad(Item * _projectile,
                                   unsigned int & amount,
                                   std::string & error) const
{
    if (!this->canLoadWith(_projectile, error))
    {
        return false;
    }
    // Set by default the amount to load to the maximum.
    amount = this->model->toMagazine()->maxAmount;
    unsigned int amountAlreadyLoaded = 0;
    // Retrieve any already loaded projectiles.
    auto loadedProjectile = this->getAlreadyLoadedProjectile();
    if (loadedProjectile != nullptr)
    {
        // Set the amount of already loaded projectiles.
        amountAlreadyLoaded = loadedProjectile->quantity;
        if (amount <= amountAlreadyLoaded)
        {
            error = this->getNameCapital(true) +
                    " is already at full capacity.";
            return false;
        }
        amount -= amountAlreadyLoaded;
    }
    return true;
}

Item * MagazineItem::getAlreadyLoadedProjectile() const
{
    if (!this->isEmpty())
    {
        // Get the first element of the content.
        auto projectile = content.front();
        if (projectile != nullptr)
        {
            if (projectile->getType() == ModelType::Projectile)
            {
                return projectile;
            }
        }
    }
    return nullptr;
}
