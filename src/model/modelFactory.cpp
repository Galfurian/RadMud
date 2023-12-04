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

#include "model/modelFactory.hpp"

#include "model/submodel/corpseModel.hpp"
#include "model/submodel/resourceModel.hpp"
#include "model/submodel/toolModel.hpp"
#include "model/submodel/nodeModel.hpp"
#include "model/submodel/armorModel.hpp"
#include "model/submodel/bookModel.hpp"
#include "model/submodel/containerModel.hpp"
#include "model/submodel/currencyModel.hpp"
#include "model/submodel/foodModel.hpp"
#include "model/submodel/furnitureModel.hpp"
#include "model/submodel/keyModel.hpp"
#include "model/submodel/lightModel.hpp"
#include "model/submodel/liquidContainerModel.hpp"
#include "model/submodel/mechanismModel.hpp"
#include "model/submodel/projectileModel.hpp"
#include "model/submodel/ropeModel.hpp"
#include "model/submodel/seedModel.hpp"
#include "model/submodel/shieldModel.hpp"
#include "model/submodel/shopModel.hpp"
#include "model/submodel/vehicleModel.hpp"
#include "model/submodel/magazineModel.hpp"
#include "model/submodel/meleeWeaponModel.hpp"

std::shared_ptr<ItemModel> ModelFactory::newModel(const ModelType & type)
{
    if (type == ModelType::Corpse)
    {
        return std::make_shared<CorpseModel>();
    }
    if (type == ModelType::MeleeWeapon)
    {
        return std::make_shared<MeleeWeaponModel>();
    }
    if (type == ModelType::RangedWeapon)
    {
        return std::make_shared<RangedWeaponModel>();
    }
    if (type == ModelType::Armor)
    {
        return std::make_shared<ArmorModel>();
    }
    if (type == ModelType::Book)
    {
        return std::make_shared<BookModel>();
    }
    if (type == ModelType::Container)
    {
        return std::make_shared<ContainerModel>();
    }
    if (type == ModelType::Currency)
    {
        return std::make_shared<CurrencyModel>();
    }
    if (type == ModelType::Food)
    {
        return std::make_shared<FoodModel>();
    }
    if (type == ModelType::Furniture)
    {
        return std::make_shared<FurnitureModel>();
    }
    if (type == ModelType::Key)
    {
        return std::make_shared<KeyModel>();
    }
    if (type == ModelType::Light)
    {
        return std::make_shared<LightModel>();
    }
    if (type == ModelType::LiquidContainer)
    {
        return std::make_shared<LiquidContainerModel>();
    }
    if (type == ModelType::Mechanism)
    {
        return std::make_shared<MechanismModel>();
    }
    if (type == ModelType::Node)
    {
        return std::make_shared<NodeModel>();
    }
    if (type == ModelType::Projectile)
    {
        return std::make_shared<ProjectileModel>();
    }
    if (type == ModelType::Resource)
    {
        return std::make_shared<ResourceModel>();
    }
    if (type == ModelType::Rope)
    {
        return std::make_shared<RopeModel>();
    }
    if (type == ModelType::Seed)
    {
        return std::make_shared<SeedModel>();
    }
    if (type == ModelType::Shield)
    {
        return std::make_shared<ShieldModel>();
    }
    if (type == ModelType::Shop)
    {
        return std::make_shared<ShopModel>();
    }
    if (type == ModelType::Tool)
    {
        return std::make_shared<ToolModel>();
    }
    if (type == ModelType::Vehicle)
    {
        return std::make_shared<VehicleModel>();
    }
    if (type == ModelType::Magazine)
    {
        return std::make_shared<MagazineModel>();
    }
    return nullptr;
}
