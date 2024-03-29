/// @file   corpseItem.hpp
/// @author Enrico Fraccaroli
/// @date   Aug 17 2016
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

#include "item/item.hpp"

/// @brief Holds details about a corpse.
class CorpseItem :
    public Item
{
public:
    /// Store the remaining body parts not yet processed.
    std::vector<std::shared_ptr<BodyPart>> remainingBodyParts;

    /// @brief Constructor.
    CorpseItem();

    virtual ~CorpseItem();

    void removeFromMud() override;

    bool updateOnDB() override;

    bool removeOnDB() override;

    void getSheet(Table & sheet) const override;

    std::string lookContent() override;

    bool isAContainer() const override;

    /// @brief Provides the remaining body parts.
    std::shared_ptr<BodyPart> getAvailableBodyPart();

    /// @brief Removes the given body part from the pool.
    bool removeBodyPart(const std::shared_ptr<BodyPart> & bodyPart);
};
