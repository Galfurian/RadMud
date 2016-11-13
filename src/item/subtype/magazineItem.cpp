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
        Item * projectile = this->content.front();
        if (projectile == nullptr)
        {
            sheet.addRow({"Projectile", "NULL"});
        }
        else if (projectile->getType() != ModelType::Projectile)
        {
            sheet.addRow({"Projectile", "NULL"});
        }
        else
        {
            sheet.addRow({"Projectile", projectile->getName(false)});
        }
    }
}

bool MagazineItem::canLoadWith(Item * projectile, std::string & error) const
{
    if (projectile->getType() != ModelType::Projectile)
    {
        error = "You can't load " + this->getName(true) + " with " + projectile->getName(true);
        return false;
    }
    // Retrieve any already loaded projectiles.
    Item * alreadyLoaded = this->getAlreadyLoadedProjectile();
    if (alreadyLoaded != nullptr)
    {
        // If there are projectiles inside, check if the two types of projectiles are compatible.
        if (!alreadyLoaded->canStackWith(projectile))
        {
            error = "The magazine already contains a different type of projectiles";
            return false;
        }
    }
    return true;
}

bool MagazineItem::getAmountToLoad(Item * projectile, unsigned int & ammount, std::string & error) const
{
    if (!this->canLoadWith(projectile, error))
    {
        return false;
    }
    // Set by default the ammout to load to the maximum.
    ammount = this->model->toMagazine()->maxAmmount;
    unsigned int ammountAlreadyLoaded = 0;
    // Retrieve any already loaded projectiles.
    Item * alreadyLoaded = this->getAlreadyLoadedProjectile();
    if (alreadyLoaded != nullptr)
    {
        // Set the ammount of already loaded projectiles.
        ammountAlreadyLoaded = alreadyLoaded->quantity;
        if (ammount <= ammountAlreadyLoaded)
        {
            error = this->getNameCapital(true) + " is already at full capacity.";
            return false;
        }
        ammount -= ammountAlreadyLoaded;
    }
    return true;
}

Item * MagazineItem::getAlreadyLoadedProjectile() const
{
    if (!this->isEmpty())
    {
        Item * projectile = this->content.front();
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
