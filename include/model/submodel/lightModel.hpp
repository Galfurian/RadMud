/// @file   lightModel.hpp
/// @brief  Define variables and methods of Light.
/// @author Enrico Fraccaroli
/// @date   Jul 6 2016
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

#include "enumerators/resourceType.hpp"
#include "model/itemModel.hpp"

/// Flags of a light source.
using LightModelFlags = enum class LightModelFlags_t :
    unsigned int
{
    None,             ///< [0]   No flag.
    AlwaysActive = 1, ///< [1]   The light source is always active.
    CanUseToCook = 2, ///< [2]   The light source can be used to cook.
    NeedToKindle = 4  ///< [4]
    ///< [8]
    ///< [16]
    ///< [32]
    ///< [64]
    ///< [128]
};

/// @brief Model of a light source.
class LightModel :
    public ItemModel
{
public:
    /// The type of fuel used to keep the light on.
    ResourceType fuelType;
    /// The maximum radius of the light.
    int radius;
    /// Maximum contained weight.
    double maxWeight;
    /// The flags of the light source.
    unsigned int lightSourceFlags;

    LightModel();

    virtual ~LightModel();

    ModelType getType() const override;

    std::string getTypeName() const override;

    bool setModel(const std::string & source) override;

    void getSheet(Table & sheet) const override;
};
