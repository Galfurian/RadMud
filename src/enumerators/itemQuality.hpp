/// @file   itemQuality.hpp
/// @brief  Define the item quality class.
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

/// The quality of an item.
class ItemQuality :
    public BaseEnumerator
{
public:
    /// List of possible quality values.
    enum Enum
    {
        None,       ///< No quality.
        Disastrous, ///< Disastrous quality.
        Poor,       ///< Poor quality.
        Normal,     ///< Normal quality.
        Fine,       ///< Fine quality.
        Masterful   ///< Masterful quality.
    };

    /// @brief Constructor.
    ItemQuality() :
        BaseEnumerator()
    {
        // Nothing to do.
    }

    /// @brief Constructor from unsigned int.
    ItemQuality(const unsigned int & _value) :
        BaseEnumerator(_value)
    {
        // Nothing to do.
    }

    /// @brief Constructor from enum.
    ItemQuality(const Enum & _value) :
        BaseEnumerator(_value)
    {
        // Nothing to do.
    }

    /// @brief Returns the quality as string.
    std::string toString() const override;

    /// @brief Returns the quality modifier.
    double getModifier() const;
};
