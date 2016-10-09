/// @file   itemModel.cpp
/// @brief  Implements item model methods.
/// @author Enrico Fraccaroli
/// @date   Mar 31 2015
/// @copyright
/// Copyright (c) 2015, 2016 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
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

#include "itemModel.hpp"

#include "../mud.hpp"

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
#include "rangedWeaponModel.hpp"

ItemModel::ItemModel() :
    vnum(),
    name(),
    article(),
    shortdesc(),
    keys(),
    description(),
    slot(),
    modelFlags(),
    baseWeight(),
    basePrice(),
    condition(),
    decay(),
    material(),
    tileSet(),
    tileId()
{
    // Nothing to do.
}

ItemModel::~ItemModel()
{
    Logger::log(LogLevel::Debug, "Deleted model\t\t[%s]\t\t(%s)", ToString(this->vnum), this->name);
}

ModelType ItemModel::getType() const
{
    return ModelType::NoType;
}

void ItemModel::getSheet(Table & sheet) const
{
    // Add the columns.
    sheet.addColumn("Attribute", StringAlign::Left);
    sheet.addColumn("Value", StringAlign::Left);
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
    sheet.addRow({"Slot", GetEquipmentSlotName(this->slot)});
    sheet.addRow({"Flags", GetModelFlagString(this->modelFlags)});
    sheet.addRow({"Condition", ToString(this->condition)});
    sheet.addRow({"Decay", ToString(this->decay)});
    sheet.addRow({"Material", GetMaterialTypeName(this->material)});
    sheet.addRow({"Tile", ToString(this->condition)});
    sheet.addRow({"Condition", ToString(this->tileSet) + ":" + ToString(this->tileId)});
}

Item * ItemModel::createItem(
    std::string maker,
    Material * composition,
    bool isForMobile,
    const ItemQuality & itemQuality,
    const unsigned int & quantity)
{
    if (composition->type != this->material)
    {
        Logger::log(LogLevel::Error, "Wrong type of material.");
        // Return pointer to nothing.
        return nullptr;
    }

    // Instantiate the new item.
    Item * newItem = GenerateItem(this->getType());
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
        newItem->vnum = -1;
    }
    else
    {
        newItem->vnum = Mud::instance().getMaxVnumItem() + 1;
    }
    newItem->model = this;
    newItem->quantity = quantity;
    newItem->maker = maker;
    newItem->composition = composition;
    newItem->quality = itemQuality;

    // Then set the rest.
    {
        // Evaluate the base value.
        auto valBase = this->basePrice;
        // Evaluate the modifier due to item's quality.
        auto valQuality = static_cast<unsigned int>(valBase * itemQuality.getModifier());
        // Evaluate the modifier due to item's material.
        auto valMaterial = static_cast<unsigned int>(valBase * composition->getWorthModifier());
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
        auto valQuality = static_cast<unsigned int>(valBase * itemQuality.getModifier());
        // Evaluate the modifier due to item's material.
        auto valMaterial = static_cast<unsigned int>(valBase * composition->getHardnessModifier());
        // Evaluate the result.
        newItem->maxCondition = ((valBase + valQuality + valMaterial) / 3);
        newItem->condition = newItem->maxCondition;
    }
    newItem->currentSlot = slot;

    // If the item is for a mobile, do not add the item to the MUD nor to the DB and do not check its correctness.
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
    assert(vnum > 0);
    assert(!name.empty());
    assert(!article.empty());
    assert(!shortdesc.empty());
    assert(!keys.empty());
    assert(!description.empty());
    if ((this->getType() == ModelType::Armor) ||
        (this->getType() == ModelType::MeleeWeapon) ||
        (this->getType() == ModelType::RangedWeapon))
    {
        assert(slot != EquipmentSlot::None);
    }
    assert(condition > 0);
    assert(decay > 0);
    assert(this->material != MaterialType::NoType);
    assert(tileSet >= 0);
    assert(tileId >= 0);
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
        FindAndReplace(&source, "&M", ToLower(itemMaterial->article + ' ' + itemMaterial->name));
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

std::string ItemModel::getName(Material * itemMaterial, const ItemQuality & itemQuality) const
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

std::string ItemModel::getDescription(Material * itemMaterial, const ItemQuality & itemQuality)
{
    // Make a copy of the description.
    std::string output = description;
    replaceSymbols(output, itemMaterial, itemQuality);
    return output;
}

bool ItemModel::mustBeWielded()
{
    return ((slot == EquipmentSlot::RightHand) || (slot == EquipmentSlot::LeftHand));
}

void ItemModel::luaRegister(lua_State * L)
{
    luabridge::getGlobalNamespace(L) //
        .beginClass<ItemModel>("ItemModel") //
        .addData("vnum", &ItemModel::vnum) //
        .addData("condition", &ItemModel::condition) //
        .addData("decay", &ItemModel::decay) //
        .addFunction("getType", &ItemModel::getType) //
        .endClass();
}

std::string ItemModel::getTile(int offset)
{
    if (Formatter::getFormat() == Formatter::TELNET)
    {
        return ToString(tileSet) + ":" + ToString(tileId + offset);
    }
    else
    {
        // TODO: Too easy this way.
        if (this->getType() == ModelType::Armor)
        {
            return "a";
        }
        else if ((this->getType() == ModelType::MeleeWeapon) || (this->getType() == ModelType::RangedWeapon))
        {
            return "w";
        }
        else
        {
            return "i";
        }
    }
}

ArmorModel * ItemModel::toArmor()
{
    return static_cast<ArmorModel *>(this);
}

BookModel * ItemModel::toBook()
{
    return static_cast<BookModel *>(this);
}

ContainerModel * ItemModel::toContainer()
{
    return static_cast<ContainerModel *>(this);
}

CorpseModel * ItemModel::toCorpse()
{
    return static_cast<CorpseModel *>(this);
}

CurrencyModel * ItemModel::toCurrency()
{
    return static_cast<CurrencyModel *>(this);
}

FoodModel * ItemModel::toFood()
{
    return static_cast<FoodModel *>(this);
}

FurnitureModel * ItemModel::toFurniture()
{
    return static_cast<FurnitureModel *>(this);
}

KeyModel * ItemModel::toKey()
{
    return static_cast<KeyModel *>(this);
}

LightModel * ItemModel::toLight()
{
    return static_cast<LightModel *>(this);
}

LiquidContainerModel * ItemModel::toLiquidContainer()
{
    return static_cast<LiquidContainerModel *>(this);
}

MechanismModel * ItemModel::toMechanism()
{
    return static_cast<MechanismModel *>(this);
}

NodeModel * ItemModel::toNode()
{
    return static_cast<NodeModel *>(this);
}

ProjectileModel * ItemModel::toProjectile()
{
    return static_cast<ProjectileModel *>(this);
}

ResourceModel * ItemModel::toResource()
{
    return static_cast<ResourceModel *>(this);
}

RopeModel * ItemModel::toRope()
{
    return static_cast<RopeModel *>(this);
}

SeedModel * ItemModel::toSeed()
{
    return static_cast<SeedModel *>(this);
}

ShieldModel * ItemModel::toShield()
{
    return static_cast<ShieldModel *>(this);
}

ShopModel * ItemModel::toShop()
{
    return static_cast<ShopModel *>(this);
}

ToolModel * ItemModel::toTool()
{
    return static_cast<ToolModel *>(this);
}

VehicleModel * ItemModel::toVehicle()
{
    return static_cast<VehicleModel *>(this);
}

MeleeWeaponModel * ItemModel::toMeleeWeapon()
{
    return static_cast<MeleeWeaponModel *>(this);
}

RangedWeaponModel * ItemModel::toRangedWeapon()
{
    return static_cast<RangedWeaponModel *>(this);
}

MagazineModel * ItemModel::toMagazine()
{
    return static_cast<MagazineModel *>(this);
}

ItemModel * GenerateModel(const ModelType & type)
{
    switch (type)
    {
        case ModelType::Corpse:
            return new CorpseModel();
        case ModelType::Armor:
            return new ArmorModel();
        case ModelType::Book:
            return new BookModel();
        case ModelType::Container:
            return new ContainerModel();
        case ModelType::Currency:
            return new CurrencyModel();
        case ModelType::Food:
            return new FoodModel();
        case ModelType::Furniture:
            return new FurnitureModel();
        case ModelType::Key:
            return new KeyModel();
        case ModelType::Light:
            return new LightModel();
        case ModelType::LiquidContainer:
            return new LiquidContainerModel();
        case ModelType::Mechanism:
            return new MechanismModel();
        case ModelType::Node:
            return new NodeModel();
        case ModelType::Projectile:
            return new ProjectileModel();
        case ModelType::Resource:
            return new ResourceModel();
        case ModelType::Rope:
            return new RopeModel();
        case ModelType::Seed:
            return new SeedModel();
        case ModelType::Shield:
            return new ShieldModel();
        case ModelType::Shop:
            return new ShopModel();
        case ModelType::Tool:
            return new ToolModel();
        case ModelType::Vehicle:
            return new VehicleModel();
        case ModelType::MeleeWeapon:
            return new MeleeWeaponModel();
        case ModelType::RangedWeapon:
            return new RangedWeaponModel();
        case ModelType::Magazine:
            return new MagazineModel();
        case ModelType::NoType:
            return nullptr;
    }
    return nullptr;
}

std::string GetModelFlagString(unsigned int flags)
{
    std::string flagList;
    if (HasFlag(flags, ModelFlag::Static))
    { flagList += "|Static"; }
    if (HasFlag(flags, ModelFlag::Invisible))
    { flagList += "|Invisible"; }
    if (HasFlag(flags, ModelFlag::Unbreakable))
    { flagList += "|Unbreakable"; }
    if (HasFlag(flags, ModelFlag::NoSaleable))
    { flagList += "|NoSaleable"; }
    if (HasFlag(flags, ModelFlag::TwoHand))
    { flagList += "|TwoHand"; }
    if (HasFlag(flags, ModelFlag::CanClose))
    { flagList += "|CanClose"; }
    if (HasFlag(flags, ModelFlag::CanSeeThrough))
    { flagList += "|CanSeeThrough"; }
    if (HasFlag(flags, ModelFlag::CanBeStacked))
    { flagList += "|CanBeStacked"; }
    if (!flagList.empty())
    {
        flagList += "|";
    }
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
