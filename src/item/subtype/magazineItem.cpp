/// @file   magazineItem.cpp
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