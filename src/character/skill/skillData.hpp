/// @file   skillData.hpp
/// @author Enrico Fraccaroli
/// @date   ott 13 2017
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

#include "skill.hpp"
#include "effect.hpp"
#include "skillRank.hpp"
#include "radMudTypes.hpp"

class SkillData
{
public:
    /// The skill vnum.
    VnumType skillVnum;
    /// The skill level.
    unsigned int skillLevel;
    /// A pointer to the skill.
    std::shared_ptr<Skill> skill;

    /// @brief Constructor.
    SkillData(const VnumType & _skillVnum,
              const unsigned int & _skillLevel,
              std::shared_ptr<Skill> _skill) :
        skillVnum(_skillVnum),
        skillLevel(_skillLevel),
        skill(std::move(_skill))
    {
        // Nothing to do.
    }

    /// @brief Constructor.
    SkillData(std::shared_ptr<Skill> _skill,
              const unsigned int & _skillLevel) :
        skillVnum(_skill->vnum),
        skillLevel(_skillLevel),
        skill(std::move(_skill))
    {
        // Nothing to do.
    }

    inline SkillRank getSkillRank() const
    {
        return SkillRank::getSkillRank(skillLevel);
    }

    inline void improveSkill(const unsigned int & skillImprovement)
    {
        skillLevel += skillImprovement;
        if (skillLevel > SkillRank::getSkillCap())
        {
            skillLevel = SkillRank::getSkillCap();
        }
    }
};

class SkillEffect
{
public:
    /// The character affected by the effect.
    Character * affected;
    /// Name of the modifier.
    std::string name;
    /// The skill which produces the effect.
    std::shared_ptr<SkillData> skillData;
    /// The internal modifier manager.
    std::shared_ptr<ModifierManager> modifierManager;

    /// @brief Constructor.
    SkillEffect(Character * _affected,
                std::string _name,
                std::shared_ptr<SkillData> _skillData) :
        affected(_affected),
        name(std::move(_name)),
        skillData(std::move(_skillData)),
        modifierManager(std::make_shared<ModifierManager>())
    {
        // Nothing to do.
    }
};