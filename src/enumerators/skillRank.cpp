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

#include "skillRank.hpp"

std::string SkillRank::toString() const
{
    if (value == Newbie) return "Newbie";
    if (value == Novice) return "Novice";
    if (value == Rookie) return "Rookie";
    if (value == Beginner) return "Beginner";
    if (value == Talented) return "Talented";
    if (value == Skilled) return "Skilled";
    if (value == Intermediate) return "Intermediate";
    if (value == Seasoned) return "Seasoned";
    if (value == Proficient) return "Proficient";
    if (value == Experienced) return "Experienced";
    if (value == Advanced) return "Advanced";
    if (value == Expert) return "Expert";
    if (value == Specialist) return "Specialist";
    if (value == Master) return "Master";
    return "None";
}

SkillRank SkillRank::getSkillRank(unsigned int & _value)
{
//    if (_value <= (1000 + (1000 * Newbie))) return Newbie;
//    if (_value <= (1000 + (1000 * Novice))) return Novice;
//    if (_value <= (1000 + (1000 * Rookie))) return Rookie;
//    if (_value <= (1000 + (1000 * Beginner))) return Beginner;
//    if (_value <= (1000 + (1000 * Talented))) return Talented;
//    if (_value <= (1000 + (1000 * Skilled))) return Skilled;
//    if (_value <= (1000 + (1000 * Intermediate))) return Intermediate;
//    if (_value <= (1000 + (1000 * Seasoned))) return Seasoned;
//    if (_value <= (1000 + (1000 * Proficient))) return Proficient;
//    if (_value <= (1000 + (1000 * Experienced))) return Experienced;
//    if (_value <= (1000 + (1000 * Advanced))) return Advanced;
//    if (_value <= (1000 + (1000 * Expert))) return Expert;
//    if (_value <= (1000 + (1000 * Specialist))) return Specialist;
//    if (_value <= (1000 + (1000 * Master))) return Master;
//    return Newbie;
    unsigned int rankThreshold = (1000 + (1000 * Newbie));
    if (_value <= rankThreshold) return Newbie;         // 2000
    rankThreshold += (1000 + (1000 * Novice));
    if (_value <= rankThreshold) return Novice;         // 5000
    rankThreshold += (1000 + (1000 * Rookie));
    if (_value <= rankThreshold) return Rookie;         // 9000
    rankThreshold += (1000 + (1000 * Beginner));
    if (_value <= rankThreshold) return Beginner;       // 14000
    rankThreshold += (1000 + (1000 * Talented));
    if (_value <= rankThreshold) return Talented;       // 20000
    rankThreshold += (1000 + (1000 * Skilled));
    if (_value <= rankThreshold) return Skilled;        // 27000
    rankThreshold += (1000 + (1000 * Intermediate));
    if (_value <= rankThreshold) return Intermediate;   // 35000
    rankThreshold += (1000 + (1000 * Seasoned));
    if (_value <= rankThreshold) return Seasoned;       // 44000
    rankThreshold += (1000 + (1000 * Proficient));
    if (_value <= rankThreshold) return Proficient;     // 54000
    rankThreshold += (1000 + (1000 * Experienced));
    if (_value <= rankThreshold) return Experienced;    // 65000
    rankThreshold += (1000 + (1000 * Advanced));
    if (_value <= rankThreshold) return Advanced;       // 77000
    rankThreshold += (1000 + (1000 * Expert));
    if (_value <= rankThreshold) return Expert;         // 90000
    rankThreshold += (1000 + (1000 * Specialist));
    if (_value <= rankThreshold) return Specialist;     // 104000
    rankThreshold += (1000 + (1000 * Master));
    if (_value <= rankThreshold) return Master;         // 119000
    return Newbie;
}

unsigned int SkillRank::getSkillCap()
{
    return 119000;
}
