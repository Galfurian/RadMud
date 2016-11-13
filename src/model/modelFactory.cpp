/// @file   modelFactory.cpp
/// @brief  Implements the factory for item model.
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
