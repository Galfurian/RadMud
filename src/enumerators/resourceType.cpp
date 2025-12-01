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

#include "enumerators/resourceType.hpp"

#include <string>
#include <algorithm>

std::string resource_type_to_string(ResourceType type)
{
    switch (type) {
        case ResourceType::Coal:          return "Coal";
        case ResourceType::Ore:           return "Ore";
        case ResourceType::Bar:           return "Bar";
        case ResourceType::Log:           return "Log";
        case ResourceType::Plank:         return "Plank";
        case ResourceType::Tree:          return "Tree";
        case ResourceType::Fastener:      return "Fastener";
        case ResourceType::Leather:       return "Leather";
        case ResourceType::Cloth:         return "Cloth";
        case ResourceType::StoneBlock:    return "StoneBlock";
        case ResourceType::MetalVein:     return "MetalVein";
        case ResourceType::StoneMonolith: return "StoneMonolith";
        case ResourceType::Pen:           return "Pen";
        case ResourceType::Trash:         return "Trash";
        case ResourceType::Meat:          return "Meat";
        case ResourceType::Bone:          return "Bone";
        case ResourceType::Skull:         return "Skull";
        case ResourceType::Nail:          return "Nail";
        case ResourceType::None:
        default:                        return "None";
    }
}

ResourceType resource_type_from_string(const std::string &str)
{
    std::string s = str;
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    if (s == "coal")           return ResourceType::Coal;
    if (s == "ore")            return ResourceType::Ore;
    if (s == "bar")            return ResourceType::Bar;
    if (s == "log")            return ResourceType::Log;
    if (s == "plank")          return ResourceType::Plank;
    if (s == "tree")           return ResourceType::Tree;
    if (s == "fastener")       return ResourceType::Fastener;
    if (s == "leather")        return ResourceType::Leather;
    if (s == "cloth")          return ResourceType::Cloth;
    if (s == "stoneblock")     return ResourceType::StoneBlock;
    if (s == "metalvein")      return ResourceType::MetalVein;
    if (s == "stonemonolith")  return ResourceType::StoneMonolith;
    if (s == "pen")            return ResourceType::Pen;
    if (s == "trash")          return ResourceType::Trash;
    if (s == "meat")           return ResourceType::Meat;
    if (s == "bone")           return ResourceType::Bone;
    if (s == "skull")          return ResourceType::Skull;
    if (s == "nail")           return ResourceType::Nail;
    return ResourceType::None;
}
