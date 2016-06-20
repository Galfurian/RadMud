/// @file   skill.hpp
/// @brief  Define the skill variables.
/// @author Enrico Fraccaroli
/// @date   Aug 23 2014
/// @copyright
/// Copyright (c) 2014, 2015, 2016 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
/// Permission to use, copy, modify, and distribute this software for any
/// purpose with or without fee is hereby granted, provided that the above
/// copyright notice and this permission notice appear in all copies.
///
/// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
/// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
/// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
/// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
/// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
/// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
/// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

#ifndef SKILL_HPP
#define SKILL_HPP

#include <string>
#include <vector>
#include <map>

/// @brief Holds details about a skill.
class Skill
{
    public:
        /// The skill virtual number.
        int vnum;
        /// The name of the skill.
        std::string name;
        /// The description of the skill.
        std::string description;
        /// The main attribute of the skill.
        int attribute;

        /// @brief Constructor.
        Skill();

        /// @brief Disable Copy Construct.
        Skill(Skill const &) = delete;

        /// @brief Disable Move construct.
        Skill(Skill &&) = delete;

        /// @brief Disable Copy assign.
        Skill & operator=(Skill const &) = delete;

        /// @brief Disable Move assign.
        Skill & operator=(Skill &&) = delete;

        /// @brief Destructor.
        ~Skill();

        /// @brief Check the correctness of the continent.
        /// @return <b>True</b> if it has correct values,<br><b>False</b> otherwise.
        bool check();
};

/// Map of skills with values.
typedef std::map<int, Skill *> SkillMap;

#endif
