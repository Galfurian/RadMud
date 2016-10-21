/// @file   itemFactory.cpp
/// @brief  Implements the factory for items.
/// @author Enrico Fraccaroli
/// @date   Oct 21 2015
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

#include "itemFactory.hpp"

#include "shopItem.hpp"
#include "armorItem.hpp"
#include "corpseItem.hpp"
#include "currencyItem.hpp"
#include "meleeWeaponItem.hpp"
#include "rangedWeaponItem.hpp"
#include "magazineItem.hpp"

Item * ItemFactory::newItem(const ModelType & type)
{
    if (type == ModelType::NoType)
    {
        return nullptr;
    }
    if (type == ModelType::Armor)
    {
        return new ArmorItem();
    }
    if (type == ModelType::Shop)
    {
        return new ShopItem();
    }
    if (type == ModelType::MeleeWeapon)
    {
        return new MeleeWeaponItem();
    }
    if (type == ModelType::RangedWeapon)
    {
        return new RangedWeaponItem();
    }
    if (type == ModelType::Currency)
    {
        return new CurrencyItem();
    }
    if (type == ModelType::Corpse)
    {
        return new CorpseItem();
    }
    if (type == ModelType::Magazine)
    {
        return new MagazineItem();
    }
    return new Item();
}
