/// @file   containerModel.hpp
/// @brief  Define variables and methods of Container.
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

#include "model/itemModel.hpp"

/// Used to determine the flag of the container.
using ContainerFlag = enum class ContainerFlag_t
{
    None = 0,       ///< No flag.
    CanClose = 1,   ///< Can be closed.
    CanBurgle = 2,  ///< Can be burgled.
    CanSee = 4,     ///< Can see inside even if closed.
};

/// @brief Model of a container of objects.
class ContainerModel :
    public ItemModel
{
public:
    /// The maximum weight which this container can carry.
    double maxWeight;
    /// The flags of the container.
    unsigned int containerFlags;
    /// The vnum of the key which opens this container.
    unsigned int keyVnum;
    /// The lockpicking difficulty.
    unsigned int difficulty;

    ContainerModel();

    virtual ~ContainerModel();

    ModelType getType() const override;

    std::string getTypeName() const override;

    bool setModel(const std::string & source) override;

    void getSheet(Table & sheet) const override;
};

/// @addtogroup FlagsToList
/// @{

/// @brief Return a list of string contain the Container flags contained
/// inside the value.
std::string GetContainerFlagString(unsigned int flags);

/// @}
