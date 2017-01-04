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

#include "materialType.hpp"

MaterialType::MaterialType() :
    materialType(None)
{
    // Nothing to do.
}

MaterialType::MaterialType(const unsigned int & _materialType) :
    materialType(None)
{
    if (_materialType == 1) materialType = Metal;
    else if (_materialType == 2) materialType = Stone;
    else if (_materialType == 3) materialType = Wood;
    else if (_materialType == 4) materialType = Skin;
    else if (_materialType == 5) materialType = Cloth;
    else if (_materialType == 6) materialType = Vegetable;
    else if (_materialType == 7) materialType = Meat;
    else if (_materialType == 8) materialType = Glass;
    else if (_materialType == 9) materialType = Paper;
    else if (_materialType == 10) materialType = Coal;
    else materialType = None;
}

MaterialType::MaterialType(const Enum & _materialType) :
    materialType(_materialType)
{
    // Nothing to do.
}

MaterialType::MaterialType(const std::string & _materialType) :
    materialType()
{
    if (_materialType == "Metal") materialType = Metal;
    else if (_materialType == "Stone") materialType = Stone;
    else if (_materialType == "Wood") materialType = Wood;
    else if (_materialType == "Skin") materialType = Skin;
    else if (_materialType == "Cloth") materialType = Cloth;
    else if (_materialType == "Vegetable") materialType = Vegetable;
    else if (_materialType == "Meat") materialType = Meat;
    else if (_materialType == "Glass") materialType = Glass;
    else if (_materialType == "Paper") materialType = Paper;
    else if (_materialType == "Coal") materialType = Coal;
    else materialType = None;
}

bool MaterialType::isValid(const unsigned int & _materialType)
{
    return (_materialType >= 1) && (_materialType <= 10);
}

bool MaterialType::isValid(const std::string & _materialType)
{
    if (_materialType == "Metal") return true;
    if (_materialType == "Stone") return true;
    if (_materialType == "Wood") return true;
    if (_materialType == "Skin") return true;
    if (_materialType == "Cloth") return true;
    if (_materialType == "Vegetable") return true;
    if (_materialType == "Meat") return true;
    if (_materialType == "Glass") return true;
    if (_materialType == "Paper") return true;
    return (_materialType == "Coal");
}

std::string MaterialType::toString() const
{
    if (materialType == MaterialType::Metal) return "Metal";
    if (materialType == MaterialType::Stone) return "Stone";
    if (materialType == MaterialType::Wood) return "Wood";
    if (materialType == MaterialType::Skin) return "Skin";
    if (materialType == MaterialType::Cloth) return "Cloth";
    if (materialType == MaterialType::Vegetable) return "Vegetable";
    if (materialType == MaterialType::Meat) return "Meat";
    if (materialType == MaterialType::Glass) return "Glass";
    if (materialType == MaterialType::Paper) return "Paper";
    if (materialType == MaterialType::Coal) return "Coal";
    return "None";
}

unsigned int MaterialType::toUInt() const
{
    return static_cast<unsigned int>(materialType);
}

bool MaterialType::operator==(const MaterialType & rhs) const
{
    return materialType == rhs.materialType;
}

bool MaterialType::operator==(const MaterialType::Enum & rhs) const
{
    return materialType == rhs;
}

bool MaterialType::operator!=(const MaterialType & rhs) const
{
    return materialType != rhs.materialType;
}

bool MaterialType::operator!=(const MaterialType::Enum & rhs) const
{
    return materialType != rhs;
}

bool MaterialType::operator<(const MaterialType & rhs) const
{
    return materialType < rhs.materialType;
}

bool MaterialType::operator<(const MaterialType::Enum & rhs) const
{
    return materialType < rhs;
}
