/// @file   statusModifier.cpp
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

#include "statusModifier.hpp"

StatusModifier::StatusModifier() :
    value(None)
{
    // Nothing to do.
}

StatusModifier::StatusModifier(const unsigned int & _value) :
    value()
{
    if (_value == 1) value = IncreaseHealth;
    else if (_value == 2) value = DecreaseHealth;
    else if (_value == 3) value = IncreaseHealthRegeneration;
    else if (_value == 4) value = DecreaseHealthRegeneration;
    else if (_value == 5) value = IncreaseStamina;
    else if (_value == 6) value = DecreaseStamina;
    else if (_value == 7) value = IncreaseStaminaRegeneration;
    else if (_value == 8) value = DecreaseStaminaRegeneration;
    else value = None;
}

StatusModifier::StatusModifier(const Enum & _value) :
    value(_value)
{
    // Nothing to do.
}

bool StatusModifier::isValid(const unsigned int & _value)
{
    return (_value >= 1) && (_value <= 8);
}

std::string StatusModifier::toString() const
{
    if (value == StatusModifier::IncreaseHealth)
        return "IncreaseHealth";
    if (value == StatusModifier::DecreaseHealth)
        return "DecreaseHealth";
    if (value == StatusModifier::IncreaseHealthRegeneration)
        return "IncreaseHealthRegeneration";
    if (value == StatusModifier::DecreaseHealthRegeneration)
        return "DecreaseHealthRegeneration";
    if (value == StatusModifier::IncreaseStamina)
        return "IncreaseStamina";
    if (value == StatusModifier::DecreaseStamina)
        return "DecreaseStamina";
    if (value == StatusModifier::IncreaseStaminaRegeneration)
        return "IncreaseStaminaRegeneration";
    if (value == StatusModifier::DecreaseStaminaRegeneration)
        return "DecreaseStaminaRegeneration";
    else return "None";
}

unsigned int StatusModifier::toUInt() const
{
    return static_cast<unsigned int>(value);
}
