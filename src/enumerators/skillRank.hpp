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

#include "baseEnumerator.hpp"

/// Represents the ranks of a skill.
class SkillRank :
    public BaseEnumerator
{
public:
    /// The ranks of a skill.
    enum Enum
    {
        None,           ///< [0]
        Newbie,         ///< [1]
        Novice,         ///< [2]
        Rookie,         ///< [3]
        Beginner,       ///< [4]
        Talented,       ///< [5]
        Skilled,        ///< [6]
        Intermediate,   ///< [7]
        Seasoned,       ///< [8]
        Proficient,     ///< [9]
        Experienced,    ///< [10]
        Advanced,       ///< [11]
        Expert,         ///< [12]
        Specialist,     ///< [13]
        Master          ///< [14]
    };

    /// @brief Constructor.
    SkillRank() :
        BaseEnumerator()
    {
        // Nothing to do.
    }

    /// @brief Constructor from unsigned int.
    SkillRank(const unsigned int & _value) :
        BaseEnumerator(_value)
    {
        // Nothing to do.
    }

    /// @brief Constructor from enum.
    SkillRank(const Enum & _value) :
        BaseEnumerator(_value)
    {
        // Nothing to do.
    }

    /// @brief Returns the skill rank as string.
    std::string toString() const override;

    /// @brief Given a value, provides the corresponding skill rank.
    static SkillRank getSkillRank(unsigned int & _value);

    /// @brief Provides the maximum skill value.
    static unsigned int getSkillCap();
};
