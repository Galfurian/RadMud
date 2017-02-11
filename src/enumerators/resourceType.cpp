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
#include "luaBridge.hpp"
#include "utils.hpp"

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
    else if (_resourceType == 15) resourceType = Meat;
    else if (_resourceType == 16) resourceType = Bone;
    else if (_resourceType == 17) resourceType = Skull;
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
    if (ToLower(_resourceType) == "coal") resourceType = Coal;
    else if (ToLower(_resourceType) == "ore") resourceType = Ore;
    else if (ToLower(_resourceType) == "bar") resourceType = Bar;
    else if (ToLower(_resourceType) == "log") resourceType = Log;
    else if (ToLower(_resourceType) == "plank") resourceType = Plank;
    else if (ToLower(_resourceType) == "tree") resourceType = Tree;
    else if (ToLower(_resourceType) == "fastener") resourceType = Fastener;
    else if (ToLower(_resourceType) == "leather") resourceType = Leather;
    else if (ToLower(_resourceType) == "cloth") resourceType = Cloth;
    else if (ToLower(_resourceType) == "stoneblock") resourceType = StoneBlock;
    else if (ToLower(_resourceType) == "metalvein") resourceType = MetalVein;
    else if (ToLower(_resourceType) == "stonemonolith")
        resourceType = StoneMonolith;
    else if (ToLower(_resourceType) == "pen") resourceType = Pen;
    else if (ToLower(_resourceType) == "trash") resourceType = Trash;
    else if (ToLower(_resourceType) == "meat") resourceType = Meat;
    else if (ToLower(_resourceType) == "bone") resourceType = Bone;
    else if (ToLower(_resourceType) == "skull") resourceType = Skull;
    else resourceType = None;
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
    if (resourceType == ResourceType::Meat) return "Meat";
    if (resourceType == ResourceType::Bone) return "Bone";
    if (resourceType == ResourceType::Skull) return "Skull";
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

bool ResourceType::operator!=(const ResourceType & rhs) const
{
    return resourceType != rhs.resourceType;
}

bool ResourceType::operator<(const ResourceType & rhs) const
{
    return resourceType < rhs.resourceType;
}

void ResourceType::luaRegister(lua_State * L)
{
    luabridge::getGlobalNamespace(L)
        .beginClass<ResourceType>("ResourceType")
        .addFunction("toUInt", &ResourceType::toUInt)
        .addFunction("toString", &ResourceType::toString)
        .endClass()
        .beginEnum<ResourceType>("ResourceType")
        .addEnum("None", None)
        .addEnum("Coal", Coal)
        .addEnum("Ore", Ore)
        .addEnum("Bar", Bar)
        .addEnum("Log", Log)
        .addEnum("Plank", Plank)
        .addEnum("Tree", Tree)
        .addEnum("Fastener", Fastener)
        .addEnum("Leather", Leather)
        .addEnum("Cloth", Cloth)
        .addEnum("StoneBlock", StoneBlock)
        .addEnum("MetalVein", MetalVein)
        .addEnum("StoneMonolith", StoneMonolith)
        .addEnum("Pen", Pen)
        .addEnum("Trash", Trash)
        .addEnum("Meat", Meat)
        .addEnum("Bone", Bone)
        .addEnum("Skull", Skull)
        .endEnum();
}