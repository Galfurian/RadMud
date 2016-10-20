/// @file   containerModel.hpp
/// @brief  Define variables and methods of Container.
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

/// Return a list of string containg the Container flags contained inside the value.
std::string GetContainerFlagString(unsigned int flags);

/// @}