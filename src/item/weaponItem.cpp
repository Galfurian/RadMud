/// @file   weaponItem.cpp
/// @author Enrico Fraccaroli
/// @date   Aug 04 2016
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

#include "weaponItem.hpp"

#include "../model/weaponModel.hpp"
#include "../material.hpp"
#include "../utils.hpp"

WeaponItem::WeaponItem()
{
    // Nothing to do.
}

WeaponItem::~WeaponItem()
{
    // Nothing to do.
}

ItemType WeaponItem::getType() const
{
    return ItemType::Weapon;
}

std::string WeaponItem::getTypeName() const
{
    return "weapon";
}

void WeaponItem::getSheet(Table & sheet) const
{
    // Call the function of the father class.
    Item::getSheet(sheet);
    // Add a divider.
    sheet.addDivider();
    // Set the values.
    sheet.addRow( { "Min Damage", ToString(this->getMinDamage()) });
    sheet.addRow( { "Max Damage", ToString(this->getMaxDamage()) });
    sheet.addRow( { "Range     ", ToString(this->getRange()) });
}

unsigned int WeaponItem::rollDamage() const
{
    return TRandInteger<unsigned int>(this->getMinDamage(), this->getMaxDamage());
}

unsigned int WeaponItem::getMinDamage() const
{
    // Add the base value.
    auto valBase = this->model->toWeapon()->minDamage;
    // Evaluate the modifier due to item's quality.
    auto valQuality = static_cast<unsigned int>(valBase * quality.getModifier());
    // Evaluate the modifier due to item's condition.
    auto valCondition = static_cast<unsigned int>(valBase * this->getConditionModifier());
    // The resulting value.
    return ((valBase + valQuality + valCondition) / 3);
}

unsigned int WeaponItem::getMaxDamage() const
{
    // Add the base value.
    auto valBase = this->model->toWeapon()->maxDamage;
    // Evaluate the modifier due to item's quality.
    auto valQuality = static_cast<unsigned int>(valBase * quality.getModifier());
    // Evaluate the modifier due to item's condition.
    auto valCondition = static_cast<unsigned int>(valBase * this->getConditionModifier());
    // The resulting value.
    return ((valBase + valQuality + valCondition) / 3);
}

unsigned int WeaponItem::getRange() const
{
    // Add the base value.
    auto valBase = this->model->toWeapon()->range;
    // Evaluate the modifier due to item's quality.
    auto valQuality = static_cast<unsigned int>(valBase * quality.getModifier());
    // Evaluate the modifier due to item's condition.
    auto valCondition = static_cast<unsigned int>(valBase * this->getConditionModifier());
    // The resulting value.
    return ((valBase + valQuality + valCondition) / 3);
}
