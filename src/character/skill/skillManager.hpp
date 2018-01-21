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

#include "effect.hpp"
#include "skillData.hpp"
#include "radMudTypes.hpp"
#include "modifierManager.hpp"
#include "effectFactory.hpp"

class Character;

class SkillManager :
    public ModifierManager
{
public:
    /// The owner of the manager.
    Character * owner;
    /// The player's list of skills.
    std::vector<std::shared_ptr<SkillData>> skills;
    /// The vector of effects related to the skills.
    std::vector<std::shared_ptr<SkillEffect>> skillEffects;

    /// @brief Constructor.
    /// @param _owner The owner of the manager.
    explicit SkillManager(Character * _owner);

    /// @brief Add the given skill to the list of skills.
    /// @param skill        A pointer to the skill.
    /// @param skillLevel   The initial level of the skill.
    /// @return The outcome.
    inline bool addSkill(const std::shared_ptr<Skill> & skill,
                         const unsigned int & skillLevel = 1)
    {
        // Check if the skill is already present.
        if (this->findSkill(skill->vnum) != nullptr)
        {
            return false;
        }
        // Create a new skill data for the given skill.
        auto skillData = std::make_shared<SkillData>(skill, skillLevel);
        // Add the new skill data to the list of skills.
        skills.emplace_back(skillData);
        // Update the effects due to the new skill.
        this->updateSkillEffect(skillData);
        return true;
    }

    /// @brief Finds the given skill data.
    /// @param vnum The vnum of the skill.
    /// @return A pointer to the skill data.
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

    /// @brief Checks if the given character has unlocked new skills.
    void checkIfUnlockedSkills();

    /// @brief Activate the effects on the character based on the skill rank.
    void updateSkillEffect(std::shared_ptr<SkillData> & skillData);

    /// @brief Improves the skills which provides the given ability modifier.
    void improveAbility(const Ability & abilityModifier);

    /// @brief Improves the skills which provides the given status modifier.
    void improveStatus(const StatusModifier & statusModifier);

    /// @brief Improves the skills which provides the given combat modifier.
    void improveCombat(const CombatModifier & combatModifier);

    /// @brief Improves the skills which provides the given knowledge.
    void improveKnowledge(const Knowledge & knowledge);

private:

    inline std::shared_ptr<SkillEffect> getSkillEffect(
        const std::shared_ptr<SkillData> & skillData)
    {
        for (const auto & it : skillEffects)
        {
            if (it->skillData == skillData)
            {
                return it;
            }
        }
        return nullptr;
    }

    inline std::shared_ptr<SkillEffect> createSkillEffect(
        const std::shared_ptr<SkillData> & skillData)
    {
        auto skillEffect = this->getSkillEffect(skillData);
        if (skillEffect == nullptr)
        {
            skillEffect = std::make_shared<SkillEffect>(owner,
                                                        skillData->skill->name,
                                                        skillData);
            skillEffects.emplace_back(skillEffect);
        }
        return skillEffect;
    }
};