/// @file   itemFactory.cpp
/// @brief  Implements the factory for items.
/// @author Enrico Fraccaroli
/// @date   Oct 21 2015
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

#include "itemFactory.hpp"

#include "shopItem.hpp"
#include "armorItem.hpp"
#include "lightItem.hpp"
#include "corpseItem.hpp"
#include "currencyItem.hpp"
#include "resourceItem.hpp"
#include "magazineItem.hpp"
#include "meleeWeaponItem.hpp"
#include "rangedWeaponItem.hpp"
#include "liquidContainerItem.hpp"

Item * ItemFactory::newItem(const ModelType & type)
{
    if (type == ModelType::None)
    {
        return nullptr;
    }
    if (type == ModelType::Armor)
    {
        return new ArmorItem();
    }
    if (type == ModelType::Corpse)
    {
        return new CorpseItem();
    }
    if (type == ModelType::Currency)
    {
        return new CurrencyItem();
    }
    if (type == ModelType::Light)
    {
        return new LightItem();
    }
    if (type == ModelType::Magazine)
    {
        return new MagazineItem();
    }
    if (type == ModelType::MeleeWeapon)
    {
        return new MeleeWeaponItem();
    }
    if (type == ModelType::RangedWeapon)
    {
        return new RangedWeaponItem();
    }
    if (type == ModelType::Shop)
    {
        return new ShopItem();
    }
    if (type == ModelType::Resource)
    {
        return new ResourceItem();
    }
    if (type == ModelType::LiquidContainer)
    {
        return new LiquidContainerItem();
    }
    return new Item();
}
