/// @file   materialType.cpp
/// @brief  
/// @author Enrico Fraccaroli
/// @date   05/12/2016
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

#include "enumerators/materialType.hpp"

std::string MaterialType::toString() const
{
    if (value == MaterialType::Metal) return "Metal";
    if (value == MaterialType::Stone) return "Stone";
    if (value == MaterialType::Wood) return "Wood";
    if (value == MaterialType::Skin) return "Skin";
    if (value == MaterialType::Cloth) return "Cloth";
    if (value == MaterialType::Vegetable) return "Vegetable";
    if (value == MaterialType::Meat) return "Meat";
    if (value == MaterialType::Glass) return "Glass";
    if (value == MaterialType::Paper) return "Paper";
    if (value == MaterialType::Coal) return "Coal";
    if (value == MaterialType::Bone) return "Bone";
    return "None";
}
