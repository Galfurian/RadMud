/// @file   armorItem.cpp
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

#include "armorItem.hpp"
#include "material.hpp"
#include "armorModel.hpp"

ArmorItem::ArmorItem()
{
    // Nothing to do.
}

ArmorItem::~ArmorItem()
{
    // Nothing to do.
}

void ArmorItem::getSheet(Table & sheet) const
{
    // Call the function of the father class.
    Item::getSheet(sheet);
    // Add a divider.
    sheet.addDivider();
    // Set the values.
    sheet.addRow({"Armor Class", ToString(this->getArmorClass())});
}

unsigned int ArmorItem::getArmorClass() const
{
    // Add the base armor class.
    auto acBase = this->model->toArmor()->armorClass;
    // Evaluate the modifier due to item's quality.
    auto acQuality = static_cast<unsigned int>(acBase * quality.getModifier());
    // Evaluate the modifier due to item's condition.
    auto acCondition = static_cast<unsigned int>(acBase * this->getConditionModifier());
    // Evaluate the modifier due to item's material.
    auto acMaterial = static_cast<unsigned int>(acBase * this->composition->getHardnessModifier());
    // The resulting armor class.
    return ((acBase + acQuality + acCondition + acMaterial) / 4);
}
