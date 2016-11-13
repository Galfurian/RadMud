/// @file   resourceModel.hpp
/// @brief  Define variables and methods of Resource.
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

#include "itemModel.hpp"

/// Used to determine the type of the resource.
using ResourceType = enum class ResourceType_t
{
    NoType,
    Coal,           ///< [1] COAL
    Ore,            ///< [2] ORE
    Bar,            ///< [3] BAR
    Log,            ///< [4] LOG
    Plank,          ///< [5] PLANK
    Tree,           ///< [6] TREE
    Fastener,       ///< [7] FASTENER
    Leather,        ///< [8] LEATHER
    Cloth,          ///< [9] CLOTH
    StoneBlock,     ///< [10] STONE_BLOCK
    MetalVein,      ///< [11] METAL_VEIN
    StoneMonolith,  ///< [12] STONE_MONOLITH
    Pen,            ///< [13] Pen
    Trash           ///< [14] Trash (Anything which has no specific use)
};

/// @brief Model of resources.
class ResourceModel :
    public ItemModel
{
public:
    /// Type of resource.
    ResourceType resourceType;

    ResourceModel();

    virtual ~ResourceModel();

    ModelType getType() const override;

    std::string getTypeName() const override;

    bool setModel(const std::string & source) override;

    void getSheet(Table & sheet) const override;
};

/// @addtogroup EnumToString
/// @{

/// Return the string describing the type of a Resource.
std::string GetResourceTypeName(ResourceType type);

///@}
