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
        flags(),
        weight(),
        price(),
        condition(),
        decay(),
        material(),
        tileSet(),
        tileId(),
        functions()
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
bool ItemModel::setFunctions(std::string source)
{
    if (source.empty())
    {
        Logger::log(LogLevel::Error, "Function list is empty (%s).", this->name);
        return false;
    }
    std::vector<std::string> functionList = SplitString(source, " ");
    if (functionList.size() != 6)
    {
        Logger::log(LogLevel::Error, "Function list contains the wrong number of parameters.");
        return false;
    }
    for (auto it : functionList)
    {
        int value = ToNumber<int>(it);
        if (value < 0)
        {
            Logger::log(LogLevel::Error, "Function list contains a negative value.");
            return false;
        }
        functions.push_back(static_cast<unsigned int>(value));
    }
    return true;
}

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

bool ItemModel::replaceSymbols(std::string & source, Material * itemMaterial, ItemQuality itemQuality)
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
    arguments.push_back(ToString(flags));

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

std::string ItemModel::getSpecificTypeName()
{
    std::string output;
    if (this->modelType == ModelType::Weapon)
    {
        output = GetWeaponTypeName(getWeaponFunc().type);
    }
    else if (this->modelType == ModelType::Armor)
    {
        output = GetArmorSizeName(getArmorFunc().size);
    }
    else if (this->modelType == ModelType::Shield)
    {
        output = GetShieldSizeName(getShieldFunc().size);
    }
    else if (this->modelType == ModelType::Tool)
    {
        output = GetToolTypeName(getToolFunc().type);
    }
    else if (this->modelType == ModelType::Node)
    {
        output = GetNodeTypeName(getNodeFunc().type);
    }
    else if (modelType == ModelType::Resource)
    {
        output = GetResourceTypeName(getResourceFunc().type);
    }
    else if (modelType == ModelType::Seed)
    {
        output = GetSeedTypeName(getSeedFunc().type);
    }
    else if (modelType == ModelType::Mechanism)
    {
        output = GetMechanismTypeName(getMechanismFunc().type);
    }
    return output;
}

WeaponFunc ItemModel::getWeaponFunc()
{
    WeaponFunc func;
    func.type = static_cast<WeaponType>(functions[0]);
    func.minDamage = functions[1];
    func.maxDamage = functions[2];
    func.range = functions[3];
    return func;
}

ArmorFunc ItemModel::getArmorFunc()
{
    ArmorFunc func;
    func.size = static_cast<ArmorSize>(functions[0]);
    func.damageAbs = functions[1];
    func.allowedAnatomy = functions[2];
    return func;
}

ShieldFunc ItemModel::getShieldFunc()
{
    ShieldFunc func;
    func.size = static_cast<ShieldSize>(functions[0]);
    func.parryChance = functions[1];
    return func;
}

ProjectileFunc ItemModel::getProjectileFunc()
{
    ProjectileFunc func;
    func.damageBonus = functions[0];
    func.rangeBonus = functions[1];
    return func;
}

ContainerFunc ItemModel::getContainerFunc()
{
    ContainerFunc func;
    func.maxWeight = functions[0];
    func.flags = functions[1];
    func.keyVnum = functions[2];
    func.difficulty = functions[3];
    return func;
}

LiqContainerFunc ItemModel::getLiqContainerFunc()
{
    LiqContainerFunc func;
    func.maxWeight = functions[0];
    func.flags = functions[1];
    return func;
}

ToolFunc ItemModel::getToolFunc()
{
    ToolFunc func;
    func.type = static_cast<ToolType>(functions[0]);
    return func;
}

NodeFunc ItemModel::getNodeFunc()
{
    NodeFunc func;
    func.type = static_cast<NodeType>(functions[0]);
    func.provides = functions[1];
    return func;
}

ResourceFunc ItemModel::getResourceFunc()
{
    ResourceFunc func;
    func.type = static_cast<ResourceType>(functions[0]);
    return func;
}

SeedFunc ItemModel::getSeedFunc()
{
    SeedFunc func;
    func.type = static_cast<SeedType>(functions[0]);
    return func;
}

FoodFunc ItemModel::getFoodFunc()
{
    FoodFunc func;
    func.hours = functions[0];
    func.flags = functions[1];
    return func;
}

LightFunc ItemModel::getLightFunc()
{
    LightFunc func;
    func.maxHours = functions[0];
    func.policy = functions[1];
    return func;
}

BookFunc ItemModel::getBookFunc()
{
    BookFunc func;
    func.maxParchments = functions[0];
    return func;
}

RopeFunc ItemModel::getRopeFunc()
{
    RopeFunc func;
    func.type = functions[0];
    func.difficulty = functions[1];
    return func;
}

MechanismFunc ItemModel::getMechanismFunc()
{
    MechanismFunc func;
    func.type = static_cast<MechanismType>(functions[0]);
    if ((func.type == MechanismType::Door) || (func.type == MechanismType::Lock))
    {
        func.key = functions[1];
        func.difficulty = functions[2];
    }
    else if (func.type == MechanismType::Picklock)
    {
        func.efficency = functions[1];
    }
    else if (func.type == MechanismType::Lever)
    {
        func.command = functions[1];
        func.target = functions[2];
    }
    return func;
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
