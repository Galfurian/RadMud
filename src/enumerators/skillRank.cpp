/// @file   skillRank.cpp
/// @author Enrico Fraccaroli
/// @date   Jan 16 2017
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

#include "enumerators/skillRank.hpp"

#include <string>
#include <algorithm>

std::string skill_rank_to_string(SkillRank type)
{
    switch (type) {
        case SkillRank::Newbie:       return "Newbie";
        case SkillRank::Novice:       return "Novice";
        case SkillRank::Rookie:       return "Rookie";
        case SkillRank::Beginner:     return "Beginner";
        case SkillRank::Talented:     return "Talented";
        case SkillRank::Skilled:      return "Skilled";
        case SkillRank::Intermediate: return "Intermediate";
        case SkillRank::Seasoned:     return "Seasoned";
        case SkillRank::Proficient:   return "Proficient";
        case SkillRank::Experienced:  return "Experienced";
        case SkillRank::Advanced:     return "Advanced";
        case SkillRank::Expert:       return "Expert";
        case SkillRank::Specialist:   return "Specialist";
        case SkillRank::Master:       return "Master";
        case SkillRank::None:
        default:                     return "None";
    }
}

SkillRank string_to_skill_rank(const std::string &str)
{
    std::string s = str;
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    if (s == "newbie")       return SkillRank::Newbie;
    if (s == "novice")       return SkillRank::Novice;
    if (s == "rookie")       return SkillRank::Rookie;
    if (s == "beginner")     return SkillRank::Beginner;
    if (s == "talented")     return SkillRank::Talented;
    if (s == "skilled")      return SkillRank::Skilled;
    if (s == "intermediate") return SkillRank::Intermediate;
    if (s == "seasoned")     return SkillRank::Seasoned;
    if (s == "proficient")   return SkillRank::Proficient;
    if (s == "experienced")  return SkillRank::Experienced;
    if (s == "advanced")     return SkillRank::Advanced;
    if (s == "expert")       return SkillRank::Expert;
    if (s == "specialist")   return SkillRank::Specialist;
    if (s == "master")       return SkillRank::Master;
    return SkillRank::None;
}

SkillRank get_skill_rank(unsigned int value)
{
    unsigned int rankThreshold = (1000 + (1000 * static_cast<unsigned int>(SkillRank::Newbie)));
    if (value <= rankThreshold)
        return SkillRank::Newbie; // 2000
    rankThreshold += (1000 + (1000 * static_cast<unsigned int>(SkillRank::Novice)));
    if (value <= rankThreshold)
        return SkillRank::Novice; // 5000
    rankThreshold += (1000 + (1000 * static_cast<unsigned int>(SkillRank::Rookie)));
    if (value <= rankThreshold)
        return SkillRank::Rookie; // 9000
    rankThreshold += (1000 + (1000 * static_cast<unsigned int>(SkillRank::Beginner)));
    if (value <= rankThreshold)
        return SkillRank::Beginner; // 14000
    rankThreshold += (1000 + (1000 * static_cast<unsigned int>(SkillRank::Talented)));
    if (value <= rankThreshold)
        return SkillRank::Talented; // 20000
    rankThreshold += (1000 + (1000 * static_cast<unsigned int>(SkillRank::Skilled)));
    if (value <= rankThreshold)
        return SkillRank::Skilled; // 27000
    rankThreshold += (1000 + (1000 * static_cast<unsigned int>(SkillRank::Intermediate)));
    if (value <= rankThreshold)
        return SkillRank::Intermediate; // 35000
    rankThreshold += (1000 + (1000 * static_cast<unsigned int>(SkillRank::Seasoned)));
    if (value <= rankThreshold)
        return SkillRank::Seasoned; // 44000
    rankThreshold += (1000 + (1000 * static_cast<unsigned int>(SkillRank::Proficient)));
    if (value <= rankThreshold)
        return SkillRank::Proficient; // 54000
    rankThreshold += (1000 + (1000 * static_cast<unsigned int>(SkillRank::Experienced)));
    if (value <= rankThreshold)
        return SkillRank::Experienced; // 65000
    rankThreshold += (1000 + (1000 * static_cast<unsigned int>(SkillRank::Advanced)));
    if (value <= rankThreshold)
        return SkillRank::Advanced; // 77000
    rankThreshold += (1000 + (1000 * static_cast<unsigned int>(SkillRank::Expert)));
    if (value <= rankThreshold)
        return SkillRank::Expert; // 90000
    rankThreshold += (1000 + (1000 * static_cast<unsigned int>(SkillRank::Specialist)));
    if (value <= rankThreshold)
        return SkillRank::Specialist; // 104000
    rankThreshold += (1000 + (1000 * static_cast<unsigned int>(SkillRank::Master)));
    if (value <= rankThreshold)
        return SkillRank::Master; // 119000
    return SkillRank::Newbie;
}

unsigned int get_skill_cap()
{
    return 119000;
}
