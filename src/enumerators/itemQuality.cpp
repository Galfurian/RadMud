/// @file   itemQuality.cpp
/// @brief  Implements the item quality class methods.
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

#include "itemQuality.hpp"

ItemQuality::ItemQuality(unsigned int & _quality) :
    quality()
{
    if (_quality == 0) quality = Disastrous;
    else if (_quality == 1) quality = Poor;
    else if (_quality == 2) quality = Normal;
    else if (_quality == 3) quality = Fine;
    else if (_quality == 4) quality = Masterful;
    else quality = Normal;
}

ItemQuality::ItemQuality(const Enum & _quality) :
    quality(_quality)
{
    // Nothing to do.
}

bool ItemQuality::isValid(const unsigned int & _quality)
{
    return (_quality <= 4);
}

std::string ItemQuality::toString() const
{
    if (quality == ItemQuality::Disastrous) return "Disastrous";
    if (quality == ItemQuality::Poor) return "Poor";
    if (quality == ItemQuality::Normal) return "Normal";
    if (quality == ItemQuality::Fine) return "Fine";
    else return "Masterful";
}

unsigned int ItemQuality::toUInt() const
{
    return static_cast<unsigned int>(quality);
}

double ItemQuality::getModifier() const
{
    if (quality == ItemQuality::Disastrous) return 0.5;
    if (quality == ItemQuality::Poor) return 0.75;
    if (quality == ItemQuality::Normal) return 1.0;
    if (quality == ItemQuality::Fine) return 1.50;
    return 2.00;
}

bool ItemQuality::operator==(const ItemQuality::Enum & rhs) const
{
    return quality == rhs;
}

bool ItemQuality::operator!=(const ItemQuality::Enum & rhs) const
{
    return quality != rhs;
}
