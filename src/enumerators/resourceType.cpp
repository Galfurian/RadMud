/// @file   resourceType.cpp
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

#include "resourceType.hpp"

ResourceType::ResourceType() :
    resourceType(None)
{
    // Nothing to do.
}

ResourceType::ResourceType(const unsigned int & _resourceType) :
    resourceType(None)
{
    if (_resourceType == 1) resourceType = Coal;
    else if (_resourceType == 2) resourceType = Ore;
    else if (_resourceType == 3) resourceType = Bar;
    else if (_resourceType == 4) resourceType = Log;
    else if (_resourceType == 5) resourceType = Plank;
    else if (_resourceType == 6) resourceType = Tree;
    else if (_resourceType == 7) resourceType = Fastener;
    else if (_resourceType == 8) resourceType = Leather;
    else if (_resourceType == 9) resourceType = Cloth;
    else if (_resourceType == 10) resourceType = StoneBlock;
    else if (_resourceType == 11) resourceType = MetalVein;
    else if (_resourceType == 12) resourceType = StoneMonolith;
    else if (_resourceType == 13) resourceType = Pen;
    else if (_resourceType == 14) resourceType = Trash;
    else resourceType = None;
}

ResourceType::ResourceType(const Enum & _resourceType) :
    resourceType(_resourceType)
{
    // Nothing to do.
}

ResourceType::ResourceType(const std::string & _resourceType) :
    resourceType()
{
    if (_resourceType == "Coal") resourceType = Coal;
    else if (_resourceType == "Ore") resourceType = Ore;
    else if (_resourceType == "Bar") resourceType = Bar;
    else if (_resourceType == "Log") resourceType = Log;
    else if (_resourceType == "Plank") resourceType = Plank;
    else if (_resourceType == "Tree") resourceType = Tree;
    else if (_resourceType == "Fastener") resourceType = Fastener;
    else if (_resourceType == "Leather") resourceType = Leather;
    else if (_resourceType == "Cloth") resourceType = Cloth;
    else if (_resourceType == "StoneBlock") resourceType = StoneBlock;
    else if (_resourceType == "MetalVein") resourceType = MetalVein;
    else if (_resourceType == "StoneMonolith") resourceType = StoneMonolith;
    else if (_resourceType == "Pen") resourceType = Pen;
    else if (_resourceType == "Trash") resourceType = Trash;
    else resourceType = None;
}

bool ResourceType::isValid(const unsigned int & _resourceType)
{
    return (_resourceType >= 1) && (_resourceType <= 14);
}

bool ResourceType::isValid(const std::string & _resourceType)
{
    if (_resourceType == "Coal") return true;
    if (_resourceType == "Ore") return true;
    if (_resourceType == "Bar") return true;
    if (_resourceType == "Log") return true;
    if (_resourceType == "Plank") return true;
    if (_resourceType == "Tree") return true;
    if (_resourceType == "Fastener") return true;
    if (_resourceType == "Leather") return true;
    if (_resourceType == "Cloth") return true;
    if (_resourceType == "StoneBlock") return true;
    if (_resourceType == "MetalVein") return true;
    if (_resourceType == "StoneMonolith") return true;
    if (_resourceType == "Pen") return true;
    return (_resourceType == "Trash");
}

std::string ResourceType::toString() const
{
    if (resourceType == ResourceType::Coal) return "Natural Coal";
    if (resourceType == ResourceType::Ore) return "Metal Ore";
    if (resourceType == ResourceType::Bar) return "Metal Bar";
    if (resourceType == ResourceType::Log) return "Wood Log";
    if (resourceType == ResourceType::Plank) return "Wood Plank";
    if (resourceType == ResourceType::Tree) return "Tree";
    if (resourceType == ResourceType::Fastener) return "Fastener";
    if (resourceType == ResourceType::Leather) return "Leather";
    if (resourceType == ResourceType::Cloth) return "Cloth";
    if (resourceType == ResourceType::StoneBlock) return "Stone Block";
    if (resourceType == ResourceType::MetalVein) return "Metal Vein";
    if (resourceType == ResourceType::StoneMonolith) return "Stone Monolith";
    if (resourceType == ResourceType::Pen) return "Pen";
    if (resourceType == ResourceType::Trash) return "Trash";
    return "None";
}

unsigned int ResourceType::toUInt() const
{
    return static_cast<unsigned int>(resourceType);
}

bool ResourceType::operator==(const ResourceType & rhs) const
{
    return resourceType == rhs.resourceType;
}

bool ResourceType::operator==(const ResourceType::Enum & rhs) const
{
    return resourceType == rhs;
}

bool ResourceType::operator!=(const ResourceType & rhs) const
{
    return resourceType != rhs.resourceType;
}

bool ResourceType::operator!=(const ResourceType::Enum & rhs) const
{
    return resourceType != rhs;
}

bool ResourceType::operator<(const ResourceType & rhs) const
{
    return resourceType < rhs.resourceType;
}

bool ResourceType::operator<(const ResourceType::Enum & rhs) const
{
    return resourceType < rhs;
}
