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

#include "model/itemModel.hpp"

#include "model/submodel/armorModel.hpp"
#include "model/submodel/bookModel.hpp"
#include "model/submodel/containerModel.hpp"
#include "model/submodel/currencyModel.hpp"
#include "model/submodel/foodModel.hpp"
#include "model/submodel/furnitureModel.hpp"
#include "model/submodel/keyModel.hpp"
#include "model/submodel/lightModel.hpp"
#include "model/submodel/liquidContainerModel.hpp"
#include "model/submodel/mechanismModel.hpp"
#include "model/submodel/projectileModel.hpp"
#include "model/submodel/ropeModel.hpp"
#include "model/submodel/seedModel.hpp"
#include "model/submodel/shieldModel.hpp"
#include "model/submodel/shopModel.hpp"
#include "model/submodel/vehicleModel.hpp"
#include "model/submodel/magazineModel.hpp"
#include "model/submodel/meleeWeaponModel.hpp"
#include "model/submodel/resourceModel.hpp"
#include "model/submodel/toolModel.hpp"

#include "item/itemFactory.hpp"
#include "utilities/logger.hpp"
#include "mud.hpp"

ItemModel::ItemModel() :
    vnum(),
    name(),
    article(),
    shortdesc(),
    keys(),
    description(),
    bodyParts(),
    modelFlags(),
    baseWeight(),
    basePrice(),
    condition(),
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

void ItemModel::getSheet(Table & sheet) const
{
    // Add the columns.
    sheet.addColumn("Attribute", align::left);
    sheet.addColumn("Value", align::left);
    // Set the values.
    sheet.addRow({"Vnum", ToString(this->vnum)});
    sheet.addRow({"Name", this->name});
    sheet.addRow({"Article", this->article});
    sheet.addRow({"Short Description", this->shortdesc});
    std::string keyGroup;
    for (auto it : this->keys)
    {
        keyGroup += " " + it;
    }
    sheet.addRow({"Keys", keyGroup});
    sheet.addRow({"Description", this->description});
    sheet.addRow({"Type", this->getTypeName()});
    for (auto bodyPart : bodyParts)
    {
        sheet.addRow({"Body Part", bodyPart->name});
    }
    sheet.addRow({"Flags", GetModelFlagString(this->modelFlags)});
    sheet.addRow({"Condition", ToString(this->condition)});
    sheet.addRow({"Material", this->material.toString()});
    sheet.addRow({"Tile", ToString(this->condition)});
    sheet.addRow({"Condition", ToString(this->tileSet) + ":" +
                               ToString(this->tileId)});
}

Item * ItemModel::createItem(
    std::string maker,
    Material * composition,
    bool isForMobile,
    const ItemQuality & itemQuality,
    const unsigned int & quantity)
{
    if (composition == nullptr)
    {
        Logger::log(LogLevel::Error, "Received nullptr material.");
        return nullptr;
    }
    if (composition->type != this->material)
    {
        Logger::log(LogLevel::Error, "Wrong type of material.");
        return nullptr;
    }
    // Instantiate the new item.
    auto newItem = ItemFactory::newItem(this->getType());
    if (newItem == nullptr)
    {
        Logger::log(LogLevel::Error, "Cannot create the new item.");
        // Return pointer to nothing.
        return nullptr;
    }
    // First set: Vnum, Model, Maker, Composition, Quality.
    // If the item is for a mobile, set the vnum to -1.
    if (isForMobile)
    {
        // Set a fake vnum.
        newItem->vnum = -1;
        // Set the item a temporary.
        SetFlag(newItem->flags, ItemFlag::Temporary);
    }
    else
    {
        newItem->vnum = Mud::instance().getMaxVnumItem() + 1;
    }
    newItem->model = this->shared_from_this();
    newItem->quantity = quantity;
    newItem->maker = maker;
    newItem->composition = composition;
    newItem->quality = itemQuality;
    // Then set the rest.
    {
        // Evaluate the base value.
        auto valBase = this->basePrice;
        // Evaluate the modifier due to item's quality.
        auto valQuality = static_cast<unsigned int>(valBase *
                                                    itemQuality.getModifier());
        // Evaluate the modifier due to item's material.
        auto valMaterial = static_cast<unsigned int>(valBase *
                                                     composition->getWorthModifier());
        // Evaluate the result.
        newItem->price = ((valBase + valQuality + valMaterial) / 3);
    }
    {
        // Evaluate the base value.
        auto valBase = this->baseWeight;
        // Evaluate the modifier due to item's quality.
        auto valQuality = valBase * (1.0 / itemQuality.getModifier());
        // Evaluate the modifier due to item's material.
        auto valMaterial = valBase * composition->getLightnessModifier();
        // Evaluate the result.
        newItem->weight = ((valBase + valQuality + valMaterial) / 3.0);
    }
    {
        // Evaluate the base value.
        auto valBase = this->condition;
        // Evaluate the modifier due to item's quality.
        auto valQuality = valBase * itemQuality.getModifier();
        // Evaluate the modifier due to item's material.
        auto valMaterial = valBase * composition->getHardnessModifier();
        // Evaluate the result.
        newItem->maxCondition = ((valBase + valQuality + valMaterial) / 3);
        newItem->condition = newItem->maxCondition;
    }
    // If the item is for a mobile, do not add the item to the MUD nor to the
    //  DB and do not check its correctness.
    if (isForMobile)
    {
        return newItem;
    }

    if (!newItem->check())
    {
        Logger::log(LogLevel::Error, "Cannot create the new item.");
        // Delete the item.
        delete (newItem);
        // Return pointer to nothing.
        return nullptr;
    }
    // Insert into the item_list the new item.
    if (!Mud::instance().addItem(newItem))
    {
        Logger::log(LogLevel::Error, "Cannot save the new item on MUD.");
        // Delete the item.
        delete (newItem);
        // Return pointer to nothing.
        return nullptr;
    }
    if (!newItem->updateOnDB())
    {
        Logger::log(LogLevel::Error, "Cannot save the new item on DB.");
        // Delete the item.
        delete (newItem);
        // Return pointer to nothing.
        return nullptr;
    }
    return newItem;
}

bool ItemModel::check()
{
    if (vnum <= 0) return false;
    if (name.empty()) return false;
    if (article.empty()) return false;
    if (shortdesc.empty()) return false;
    if (keys.empty()) return false;
    if (description.empty()) return false;
    if (condition <= 0) return false;
    if (this->material == MaterialType::None) return false;
    if (tileSet < 0) return false;
    if (tileId < 0) return false;
    return true;
}

bool ItemModel::replaceSymbols(
    std::string & source,
    Material * itemMaterial,
    const ItemQuality & itemQuality) const
{
    bool modified = false;
    if (itemMaterial)
    {
        modified = true;
        FindAndReplace(&source, "&m", ToLower(itemMaterial->name));
        FindAndReplace(&source, "&M", ToLower(
            itemMaterial->article + ' ' + itemMaterial->name));
    }
    else
    {
        FindAndReplace(&source, "&m", "");
        FindAndReplace(&source, "&M", "");
    }
    if (itemQuality != ItemQuality::Normal)
    {
        modified = true;
        FindAndReplace(&source, "&q", " " + ToLower(itemQuality.toString()));
    }
    else
    {
        FindAndReplace(&source, "&q", "");
    }
    return modified;
}

std::string ItemModel::getName(Material * itemMaterial,
                               const ItemQuality & itemQuality) const
{
    // Make a copy of the short description.
    std::string output = shortdesc;
    // Try to replace the symbols inside the short description.
    if (!this->replaceSymbols(output, itemMaterial, itemQuality))
    {
        output = article + " " + name;
    }
    return output;
}

std::string ItemModel::getDescription(Material * itemMaterial,
                                      const ItemQuality & itemQuality)
{
    // Make a copy of the description.
    std::string output = description;
    replaceSymbols(output, itemMaterial, itemQuality);
    return output;
}

std::vector<std::shared_ptr<BodyPart>> ItemModel::getBodyParts(Race * race)
{
    std::vector<std::shared_ptr<BodyPart>> filteredBodyParts;
    for (auto raceBodyPart: race->bodyParts)
    {
        for (auto bodyPart : bodyParts)
        {
            if (raceBodyPart->vnum == bodyPart->vnum)
            {
                filteredBodyParts.emplace_back(bodyPart);
            }
        }
    }
    return filteredBodyParts;
}

std::string ItemModel::getTile(int offset)
{
    if (Formatter::getFormat() == Formatter::CLIENT)
    {
        return ToString(tileSet) + ":" + ToString(tileId + offset);
    }
    // TODO: Too easy this way.
    if (this->getType() == ModelType::Armor)
    {
        return "a";
    }
    if ((this->getType() == ModelType::MeleeWeapon) ||
        (this->getType() == ModelType::RangedWeapon))
    {
        return "w";
    }
    return "i";

}

std::shared_ptr<ArmorModel> ItemModel::toArmor()
{
    return std::static_pointer_cast<ArmorModel>(this->shared_from_this());
}

std::shared_ptr<BookModel> ItemModel::toBook()
{
    return std::static_pointer_cast<BookModel>(this->shared_from_this());
}

std::shared_ptr<ContainerModel> ItemModel::toContainer()
{
    return std::static_pointer_cast<ContainerModel>(this->shared_from_this());
}

std::shared_ptr<CorpseModel> ItemModel::toCorpse()
{
    return std::static_pointer_cast<CorpseModel>(this->shared_from_this());
}

std::shared_ptr<CurrencyModel> ItemModel::toCurrency()
{
    return std::static_pointer_cast<CurrencyModel>(this->shared_from_this());
}

std::shared_ptr<FoodModel> ItemModel::toFood()
{
    return std::static_pointer_cast<FoodModel>(this->shared_from_this());
}

std::shared_ptr<FurnitureModel> ItemModel::toFurniture()
{
    return std::static_pointer_cast<FurnitureModel>(this->shared_from_this());
}

std::shared_ptr<KeyModel> ItemModel::toKey()
{
    return std::static_pointer_cast<KeyModel>(this->shared_from_this());
}

std::shared_ptr<LightModel> ItemModel::toLight()
{
    return std::static_pointer_cast<LightModel>(this->shared_from_this());
}

std::shared_ptr<LiquidContainerModel> ItemModel::toLiquidContainer()
{
    return std::static_pointer_cast<LiquidContainerModel>(
        this->shared_from_this());
}

std::shared_ptr<MechanismModel> ItemModel::toMechanism()
{
    return std::static_pointer_cast<MechanismModel>(this->shared_from_this());
}

std::shared_ptr<NodeModel> ItemModel::toNode()
{
    return std::static_pointer_cast<NodeModel>(this->shared_from_this());
}

std::shared_ptr<ProjectileModel> ItemModel::toProjectile()
{
    return std::static_pointer_cast<ProjectileModel>(this->shared_from_this());
}

std::shared_ptr<ResourceModel> ItemModel::toResource()
{
    return std::static_pointer_cast<ResourceModel>(this->shared_from_this());
}

std::shared_ptr<RopeModel> ItemModel::toRope()
{
    return std::static_pointer_cast<RopeModel>(this->shared_from_this());
}

std::shared_ptr<SeedModel> ItemModel::toSeed()
{
    return std::static_pointer_cast<SeedModel>(this->shared_from_this());
}

std::shared_ptr<ShieldModel> ItemModel::toShield()
{
    return std::static_pointer_cast<ShieldModel>(this->shared_from_this());
}

std::shared_ptr<ShopModel> ItemModel::toShop()
{
    return std::static_pointer_cast<ShopModel>(this->shared_from_this());
}

std::shared_ptr<ToolModel> ItemModel::toTool()
{
    return std::static_pointer_cast<ToolModel>(this->shared_from_this());
}

std::shared_ptr<VehicleModel> ItemModel::toVehicle()
{
    return std::static_pointer_cast<VehicleModel>(this->shared_from_this());
}

std::shared_ptr<MeleeWeaponModel> ItemModel::toMeleeWeapon()
{
    return std::static_pointer_cast<MeleeWeaponModel>(this->shared_from_this());
}

std::shared_ptr<RangedWeaponModel> ItemModel::toRangedWeapon()
{
    return std::static_pointer_cast<RangedWeaponModel>(
        this->shared_from_this());
}

std::shared_ptr<MagazineModel> ItemModel::toMagazine()
{
    return std::static_pointer_cast<MagazineModel>(this->shared_from_this());
}

std::string GetModelFlagString(unsigned int flags)
{
    std::string flagList;
    if (HasFlag(flags, ModelFlag::Static)) flagList += "|Static";
    if (HasFlag(flags, ModelFlag::Invisible)) flagList += "|Invisible";
    if (HasFlag(flags, ModelFlag::Unbreakable)) flagList += "|Unbreakable";
    if (HasFlag(flags, ModelFlag::NoSaleable)) flagList += "|NoSaleable";
    if (HasFlag(flags, ModelFlag::TwoHand)) flagList += "|TwoHand";
    if (HasFlag(flags, ModelFlag::CanClose)) flagList += "|CanClose";
    if (HasFlag(flags, ModelFlag::CanSeeThrough)) flagList += "|CanSeeThrough";
    if (HasFlag(flags, ModelFlag::CanBeStacked)) flagList += "|CanBeStacked";
    if (!flagList.empty()) flagList += "|";
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
