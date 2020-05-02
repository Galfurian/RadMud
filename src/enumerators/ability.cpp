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

#include "enumerators/ability.hpp"

std::string Ability::toString() const
{
	if (value == Ability::Strength)
		return "Strength";
	else if (value == Ability::Agility)
		return "Agility";
	else if (value == Ability::Perception)
		return "Perception";
	else if (value == Ability::Constitution)
		return "Constitution";
	else if (value == Ability::Intelligence)
		return "Intelligence";
	else
		return "none";
}

std::string Ability::getAbbreviation() const
{
	if (value == Ability::Strength)
		return "Str";
	else if (value == Ability::Agility)
		return "Agi";
	else if (value == Ability::Perception)
		return "Per";
	else if (value == Ability::Constitution)
		return "Con";
	else if (value == Ability::Intelligence)
		return "Int";
	else
		return "Nil";
}

std::string Ability::getDescription() const
{
	if (value == Ability::Strength) {
		return std::string() + "Strength is important for increasing the" +
			   " Carrying Weight and satisfying the minimum" +
			   " Strength requirements for some weapons and armors.";
	}
	if (value == Ability::Agility) {
		return std::string() + "Besides increasing mobility in combat, it increases" +
			   " the recharge speed of all the weapons, as well as" +
			   " the ability to use light armor.";
	}
	if (value == Ability::Perception) {
		return std::string() + "The ability to see, hear, taste and notice unusual" +
			   " things. A high Perception is important for a" + " sharpshooter.";
	}
	if (value == Ability::Constitution) {
		return std::string() + "Stamina and physical toughness. A character with" +
			   " a high Endurance will survive where others may not.";
	}
	if (value == Ability::Intelligence) {
		return std::string() + "Knowledge, wisdom and the value to think quickly," +
			   " this attribute is important for any character.";
	}
	return "none";
}

unsigned int Ability::getModifier(const unsigned int &_value)
{
	if (_value <= 10) {
		return 0;
	}
	return (_value - 10) / 2;
}
