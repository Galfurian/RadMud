/// @file   seedModel.hpp
/// @brief  Define variables and methods of Seed.
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
using SeedType = enum class SeedType_t
{
    Plant,  ///< A generic seed.
    Tree,   ///< A seed used to plant a tree.
};

/// @brief Model of a seed.
class SeedModel :
    public ItemModel
{
public:
    /// Type of seed.
    SeedType seedType;

    SeedModel();

    virtual ~SeedModel();

    virtual ModelType getType() const;

    virtual std::string getTypeName() const;

    virtual bool setModel(const std::string & source);

    virtual void getSheet(Table & sheet) const;
};

/// @addtogroup EnumToString
/// @{

/// Return the string describing the type of a Seed.
std::string GetSeedTypeName(SeedType type);

///@}

