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

// Basic Include.
#include "../mud.hpp"
#include "../logger.hpp"
#include "../material.hpp"
#include "../constants.hpp"
#include "../luabridge/LuaBridge.h"
#include "itemModel.hpp"

#include "armorModel.hpp"
#include "bookModel.hpp"
#include "containerModel.hpp"
#include "currencyModel.hpp"
#include "foodModel.hpp"
#include "furnitureModel.hpp"
#include "itemModel.hpp"
#include "keyModel.hpp"
#include "lightModel.hpp"
#include "liquidContainerModel.hpp"
#include "mechanismModel.hpp"
#include "nodeModel.hpp"
#include "projectileModel.hpp"
#include "resourceModel.hpp"
#include "ropeModel.hpp"
#include "seedModel.hpp"
#include "shieldModel.hpp"
#include "toolModel.hpp"
#include "vehicleModel.hpp"
#include "weaponModel.hpp"

using namespace std;

ItemModel::ItemModel() :
        vnum(),
        name(),
        article(),
        shortdesc(),
        keys(),
        description(),
        modelType(),
        slot(),
        modelFlags(),
        weight(),
        price(),
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

///////////////////////////////////////////////////////////
// CHECKER ////////////////////////////////////////////////
///////////////////////////////////////////////////////////
bool ItemModel::check()
{
    assert(vnum > 0);
    assert(!name.empty());
    assert(!article.empty());
    assert(!shortdesc.empty());
    assert(!keys.empty());
    assert(!description.empty());
    assert(modelType != ModelType::NoType);
    if ((modelType == ModelType::Armor) || (modelType == ModelType::Weapon))
    {
        assert(slot != EquipmentSlot::None);
    }
    assert(weight > 0);
    assert(price >= 0);
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
    ItemQuality itemQuality)
{
    bool modified = false;
    if (itemMaterial)
    {
        modified = true;
        FindAndReplace(source, "&m", ToLower(itemMaterial->name));
        FindAndReplace(source, "&M", ToLower(itemMaterial->article + ' ' + itemMaterial->name));
    }
    else
    {
        FindAndReplace(source, "&m", "");
        FindAndReplace(source, "&M", "");
    }
    if (itemQuality != ItemQuality::Normal)
    {
        modified = true;
        FindAndReplace(source, "&q", " " + ToLower(GetItemQualityName(itemQuality)));
    }
    else
    {
        FindAndReplace(source, "&q", "");
    }
    return modified;
}

std::string ItemModel::getName(Material * itemMaterial, ItemQuality itemQuality)
{
    // Make a copy of the short description.
    std::string output = shortdesc;
    // Try to replace the symbols inside the short description.
    if (!replaceSymbols(output, itemMaterial, itemQuality))
    {
        output = article + " " + name;
    }
    return output;
}

std::string ItemModel::getDescription(Material * itemMaterial, ItemQuality itemQuality)
{
    // Make a copy of the description.
    std::string output = description;
    replaceSymbols(output, itemMaterial, itemQuality);
    return output;
}

///////////////////////////////////////////////////////////
// ITEM CREATION //////////////////////////////////////////
///////////////////////////////////////////////////////////

Item * ItemModel::createItem(std::string maker, Material * composition, ItemQuality itemQuality)
{
    // Instantiate the new item.
    Item * newItem = new Item();
    // If the new item is a corpse, don't use the normal item's vnum.
    if (this->modelType == ModelType::Corpse)
    {
        newItem->vnum = Mud::instance().getMinVnumCorpse() - 1;
    }
    else
    {
        newItem->vnum = Mud::instance().getMaxVnumItem() + 1;
    }
    newItem->model = this;
    newItem->maker = maker;
    newItem->condition = condition;
    newItem->composition = composition;
    newItem->quality = itemQuality;
    newItem->flags = 0;
    newItem->room = nullptr;
    newItem->owner = nullptr;
    newItem->container = nullptr;
    newItem->currentSlot = slot;
    newItem->content = std::vector<Item *>();
    newItem->contentLiq = LiquidContent();

    // If the newly created item is a corpse, return it and don't save it on DB.
    if (this->modelType == ModelType::Corpse)
    {
        Mud::instance().addCorpse(newItem);
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

    // Prepare the vector used to insert into the database.
    std::vector<std::string> arguments;
    arguments.push_back(ToString(newItem->vnum));
    arguments.push_back(ToString(vnum));
    arguments.push_back(maker);
    arguments.push_back(ToString(condition));
    arguments.push_back(ToString(composition->vnum));
    arguments.push_back(EnumToString(itemQuality));
    arguments.push_back(ToString(modelFlags));

    if (SQLiteDbms::instance().insertInto("Item", arguments))
    {
        // Insert into the item_list the new item.
        Mud::instance().addItem(newItem);
    }
    else
    {
        SQLiteDbms::instance().rollbackTransection();
    }

    return newItem;
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
    .addData("type", &ItemModel::modelType) //
    .addData("weight", &ItemModel::weight) //
    .addData("price", &ItemModel::price) //
    .addData("condition", &ItemModel::condition) //
    .addData("decay", &ItemModel::decay) //
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
        if (modelType == ModelType::Armor)
        {
            return "a";
        }
        else if (modelType == ModelType::Weapon)
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

ToolModel * ItemModel::toTool()
{
    return static_cast<ToolModel *>(this);
}

VehicleModel * ItemModel::toVehicle()
{
    return static_cast<VehicleModel *>(this);
}

WeaponModel * ItemModel::toWeapon()
{
    return static_cast<WeaponModel *>(this);
}

std::string GetModelFlagString(unsigned int flags)
{
    std::string flagList;
    if (HasFlag(flags, ModelFlag::Static)) flagList += "|Static";
    if (HasFlag(flags, ModelFlag::Invisible)) flagList += "|Invisible";
    if (HasFlag(flags, ModelFlag::Unbreakable)) flagList += "|Unbreakable";
    if (HasFlag(flags, ModelFlag::NoSaleable)) flagList += "|NoSaleable";
    if (HasFlag(flags, ModelFlag::TwoHand)) flagList += "|TwoHand";
    flagList += "|";
    return flagList;
}