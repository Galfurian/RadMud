/// @file   rangedWeaponItem.cpp
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

#include "rangedWeaponItem.hpp"
#include "rangedWeaponModel.hpp"
#include "magazineModel.hpp"
#include "projectileModel.hpp"
#include "formatter.hpp"

RangedWeaponItem::RangedWeaponItem()
{
    // Nothing to do.
}

RangedWeaponItem::~RangedWeaponItem()
{
    // Nothing to do.
}

void RangedWeaponItem::getSheet(Table & sheet) const
{
    // Call the function of the father class.
    Item::getSheet(sheet);
    // Add a divider.
    sheet.addDivider();
    // Set the values.
    sheet.addRow({"Min Damage", ToString(this->getMinDamage())});
    sheet.addRow({"Max Damage", ToString(this->getMaxDamage())});
    sheet.addRow({"Range     ", ToString(this->getRange())});
}

std::string RangedWeaponItem::lookContent()
{
    std::string output;
    if (content.empty())
    {
        output += Formatter::italic() + "It does not contain any magazine.\n" +
                  Formatter::reset();
    }
    else
    {
        auto containedMagazine = this->getAlreadyLoadedMagazine();
        if (containedMagazine != nullptr)
        {
            output += Formatter::italic();
            output +=
                "It is loaded with " + containedMagazine->getName(true) + "\n";
            output += Formatter::reset();
        }
    }
    return output;
}

unsigned int RangedWeaponItem::rollDamage() const
{
    return TRandInteger<unsigned int>(this->getMinDamage(),
                                      this->getMaxDamage());
}

unsigned int RangedWeaponItem::getMinDamage() const
{
    // Add the base value.
    auto valBase = this->model->toRangedWeapon()->minDamage;
    // Evaluate the modifier due to item's quality.
    valBase = SafeSum(valBase, valBase * quality.getModifier());
    // Evaluate the modifier due to item's condition.
    valBase = SafeSum(valBase, valBase * this->getConditionModifier());
    // Evaluate the mean.
    valBase = (valBase / 3);
    // Add the bonus from the contained projectiles.
    std::string error;
    auto projectile = this->retrieveProjectile(error);
    if (projectile != nullptr)
    {
        valBase = SafeSum(valBase,
                          projectile->model->toProjectile()->damageBonus);
    }
    return valBase;
}

unsigned int RangedWeaponItem::getMaxDamage() const
{
    // Add the base value.
    auto valBase = this->model->toRangedWeapon()->maxDamage;
    // Evaluate the modifier due to item's quality.
    valBase = SafeSum(valBase, valBase * quality.getModifier());
    // Evaluate the modifier due to item's condition.
    valBase = SafeSum(valBase, valBase * this->getConditionModifier());
    // Evaluate the mean.
    valBase = (valBase / 3);
    // Add the bonus from the contained projectiles.
    std::string error;
    auto projectile = this->retrieveProjectile(error);
    if (projectile != nullptr)
    {
        valBase = SafeSum(valBase,
                          projectile->model->toProjectile()->damageBonus);
    }
    return valBase;
}

int RangedWeaponItem::getRange() const
{
    // Add the base value.
    auto valBase = this->model->toRangedWeapon()->range;
    // Evaluate the modifier due to item's quality.
    valBase += static_cast<int>(valBase * quality.getModifier());
    // Evaluate the modifier due to item's condition.
    valBase += static_cast<int>(valBase * this->getConditionModifier());
    // Evaluate the mean.
    valBase /= 3;
    // Add the bonus from the contained projectiles.
    std::string error;
    auto projectile = this->retrieveProjectile(error);
    if (projectile != nullptr)
    {
        valBase += projectile->model->toProjectile()->rangeBonus;
    }
    return valBase;
}

bool RangedWeaponItem::canBeReloadedWith(Item * magazine) const
{
    if (magazine->getType() != ModelType::Magazine)
    {
        return false;
    }
    if (model->getType() != ModelType::Projectile)
    {
        return false;
    }
    return (magazine->model->toMagazine()->projectileType ==
            model->toRangedWeapon()->rangedWeaponType);
}


MagazineItem * RangedWeaponItem::getAlreadyLoadedMagazine() const
{
    if (this->isEmpty())
    {
        return nullptr;
    }
    Item * magazine = this->content.front();
    if (magazine == nullptr)
    {
        return nullptr;
    }
    if (magazine->getType() != ModelType::Magazine)
    {
        return nullptr;
    }
    return magazine->toMagazineItem();
}

Item * RangedWeaponItem::retrieveProjectile(std::string & error) const
{
    auto magazine = this->getAlreadyLoadedMagazine();
    if (magazine == nullptr)
    {
        error = "You need to reload " + this->getName(true);
    }
    else
    {
        auto projectile = magazine->getAlreadyLoadedProjectile();
        if (projectile == nullptr)
        {
            error = this->getNameCapital(true) + " contains an empty magazine";
        }
        else
        {
            return projectile;
        }
    }
    return nullptr;
}
