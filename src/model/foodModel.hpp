/// @file   foodModel.hpp
/// @brief  Define variables and methods of Food.
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

/// Used to determine the flag of the food.
using FoodFlag = enum class FoodFlag_t
{
    None = 0,       ///< No flag.
    Poisoned = 1,   ///< Food is poisoned.
    Raw = 2,        ///< Food is not cocked.
};

/// @brief Model of food.
class FoodModel :
    public ItemModel
{
public:
    /// Hours of feeding.
    unsigned int hours;
    /// The food flags.
    unsigned int foodFlags;

    FoodModel();

    virtual ~FoodModel();

    ModelType getType() const override;

    std::string getTypeName() const override;

    bool setModel(const std::string & source) override;

    void getSheet(Table & sheet) const override;
};

/// @addtogroup FlagsToList
/// @{

/// Return a list of string containg the Food flags contained inside the value.
std::string GetFoodFlagString(unsigned int flags);

/// @}
