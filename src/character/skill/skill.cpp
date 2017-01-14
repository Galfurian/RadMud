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
#include "player.hpp"

Skill::Skill() :
    vnum(),
    name(),
    description(),
    attribute(),
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
    assert(vnum > 0);
    assert(!name.empty());
    assert(!description.empty());
    assert(attribute > 0);
    assert(stage > 0);
    return true;
}

void Skill::updateSkillEffects(Player * player)
{
    // TODO: Determine the increment brought by each skill based on its rank.
    // -------------------------------------------------------------------------
    // Phase 1: Deactivate all the passive effects.
    player->effects.removeAllPassiveEffect();
    // -------------------------------------------------------------------------
    // Phase 2: Activate all the passive effects due to the skills.
    // Create a new skill effect.
    auto skillEffect = EffectFactory::skillEffect(player, name);
    // Iterate the player's skills.
    for (auto it : player->skills)
    {
        // Save the skill.
        auto skill = it.first;
        // Save the skill rank.
        auto skillRank = it.second;
        // Iterate through the modifiers of the skill.
        for (auto it2 : skill->abilityModifier)
        {
            // Skip the ability modifier if the rank is lower.
            if (skillRank < it2.second) continue;
            // Otherwise add the modifier based on the skill rank.
            auto it3 = skillEffect.effectAbilityModifier.find(it2.first);
            if (it3 == skillEffect.effectAbilityModifier.end())
            {
                skillEffect.effectAbilityModifier.insert(it2);
            }
            else
            {
                it3->second += 1;
            }
        }
        // Iterate through the modifiers of the skill.
        for (auto it2 : skill->combatModifier)
        {
            // Skip the ability modifier if the rank is lower.
            if (skillRank < it2.second) continue;
            // Otherwise add the modifier based on the skill rank.
            auto it3 = skillEffect.effectCombatModifier.find(it2.first);
            if (it3 == skillEffect.effectCombatModifier.end())
            {
                skillEffect.effectCombatModifier.insert(it2);
            }
            else
            {
                it3->second += 1;
            }
        }
        // Iterate through the modifiers of the skill.
        for (auto it2 : skill->statusModifier)
        {
            // Skip the ability modifier if the rank is lower.
            if (skillRank < it2.second) continue;
            // Otherwise add the modifier based on the skill rank.
            auto it3 = skillEffect.effectStatusModifier.find(it2.first);
            if (it3 == skillEffect.effectStatusModifier.end())
            {
                skillEffect.effectStatusModifier.insert(it2);
            }
            else
            {
                it3->second += 1;
            }
        }
        // Iterate through the modifiers of the skill.
        for (auto it2 : skill->knowledge)
        {
            // Skip the ability modifier if the rank is lower.
            if (skillRank < it2.second) continue;
            // Otherwise add the modifier based on the skill rank.
            auto it3 = skillEffect.effectKnowledge.find(it2.first);
            if (it3 == skillEffect.effectKnowledge.end())
            {
                skillEffect.effectKnowledge.insert(it2);
            }
            else
            {
                it3->second = true;
            }
        }
    }
    // -------------------------------------------------------------------------
    // Phase 3: Add the passive effect to the player.
    player->effects.addPassiveEffect(skillEffect);
}