/// @file   ability.cpp
/// @brief  Implements the ability class methods.
/// @author Enrico Fraccaroli
/// @date   Nov 19 2016
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

#include "ability.hpp"

Ability::Ability() :
    ability(None)
{
    // Nothing to do.
}

Ability::Ability(const unsigned int & _ability) :
    ability()
{
    if (_ability == 1) ability = Strength;
    else if (_ability == 2) ability = Agility;
    else if (_ability == 3) ability = Perception;
    else if (_ability == 4) ability = Constitution;
    else if (_ability == 5) ability = Intelligence;
    else ability = None;
}

Ability::Ability(const Enum & _ability) :
    ability(_ability)
{
    // Nothing to do.
}

Ability::Ability(const std::string & _ability) :
    ability()
{
    if (_ability == "strength") ability = Strength;
    else if (_ability == "agility") ability = Agility;
    else if (_ability == "perception") ability = Perception;
    else if (_ability == "constitution") ability = Constitution;
    else if (_ability == "intelligence") ability = Intelligence;
    else ability = None;
}

bool Ability::isValid(const unsigned int & _ability)
{
    return (_ability >= 1) && (_ability <= 5);
}

bool Ability::isValid(const std::string & _ability)
{
    if (_ability == "strength") return true;
    if (_ability == "agility") return true;
    if (_ability == "perception") return true;
    if (_ability == "constitution") return true;
    return (_ability == "intelligence");
}

std::string Ability::toString() const
{
    if (ability == Ability::Strength) return "strength";
    else if (ability == Ability::Agility) return "agility";
    else if (ability == Ability::Perception) return "perception";
    else if (ability == Ability::Constitution) return "constitution";
    else if (ability == Ability::Intelligence) return "intelligence";
    else return "none";
}

std::string Ability::getDescription() const
{
    if (ability == Ability::Strength)
    {
        return std::string() +
               "Strength is important for increasing the" +
               " Carrying Weight and satisfying the minimum" +
               " Strength requirements for some weapons and armors.";
    }
    if (ability == Ability::Agility)
    {
        return std::string() +
               "Besides increasing mobility in combat, it increases" +
               " the recharge speed of all the weapons, as well as" +
               " the ability to use light armor.";
    }
    if (ability == Ability::Perception)
    {
        return std::string() +
               "The ability to see, hear, taste and notice unusual" +
               " things. A high Perception is important for a" +
               " sharpshooter.";
    }
    if (ability == Ability::Constitution)
    {
        return std::string() +
               "Stamina and physical toughness. A character with" +
               " a high Endurance will survive where others may not.";
    }
    if (ability == Ability::Intelligence)
    {
        return std::string() +
               "Knowledge, wisdom and the ability to think quickly," +
               " this attribute is important for any character.";
    }
    return "none";
}

unsigned int Ability::toUInt() const
{
    return static_cast<unsigned int>(ability);
}

unsigned int Ability::getModifier(const unsigned int & value)
{
    if (value <= 10)
    {
        return 0;
    }
    return (value - 10) / 2;
}

AbilityModifier Ability::getIncreaseModifier() const
{
    if (ability == Ability::Strength)
        return AbilityModifier::IncreaseStrength;
    if (ability == Ability::Agility)
        return AbilityModifier::IncreaseAgility;
    if (ability == Ability::Perception)
        return AbilityModifier::IncreasePerception;
    if (ability == Ability::Constitution)
        return AbilityModifier::IncreaseConstitution;
    if (ability == Ability::Intelligence)
        return AbilityModifier::IncreaseIntelligence;
    return AbilityModifier::None;
}

AbilityModifier Ability::getDecreaseModifier() const
{
    if (ability == Ability::Strength)
        return AbilityModifier::DecreaseStrength;
    if (ability == Ability::Agility)
        return AbilityModifier::DecreaseAgility;
    if (ability == Ability::Perception)
        return AbilityModifier::DecreasePerception;
    if (ability == Ability::Constitution)
        return AbilityModifier::DecreaseConstitution;
    if (ability == Ability::Intelligence)
        return AbilityModifier::DecreaseIntelligence;
    return AbilityModifier::None;
}

bool Ability::operator==(const Ability & rhs) const
{
    return ability == rhs.ability;
}

bool Ability::operator==(const Ability::Enum & rhs) const
{
    return ability == rhs;
}

bool Ability::operator!=(const Ability::Enum & rhs) const
{
    return ability != rhs;
}

bool Ability::operator<(const Ability & rhs) const
{
    return ability < rhs.ability;
}
