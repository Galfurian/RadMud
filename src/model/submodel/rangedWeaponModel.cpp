/// @file   rangedWeaponModel.cpp
/// @brief  Iplement the methods for Ranged Weapons.
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

#include "model/submodel/rangedWeaponModel.hpp"

#include "utilities/logger.hpp"

RangedWeaponModel::RangedWeaponModel() : rangedWeaponType(), minDamage(), maxDamage(), range()
{
	// Nothing to do.
}

RangedWeaponModel::~RangedWeaponModel()
{
	// Nothing to do.
}

ModelType RangedWeaponModel::getType() const
{
	return ModelType::RangedWeapon;
}

std::string RangedWeaponModel::getTypeName() const
{
	return "Ranged Weapon";
}

bool RangedWeaponModel::setModel(const std::vector<std::string> &source)
{
	if (source.empty()) {
		MudLog(LogLevel::Error, "Function list is empty (%s).", name);
		return false;
	}
	if (source.size() != 4) {
		MudLog(LogLevel::Error, "Wrong number of parameters for Ranged Weapon Model (%s).", name);
		return false;
	}
	this->rangedWeaponType = static_cast<RangedWeaponType>(ToNumber<unsigned int>(source[0]));
	this->minDamage = ToNumber<unsigned int>(source[1]);
	this->maxDamage = ToNumber<unsigned int>(source[2]);
	this->range = ToNumber<int>(source[3]);
	return true;
}

void RangedWeaponModel::getSheet(Table &sheet) const
{
	// Call the function of the father class.
	ItemModel::getSheet(sheet);
	// Add a divider.
	sheet.addDivider();
	// Set the values.
	sheet.addRow({ "Ranged Weapon Type", GetRangedWeaponTypeName(this->rangedWeaponType) });
	sheet.addRow({ "Minimum Damage", ToString(this->minDamage) });
	sheet.addRow({ "Maximum Damage", ToString(this->maxDamage) });
	sheet.addRow({ "Range", ToString(this->range) });
}

std::string GetRangedWeaponTypeName(RangedWeaponType type)
{
	if (type == RangedWeaponType::Pistol)
		return "Pistol";
	if (type == RangedWeaponType::Rifle)
		return "Rifle";
	if (type == RangedWeaponType::Shotgun)
		return "Shotgun";
	if (type == RangedWeaponType::HeavyWeapon)
		return "Heavy Weapon";
	if (type == RangedWeaponType::EnergyPistol)
		return "Energy Pistol";
	if (type == RangedWeaponType::EnergyRifle)
		return "Energy Rifle";
	if (type == RangedWeaponType::EnergyHeavyWeapon)
		return "Energy Heavy Weapon";
	if (type == RangedWeaponType::Granade)
		return "Granade";
	if (type == RangedWeaponType::Thrown)
		return "Thrown";
	return "No Weapon Type";
}
