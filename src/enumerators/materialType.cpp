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


std::string material_type_to_string(MaterialType type)
{
    switch (type) {
    case MaterialType::Metal: return "Metal";
    case MaterialType::Stone: return "Stone";
    case MaterialType::Wood: return "Wood";
    case MaterialType::Skin: return "Skin";
    case MaterialType::Cloth: return "Cloth";
    case MaterialType::Vegetable: return "Vegetable";
    case MaterialType::Meat: return "Meat";
    case MaterialType::Glass: return "Glass";
    case MaterialType::Paper: return "Paper";
    case MaterialType::Coal: return "Coal";
    case MaterialType::Bone: return "Bone";
    case MaterialType::None:
    default:
        return "None";
    }
}

MaterialType string_to_material_type(const std::string & value)
{
    if (value == "Metal") return MaterialType::Metal;
    else if (value == "Stone") return MaterialType::Stone;
    else if (value == "Wood") return MaterialType::Wood;
    else if (value == "Skin") return MaterialType::Skin;
    else if (value == "Cloth") return MaterialType::Cloth;
    else if (value == "Vegetable") return MaterialType::Vegetable;
    else if (value == "Meat") return MaterialType::Meat;
    else if (value == "Glass") return MaterialType::Glass;
    else if (value == "Paper") return MaterialType::Paper;
    else if (value == "Coal") return MaterialType::Coal;
    else if (value == "Bone") return MaterialType::Bone;
    else return MaterialType::None;
}
