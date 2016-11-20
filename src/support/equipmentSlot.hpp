/// @file   equipmentSlot.hpp
/// @brief  Define the equipment slot class.
/// @author Enrico Fraccaroli
/// @date   Nov 19 2016
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

#include <string>

/// Provides a complete controll on equipment slots.
class EquipmentSlot
{
public:
    /// The possible equipment slots.
    enum Enum
    {
        None,
        Head = 1,
        Torso = 2,
        Back = 4,
        Legs = 8,
        Feet = 16,
        RightHand = 32,
        LeftHand = 64,
    };

    /// @brief Constructor from uint.
    EquipmentSlot();

    /// @brief Constructor from uint.
    EquipmentSlot(const unsigned int & _equipmentSlot);

    /// @brief Constructor from enum.
    EquipmentSlot(const Enum & _equipmentSlot);

    /// @brief Constructor from string.
    EquipmentSlot(const std::string & _equipmentSlot);

    /// @brief Check is the given number is a valid equipment slot.
    static bool isValid(const unsigned int & _equipmentSlot);

    /// @brief Check is the given string is a valid equipment slot.
    static bool isValid(const std::string & _equipmentSlot);

    /// @brief Returns the equipment slot as string.
    std::string toString() const;

    /// @brief Returns the equipment slot as number.
    unsigned int toUInt() const;

    /// @brief Equality operator w.r.t. a equipment slot object.
    bool operator==(const EquipmentSlot & rhs) const;

    /// @brief Equality operator w.r.t. a equipment slot enum.
    bool operator==(const EquipmentSlot::Enum & rhs) const;

    /// @brief Inequality operator w.r.t. a equipment slot enum.
    bool operator!=(const EquipmentSlot::Enum & rhs) const;

private:
    /// Internal equipment slot value.
    Enum equipmentSlot;
};
