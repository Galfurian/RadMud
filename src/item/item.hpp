/// @file   item.hpp
/// @brief  Define item variables and methods.
/// @author Enrico Fraccaroli
/// @date   Aug 25 2014
/// @copyright
/// Copyright (c) 2014, 2015, 2016 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
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

#pragma once

#include <vector>
#include <list>
#include <map>

#include "../liquid.hpp"
#include "../lua/lua_script.hpp"
#include "../model/itemModel.hpp"
#include "../model/nodeModel.hpp"
#include "../utilities/table.hpp"

#include "itemContainer.hpp"

class Room;

class Character;

class Material;

class ShopItem;

class ArmorItem;

class MeleeWeaponItem;

class RangedWeaponItem;

class CurrencyItem;

class CorpseItem;

/// @brief Holds details about items.
class Item
{
public:
    /// Item vnum.
    int vnum;
    /// The type of item.
    ModelType type;
    /// Item model.
    ItemModel * model;
    /// The number of stacked items.
    unsigned int quantity;
    /// The player that created the item.
    std::string maker;
    /// The item's price.
    unsigned int price;
    /// The item's weight.
    double weight;
    /// The item's condition.
    unsigned int condition;
    /// The maximum condition.
    unsigned int maxCondition;
    /// The composing material of the item.
    Material * composition;
    /// The quality of the item.
    ItemQuality quality;
    /// The item flags.
    unsigned int flags;
    /// Pointer to the room, where the item resides.
    Room * room;
    /// Pointer to the character who owns this item.
    Character * owner;
    /// Pointer to the item which contains this item.
    Item * container;
    /// Current equipment slot.
    EquipmentSlot currentSlot;
    /// List of items contained in this one.
    ItemContainer content;
    /// The liquid inside the container.
    std::pair<Liquid *, double> contentLiq;

    /// @brief Constructor - Create a new empty item.
    Item();

    /// @brief Disable Copy Construct.
    Item(Item const &) = delete;

    /// @brief Disable Move construct.
    Item(Item &&) = delete;

    /// @brief Disable Copy assign.
    Item & operator=(Item const &) = delete;

    /// @brief Disable Move assign.
    Item & operator=(Item &&) = delete;

    /// @brief Destructor - Is a method which is automatically invoked when the object is destroyed.
    virtual ~Item();

    /// @brief Check the correctness of the item.
    /// @return <b>True</b> if the item has correct values,<br>
    ///         <b>False</b> otherwise.
    virtual bool check(bool complete = false);

    /// @brief This function is used to remove the item from everywhere.
    virtual void removeFromMud();

    /// @brief Create or Update the item entry on database.
    /// @return <b>True</b> if the execution goes well,<br>
    ///         <b>False</b> otherwise.
    virtual bool updateOnDB();

    /// @brief Remove the item on database.
    /// @return <b>True</b> if the execution goes well,<br>
    ///         <b>False</b> otherwise.
    virtual bool removeOnDB();

    /// @brief Fills the provided table with the information concerning the item.
    /// @param sheet The table that has to be filled.
    virtual void getSheet(Table & sheet) const;

    /// @brief Check if the item can be deconstructed.
    /// @param error In case the item cannot be deconstructed, error contains the reason.
    /// @return <b>True</b> if it can be deconstructed,<br>
    ///         <b>False</b> otherwise.
    virtual bool canDeconstruct(std::string & error) const;

    /// Provides the type of item.
    ModelType getType() const;

    /// Provides a string representing the type of item.
    std::string getTypeName() const;

    /// @brief Check if the provided item can be stacked with this one.
    /// @param item The item to check.
    /// @return <b>True</b> if it can be stacked,<br>
    ///         <b>False</b> otherwise.
    bool canStackWith(Item * item) const;

    Item * removeFromStack(Character * actor, unsigned int & _quantity);

    /// @brief Check if the item has the desired key.
    /// @param key The key to search.
    /// @return <b>True</b> if the operations succeeded,<br>
    ///         <b>False</b> Otherwise.
    bool hasKey(std::string key);

    /// @brief Provides the maximum condition of the item, given
    ///         quality and material.
    unsigned int getMaxCondition() const;

    /// @brief Trigger a decay cycle.
    /// @return <b>True</b> if the item is destroyed,<br>
    ///         <b>False</b> otherwise.
    bool triggerDecay();

    /// @brief Provides the modifier due to the item's condition.
    double getConditionModifier() const;

    /// @brief Get the item current condition in text.
    /// @return The condition of the item.
    std::string getConditionDescription();

    /// @brief Provides the price of the item based on its quality, material and condition.
    /// @param entireStack If <b>true</b> this function returns the price of the entire stack.
    virtual unsigned int getPrice(bool entireStack) const;

    /// @brief Get the item weight, plus eventually contained item weight.
    /// @param entireStack If <b>true</b> this function returns the weight of the entire stack.
    /// @return The total weight of the item.
    virtual double getWeight(bool entireStack) const;

    /// @brief Return the name of the item.
    /// @param colored If <b>true</b> the name also include formatting color.
    /// @return The name of the item.
    std::string getName(bool colored = false) const;

    /// @brief Return the name of the item with the first letter capitalized.
    /// @param colored If <b>true</b> the name also include formatting color.
    /// @return The name of the item.
    std::string getNameCapital(bool colored = false) const;

    /// @brief Return the description of the item.
    /// @return The description of the item.
    std::string getDescription();

    /// @brief Provide a detailed description of the item, including it's condition.
    /// @return A detailed description of the item.
    std::string getLook();

    /// @brief Check if the item is a node of the given type.
    /// @param nodeType The type of node to check.
    /// @return <b>True</b> if the node has the right node type,<br><b>False</b> otherwise.
    bool hasNodeType(NodeType nodeType);

    /// @brief Check if the item is a valid container.
    /// @return <b>True</b> if it is a valid container,<br>
    ///         <b>False</b> otherwise.
    bool isAContainer() const;

    /// @brief Check if the container is empty.
    /// @return <b>True</b> if the item is empty,<br>
    ///         <b>False</b> otherwise.
    bool isEmpty() const;

    /// @brief Return the total space of the container.
    /// @return The total space as an integer.
    double getTotalSpace() const;

    /// @brief Return the used space of the container.
    /// @return The used space as an integer.
    double getUsedSpace() const;

    /// @brief Return the free space inside the container.
    /// @return The free unit of space as an integer.
    double getFreeSpace() const;

    /// @brief Check if this item can contain the passed one.
    /// @param item    The item to check.
    /// @param ammount The ammount to check.
    /// @return <b>True</b> if it can be contained,<br>
    ///         <b>False</b> otherwise.
    bool canContain(Item * item, const unsigned int & ammount) const;

    /// @brief Load an item inside the container and update the database.
    /// @param item     The item to load in.
    /// @param updateDB If the action has to be updated on the database.
    void putInside(Item *& item, bool updateDB = true);

    /// @brief Extract an item from the container.
    /// @param item The item to load in.
    /// @param updateDB If the action has to be updated on the database.
    /// @return <b>True</b> if the item has been taken out,<br>
    ///         <b>False</b> otherwise.
    bool takeOut(Item * item, bool updateDB = true);

    /// @brief Check if this item can contain the passed one.
    /// @param liquid  The liquid to pour in.
    /// @param ammount The ammount of liquid.
    /// @return <b>True</b> if it can be contained,<br>
    ///         <b>False</b> otherwise.
    bool canContainLiquid(Liquid * liquid, const double & ammount) const;

    /// @brief Load some liquid inside the container and update the database.
    /// @param liquid   The liquid to load in.
    /// @param ammount  The ammount of liquid.
    /// @param updateDB If the action has to be updated on the database.
    /// @return <b>True</b> if the operation is a success,<br>
    ///         <b>False</b> otherwise.
    bool pourIn(Liquid * liquid, const double & ammount, bool updateDB = true);

    /// @brief Extract some liquid from the container and update the database.
    /// @param ammount  The ammount of liquid.
    /// @param updateDB If the action has to be updated on the database.
    /// @return <b>True</b> if the operation is a success,<br><b>False</b> otherwise.
    bool pourOut(const double & ammount, bool updateDB = true);

    /// @brief Search for the item inside the container.
    /// @param search_parameter The item to search.
    /// @param number           Position of the item we want to look for.
    /// @return The item, if it's in the container.
    Item * findContent(std::string search_parameter, int & number);

    /// @brief Return the description of the content.
    /// @return The string describing the content.
    virtual std::string lookContent();

    /// @brief Set the equipment slot where this item must be weared.
    /// @param _currentSlot The new equipment slot.
    void setCurrentSlot(EquipmentSlot _currentSlot);

    /// @brief Return the current equipment slot.
    /// @return The equipment slot.
    EquipmentSlot getCurrentSlot();

    /// @brief Return the current equipment slot name.
    /// @return The equipment slot name.
    std::string getCurrentSlotName();

    /// @brief Returns the model <b>statically</b> casted to Shop.
    ShopItem * toShopItem();

    /// @brief Returns the model <b>statically</b> casted to Armor.
    ArmorItem * toArmorItem();

    /// @brief Returns the model <b>statically</b> casted to Melee Weapon.
    MeleeWeaponItem * toMeleeWeaponItem();

    /// @brief Returns the model <b>statically</b> casted to Ranged Weapon.
    RangedWeaponItem * toRangedWeaponItem();

    /// @brief Returns the model <b>statically</b> casted to Currency.
    CurrencyItem * toCurrencyItem();

    /// @brief Returns the model <b>statically</b> casted to Currency.
    CorpseItem * toCorpseItem();

    /// @brief Function used to register inside the lua environment the class.
    /// @param L The lua environment.
    static void luaRegister(lua_State * L);

    /// @brief Operator used to order the items based on their name.
    bool operator<(Item & rhs) const;
};

/// @brief Function which instantiate the item of the same type of the passed model.
/// @param type The type of item.
/// @return the generated item.
Item * GenerateItem(const ModelType & type);
