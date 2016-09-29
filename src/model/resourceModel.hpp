/// @file   resourceModel.hpp
/// @brief  Define variables and methods of Resource.
/// @author Enrico Fraccaroli
/// @date   Jul 6 2016
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

#pragma once

#include "itemModel.hpp"

/// Used to determine the type of the resource.
typedef enum class ResourceType_t
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
} ResourceType;

/// @brief Model of resources.
class ResourceModel :
    public ItemModel
{
public:
    /// Type of resource.
    ResourceType resourceType;

    ResourceModel();

    virtual ~ResourceModel();

    virtual ModelType getType() const;

    virtual std::string getTypeName() const;

    virtual bool setModel(const std::string & source);

    virtual void getSheet(Table & sheet) const;
};

/// @addtogroup EnumToString
/// @{

/// Return the string describing the type of a Resource.
std::string GetResourceTypeName(ResourceType type);

///@}
