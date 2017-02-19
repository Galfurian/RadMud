/// @file   shieldModel.hpp
/// @brief  Define variables and methods of Shield.
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

/// Shield sizes.
using ShieldSize = enum class ShieldSize_t
{
    All,    ///< [0] All kind of size.
    Light,  ///< [1] Light Shield
    Medium, ///< [2] Medium Shield
    Heavy   ///< [3] Heavy Shield
};

/// @brief Model of a shield.
class ShieldModel :
    public ItemModel
{
public:
    /// The size of the shield.
    ShieldSize size;
    /// The chace to parry with this weapon.
    unsigned int parryChance;

    ShieldModel();

    virtual ~ShieldModel();

    ModelType getType() const override;

    std::string getTypeName() const override;

    bool setModel(const std::string & source) override;

    void getSheet(Table & sheet) const override;
};

/// @brief Casts a pointer from ItemModel to ShieldModel.
inline std::shared_ptr<ShieldModel> ModelToShield(
    const std::shared_ptr<ItemModel> & itemModel)
{
    if (itemModel == nullptr) return nullptr;
    if (itemModel->getType() != ModelType::Shield) return nullptr;
    return std::static_pointer_cast<ShieldModel>(itemModel);
}
