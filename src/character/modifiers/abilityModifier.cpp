/// @file   abilityModifier.cpp
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

#include "abilityModifier.hpp"

AbilityModifier::AbilityModifier() :
    value(None)
{
    // Nothing to do.
}

AbilityModifier::AbilityModifier(const unsigned int & _value) :
    value()
{
    if (_value == 1) value = IncreaseStrength;
    else if (_value == 2) value = DecreaseStrength;
    else if (_value == 3) value = IncreaseAgility;
    else if (_value == 4) value = DecreaseAgility;
    else if (_value == 5) value = IncreasePerception;
    else if (_value == 6) value = DecreasePerception;
    else if (_value == 7) value = IncreaseConstitution;
    else if (_value == 8) value = DecreaseConstitution;
    else if (_value == 9) value = IncreaseIntelligence;
    else if (_value == 10) value = DecreaseIntelligence;
    else value = None;
}

AbilityModifier::AbilityModifier(const std::string & _value) :
    value()
{
    if (_value == "IncreaseStrength")
        value = AbilityModifier::IncreaseStrength;
    else if (_value == "DecreaseStrength")
        value = AbilityModifier::DecreaseStrength;
    else if (_value == "IncreaseAgility")
        value = AbilityModifier::IncreaseAgility;
    else if (_value == "DecreaseAgility")
        value = AbilityModifier::DecreaseAgility;
    else if (_value == "IncreasePerception")
        value = AbilityModifier::IncreasePerception;
    else if (_value == "DecreasePerception")
        value = AbilityModifier::DecreasePerception;
    else if (_value == "IncreaseConstitution")
        value = AbilityModifier::IncreaseConstitution;
    else if (_value == "DecreaseConstitution")
        value = AbilityModifier::DecreaseConstitution;
    else if (_value == "IncreaseIntelligence")
        value = AbilityModifier::IncreaseIntelligence;
    else if (_value == "DecreaseIntelligence")
        value = AbilityModifier::DecreaseIntelligence;
    else
        value = None;
}

AbilityModifier::AbilityModifier(const Enum & _value) :
    value(_value)
{
    // Nothing to do.
}

bool AbilityModifier::isValid(const unsigned int & _value)
{
    return (_value >= 1) && (_value <= 10);
}

std::string AbilityModifier::toString() const
{
    if (value == AbilityModifier::IncreaseStrength)
        return "IncreaseStrength";
    if (value == AbilityModifier::DecreaseStrength)
        return "DecreaseStrength";
    if (value == AbilityModifier::IncreaseAgility)
        return "IncreaseAgility";
    if (value == AbilityModifier::DecreaseAgility)
        return "DecreaseAgility";
    if (value == AbilityModifier::IncreasePerception)
        return "IncreasePerception";
    if (value == AbilityModifier::DecreasePerception)
        return "DecreasePerception";
    if (value == AbilityModifier::IncreaseConstitution)
        return "IncreaseConstitution";
    if (value == AbilityModifier::DecreaseConstitution)
        return "DecreaseConstitution";
    if (value == AbilityModifier::IncreaseIntelligence)
        return "IncreaseIntelligence";
    if (value == AbilityModifier::DecreaseIntelligence)
        return "DecreaseIntelligence";
    else return "None";
}

unsigned int AbilityModifier::toUInt() const
{
    return static_cast<unsigned int>(value);
}