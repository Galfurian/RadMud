/// @file   itemContainer.hpp
/// @author Enrico Fraccaroli
/// @date   Aug 12, 2016
/// @copyright
/// Copyright (c) 2016 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
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

class Item;

#include <vector>
#include <string>

/// @brief Custom vector of items.
class ItemContainer :
    public std::vector<Item *>
{
public:
    /// Type of ordering.
    enum Order
    {
        ByName,
        ByWeight,
        ByPrice
    };

    /// @brief Constructor.
    ItemContainer();

    /// @brief Special function used to add an item to the container.
    /// @details
    /// Before adding the item to this container a check is performed.
    /// If the pushed item is stackable and can be stacked with a target
    ///  item already contained inside this container, the pushed item is
    ///  deleted and its pointer is modifier and pointed to the target item.
    /// @param item The reference to the pointer of the pushed item.
    void push_back_item(Item *& item);

    /// Allows to remove the given item.
    /// @param item The item that has to be removed from the container.
    /// @return If the item has been removed.
    bool removeItem(Item * item);

    /// Search the item with the same vnum of the provided one.
    /// @param vnum The vnum of the item to be searched.
    /// @return The item, if found.
    Item * findItem(const int & vnum) const;

    /// @brief Returns a stacked version of the content.
    /// @details
    /// There are some items which canno be stacked, thus this function
    /// provides a stacked list which should ease printing the content.
    /// @return The stacked version of the content.
    std::vector<std::pair<Item *, int>> toStack() const;

    /// Orders the content based on the given parameter.
    void orderBy(const Order & order);

    /// Returns the string version of the given parameter.
    static std::string orderToString(const Order & order);

private:
    /// Function used to order the content by name.
    static bool orderItemByName(Item * first, Item * second);

    /// Function used to order the content by weight.
    static bool orderItemByWeight(Item * first, Item * second);

    /// Function used to order the content by price.
    static bool orderItemByPrice(Item * first, Item * second);
};
