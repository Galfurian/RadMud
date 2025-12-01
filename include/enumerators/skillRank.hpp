/// @file   skillRank.hpp
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

#pragma once

#include <cstdint>
#include <string>

/// @brief Used to determine the rank of a skill.
enum class SkillRank : uint8_t {
    None,         ///< [0]
    Newbie,       ///< [1]
    Novice,       ///< [2]
    Rookie,       ///< [3]
    Beginner,     ///< [4]
    Talented,     ///< [5]
    Skilled,      ///< [6]
    Intermediate, ///< [7]
    Seasoned,     ///< [8]
    Proficient,   ///< [9]
    Experienced,  ///< [10]
    Advanced,     ///< [11]
    Expert,       ///< [12]
    Specialist,   ///< [13]
    Master        ///< [14]
};

/// @brief Converts a SkillRank to its string representation.
std::string skill_rank_to_string(SkillRank type);

/// @brief Converts a string to a SkillRank. Returns SkillRank::None if not found.
SkillRank string_to_skill_rank(const std::string &str);

/// @brief Given a value, provides the corresponding skill rank.
SkillRank get_skill_rank(unsigned int value);

/// @brief Provides the maximum skill value.
unsigned int get_skill_cap();
