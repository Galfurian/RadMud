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

#include "skillManager.hpp"
#include "effectFactory.hpp"
#include "character.hpp"
#include "mud.hpp"

void SkillManager::updateSkillEffects()
{
    // TODO: Determine the increment brought by each skill based on its rank.
    // TODO: Right now the modifier is based on the rank of the skill.
    // -------------------------------------------------------------------------
    // Phase 1: Deactivate all the passive effects.
    owner->effectManager.removeAllPassiveEffect();
    // Iterate the character's skills.
    for (const auto & skillData : skills)
    {
        // ---------------------------------------------------------------------
        // Phase 2: Activate all the passive effects due to the skill.
        // Get the skill.
        const auto & skill = skillData->skill;
        // Save the skill rank.
        auto skillRank = skillData->getSkillRank().to<int>();
        // Create a new skill effect.
        auto skillEffect = EffectFactory::skillEffect(owner, skill->name);
        // Iterate through the modifiers of the skill.
        for (auto it2 : skill->abilityModifier)
        {
            // Evaluate the modifier.
            auto modifier = it2.second * skillRank;
            // Otherwise add the modifier based on the skill rank.
            auto it3 = skillEffect.effectAbilityModifier.find(it2.first);
            if (it3 == skillEffect.effectAbilityModifier.end())
            {
                skillEffect.effectAbilityModifier.insert(
                    std::make_pair(it2.first, modifier));
            }
            else
            {
                it3->second += modifier;
            }
        }
        // Iterate through the modifiers of the skill.
        for (auto it2 : skill->combatModifier)
        {
            // Evaluate the modifier.
            auto modifier = it2.second * skillRank;
            // Otherwise add the modifier based on the skill rank.
            auto it3 = skillEffect.effectCombatModifier.find(it2.first);
            if (it3 == skillEffect.effectCombatModifier.end())
            {
                skillEffect.effectCombatModifier.insert(
                    std::make_pair(it2.first, modifier));
            }
            else
            {
                it3->second += modifier;
            }
        }
        // Iterate through the modifiers of the skill.
        for (auto it2 : skill->statusModifier)
        {
            // Evaluate the modifier.
            auto modifier = it2.second * skillRank;
            // Otherwise add the modifier based on the skill rank.
            auto it3 = skillEffect.effectStatusModifier.find(it2.first);
            if (it3 == skillEffect.effectStatusModifier.end())
            {
                skillEffect.effectStatusModifier.insert(
                    std::make_pair(it2.first, modifier));
            }
            else
            {
                it3->second += modifier;
            }
        }
        // Iterate through the modifiers of the skill.
        for (auto it2 : skill->knowledge)
        {
            // Otherwise add the modifier based on the skill rank.
            auto it3 = skillEffect.effectKnowledge.find(it2.first);
            if (it3 == skillEffect.effectKnowledge.end())
            {
                skillEffect.effectKnowledge.insert(
                    std::make_pair(it2.first, skillRank));
            }
            else
            {
                it3->second += skillRank;
            }
        }
        // ---------------------------------------------------------------------
        // Phase 3: Add the passive effect to the character.
        owner->effectManager.addPassiveEffect(skillEffect);
    }
}

void SkillManager::checkIfUnlockedSkills()
{
    std::vector<VnumType> masterSkills;
    for (const auto & skillData : skills)
    {
        // Get the current skill rank.
        if (skillData->getSkillRank() == SkillRank::Master)
        {
            masterSkills.emplace_back(skillData->skillVnum);
        }
    }
    for (const auto & skill : Mud::instance().mudSkills)
    {
        // Check if the character posses the skill.
        if (this->findSkill(skill->vnum) != nullptr)
        {
            continue;
        }
        // Check if the character has the pre-requisite for the skill.
        auto hasRequiredSkill = true;
        for (const auto & requiredSkill : skill->requiredSkills)
        {
            auto findIt = std::find(masterSkills.begin(),
                                    masterSkills.end(),
                                    requiredSkill);
            if (findIt == masterSkills.end())
            {
                hasRequiredSkill = false;
                break;
            }
        }
        // Add the new skill to the character.
        if (hasRequiredSkill)
        {
            this->addSkill(skill);
        }
    }
}

void SkillManager::improveAbility(const Ability & abilityModifier)
{
    for (auto & skillData : skills)
    {
        // Get the skill.
        const auto & skill = skillData->skill;
        // Check if the skill provides the given modifier.
        if (skill->abilityModifier.find(abilityModifier) ==
            skill->abilityModifier.end())
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
            // Update the effects due to the skills.
            this->updateSkillEffects();
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
        if (skill->statusModifier.find(statusModifier) ==
            skill->statusModifier.end())
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
            // Update the effects due to the skills.
            this->updateSkillEffects();
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
        if (skill->combatModifier.find(combatModifier) ==
            skill->combatModifier.end())
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
            // Update the effects due to the skills.
            this->updateSkillEffects();
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
        if (skill->knowledge.find(knowledge) == skill->knowledge.end())
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
            // Update the effects due to the skills.
            this->updateSkillEffects();
        }
    }
}

