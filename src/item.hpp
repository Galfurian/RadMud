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

#ifndef ITEM_HPP
#define ITEM_HPP

#include <vector>
#include <map>

#include "liquid.hpp"
#include "lua/lua_script.hpp"
#include "model.hpp"

class Room;
class Character;
class Material;

/// @brief Holds details about items.
class Item
{
    public:
        /// Item vnum.
        int vnum;
        /// Item model.
        Model * model;
        /// The player that created the item.
        std::string maker;
        /// In which condition is the item.
        int condition;
        /// The composing material of the item.
        Material * composition;
        /// The quality of the item.
        ItemQuality quality;
        /// The item flags.
        int flags;

        /// Pointer to the room, where the item resides.
        Room * room;
        /// Pointer to the character who owns this item.
        Character * owner;
        /// Pointer to the item which contains this item.
        Item * container;
        /// Current equipment slot.
        EquipmentSlot currentSlot;
        /// List of items contained in this one.
        std::vector<Item *> content;
        /// The liquid inside the container.
        LiquidContent contentLiq;

        /// @brief Constructor - Create a new empty item.
        Item();

        /// @brief Destructor - Is a method which is automatically invoked when the object is destroyed.
        ~Item();

        /// @brief Disable copy.
        Item(const Item&) = delete;

        /// @brief Disable assignment.
        Item & operator=(const Item&) = delete;

        /// @brief Check the correctness of the item.
        /// @return <b>True</b> if the item has correct values,<br><b>False</b> otherwise.
        bool check(bool complete = false);

        /// @brief This function is used to destroy the item.
        void destroy();

        /// @brief Save the item on database.
        /// @return <b>True</b> if the execution goes well,<br><b>False</b> otherwise.
        bool updateOnDB();

        /// @brief Remove the item on database.
        /// @return <b>True</b> if the execution goes well,<br><b>False</b> otherwise.
        bool removeOnDB();

        /// @brief Check if the item has the desired key.
        /// @param key The key to search.
        /// @return <b>True</b> if the operations succeeded,<br><b>False</b> Otherwise.
        bool hasKey(std::string key);

        /// @brief Decrease the item condition and check if it reache 0.
        /// @param degradation How much the item's condition must decrease.
        /// @return <b>True</b> if condition <= 0, <b>False</b> otherwise.
        bool editCondition(int degradation);

        /// @brief Trigger a decay cycle.
        void decayCondition();

        /// @brief Get the item current condition in text.
        /// @return The condition of the item.
        std::string getCondition();

        /// @brief Get the item weight, plus eventually contained item weight.
        /// @return The total weight of the item.
        int getWeight();

        /// @brief Return the name of the item.
        /// @return The name of the item.
        std::string getName();

        /// @brief Return the name of the item with the first letter capitalized.
        /// @return The name of the item.
        std::string getNameCapital();

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

        /// @brief Check if the container is empty.
        /// @return <b>True</b> if the item is empty,<br><b>False</b> otherwise.
        bool isEmpty();

        /// @brief Return the total space of the container.
        /// @return The total space as an integer.
        int getTotalSpace();

        /// @brief Return the used space of the container.
        /// @return The used space as an integer.
        int getUsedSpace();

        /// @brief Return the free space inside the container.
        /// @return The free unit of space as an integer.
        int getFreeSpace();

        /// @brief Load an item inside the container and update the database.
        /// @param item The item to load in.
        /// @return <b>True</b> if the item is a container,<br><b>False</b> otherwise.
        bool putInside(Item * item);

        /// @brief Extract an item from the container.
        /// @param item The item to load in.
        void takeOut(Item * item);

        /// @brief Load some liquid inside the container and update the database.
        /// @param liquid   The liquid to load in.
        /// @param quantity The quantity of liquid.
        /// @return <b>True</b> if the operation is a success,<br><b>False</b> otherwise.
        bool pourIn(Liquid * liquid, int quantity);

        /// @brief Extract some liquid from the container and update the database.
        /// @param quantity The quantity of liquid.
        /// @return <b>True</b> if the operation is a success,<br><b>False</b> otherwise.
        bool pourOut(unsigned int quantity);

        /// @brief Search for the item inside the container.
        /// @param search_parameter The item to search.
        /// @param number           Position of the item we want to look for.
        /// @return The item, if it's in the container.
        Item * findContent(std::string search_parameter, int & number);

        /// @brief Return the description of the content.
        /// @return The string describing the content.
        std::string lookContent();

        /// @brief Set the equipment slot where this item must be weared.
        /// @param _currentSlot The new equipment slot.
        void setCurrentSlot(EquipmentSlot _currentSlot);

        /// @brief Return the current equipment slot.
        /// @return The equipment slot.
        EquipmentSlot getCurrentSlot();

        /// @brief Return the current equipment slot name.
        /// @return The equipment slot name.
        std::string getCurrentSlotName();

        /// @brief Function used to register inside the lua environment the class.
        /// @param L The lua environment.
        static void luaRegister(lua_State * L);

        /// @brief Operator used to order the items based on their name.
        inline bool operator<(Item & rhs)
        {
            return getName() < rhs.getName();
        }
};

/// Item list handler.
typedef std::vector<Item *> ItemList;

/// Vector of items with counter.
typedef std::vector<std::pair<Item *, int>> ItemListCounted;

/// Item map handler.
typedef std::map<int, Item *> ItemMap;

/// ItemMap iterator.
typedef ItemMap::iterator ItemMapIterator;

/// @brief Return the list of items grouped.
/// @param items The item list to group.
/// @return A vector which has for each item listed the number of occurence of that item.
ItemListCounted GroupItems(ItemList items);

/// It's a pointer to an ordering function for items.
typedef bool (*ItemSorter)(Item * first, Item * second);

/// @brief Compare the names of the two item.
/// @param first  The first item.
/// @param second The second item.
/// @return <b>True</b> if the first name is beefore the second.<br><b>False</b> otherwise.
bool OrderItemByName(Item * first, Item * second);

/// @brief Compare the weight of the two item.
/// @param first  The first item.
/// @param second The second item.
/// @return <b>True</b> if the first item is lighter then the second.<br><b>False</b> otherwise.
bool OrderItemByWeight(Item * first, Item * second);

#endif