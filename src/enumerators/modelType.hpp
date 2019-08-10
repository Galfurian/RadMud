/// @file   modelType.hpp
/// @brief  Define a class which provides all the type of models.
/// @author Enrico Fraccaroli
/// @date   Nov 21 2016
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

#include "baseEnumerator.hpp"

/// @brief Used to determine the type of the model.
class ModelType :
    public BaseEnumerator
{
public:
    /// The possible types of model.
    enum Enum
    {
        None,            ///< [0] No type.
        Corpse,          ///< [1] A corpse.
        MeleeWeapon,     ///< [2] Any melee weapon.
        RangedWeapon,    ///< [3] Any ranged weapon.
        Armor,           ///< [4] Any armor.
        Shield,          ///< [5] A shield.
        Projectile,      ///< [6] Projectiles.
        Container,       ///< [7] A container for items.
        LiquidContainer, ///< [8] A container for liquids.
        Tool,            ///< [9] Any tool.
        Node,            ///< [10] Any node of resources.
        Resource,        ///< [11] A resource.
        Seed,            ///< [12] Any kind of seed.
        Key,             ///< [13] A key.
        Furniture,       ///< [14] A furniture (eg. chair, bed, painting, table and so on).
        Food,            ///< [15] A food.
        Light,           ///< [16] A source of light ((eg. torch, lamp, candle and so on).
        Vehicle,         ///< [17] Any kind of vehicle.
        Book,            ///< [18] Container of parchments.
        Rope,            ///< [19] A generic rope.
        Mechanism,       ///< [20] Any kind of mechanism.
        Currency,        ///< [21] Any kind of currency.
        Shop,            ///< [22] A shop.
        Magazine         ///< [23] A magazine for ammunitions.
    };

    /// @brief Constructor.
    ModelType() :
        BaseEnumerator()
    {
        // Nothing to do.
    }

    /// @brief Constructor from unsigned int.
    explicit ModelType(const unsigned int & _value) :
        BaseEnumerator(_value)
    {
        // Nothing to do.
    }

    /// @brief Constructor from enum.
    ModelType(const Enum & _value) :
        BaseEnumerator(_value)
    {
        // Nothing to do.
    }

    /// @brief Returns the type of model as string.
    std::string toString() const override;
};
