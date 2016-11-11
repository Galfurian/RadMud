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

unsigned int RangedWeaponItem::rollDamage() const
{
    return TRandInteger<unsigned int>(this->getMinDamage(), this->getMaxDamage());
}

unsigned int RangedWeaponItem::getMinDamage() const
{
    // Add the base value.
    auto valBase = this->model->toRangedWeapon()->minDamage;
    // Evaluate the modifier due to item's quality.
    auto valQuality = static_cast<unsigned int>(valBase * quality.getModifier());
    // Evaluate the modifier due to item's condition.
    auto valCondition = static_cast<unsigned int>(valBase * this->getConditionModifier());
    // The resulting value.
    return ((valBase + valQuality + valCondition) / 3);
}

unsigned int RangedWeaponItem::getMaxDamage() const
{
    // Add the base value.
    auto valBase = this->model->toRangedWeapon()->maxDamage;
    // Evaluate the modifier due to item's quality.
    auto valQuality = static_cast<unsigned int>(valBase * quality.getModifier());
    // Evaluate the modifier due to item's condition.
    auto valCondition = static_cast<unsigned int>(valBase * this->getConditionModifier());
    // The resulting value.
    return ((valBase + valQuality + valCondition) / 3);
}

unsigned int RangedWeaponItem::getRange() const
{
    // Add the base value.
    auto valBase = this->model->toRangedWeapon()->range;
    // Evaluate the modifier due to item's quality.
    auto valQuality = static_cast<unsigned int>(valBase * quality.getModifier());
    // Evaluate the modifier due to item's condition.
    auto valCondition = static_cast<unsigned int>(valBase * this->getConditionModifier());
    // The resulting value.
    return ((valBase + valQuality + valCondition) / 3);
}

bool RangedWeaponItem::canBeReloadedWith(Item * magazine) const
{
    RangedWeaponModel * rangedWeaponModel = this->model->toRangedWeapon();
    if (magazine->getType() == ModelType::Magazine)
    {
        MagazineModel * magazineModel = magazine->model->toMagazine();
        if (magazineModel->projectileType == rangedWeaponModel->rangedWeaponType)
        {
            return true;
        }
    }
    return false;
}


Item * RangedWeaponItem::getAlreadyLoadedMagazine() const
{
    if (!this->isEmpty())
    {
        Item * magazine = this->content.front();
        if (magazine != nullptr)
        {
            if (magazine->getType() == ModelType::Magazine)
            {
                return magazine;
            }
        }
    }
    return nullptr;
}
