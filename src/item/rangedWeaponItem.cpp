/// @file   rangedWeaponItem.cpp
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

#include "rangedWeaponItem.hpp"
#include "../model/submodel/rangedWeaponModel.hpp"
#include "../model/submodel/magazineModel.hpp"

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
