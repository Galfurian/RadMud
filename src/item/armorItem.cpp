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
    // The armor model.
    ArmorModel * armorModel = this->model->toArmor();
    // The resulting armor class.
    unsigned int result = 0;
    // Add the damage absorption.
    result += armorModel->damageAbs;
    // Add the material's hardness.
    // TODO: Need to be fixed, base value of damage abs is too low.
    result += static_cast<unsigned int>(static_cast<double>(armorModel->damageAbs / 100)
        * this->composition->hardness);
    // Modify based on the item's quality.
    if (this->quality == ItemQuality::Disastrous)
    {
        result = static_cast<unsigned int>(result * 0.5);
    }
    else if (this->quality == ItemQuality::Poor)
    {
        result = static_cast<unsigned int>(result * 0.75);
    }
    else if (this->quality == ItemQuality::Fine)
    {
        result = static_cast<unsigned int>(result * 1.25);
    }
    else if (this->quality == ItemQuality::Masterful)
    {
        result = static_cast<unsigned int>(result * 1.50);
    }
    return result;
}
