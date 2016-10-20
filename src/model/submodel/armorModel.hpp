/// @file   armorModel.hpp
/// @brief  Define variables and methods of Armor.
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

/// Sizes of armor.
using ArmorSize = enum class ArmorSize_t
{
    All,    ///< [0] All kind of size.
    Light,  ///< [1] Light Armor
    Medium, ///< [2] Medium Armor
    Heavy   ///< [3] Heavy Armor
};

/// @brief Model of an armor.
class ArmorModel :
    public ItemModel
{
public:
    /// The size of the armor.
    ArmorSize size;
    /// The Armor Class (AC).
    unsigned int armorClass;
    /// The anatomy which a character must have in order to wear this armor.
    unsigned int allowedAnatomy;

    ArmorModel();

    virtual ~ArmorModel();

    ModelType getType() const override;

    std::string getTypeName() const override;

    bool setModel(const std::string & source) override;

    void getSheet(Table & sheet) const override;
};

/// @addtogroup EnumToString
/// @{

/// Return the string describing the size of an Armor.
std::string GetArmorSizeName(ArmorSize armorSize);

///@}