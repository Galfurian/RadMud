/// @file   itemContainer.cpp
/// @author Enrico Fraccaroli
/// @date   Aug 12 2016
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

#include "itemVector.hpp"
#include "updater.hpp"
#include "item.hpp"

ItemVector::ItemVector()
{
    // Nothing to do.
}

void ItemVector::push_back_item(Item *& item)
{
    auto stacked = false;
    for (auto it = this->begin(); it != this->end(); ++it)
    {
        Item * content = (*it);
        if (item->canStackWith(content))
        {
            content->quantity += item->quantity;
            MudUpdater::instance().addItemToDestroy(item);
            item = content;
            stacked = true;
            content->updateOnDB();
            break;
        }
    }
    if (!stacked)
    {
        this->push_back(item);
    }
}

bool ItemVector::removeItem(Item * item)
{
    for (auto it = this->begin(); it != this->end(); ++it)
    {
        Item * contained = (*it);
        if (contained->vnum == item->vnum)
        {
            this->erase(it);
            return true;
        }
    }
    return false;
}

Item * ItemVector::findItem(const int & vnum) const
{
    for (const_iterator it = this->begin(); it != this->end(); ++it)
    {
        if ((*it)->vnum == vnum)
        {
            return (*it);
        }
    }
    return nullptr;
}

std::vector<std::pair<Item *, int>> ItemVector::toStack() const
{
    std::vector<std::pair<Item *, int>> numberedItems;
    for (auto item : (*this))
    {
        bool missing = true;
        for (auto & it2 : numberedItems)
        {
            if (it2.first->getName() == item->getName())
            {
                if (HasFlag(it2.first->flags, ItemFlag::Built))
                {
                    if (HasFlag(item->flags, ItemFlag::Built))
                    {
                        missing = false;
                        it2.second++;
                        break;
                    }
                }
                else
                {
                    missing = false;
                    it2.second++;
                    break;
                }
            }
        }
        if (missing)
        {
            numberedItems.push_back(std::make_pair(item, 1));
        }
    }
    return numberedItems;
}

void ItemVector::orderBy(const ItemVector::Order & order)
{
    auto orderingFunction = ItemVector::orderItemByName;
    if (order == Order::ByWeight)
    {
        orderingFunction = ItemVector::orderItemByWeight;
    }
    else if (order == Order::ByPrice)
    {
        orderingFunction = ItemVector::orderItemByPrice;
    }
    std::sort(this->begin(), this->end(), orderingFunction);
}

std::string ItemVector::orderToString(const Order & order)
{
    if (order == Order::ByName) return "name";
    else if (order == Order::ByWeight) return "weight";
    else return "price";
}

bool ItemVector::orderItemByName(Item * first, Item * second)
{
    return first->getName() < second->getName();
}

bool ItemVector::orderItemByWeight(Item * first, Item * second)
{
    return first->getWeight(true) < second->getWeight(true);
}

bool ItemVector::orderItemByPrice(Item * first, Item * second)
{
    return first->getPrice(true) < second->getPrice(true);
}

