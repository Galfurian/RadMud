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

std::string ModelType::toString() const
{
    if (value == Corpse) return "Corpse";
    if (value == MeleeWeapon) return "MeleeWeapon";
    if (value == RangedWeapon) return "RangedWeapon";
    if (value == Armor) return "Armor";
    if (value == Shield) return "Shield";
    if (value == Projectile) return "Projectile";
    if (value == Container) return "Container";
    if (value == LiquidContainer) return "LiquidContainer";
    if (value == Tool) return "Tool";
    if (value == Node) return "Node";
    if (value == Resource) return "Resource";
    if (value == Seed) return "Seed";
    if (value == Key) return "Key";
    if (value == Furniture) return "Furniture";
    if (value == Food) return "Food";
    if (value == Light) return "Light";
    if (value == Vehicle) return "Vehicle";
    if (value == Book) return "Book";
    if (value == Rope) return "Rope";
    if (value == Mechanism) return "Mechanism";
    if (value == Currency) return "Currency";
    if (value == Shop) return "Shop";
    if (value == Magazine) return "Magazine";
    return "None";
}
