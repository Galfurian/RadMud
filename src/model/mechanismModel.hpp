/// @file   mechanismModel.hpp
/// @brief  Define variables and methods of Mechanism.
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
typedef enum class MechanismTypes
{
    /// No flag
    None,
    /// [1] The mechanism is a door.
    Door,
    /// [2] The mechanism is a lock.
    Lock,
    /// [3] The mechanism is a picklock.
    Picklock,
    /// [4] The mechanism is a lever.
    Lever
} MechanismType;

/// @brief Model of a general mechanism.
class MechanismModel: public ItemModel
{
    public:
        /// Type of mechanism.
        MechanismType mechanismType;
        /// The vnum of the key which opens the door/lock.
        unsigned int key;
        /// Lockpicking difficulty.
        unsigned int difficulty;
        /// Lockpick efficency.
        unsigned int efficency;
        /// Lever command.
        unsigned int command;
        /// Vnum of the target of the lever.
        unsigned int target;

        MechanismModel();

        virtual ~MechanismModel();

        virtual ModelType getType() const;

        virtual std::string getTypeName() const;

        virtual bool setModel(const std::string & source);

        virtual void getSheet(Table & sheet) const;
};

/// @addtogroup EnumToString
/// @{

/// Return the string describing the type of a Mechanism.
std::string GetMechanismTypeName(MechanismType type);

///@}

