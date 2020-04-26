/// @file   itemVector.hpp
/// @author Enrico Fraccaroli
/// @date   Aug 12, 2016
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

class Item;

#include <vector>
#include <string>

/// @brief Custom vector of items.
class ItemVector : public std::vector<Item *> {
public:
	/// Type of ordering.
	enum Order { ByName, ByWeight, ByPrice };

	/// @brief Constructor.
	ItemVector();

	/// @brief Special function used to add an item to the container.
	/// @details
	/// Before adding the item to this container a check is performed.
	/// If the pushed item is stackable and can be stacked with a target
	///  item already contained inside this container, the pushed item is
	///  deleted and its pointer is modifier and pointed to the target item.
	/// @param item The reference to the pointer of the pushed item.
	void emplace_back_item(Item *&item);

	/// Allows to remove the given item.
	/// @param item The item that has to be removed from the container.
	/// @return If the item has been removed.
	bool removeItem(Item *item);

	/// Search the item with the same vnum of the provided one.
	/// @param vnum The vnum of the item to be searched.
	/// @return The item, if found.
	Item *findItem(const unsigned int &vnum) const;

	/// @brief Returns a stacked version of the content.
	/// @details
	/// There are some items which canno be stacked, thus this function
	/// provides a stacked list which should ease printing the content.
	/// @return The stacked version of the content.
	std::vector<std::pair<Item *, int> > toStack() const;

	/// Orders the content based on the given parameter.
	void orderBy(const Order &order);

	/// Returns the string version of the given parameter.
	static std::string orderToString(const Order &order);

private:
	/// Function used to order the content by name.
	static bool orderItemByName(Item *first, Item *second);

	/// Function used to order the content by weight.
	static bool orderItemByWeight(Item *first, Item *second);

	/// Function used to order the content by price.
	static bool orderItemByPrice(Item *first, Item *second);
};
