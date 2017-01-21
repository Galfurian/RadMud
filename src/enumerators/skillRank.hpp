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

#include <string>

/// Represents the ranks of a skill.
class SkillRank
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
    SkillRank();

    /// @brief Constructor.
    SkillRank(const unsigned int & _value);

    /// @brief Constructor.
    SkillRank(const Enum & _value);

    /// @brief Check is the given number is a valid skill rank.
    static bool isValid(const unsigned int & _value);

    /// @brief Returns the skill rank as string.
    std::string toString() const;

    /// @brief Returns the skill rank as number.
    unsigned int toUInt() const;

    template<typename ValueType>
    ValueType cast_to() const
    {
        static_assert((std::is_same<ValueType, int>::value ||
                       std::is_same<ValueType, unsigned int>::value ||
                       std::is_same<ValueType, double>::value),
                      "template parameter is of the wrong type");
        return static_cast<ValueType>(value);
    }

    static SkillRank getSkillRank(unsigned int & _value);

    static unsigned int getSkillCap();

    /// @brief Equality operator.
    bool operator==(const SkillRank & rhs) const;

    /// @brief Inequality operator.
    bool operator!=(const SkillRank & rhs) const;

    /// @brief Lesser operator.
    bool operator<(const SkillRank & rhs) const;

private:
    /// Internal value.
    Enum value;
};
