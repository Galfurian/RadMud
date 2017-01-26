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

#include "combatModifier.hpp"

CombatModifier::CombatModifier() :
    value(None)
{
    // Nothing to do.
}

CombatModifier::CombatModifier(const unsigned int & _value) :
    value()
{
    if (_value == static_cast<unsigned int>(UnarmedHitRoll))
        value = UnarmedHitRoll;
    else if (_value == static_cast<unsigned int>(UnarmedDamage))
        value = UnarmedDamage;
    else if (_value == static_cast<unsigned int>(MeleeWeaponHitRoll))
        value = MeleeWeaponHitRoll;
    else if (_value == static_cast<unsigned int>(MeleeWeaponDamage))
        value = MeleeWeaponDamage;
    else if (_value == static_cast<unsigned int>(RangedWeaponHitRoll))
        value = RangedWeaponHitRoll;
    else if (_value == static_cast<unsigned int>(RangedWeaponDamage))
        value = RangedWeaponDamage;
    else if (_value == static_cast<unsigned int>(RangedAimSpeed))
        value = RangedAimSpeed;
    else if (_value == static_cast<unsigned int>(ArmorClass))
        value = ArmorClass;
    else value = None;
}

CombatModifier::CombatModifier(const Enum & _value) :
    value(_value)
{
    // Nothing to do.
}

bool CombatModifier::isValid(const unsigned int & _value)
{
    return (_value >= 1) && (_value <= 7);
}

std::string CombatModifier::toString() const
{
    if (value == CombatModifier::UnarmedHitRoll)
        return "UnarmedHitRoll";
    if (value == CombatModifier::UnarmedDamage)
        return "UnarmedDamage";
    if (value == CombatModifier::MeleeWeaponHitRoll)
        return "MeleeWeaponHitRoll";
    if (value == CombatModifier::MeleeWeaponDamage)
        return "MeleeWeaponDamage";
    if (value == CombatModifier::RangedWeaponHitRoll)
        return "RangedWeaponHitRoll";
    if (value == CombatModifier::RangedWeaponDamage)
        return "RangedWeaponDamage";
    if (value == CombatModifier::RangedAimSpeed)
        return "RangedAimSpeed";
    if (value == CombatModifier::ArmorClass)
        return "ArmorClass";
    else return "None";
}

unsigned int CombatModifier::toUInt() const
{
    return static_cast<unsigned int>(value);
}
