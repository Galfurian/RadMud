/// @file   modelFactory.cpp
/// @brief  Implements the factory for item model.
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

#include "modelFactory.hpp"

#include "corpseModel.hpp"
#include "resourceModel.hpp"
#include "toolModel.hpp"
#include "nodeModel.hpp"
#include "armorModel.hpp"
#include "bookModel.hpp"
#include "containerModel.hpp"
#include "currencyModel.hpp"
#include "foodModel.hpp"
#include "furnitureModel.hpp"
#include "keyModel.hpp"
#include "lightModel.hpp"
#include "liquidContainerModel.hpp"
#include "mechanismModel.hpp"
#include "projectileModel.hpp"
#include "ropeModel.hpp"
#include "seedModel.hpp"
#include "shieldModel.hpp"
#include "shopModel.hpp"
#include "vehicleModel.hpp"
#include "magazineModel.hpp"
#include "meleeWeaponModel.hpp"

ItemModel * ModelFactory::newModel(const ModelType & type)
{
    if (type == ModelType::Corpse)
    {
        return new CorpseModel();
    }
    if (type == ModelType::MeleeWeapon)
    {
        return new MeleeWeaponModel();
    }
    if (type == ModelType::RangedWeapon)
    {
        return new RangedWeaponModel();
    }
    if (type == ModelType::Armor)
    {
        return new ArmorModel();
    }
    if (type == ModelType::Book)
    {
        return new BookModel();
    }
    if (type == ModelType::Container)
    {
        return new ContainerModel();
    }
    if (type == ModelType::Currency)
    {
        return new CurrencyModel();
    }
    if (type == ModelType::Food)
    {
        return new FoodModel();
    }
    if (type == ModelType::Furniture)
    {
        return new FurnitureModel();
    }
    if (type == ModelType::Key)
    {
        return new KeyModel();
    }
    if (type == ModelType::Light)
    {
        return new LightModel();
    }
    if (type == ModelType::LiquidContainer)
    {
        return new LiquidContainerModel();
    }
    if (type == ModelType::Mechanism)
    {
        return new MechanismModel();
    }
    if (type == ModelType::Node)
    {
        return new NodeModel();
    }
    if (type == ModelType::Projectile)
    {
        return new ProjectileModel();
    }
    if (type == ModelType::Resource)
    {
        return new ResourceModel();
    }
    if (type == ModelType::Rope)
    {
        return new RopeModel();
    }
    if (type == ModelType::Seed)
    {
        return new SeedModel();
    }
    if (type == ModelType::Shield)
    {
        return new ShieldModel();
    }
    if (type == ModelType::Shop)
    {
        return new ShopModel();
    }
    if (type == ModelType::Tool)
    {
        return new ToolModel();
    }
    if (type == ModelType::Vehicle)
    {
        return new VehicleModel();
    }
    if (type == ModelType::Magazine)
    {
        return new MagazineModel();
    }
    return nullptr;
}
