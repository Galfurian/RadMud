/// @file   itemModel.hpp
/// @brief  Define item model variables and methods.
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

#pragma once

#include <string>
#include <vector>
#include <set>
#include <map>

#include "equipmentSlot.hpp"
#include "itemQuality.hpp"
#include "lua_script.hpp"
#include "defines.hpp"
#include "table.hpp"
#include "utils.hpp"

/// Used to determine the type of the model.
using ModelType = enum class ModelType_t
{
    NoType,             ///< [0] No type.
    Corpse,             ///< [1] A corpse.
    MeleeWeapon,        ///< [2] Any melee weapon.
    RangedWeapon,       ///< [3] Any ranged weapon.
    Armor,              ///< [4] Any armor.
    Shield,             ///< [5] A shield.
    Projectile,         ///< [6] Projectiles.
    Container,          ///< [7] A container for items.
    LiquidContainer,    ///< [8] A container for liquids.
    Tool,               ///< [9] Any tool.
    Node,               ///< [10] Any node of resources.
    Resource,           ///< [11] A resource.
    Seed,               ///< [12] Any kind of seed.
    Key,                ///< [13] A key.
    Furniture,          ///< [14] A forniture (eg. chair, bed, painting, table and so on).
    Food,               ///< [15] A food.
    Light,              ///< [16] A source of light ((eg. torch, lamp, candle and so on).
    Vehicle,            ///< [17] Any kind of vehicle.
    Book,               ///< [18] Container of parchements.
    Rope,               ///< [19] A generic rope.
    Mechanism,          ///< [20] Any kind of mechanism.
    Currency,           ///< [21] Any kind of currency.
    Shop,               ///< [22] A shop.
    Magazine            ///< [23] A magazine for ammunitions.
};

/// Used to determine the flag of the model.
using ModelFlag = enum class ModelFlag_t
{
    None = 0,           ///< [0]    No flag.
    Static = 1,         ///< [1]    The item can't be collected, put down or moved.
    Invisible = 2,      ///< [2]    The item can't be seen.
    Unbreakable = 4,    ///< [4]    The item can't be damaged.
    NoSaleable = 8,     ///< [8]    The item cannot be sold.
    TwoHand = 16,       ///< [16]   Must be wielded with two hands.
    CanClose = 32,      ///< [32]   It can be closed.
    CanSeeThrough = 64, ///< [64]   Even if it is closed, a character can see through it.
    CanBeStacked = 128  ///< [128]  The items with this flag can be stacked.
};

class Item;

class Player;

class Material;

class ArmorModel;

class BookModel;

class ContainerModel;

class CorpseModel;

class CurrencyModel;

class FoodModel;

class FurnitureModel;

class KeyModel;

class LightModel;

class LiquidContainerModel;

class MechanismModel;

class NodeModel;

class ProjectileModel;

class ResourceModel;

class RopeModel;

class SeedModel;

class ShieldModel;

class ShopModel;

class ToolModel;

class VehicleModel;

class MeleeWeaponModel;

class RangedWeaponModel;

class MagazineModel;

/// @brief Holds details about a model of item.
class ItemModel
{
public:
    /// Unique vnum.
    int vnum;
    /// The model name.
    std::string name;
    /// The model article.
    std::string article;
    /// The model short description.
    std::string shortdesc;
    /// The model keys.
    std::vector<std::string> keys;
    /// The model description.
    std::string description;
    /// Store here the position where the model can be equipped.
    EquipmentSlot slot;
    /// The model flags.
    unsigned int modelFlags;
    /// The model base weight.
    double baseWeight;
    /// The model base price.
    unsigned int basePrice;
    /// The model maximum condition.
    unsigned int condition;
    /// The model maximum condition.
    unsigned int decay;
    /// The model's material.
    MaterialType material;
    /// TileSet of the icon.
    int tileSet;
    /// TileId of the icon.
    int tileId;

    /// @brief Constructor.
    ItemModel();

    /// @brief Disable Copy Construct.
    ItemModel(ItemModel const &) = delete;

    /// @brief Disable Move construct.
    ItemModel(ItemModel &&) = delete;

    /// @brief Disable Copy assign.
    ItemModel & operator=(ItemModel const &) = delete;

    /// @brief Disable Move assign.
    ItemModel & operator=(ItemModel &&) = delete;

    /// @brief Destructor.
    virtual ~ItemModel();

    /// @return Provides the type of model.
    virtual ModelType getType() const;

    /// @return Provides a string representing the type of model.
    virtual std::string getTypeName() const = 0;

    /// @return Extracts from the source string the information
    ///          about the model and set them.
    /// @param source The source string.
    /// @return <b>True</b> if the information are correct,<br>
    ///         <b>False</b> otherwise.
    virtual bool setModel(const std::string & source) = 0;

    /// @brief Fills the provided table with the information concerning the model.
    /// @param sheet The table that has to be filled.
    virtual void getSheet(Table & sheet) const;

    /// @brief Create a new item starting from this model.
    /// @param maker       The player that create the item.
    /// @param composition The composition of the item.
    /// @param isForMobile Defines if the item is for a mobile, thus it is not save on the DB nor on the MUD.
    /// @param itemQuality The quality of the item.
    /// @param quantity    The quantity.
    /// @return The newly created item.
    virtual Item * createItem(
        std::string maker,
        Material * composition,
        bool isForMobile = false,
        const ItemQuality & itemQuality = ItemQuality::Normal,
        const unsigned int & quantity = 1);

    /// @brief Check the correctness of the model.
    /// @return <b>True</b> if the model has correct values,<br>
    ///         <b>False</b> otherwise.
    bool check();

    /// @brief Given the argumtens the function replace the symbols inside the source string
    ///         using the provided arguments.
    /// @param source       The source string.
    /// @param itemMaterial The material of which the model is made.
    /// @param itemQuality  The quality of the model.
    /// @return <b>True</b> if something changed inside the string,<br>
    ///         <b>False</b> otherwise.
    bool replaceSymbols(
        std::string & source,
        Material * itemMaterial = nullptr,
        const ItemQuality & itemQuality = ItemQuality::Normal) const;

    /// @brief Returns the name of the model depending on the passed arguments.
    /// @param itemMaterial The material of which the model is made.
    /// @param itemQuality  The quality of the model.
    /// @return The specific name of the model.
    std::string getName(Material * itemMaterial = nullptr, const ItemQuality & itemQuality =
    ItemQuality::Normal) const;

    /// @brief Returns the description of the model depending on the passed arguments.
    /// @param itemMaterial The material of which the model is made.
    /// @param itemQuality  The quality of the model.
    /// @return The specific description of the model.
    std::string getDescription(
        Material * itemMaterial = nullptr,
        const ItemQuality & itemQuality = ItemQuality::Normal);

    /// @brief Check if the item must be wielded.
    /// @return <b>True</b> if the item must be wielded,<br><b>False</b> Otherwise.
    bool mustBeWielded();

    /// @brief Function used to register inside the lua environment the class.
    /// @param L The lua environment.
    static void luaRegister(lua_State * L);

    /// @brief Returns the tile of the model.
    /// @param offset The ofset of the tile.
    /// @return The string which contains the code of the tile.
    std::string getTile(int offset = 0);

public:
    /// @brief Returns the model <b>statically</b> casted to Armor.
    ArmorModel * toArmor();

    /// @brief Returns the model <b>statically</b> casted to Book.
    BookModel * toBook();

    /// @brief Returns the model <b>statically</b> casted to Container.
    ContainerModel * toContainer();

    /// @brief Returns the model <b>statically</b> casted to Corpse.
    CorpseModel * toCorpse();

    /// @brief Returns the model <b>statically</b> casted to Currency.
    CurrencyModel * toCurrency();

    /// @brief Returns the model <b>statically</b> casted to Food.
    FoodModel * toFood();

    /// @brief Returns the model <b>statically</b> casted to Furniture.
    FurnitureModel * toFurniture();

    /// @brief Returns the model <b>statically</b> casted to Key.
    KeyModel * toKey();

    /// @brief Returns the model <b>statically</b> casted to Light.
    LightModel * toLight();

    /// @brief Returns the model <b>statically</b> casted to Liquid Container.
    LiquidContainerModel * toLiquidContainer();

    /// @brief Returns the model <b>statically</b> casted to Mechanism.
    MechanismModel * toMechanism();

    /// @brief Returns the model <b>statically</b> casted to Node.
    NodeModel * toNode();

    /// @brief Returns the model <b>statically</b> casted to Projectile.
    ProjectileModel * toProjectile();

    /// @brief Returns the model <b>statically</b> casted to Resource.
    ResourceModel * toResource();

    /// @brief Returns the model <b>statically</b> casted to Rope.
    RopeModel * toRope();

    /// @brief Returns the model <b>statically</b> casted to Seed.
    SeedModel * toSeed();

    /// @brief Returns the model <b>statically</b> casted to Shield.
    ShieldModel * toShield();

    /// @brief Returns the model <b>statically</b> casted to shop.
    ShopModel * toShop();

    /// @brief Returns the model <b>statically</b> casted to Tool.
    ToolModel * toTool();

    /// @brief Returns the model <b>statically</b> casted to Vehicle.
    VehicleModel * toVehicle();

    /// @brief Returns the model <b>statically</b> casted to Melee Weapon.
    MeleeWeaponModel * toMeleeWeapon();

    /// @brief Returns the model <b>statically</b> casted to Ranged Weapon.
    RangedWeaponModel * toRangedWeapon();

    /// @brief Returns the model <b>statically</b> casted to Magazine.
    MagazineModel * toMagazine();
};

/// @defgroup FlagsToList Flags to List of Strings.
/// @brief All the functions necessary to transform into a list of string a pool of flags.
/// @{

/// Return a list of string containg the Model flags contained inside the value.
std::string GetModelFlagString(unsigned int flags);

/// @}
