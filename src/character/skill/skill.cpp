/// @file   skill.cpp
/// @brief  Implements the skill functions.
/// @author Enrico Fraccaroli
/// @date   Aug 23 2014
/// @copyright
/// Copyright (c) 2016 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
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

#include "skill.hpp"
#include "effectManager.hpp"
#include "effectFactory.hpp"
#include "skillRank.hpp"
#include "character.hpp"
#include "logger.hpp"
#include "mud.hpp"

Skill::Skill() :
    vnum(),
    name(),
    description(),
    ability(),
    stage(),
    requiredSkills(),
    abilityModifier(),
    combatModifier(),
    statusModifier(),
    knowledge()
{
    // Nothing to do.
}

Skill::~Skill()
{
//    Logger::log(LogLevel::Debug,
//                "Deleted skill\t\t[%s]\t\t(%s)",
//                ToString(this->vnum),
//                this->name);
}

bool Skill::check()
{
    if (vnum <= 0) return false;
    if (name.empty())return false;
    if (description.empty())return false;
    if (ability == Ability::None)return false;
    if (stage <= 0)return false;
    return true;
}

void Skill::updateSkillEffects(Character * character)
{
    // TODO: Determine the increment brought by each skill based on its rank.
    // TODO: Right now the modifier is based on the rank of the skill.
    // -------------------------------------------------------------------------
    // Phase 1: Deactivate all the passive effects.
    character->effects.removeAllPassiveEffect();
    // Iterate the character's skills.
    for (const auto & skillData : character->skillManager.skills)
    {
        // ---------------------------------------------------------------------
        // Phase 2: Activate all the passive effects due to the skill.
        // Get the skill.
        const auto & skill = skillData->skill;
        // Save the skill rank.
        auto skillRank = skillData->getSkillRank().to<int>();
        // Create a new skill effect.
        auto skillEffect =
            EffectFactory::skillEffect(character, skill->name);
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
        character->effects.addPassiveEffect(skillEffect);
    }
}

void Skill::checkIfUnlockedSkills(Character * character)
{
    std::vector<VnumType> masterSkills;
    for (const auto & skillData : character->skillManager.skills)
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
        if (character->skillManager.findSkill(skill->vnum) != nullptr)
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
            character->skillManager.addSkill(skill);
        }
    }
}

void Skill::improveSkillAbilityModifier(Character * character,
                                        const Ability & abilityModifier)
{
    for (auto & skillData : character->skillManager.skills)
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
        skillData->skillLevel += 50;
        // Get the new skill rank.
        auto newSkillRank = skillData->getSkillRank();
        // Check if the skill has advanced on the ranking.
        if (skillRank < newSkillRank)
        {
            // Before updating the effects of the skills, check if the
            // character has unlocked some new skills.
            if (newSkillRank == SkillRank::Master)
            {
                Skill::checkIfUnlockedSkills(character);
            }
            // Update the effects due to the skills.
            Skill::updateSkillEffects(character);
        }
    }
}

void Skill::improveSkillStatusModifier(Character * character,
                                       const StatusModifier & statusModifier)
{
    for (auto & skillData : character->skillManager.skills)
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
        skillData->skillLevel += 50;
        // Get the new skill rank.
        auto newSkillRank = skillData->getSkillRank();
        // Check if the skill has advanced on the ranking.
        if (skillRank < newSkillRank)
        {
            // Before updating the effects of the skills, check if the
            // character has unlocked some new skills.
            if (newSkillRank == SkillRank::Master)
            {
                Skill::checkIfUnlockedSkills(character);
            }
            // Update the effects due to the skills.
            Skill::updateSkillEffects(character);
        }
    }
}

void Skill::improveSkillCombatModifier(Character * character,
                                       const CombatModifier & combatModifier)
{
    for (auto & skillData : character->skillManager.skills)
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
        skillData->skillLevel += 50;
        // Get the new skill rank.
        auto newSkillRank = skillData->getSkillRank();
        // Check if the skill has advanced on the ranking.
        if (skillRank < newSkillRank)
        {
            // Before updating the effects of the skills, check if the
            // character has unlocked some new skills.
            if (newSkillRank == SkillRank::Master)
            {
                Skill::checkIfUnlockedSkills(character);
            }
            // Update the effects due to the skills.
            Skill::updateSkillEffects(character);
        }
    }
}

void Skill::improveSkillKnowledge(Character * character,
                                  const Knowledge & knowledge)
{
    for (auto & skillData : character->skillManager.skills)
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
        skillData->skillLevel += 50;
        // Get the new skill rank.
        auto newSkillRank = skillData->getSkillRank();
        // Check if the skill has advanced on the ranking.
        if (skillRank < newSkillRank)
        {
            // Before updating the effects of the skills, check if the
            // character has unlocked some new skills.
            if (newSkillRank == SkillRank::Master)
            {
                Skill::checkIfUnlockedSkills(character);
            }
            // Update the effects due to the skills.
            Skill::updateSkillEffects(character);
        }
    }
}
