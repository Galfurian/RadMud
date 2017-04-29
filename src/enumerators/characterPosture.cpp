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

std::string CharacterPosture::toString() const
{
    if (value == Stand) return "Stand";
    else if (value == Crouch) return "Crouch";
    else if (value == Prone) return "Prone";
    else if (value == Sit) return "Sit";
    else if (value == Rest) return "Rest";
    else if (value == Sleep) return "Sleep";
    else return "None";
}

std::string CharacterPosture::getAction() const
{
    if (value == Stand) return "standing";
    else if (value == Crouch) return "crouched";
    else if (value == Prone) return "prone";
    else if (value == Sit) return "sitting";
    else if (value == Rest) return "resting";
    else if (value == Sleep) return "sleeping";
    else return "none";
}

uint32_t CharacterPosture::getSpeed() const
{
    if (value == Stand) return 2;
    else if (value == Crouch) return 4;
    else if (value == Prone) return 6;
    return 0;
}
