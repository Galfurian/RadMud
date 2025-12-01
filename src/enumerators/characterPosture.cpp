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

#include "enumerators/characterPosture.hpp"

std::string character_posture_to_string(CharacterPosture posture)
{
    switch (posture)
    {
        case CharacterPosture::Stand:
            return "Stand";
        case CharacterPosture::Crouch:
            return "Crouch";
        case CharacterPosture::Prone:
            return "Prone";
        case CharacterPosture::Sit:
            return "Sit";
        case CharacterPosture::Rest:
            return "Rest";
        case CharacterPosture::Sleep:
            return "Sleep";
        default:
            return "None";
    }
}

CharacterPosture string_to_character_posture(const std::string & value)
{
    if (value == "Stand")
        return CharacterPosture::Stand;
    else if (value == "Crouch")
        return CharacterPosture::Crouch;
    else if (value == "Prone")
        return CharacterPosture::Prone;
    else if (value == "Sit")
        return CharacterPosture::Sit;
    else if (value == "Rest")
        return CharacterPosture::Rest;
    else if (value == "Sleep")
        return CharacterPosture::Sleep;
    else
        return CharacterPosture::None;
}

std::string get_character_posture_action(CharacterPosture posture)
{
    switch (posture)
    {
        case CharacterPosture::Stand:
            return "standing";
        case CharacterPosture::Crouch:
            return "crouched";
        case CharacterPosture::Prone:
            return "prone";
        case CharacterPosture::Sit:
            return "sitting";
        case CharacterPosture::Rest:
            return "resting";
        case CharacterPosture::Sleep:
            return "sleeping";
        default:
            return "none";
    }
}

uint32_t get_character_posture_speed(CharacterPosture posture)
{
    switch (posture)
    {
        case CharacterPosture::Stand:
            return 2;
        case CharacterPosture::Crouch:
            return 4;
        case CharacterPosture::Prone:
            return 6;
        default:
            return 0;
    }
}

uint32_t get_character_posture_regain_modifier(CharacterPosture posture)
{
    switch (posture)
    {
        case CharacterPosture::Sit:
            return 1;
        case CharacterPosture::Rest:
            return 2;
        case CharacterPosture::Sleep:
            return 4;
        default:
            return 0;
    }
}
