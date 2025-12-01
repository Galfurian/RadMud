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

#include "enumerators/statusModifier.hpp"


#include <algorithm>
#include <string>
#include "enumerators/statusModifier.hpp"

std::string status_modifier_to_string(StatusModifier type)
{
    switch (type) {
        case StatusModifier::Health: return "Health";
        case StatusModifier::HealthRegeneration: return "HealthRegeneration";
        case StatusModifier::Stamina: return "Stamina";
        case StatusModifier::StaminaRegeneration: return "StaminaRegeneration";
        case StatusModifier::None:
        default: return "None";
    }
}

StatusModifier string_to_status_modifier(const std::string &str)
{
    std::string s = str;
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    if (s == "health") return StatusModifier::Health;
    if (s == "healthregeneration") return StatusModifier::HealthRegeneration;
    if (s == "stamina") return StatusModifier::Stamina;
    if (s == "staminaregeneration") return StatusModifier::StaminaRegeneration;
    return StatusModifier::None;
}
