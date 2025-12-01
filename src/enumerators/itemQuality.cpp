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

#include "enumerators/itemQuality.hpp"

std::string item_quality_to_string(ItemQuality quality)
{
    switch (quality) {
    case ItemQuality::Disastrous:
        return "Disastrous";
    case ItemQuality::Poor:
        return "Poor";
    case ItemQuality::Normal:
        return "Normal";
    case ItemQuality::Fine:
        return "Fine";
    case ItemQuality::Masterful:
        return "Masterful";
    case ItemQuality::None:
    default:
        return "None";
    }
}

ItemQuality item_quality_from_string(const std::string &value)
{
    if (value == "Disastrous")
        return ItemQuality::Disastrous;
    else if (value == "Poor")
        return ItemQuality::Poor;
    else if (value == "Normal")
        return ItemQuality::Normal;
    else if (value == "Fine")
        return ItemQuality::Fine;
    else if (value == "Masterful")
        return ItemQuality::Masterful;
    else
        return ItemQuality::None;
}

double get_item_quality_modifier(ItemQuality quality)
{
    switch (quality) {
    case ItemQuality::Disastrous:
        return 0.5;
    case ItemQuality::Poor:
        return 0.75;
    case ItemQuality::Normal:
        return 1.0;
    case ItemQuality::Fine:
        return 1.50;
    case ItemQuality::Masterful:
        return 2.00;
    case ItemQuality::None:
    default:
        return 0.25;
    }
}
