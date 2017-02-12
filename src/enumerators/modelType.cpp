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

#include "modelType.hpp"
#include "luaBridge.hpp"

ModelType::ModelType() :
    modelType(None)
{
    // Nothing to do.
}

ModelType::ModelType(const unsigned int & _modelType) :
    modelType()
{
    if (_modelType == 1) modelType = Corpse;
    else if (_modelType == 2) modelType = MeleeWeapon;
    else if (_modelType == 3) modelType = RangedWeapon;
    else if (_modelType == 4) modelType = Armor;
    else if (_modelType == 5) modelType = Shield;
    else if (_modelType == 6) modelType = Projectile;
    else if (_modelType == 7) modelType = Container;
    else if (_modelType == 8) modelType = LiquidContainer;
    else if (_modelType == 9) modelType = Tool;
    else if (_modelType == 10) modelType = Node;
    else if (_modelType == 11) modelType = Resource;
    else if (_modelType == 12) modelType = Seed;
    else if (_modelType == 13) modelType = Key;
    else if (_modelType == 14) modelType = Furniture;
    else if (_modelType == 15) modelType = Food;
    else if (_modelType == 16) modelType = Light;
    else if (_modelType == 17) modelType = Vehicle;
    else if (_modelType == 18) modelType = Book;
    else if (_modelType == 19) modelType = Rope;
    else if (_modelType == 20) modelType = Mechanism;
    else if (_modelType == 21) modelType = Currency;
    else if (_modelType == 22) modelType = Shop;
    else if (_modelType == 23) modelType = Magazine;
    else modelType = None;
}

ModelType::ModelType(const Enum & _modelType) :
    modelType(_modelType)
{
    // Nothing to do.
}

ModelType::ModelType(const std::string & _modelType) :
    modelType()
{
    if (_modelType == "Corpse") modelType = Corpse;
    else if (_modelType == "MeleeWeapon") modelType = MeleeWeapon;
    else if (_modelType == "RangedWeapon") modelType = RangedWeapon;
    else if (_modelType == "Armor") modelType = Armor;
    else if (_modelType == "Shield") modelType = Shield;
    else if (_modelType == "Projectile") modelType = Projectile;
    else if (_modelType == "Container") modelType = Container;
    else if (_modelType == "LiquidContainer") modelType = LiquidContainer;
    else if (_modelType == "Tool") modelType = Tool;
    else if (_modelType == "Node") modelType = Node;
    else if (_modelType == "Resource") modelType = Resource;
    else if (_modelType == "Seed") modelType = Seed;
    else if (_modelType == "Key") modelType = Key;
    else if (_modelType == "Furniture") modelType = Furniture;
    else if (_modelType == "Food") modelType = Food;
    else if (_modelType == "Light") modelType = Light;
    else if (_modelType == "Vehicle") modelType = Vehicle;
    else if (_modelType == "Book") modelType = Book;
    else if (_modelType == "Rope") modelType = Rope;
    else if (_modelType == "Mechanism") modelType = Mechanism;
    else if (_modelType == "Currency") modelType = Currency;
    else if (_modelType == "Shop") modelType = Shop;
    else if (_modelType == "Magazine") modelType = Magazine;
    else modelType = None;
}

bool ModelType::isValid(const unsigned int & _modelType)
{
    return (_modelType >= 1) && (_modelType <= 23);
}

bool ModelType::isValid(const std::string & _modelType)
{
    if (_modelType == "Corpse") return true;
    if (_modelType == "MeleeWeapon") return true;
    if (_modelType == "RangedWeapon")return true;
    if (_modelType == "Armor") return true;
    if (_modelType == "Shield") return true;
    if (_modelType == "Projectile") return true;
    if (_modelType == "Container") return true;
    if (_modelType == "LiquidContainer") return true;
    if (_modelType == "Tool") return true;
    if (_modelType == "Node") return true;
    if (_modelType == "Resource") return true;
    if (_modelType == "Seed") return true;
    if (_modelType == "Key") return true;
    if (_modelType == "Furniture") return true;
    if (_modelType == "Food") return true;
    if (_modelType == "Light") return true;
    if (_modelType == "Vehicle") return true;
    if (_modelType == "Book") return true;
    if (_modelType == "Rope") return true;
    if (_modelType == "Mechanism") return true;
    if (_modelType == "Currency") return true;
    if (_modelType == "Shop") return true;
    return (_modelType == "Magazine");
}

std::string ModelType::toString() const
{
    if (modelType == Corpse) return "Corpse";
    if (modelType == MeleeWeapon) return "MeleeWeapon";
    if (modelType == RangedWeapon) return "RangedWeapon";
    if (modelType == Armor) return "Armor";
    if (modelType == Shield) return "Shield";
    if (modelType == Projectile) return "Projectile";
    if (modelType == Container) return "Container";
    if (modelType == LiquidContainer) return "LiquidContainer";
    if (modelType == Tool) return "Tool";
    if (modelType == Node) return "Node";
    if (modelType == Resource) return "Resource";
    if (modelType == Seed) return "Seed";
    if (modelType == Key) return "Key";
    if (modelType == Furniture) return "Furniture";
    if (modelType == Food) return "Food";
    if (modelType == Light) return "Light";
    if (modelType == Vehicle) return "Vehicle";
    if (modelType == Book) return "Book";
    if (modelType == Rope) return "Rope";
    if (modelType == Mechanism) return "Mechanism";
    if (modelType == Currency) return "Currency";
    if (modelType == Shop) return "Shop";
    if (modelType == Magazine) return "Magazine";
    return "None";
}

unsigned int ModelType::toUInt() const
{
    return static_cast<unsigned int>(modelType);
}

bool ModelType::operator==(const ModelType & rhs) const
{
    return modelType == rhs.modelType;
}

bool ModelType::operator==(const ModelType::Enum & rhs) const
{
    return modelType == rhs;
}

bool ModelType::operator!=(const ModelType::Enum & rhs) const
{
    return modelType != rhs;
}

bool ModelType::operator<(const ModelType & rhs) const
{
    return modelType < rhs.modelType;
}
