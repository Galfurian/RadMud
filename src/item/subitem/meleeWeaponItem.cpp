/// @file   meleeWeaponItem.cpp
/// @author Enrico Fraccaroli
/// @date   Aug 04 2016
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

#include "meleeWeaponItem.hpp"
#include "meleeWeaponModel.hpp"

MeleeWeaponItem::MeleeWeaponItem()
{
	// Nothing to do.
}

MeleeWeaponItem::~MeleeWeaponItem()
{
	// Nothing to do.
}

void MeleeWeaponItem::getSheet(Table &sheet) const
{
	// Call the function of the father class.
	Item::getSheet(sheet);
	// Add a divider.
	sheet.addDivider();
	// Set the values.
	sheet.addRow({ "Min Damage", ToString(this->getMinDamage()) });
	sheet.addRow({ "Max Damage", ToString(this->getMaxDamage()) });
}

unsigned int MeleeWeaponItem::rollDamage() const
{
	return TRand<unsigned int>(this->getMinDamage(), this->getMaxDamage());
}

unsigned int MeleeWeaponItem::getMinDamage() const
{
	// Add the base value.
	auto base = this->model->to<MeleeWeaponModel>()->minDamage;
	// Evaluate the modifier due to item's quality.
	auto qual = static_cast<unsigned int>(base * quality.getModifier());
	// Evaluate the modifier due to item's condition.
	auto cond = static_cast<unsigned int>(base * this->getConditionModifier());
	return ((base + qual + cond) / 3);
}

unsigned int MeleeWeaponItem::getMaxDamage() const
{
	// Add the base value.
	auto base = this->model->to<MeleeWeaponModel>()->maxDamage;
	// Evaluate the modifier due to item's quality.
	auto qual = static_cast<unsigned int>(base * quality.getModifier());
	// Evaluate the modifier due to item's condition.
	auto cond = static_cast<unsigned int>(base * this->getConditionModifier());
	return (base + qual + cond) / 3;
}

int MeleeWeaponItem::getRange() const
{
	// Add the base value.
	auto base = this->model->to<MeleeWeaponModel>()->range;
	// Evaluate the modifier due to item's quality.
	auto qual = static_cast<int>(base * quality.getModifier());
	// Evaluate the modifier due to item's condition.
	auto cond = static_cast<int>(base * this->getConditionModifier());
	return (base + qual + cond) / 3;
}
