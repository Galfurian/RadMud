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
#include "liquidContainerItem.hpp"

Item *ItemFactory::newItem(const ModelType &type)
{
	// [0] No type.
	if (type == ModelType::None) {
		return nullptr;
	}

	// [1] A corpse.
	if (type == ModelType::Corpse) {
		return new CorpseItem();
	}

	// [2] Any melee weapon.
	if (type == ModelType::MeleeWeapon) {
		return new MeleeWeaponItem();
	}

	// [3] Any ranged weapon.
	if (type == ModelType::RangedWeapon) {
		return new RangedWeaponItem();
	}

	// [4] Any armor.
	if (type == ModelType::Armor) {
		return new ArmorItem();
	}

	// [5] A shield.

	// [6] Projectiles.

	// [7] A container for items.
	if (type == ModelType::Container) {
		return new ContainerItem();
	}

	// [8] A container for liquids.
	if (type == ModelType::LiquidContainer) {
		return new LiquidContainerItem();
	}

	// [9] Any tool.

	// [10] Any node of resources.

	// [11] A resource.
	if (type == ModelType::Resource) {
		return new ResourceItem();
	}

	// [12] Any kind of seed.

	// [13] A key.

	// [14] A furniture (eg. chair, bed, painting, table and so on).

	// [15] A food.

	// [16] A source of light ((eg. torch, lamp, candle and so on).
	if (type == ModelType::Light) {
		return new LightItem();
	}

	// [17] Any kind of vehicle.

	// [18] Container of parchments.

	// [19] A generic rope.

	// [20] Any kind of mechanism.

	// [21] Any kind of currency.
	if (type == ModelType::Currency) {
		return new CurrencyItem();
	}

	// [22] A shop.
	if (type == ModelType::Shop) {
		return new ShopItem();
	}

	// [23] A magazine for ammunitions.
	if (type == ModelType::Magazine) {
		return new MagazineItem();
	}
	return new Item();
}
