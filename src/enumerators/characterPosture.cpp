/// @file   characterPosture.cpp
/// @brief  Implements the character posture class methods.
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

#include "characterPosture.hpp"

CharacterPosture::CharacterPosture() :
    characterPosture()
{
    // Nothing to do.
}

CharacterPosture::CharacterPosture(const unsigned int & _characterPosture) :
    characterPosture()
{
    if (_characterPosture == 1) characterPosture = Stand;
    else if (_characterPosture == 2) characterPosture = Crouch;
    else if (_characterPosture == 3) characterPosture = Sit;
    else if (_characterPosture == 4) characterPosture = Prone;
    else if (_characterPosture == 5) characterPosture = Rest;
    else if (_characterPosture == 6) characterPosture = Sleep;
    else characterPosture = None;
}

CharacterPosture::CharacterPosture(const Enum & _characterPosture) :
    characterPosture(_characterPosture)
{
    // Nothing to do.
}

bool CharacterPosture::isValid(const unsigned int & _characterPosture)
{
    return (_characterPosture >= 1) && (_characterPosture <= 6);
}

std::string CharacterPosture::toString() const
{
    if (characterPosture == Stand) return "Stand";
    else if (characterPosture == Crouch) return "Crouch";
    else if (characterPosture == Sit) return "Sit";
    else if (characterPosture == Prone) return "Prone";
    else if (characterPosture == Rest) return "Rest";
    else if (characterPosture == Sleep) return "Sleep";
    else return "None";
}

unsigned int CharacterPosture::toUInt() const
{
    return static_cast<unsigned int>(characterPosture);
}

std::string CharacterPosture::getAction() const
{
    if (characterPosture == Stand) return "standing";
    else if (characterPosture == Crouch) return "crouched";
    else if (characterPosture == Sit) return "sitting";
    else if (characterPosture == Prone) return "prone";
    else if (characterPosture == Rest) return "resting";
    else if (characterPosture == Sleep) return "sleeping";
    else return "none";
}

bool CharacterPosture::operator==(const CharacterPosture & rhs) const
{
    return characterPosture == rhs.characterPosture;
}

bool CharacterPosture::operator!=(const CharacterPosture & rhs) const
{
    return characterPosture != rhs.characterPosture;
}

bool CharacterPosture::operator<=(const CharacterPosture & rhs) const
{
    return characterPosture <= rhs.characterPosture;
}

bool CharacterPosture::operator>=(const CharacterPosture & rhs) const
{
    return characterPosture >= rhs.characterPosture;
}
