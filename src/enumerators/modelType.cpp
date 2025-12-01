/// @file   modelType.cpp
/// @brief  Implements the ModelType class methods.
/// @author Enrico Fraccaroli
/// @date   Nov 21 2016
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


#include "enumerators/modelType.hpp"
#include <string>
#include <algorithm>

std::string model_type_to_string(ModelType type)
{
    switch (type) {
        case ModelType::Corpse:          return "Corpse";
        case ModelType::MeleeWeapon:     return "MeleeWeapon";
        case ModelType::RangedWeapon:    return "RangedWeapon";
        case ModelType::Armor:           return "Armor";
        case ModelType::Shield:          return "Shield";
        case ModelType::Projectile:      return "Projectile";
        case ModelType::Container:       return "Container";
        case ModelType::LiquidContainer: return "LiquidContainer";
        case ModelType::Tool:            return "Tool";
        case ModelType::Node:            return "Node";
        case ModelType::Resource:        return "Resource";
        case ModelType::Seed:            return "Seed";
        case ModelType::Key:             return "Key";
        case ModelType::Furniture:       return "Furniture";
        case ModelType::Food:            return "Food";
        case ModelType::Light:           return "Light";
        case ModelType::Vehicle:         return "Vehicle";
        case ModelType::Book:            return "Book";
        case ModelType::Rope:            return "Rope";
        case ModelType::Mechanism:       return "Mechanism";
        case ModelType::Currency:        return "Currency";
        case ModelType::Shop:            return "Shop";
        case ModelType::Magazine:        return "Magazine";
        case ModelType::None:
        default:                        return "None";
    }
}

ModelType string_to_model_type(const std::string &str)
{
    std::string s = str;
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    if (s == "corpse")           return ModelType::Corpse;
    if (s == "meleeweapon")      return ModelType::MeleeWeapon;
    if (s == "rangedweapon")     return ModelType::RangedWeapon;
    if (s == "armor")            return ModelType::Armor;
    if (s == "shield")           return ModelType::Shield;
    if (s == "projectile")       return ModelType::Projectile;
    if (s == "container")        return ModelType::Container;
    if (s == "liquidcontainer")  return ModelType::LiquidContainer;
    if (s == "tool")             return ModelType::Tool;
    if (s == "node")             return ModelType::Node;
    if (s == "resource")         return ModelType::Resource;
    if (s == "seed")             return ModelType::Seed;
    if (s == "key")              return ModelType::Key;
    if (s == "furniture")        return ModelType::Furniture;
    if (s == "food")             return ModelType::Food;
    if (s == "light")            return ModelType::Light;
    if (s == "vehicle")          return ModelType::Vehicle;
    if (s == "book")             return ModelType::Book;
    if (s == "rope")             return ModelType::Rope;
    if (s == "mechanism")        return ModelType::Mechanism;
    if (s == "currency")         return ModelType::Currency;
    if (s == "shop")             return ModelType::Shop;
    if (s == "magazine")         return ModelType::Magazine;
    return ModelType::None;
}
