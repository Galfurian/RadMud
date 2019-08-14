/// @file   itemModel.cpp
/// @brief  Implements item model methods.
/// @author Enrico Fraccaroli
/// @date   Mar 31 2015
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

#include "itemModel.hpp"

#include "armorModel.hpp"
#include "bookModel.hpp"
#include "containerModel.hpp"
#include "currencyModel.hpp"
#include "foodModel.hpp"
#include "furnitureModel.hpp"
#include "keyModel.hpp"
#include "lightModel.hpp"
#include "liquidContainerModel.hpp"
#include "mechanismModel.hpp"
#include "projectileModel.hpp"
#include "ropeModel.hpp"
#include "seedModel.hpp"
#include "shieldModel.hpp"
#include "shopModel.hpp"
#include "vehicleModel.hpp"
#include "magazineModel.hpp"
#include "meleeWeaponModel.hpp"
#include "resourceModel.hpp"
#include "toolModel.hpp"
#include "formatter.hpp"

#include "itemFactory.hpp"
#include "logger.hpp"
#include "mud.hpp"

ItemModel::ItemModel() :
	vnum(),
	name(),
	article(),
	shortdesc(),
	keys(),
	description(),
	modelFlags(),
	baseWeight(),
	basePrice(),
	material(),
	tileSet(),
	tileId()
{
	// Nothing to do.
}

ItemModel::~ItemModel()
{
	//    Logger::log(LogLevel::Debug,
	//                "Deleted model\t\t[%s]\t\t(%s)",
	//                ToString(this->vnum),
	//                this->name);
}

ModelType ItemModel::getType() const
{
	return ModelType::None;
}

void ItemModel::getSheet(Table &sheet) const
{
	// Add the columns.
	sheet.addColumn("Attribute", align::left);
	sheet.addColumn("Value", align::left);
	// Set the values.
	sheet.addRow({ "Vnum", ToString(this->vnum) });
	sheet.addRow({ "Name", this->name });
	sheet.addRow({ "Article", this->article });
	sheet.addRow({ "Short Description", this->shortdesc });
	std::string keyGroup;
	for (auto it : this->keys) {
		keyGroup += " " + it;
	}
	sheet.addRow({ "Keys", keyGroup });
	sheet.addRow({ "Description", this->description });
	sheet.addRow({ "Type", this->getTypeName() });
	sheet.addRow({ "Flags", GetModelFlagString(this->modelFlags) });
	sheet.addRow({ "Material", this->material.toString() });
	sheet.addRow({ "TileSet", ToString(tileSet) });
	sheet.addRow({ "TileId", ToString(tileId) });
	sheet.addRow({ "Condition",
				   ToString(this->tileSet) + ":" + ToString(this->tileId) });
}

Item *ItemModel::createItem(std::string maker, Material *composition,
							bool isForMobile, const ItemQuality &itemQuality,
							const unsigned int &quantity)
{
	if (composition == nullptr) {
		Logger::log(LogLevel::Error, "Received nullptr material.");
		return nullptr;
	}
	if (composition->type != this->material) {
		Logger::log(LogLevel::Error, "Wrong type of material.");
		return nullptr;
	}
	// Instantiate the new item.
	auto newItem = ItemFactory::newItem(this->getType());
	if (newItem == nullptr) {
		Logger::log(LogLevel::Error, "Cannot create the new item.");
		// Return pointer to nothing.
		return nullptr;
	}
	// First set: Vnum, Model, Maker, Composition, Quality.
	// If the item is for a mobile, set the vnum to -1.
	if (isForMobile) {
		// Set a fake vnum.
		newItem->vnum = 0;
		// Set the item a temporary.
		SetFlag(newItem->flags, ItemFlag::Temporary);
	} else {
		newItem->vnum = Mud::instance().getMaxVnumItem() + 1;
	}
	newItem->model = this->shared_from_this();
	newItem->quantity = quantity;
	newItem->maker = maker;
	newItem->composition = composition;
	newItem->quality = itemQuality;
	newItem->condition = newItem->getMaxCondition();
	// If the item is for a mobile, do not add the item to the MUD nor to the
	//  DB and do not check its correctness.
	if (isForMobile) {
		return newItem;
	}

	if (!newItem->check()) {
		Logger::log(LogLevel::Error, "Cannot create the new item.");
		// Delete the item.
		delete (newItem);
		// Return pointer to nothing.
		return nullptr;
	}
	//
	if (!newItem->updateOnDB()) {
		Logger::log(LogLevel::Error, "Cannot save the new item on DB.");
		// Delete the item.
		delete (newItem);
		// Return pointer to nothing.
		return nullptr;
	}
	// Insert into the item_list the new item.
	if (!Mud::instance().addItem(newItem)) {
		Logger::log(LogLevel::Error, "Cannot save the new item on MUD.");
		// Delete the item.
		delete (newItem);
		// Return pointer to nothing.
		return nullptr;
	}
	return newItem;
}

bool ItemModel::check()
{
	if (vnum <= 0)
		return false;
	if (name.empty())
		return false;
	if (article.empty())
		return false;
	if (shortdesc.empty())
		return false;
	if (keys.empty())
		return false;
	if (description.empty())
		return false;
	if (material == MaterialType::None)
		return false;
	if (tileSet < 0)
		return false;
	if (tileId < 0)
		return false;
	return true;
}

bool ItemModel::replaceSymbols(std::string &source, Material *itemMaterial,
							   const ItemQuality &itemQuality) const
{
	bool modified = false;
	if (itemMaterial) {
		modified = true;
		FindAndReplace(&source, "&m", ToLower(itemMaterial->name));
		FindAndReplace(
			&source, "&M",
			ToLower(itemMaterial->article + ' ' + itemMaterial->name));
	} else {
		FindAndReplace(&source, "&m", "");
		FindAndReplace(&source, "&M", "");
	}
	if (itemQuality != ItemQuality::Normal) {
		modified = true;
		FindAndReplace(&source, "&q", " " + ToLower(itemQuality.toString()));
	} else {
		FindAndReplace(&source, "&q", "");
	}
	return modified;
}

std::string ItemModel::getName(Material *itemMaterial,
							   const ItemQuality &itemQuality) const
{
	// Make a copy of the short description.
	std::string output = shortdesc;
	// Try to replace the symbols inside the short description.
	if (!this->replaceSymbols(output, itemMaterial, itemQuality)) {
		output = article + " " + name;
	}
	return output;
}

std::string ItemModel::getDescription(Material *itemMaterial,
									  const ItemQuality &itemQuality)
{
	// Make a copy of the description.
	std::string output = description;
	replaceSymbols(output, itemMaterial, itemQuality);
	return output;
}

std::string ItemModel::getTile(int offset)
{
	if (Formatter::getFormat() == Formatter::CLIENT) {
		return ToString(tileSet) + ":" + ToString(tileId + offset);
	}
	// TODO: Too easy this way.
	if (this->getType() == ModelType::Armor) {
		return "a";
	}
	if ((this->getType() == ModelType::MeleeWeapon) ||
		(this->getType() == ModelType::RangedWeapon)) {
		return "w";
	}
	return "i";
}

std::string GetModelFlagString(unsigned int flags)
{
	std::string flagList;
	if (HasFlag(flags, ModelFlag::Static))
		flagList += "|Static";
	if (HasFlag(flags, ModelFlag::Invisible))
		flagList += "|Invisible";
	if (HasFlag(flags, ModelFlag::Unbreakable))
		flagList += "|Unbreakable";
	if (HasFlag(flags, ModelFlag::NoSaleable))
		flagList += "|NoSaleable";
	if (HasFlag(flags, ModelFlag::TwoHand))
		flagList += "|TwoHand";
	if (HasFlag(flags, ModelFlag::CanClose))
		flagList += "|CanClose";
	if (HasFlag(flags, ModelFlag::CanSeeThrough))
		flagList += "|CanSeeThrough";
	if (HasFlag(flags, ModelFlag::CanBeStacked))
		flagList += "|CanBeStacked";
	if (!flagList.empty())
		flagList += "|";
	return flagList;
}

/*
 std::string GetModelTypeName(ModelType type)
 {
 if (type == ModelType::Corpse) return "Corpse";
 if (type == ModelType::Weapon) return "Weapon";
 if (type == ModelType::Armor) return "Armor";
 if (type == ModelType::Shield) return "Shield";
 if (type == ModelType::Projectile) return "Projectile";
 if (type == ModelType::Container) return "Container";
 if (type == ModelType::LiquidContainer) return "LiquidContainer";
 if (type == ModelType::Tool) return "Tool";
 if (type == ModelType::Node) return "Node";
 if (type == ModelType::Resource) return "Resource";
 if (type == ModelType::Seed) return "Seed";
 if (type == ModelType::Key) return "Key";
 if (type == ModelType::Furniture) return "Furniture";
 if (type == ModelType::Food) return "Food";
 if (type == ModelType::Light) return "Light";
 if (type == ModelType::Vehicle) return "Vehicle";
 if (type == ModelType::Book) return "Book";
 if (type == ModelType::Rope) return "Rope";
 if (type == ModelType::Mechanism) return "Mechanism";
 if (type == ModelType::Currency) return "Currency";
 return "No Model Type";
 }
 */
