/*
 * armorItem.cpp
 *
 *  Created on: Aug 4, 2016
 *      Author: enrico
 */

#include "armorItem.hpp"

#include "../model/armorModel.hpp"
#include "../material.hpp"

ArmorItem::ArmorItem()
{
    // Nothing to do.
}

ArmorItem::~ArmorItem()
{
    // Nothing to do.
}

ItemType ArmorItem::getType() const
{
    return ItemType::Armor;
}

std::string ArmorItem::getTypeName() const
{
    return "armor";
}

void ArmorItem::getSheet(Table & sheet) const
{
    // Call the function of the father class.
    Item::getSheet(sheet);
    // Add a divider.
    sheet.addDivider();
    // Set the values.
    sheet.addRow( { "Armor Class", ToString(this->getArmorClass()) });
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
