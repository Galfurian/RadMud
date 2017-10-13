/// @file   skillManager.hpp
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

#include <vector>
#include "skillData.hpp"
#include "radMudTypes.hpp"

class Character;

class SkillManager
{
public:
    /// The owner of the manager.
    Character * owner;
    /// The player's list of skills.
    std::vector<std::shared_ptr<SkillData>> skills;

    explicit SkillManager(Character * _owner) :
        owner(_owner),
        skills()
    {
        // Nothing to do.
    }

    inline bool addSkill(const std::shared_ptr<Skill> & skill,
                         const unsigned int & skillLevel = 1)
    {
        if (this->findSkill(skill->vnum) != nullptr)
        {
            return false;
        }
        skills.emplace_back(std::make_shared<SkillData>(skill, skillLevel));
        return true;
    }

    inline std::shared_ptr<SkillData> findSkill(const VnumType & vnum)
    {
        for (auto skillData : skills)
        {
            if (skillData->skillVnum == vnum)
            {
                return skillData;
            }
        }
        return nullptr;
    }

    /// @brief Activate the effects on the character based on its skill ranks.
    void updateSkillEffects();

    /// @brief Checks if the given character has unlocked new skills.
    void checkIfUnlockedSkills();

    /// @brief Improves the skills which provides the given ability modifier.
    void improveAbility(const Ability & abilityModifier);

    /// @brief Improves the skills which provides the given status modifier.
    void improveStatus(const StatusModifier & statusModifier);

    /// @brief Improves the skills which provides the given combat modifier.
    void improveCombat(const CombatModifier & combatModifier);

    /// @brief Improves the skills which provides the given knowledge.
    void improveKnowledge(const Knowledge & knowledge);

};