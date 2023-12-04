/// @file   skillManager.cpp
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

#include <cassert>

#include "character/skill/skillManager.hpp"
#include "character/character.hpp"
#include "utilities/logger.hpp"
#include "mud.hpp"

SkillManager::SkillManager(Character * _owner) :
    owner(_owner),
    skills(),
    skillEffects()
{
    assert(_owner);
    // Nothing to do.
}

void SkillManager::checkIfUnlockedSkills()
{
    std::vector<VnumType> masterSkills;
    // Create a list for all the master skills.
    for (const auto & it : skills)
    {
        // Get the current skill rank.
        if (it->getSkillRank() == SkillRank::Master)
        {
            masterSkills.emplace_back(it->skillVnum);
        }
    }
    // Checks if the character has the pre-requisite for the skill.
    auto HasPrerequisites =
        [&masterSkills](const std::shared_ptr<Skill> & skill)
        {
            for (const auto & requiredSkill : skill->requiredSkill)
            {
                if (std::find(masterSkills.begin(),
                              masterSkills.end(),
                              requiredSkill) == masterSkills.end())
                {
                    return false;
                }
            }
            return true;
        };

    for (const auto & skill : Mud::instance().mudSkills)
    {
        // Check if the character has the pre-requisite for the skill.
        if (HasPrerequisites(skill))
        {
            // Add the new skill to the character.
            this->addSkill(skill);
        }
    }
}

void SkillManager::updateSkillEffect(std::shared_ptr<SkillData> & skillData)
{
    // Get the skill.
    const auto & skill = skillData->skill;
    // Save the skill rank.
    auto skillRank = skillData->getSkillRank().to<int>();
    // Try to find the skill effect.
    auto skillEffect = this->getSkillEffect(skillData);
    if (skillEffect == nullptr)
    {
        skillEffect = this->createSkillEffect(skillData);
    } else
    {
        *(this) -= (*(skillEffect.get()));
        skillEffect->reset();
    }
    // Update the skill effect.
    skillEffect->applyModifier(skill->modifierManager, skillRank);
    *(this) += (*(skillEffect.get()));
}

void SkillManager::improveAbility(const Ability & abilityModifier)
{
    for (auto & skillData : skills)
    {
        // Get the skill.
        const auto & skill = skillData->skill;
        // Check if the skill provides the given modifier.
        if (skill->modifierManager->getAbilityMod(abilityModifier) <= 0)
        {
            continue;
        }
        // Get the current skill rank.
        auto skillRank = skillData->getSkillRank();
        // Improve the skill.
        skillData->improveSkill(50);
        // Get the new skill rank.
        auto newSkillRank = skillData->getSkillRank();
        // Check if the skill has advanced on the ranking.
        if (skillRank < newSkillRank)
        {
            // Before updating the effects of the skills, check if the
            // character has unlocked some new skills.
            if (newSkillRank == SkillRank::Master)
            {
                this->checkIfUnlockedSkills();
            }
            // Update the effects due to the given skill.
            this->updateSkillEffect(skillData);
        }
    }
}

void SkillManager::improveStatus(const StatusModifier & statusModifier)
{
    for (auto & skillData : skills)
    {
        // Get the skill.
        const auto & skill = skillData->skill;
        // Check if the skill provides the given modifier.
        if (skill->modifierManager->getStatusMod(statusModifier) <= 0)
        {
            continue;
        }
        // Get the current skill rank.
        auto skillRank = skillData->getSkillRank();
        // Improve the skill.
        skillData->improveSkill(50);
        // Get the new skill rank.
        auto newSkillRank = skillData->getSkillRank();
        // Check if the skill has advanced on the ranking.
        if (skillRank < newSkillRank)
        {
            // Before updating the effects of the skills, check if the
            // character has unlocked some new skills.
            if (newSkillRank == SkillRank::Master)
            {
                this->checkIfUnlockedSkills();
            }
            // Update the effects due to the given skill.
            this->updateSkillEffect(skillData);
        }
    }
}

void SkillManager::improveCombat(const CombatModifier & combatModifier)
{
    for (auto & skillData : skills)
    {
        // Get the skill.
        const auto & skill = skillData->skill;
        // Check if the skill provides the given modifier.
        if (skill->modifierManager->getCombatMod(combatModifier) <= 0)
        {
            continue;
        }
        // Get the current skill rank.
        auto oldSkillRank = skillData->getSkillRank();
        // Improve the skill.
        skillData->improveSkill(50);
        // Get the new skill rank.
        auto newSkillRank = skillData->getSkillRank();
        // Check if the skill has advanced on the ranking.
        if (oldSkillRank < newSkillRank)
        {
            // Before updating the effects of the skills, check if the
            // character has unlocked some new skills.
            if (newSkillRank == SkillRank::Master)
            {
                this->checkIfUnlockedSkills();
            }
            // Update the effects due to the given skill.
            this->updateSkillEffect(skillData);
        }
    }
}

void SkillManager::improveKnowledge(const Knowledge & knowledge)
{
    for (auto & skillData : skills)
    {
        // Get the skill.
        const auto & skill = skillData->skill;
        // Check if the skill provides the given knowledge.
        if (skill->modifierManager->getKnowledge(knowledge) <= 0)
        {
            continue;
        }
        // Get the current skill rank.
        auto skillRank = skillData->getSkillRank();
        // Improve the skill.
        skillData->improveSkill(50);
        // Get the new skill rank.
        auto newSkillRank = skillData->getSkillRank();
        // Check if the skill has advanced on the ranking.
        if (skillRank < newSkillRank)
        {
            // Before updating the effects of the skills, check if the
            // character has unlocked some new skills.
            if (newSkillRank == SkillRank::Master)
            {
                this->checkIfUnlockedSkills();
            }
            // Update the effects due to the given skill.
            this->updateSkillEffect(skillData);
        }
    }
}

