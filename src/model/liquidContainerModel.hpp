/// @file   liquidContainerModel.hpp
/// @brief  Define variables and methods of LiquidContainer.
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

/// Used to determine the flag of the container of liquid.
typedef enum class LiqContainerFlags
{
    /// No flag.
    None = 0,
    /// Liquid is poisoned.
    Poisoned = 1,
    /// Content never ends.
    Endless = 2,
    /// Container is destroyed once emptied.
    Destroy = 4,
} LiqContainerFlag;

/// @brief Model of a container of liquids.
class LiquidContainerModel: public ItemModel
{
    public:
        /// The maximum weight which this container can carry.
        unsigned int maxWeight;
        /// The flags of the container.
        unsigned int liquidFlags;

        LiquidContainerModel();

        virtual ~LiquidContainerModel();

        virtual ModelType getType() const;

        virtual std::string getTypeName() const;

        virtual bool setModel(const std::string & source);
};

/// @addtogroup FlagsToList
/// @{

/// Return a list of string containg the Container of Liquids flags contained inside the value.
std::string GetLiqContainerFlagString(unsigned int flags);

/// @}

