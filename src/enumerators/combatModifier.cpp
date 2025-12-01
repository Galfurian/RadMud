/// @file   combatModifier.cpp
/// @author Enrico Fraccaroli
/// @date   Jan 13 2017
/// @copyright
/// Copyright (c) 2017 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
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

#include "enumerators/combatModifier.hpp"

std::string combat_modifier_to_string(CombatModifier modifier)
{
    switch (modifier) {
    case CombatModifier::UnarmedHitRoll:
        return "UnarmedHitRoll";
    case CombatModifier::UnarmedDamage:
        return "UnarmedDamage";
    case CombatModifier::MeleeWeaponHitRoll:
        return "MeleeWeaponHitRoll";
    case CombatModifier::MeleeWeaponDamage:
        return "MeleeWeaponDamage";
    case CombatModifier::RangedWeaponHitRoll:
        return "RangedWeaponHitRoll";
    case CombatModifier::RangedWeaponDamage:
        return "RangedWeaponDamage";
    case CombatModifier::RangedAimSpeed:
        return "RangedAimSpeed";
    case CombatModifier::ArmorClass:
        return "ArmorClass";
    default:
        return "None";
    }
}

CombatModifier combat_modifier_from_string(const std::string &value)
{
    if (value == "UnarmedHitRoll")
        return CombatModifier::UnarmedHitRoll;
    else if (value == "UnarmedDamage")
        return CombatModifier::UnarmedDamage;
    else if (value == "MeleeWeaponHitRoll")
        return CombatModifier::MeleeWeaponHitRoll;
    else if (value == "MeleeWeaponDamage")
        return CombatModifier::MeleeWeaponDamage;
    else if (value == "RangedWeaponHitRoll")
        return CombatModifier::RangedWeaponHitRoll;
    else if (value == "RangedWeaponDamage")
        return CombatModifier::RangedWeaponDamage;
    else if (value == "RangedAimSpeed")
        return CombatModifier::RangedAimSpeed;
    else if (value == "ArmorClass")
        return CombatModifier::ArmorClass;
    else
        return CombatModifier::None;
}
