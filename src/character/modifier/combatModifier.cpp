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
    if (_value == 1) value = IncreaseUnarmedHitRoll;
    else if (_value == 2) value = DecreaseUnarmedHitRoll;
    else if (_value == 3) value = IncreaseUnarmedDamage;
    else if (_value == 4) value = DecreaseUnarmedDamage;
    else if (_value == 5) value = IncreaseMeleeWeaponHitRoll;
    else if (_value == 6) value = DecreaseMeleeWeaponHitRoll;
    else if (_value == 7) value = IncreaseMeleeWeaponDamage;
    else if (_value == 8) value = DecreaseMeleeWeaponDamage;
    else if (_value == 9) value = IncreaseRangedWeaponHitRoll;
    else if (_value == 10) value = DecreaseRangedWeaponHitRoll;
    else if (_value == 11) value = IncreaseRangedWeaponDamage;
    else if (_value == 12) value = DecreaseRangedWeaponDamage;
    else value = None;
}

CombatModifier::CombatModifier(const Enum & _value) :
    value(_value)
{
    // Nothing to do.
}

bool CombatModifier::isValid(const unsigned int & _value)
{
    return (_value >= 1) && (_value <= 12);
}

std::string CombatModifier::toString() const
{
    if (value == CombatModifier::IncreaseUnarmedHitRoll)
        return "IncreaseUnarmedHitRoll";
    if (value == CombatModifier::DecreaseUnarmedHitRoll)
        return "DecreaseUnarmedHitRoll";
    if (value == CombatModifier::IncreaseUnarmedDamage)
        return "IncreaseUnarmedDamage";
    if (value == CombatModifier::DecreaseUnarmedDamage)
        return "DecreaseUnarmedDamage";
    if (value == CombatModifier::IncreaseMeleeWeaponHitRoll)
        return "IncreaseMeleeWeaponHitRoll";
    if (value == CombatModifier::DecreaseMeleeWeaponHitRoll)
        return "DecreaseMeleeWeaponHitRoll";
    if (value == CombatModifier::IncreaseMeleeWeaponDamage)
        return "IncreaseMeleeWeaponDamage";
    if (value == CombatModifier::DecreaseMeleeWeaponDamage)
        return "DecreaseMeleeWeaponDamage";
    if (value == CombatModifier::IncreaseRangedWeaponHitRoll)
        return "IncreaseRangedWeaponHitRoll";
    if (value == CombatModifier::DecreaseRangedWeaponHitRoll)
        return "DecreaseRangedWeaponHitRoll";
    if (value == CombatModifier::IncreaseRangedWeaponDamage)
        return "IncreaseRangedWeaponDamage";
    if (value == CombatModifier::DecreaseRangedWeaponDamage)
        return "DecreaseRangedWeaponDamage";
    else return "None";
}

unsigned int CombatModifier::toUInt() const
{
    return static_cast<unsigned int>(value);
}
