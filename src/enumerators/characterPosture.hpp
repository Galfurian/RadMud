/// @file   characterPosture.hpp
/// @brief  Define the postures of a character.
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

#include "baseEnumerator.hpp"

/// The postures of a character.
class CharacterPosture :
    public BaseEnumerator
{
public:
    /// List of possible character's posture.
    enum Enum
    {
        None,   ///< The character has no posture.
        Stand,  ///< The character it's standing.
        Crouch, ///< The character it's crouched.
        Prone,  ///< The character it's prone.
        Sit,    ///< The character it's sitting.
        Rest,   ///< The character it's lying down.
        Sleep,  ///< The character is sleeping.
    };

    /// @brief Constructor.
    CharacterPosture() :
        BaseEnumerator()
    {
        // Nothing to do.
    }

    /// @brief Constructor from unsigned int.
    CharacterPosture(const unsigned int & _value) :
        BaseEnumerator(_value)
    {
        // Nothing to do.
    }

    /// @brief Constructor from enum.
    CharacterPosture(const Enum & _value) :
        BaseEnumerator(_value)
    {
        // Nothing to do.
    }

    /// @brief Returns the character posture as string.
    std::string toString() const override;

    /// @brief Returns the action describing the posture.
    std::string getAction() const;

    uint32_t getSpeed() const;

    uint32_t getRegainModifier() const;
};
