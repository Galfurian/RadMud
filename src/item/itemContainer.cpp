/// @file   itemContainer.cpp
/// @author Enrico Fraccaroli
/// @date   Aug 12 2016
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

#include "itemContainer.hpp"

#include "item.hpp"

ItemContainer::ItemContainer()
{
    // TODO Auto-generated constructor stub

}

ItemContainer::~ItemContainer()
{
    // TODO Auto-generated destructor stub
}

void ItemContainer::push_back_item(Item * & item)
{
    auto stacked = false;
    for (auto it = this->begin(); it != this->end(); ++it)
    {
        Item * content = (*it);
        if (item->canStackWith(content))
        {
            content->quantity += item->quantity;
            item->removeFromMud();
            item->removeOnDB();
            delete (item);
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

bool ItemContainer::removeItem(Item * item)
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

Item * ItemContainer::findItem(const int & vnum) const
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

std::vector<std::pair<Item *, int>> ItemContainer::toStack() const
{
    std::vector<std::pair<Item *, int>> numberedItems;
    for (auto it1 = this->begin(); it1 != this->end(); ++it1)
    {
        Item * item = (*it1);
        bool missing = true;
        for (auto it2 = numberedItems.begin(); it2 != numberedItems.end(); ++it2)
        {
            if (it2->first->getName() == item->getName())
            {
                if (HasFlag(it2->first->flags, ItemFlag::Built))
                {
                    if (HasFlag(item->flags, ItemFlag::Built))
                    {
                        missing = false;
                        it2->second++;
                        break;
                    }
                }
                else
                {
                    missing = false;
                    it2->second++;
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

void ItemContainer::orderBy(const ItemContainer::Order & order)
{
    if (order == Order::ByName)
    {
        std::sort(this->begin(), this->end(), ItemContainer::orderItemByName);
    }
    else if (order == Order::ByWeight)
    {
        std::sort(this->begin(), this->end(), ItemContainer::orderItemByWeight);
    }
    else if (order == Order::ByPrice)
    {
        std::sort(this->begin(), this->end(), ItemContainer::orderItemByPrice);
    }
}

std::string ItemContainer::orderToString(const Order & order)
{
    if (order == Order::ByName)
    {
        return "name";
    }
    else if (order == Order::ByWeight)
    {
        return "weight";
    }
    else
    {
        return "price";
    }
}

bool ItemContainer::orderItemByName(Item * first, Item * second)
{
    return first->getName() < second->getName();
}

bool ItemContainer::orderItemByWeight(Item * first, Item * second)
{
    return first->getWeight(true) < second->getWeight(true);
}

bool ItemContainer::orderItemByPrice(Item * first, Item * second)
{
    return first->getPrice() < second->getPrice();
}

