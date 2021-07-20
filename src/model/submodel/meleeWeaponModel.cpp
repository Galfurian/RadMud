/// @file   meleeWeaponModel.cpp
/// @brief  Iplement the methods for Weapon.
/// @author Enrico Fraccaroli
/// @date   Jul 6 2016
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

#include "model/submodel/meleeWeaponModel.hpp"

#include "utilities/logger.hpp"

MeleeWeaponModel::MeleeWeaponModel() : meleeWeaponType(), minDamage(), maxDamage(), range()
{
	// Nothing to do.
}

MeleeWeaponModel::~MeleeWeaponModel()
{
	// Nothing to do.
}

ModelType MeleeWeaponModel::getType() const
{
	return ModelType::MeleeWeapon;
}

std::string MeleeWeaponModel::getTypeName() const
{
	return "Melee Weapon";
}

bool MeleeWeaponModel::setModel(const std::vector<std::string> &source)
{
	if (source.empty()) {
		MudLog(LogLevel::Error, "Function list is empty (%s).", name);
		return false;
	}
	if (source.size() != 4) {
		MudLog(LogLevel::Error, "Wrong number of parameters for Weapon Model (%s).", name);
		return false;
	}
	meleeWeaponType = static_cast<MeleeWeaponType>(ToNumber<unsigned int>(source[0]));
	minDamage = ToNumber<unsigned int>(source[1]);
	maxDamage = ToNumber<unsigned int>(source[2]);
	range = ToNumber<int>(source[3]);
	return true;
}

void MeleeWeaponModel::getSheet(Table &sheet) const
{
	// Call the function of the father class.
	ItemModel::getSheet(sheet);
	// Add a divider.
	sheet.addDivider();
	// Set the values.
	sheet.addRow({ "Melee Weapon Type", GetMeleeWeaponTypeName(meleeWeaponType) });
	sheet.addRow({ "Minimum Damage", ToString(minDamage) });
	sheet.addRow({ "Maximum Damage", ToString(maxDamage) });
}

std::string GetMeleeWeaponTypeName(MeleeWeaponType type)
{
	if (type == MeleeWeaponType::Unarmed)
		return "Unarmed";
	if (type == MeleeWeaponType::Bladed)
		return "Bladed";
	if (type == MeleeWeaponType::Blunt)
		return "Blunt";
	if (type == MeleeWeaponType::Placed)
		return "Placed";
	return "No Weapon Type";
}
