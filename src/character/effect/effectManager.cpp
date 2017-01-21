/// @file   effectList.hpp
/// @brief  Implements a container for effects.
/// @author Enrico Fraccaroli
/// @date   Oct 22 2016
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

#include "effectManager.hpp"

EffectManager::EffectManager() :
    activeAbilityModifier(),
    activeCombatModifier(),
    activeStatusModifier(),
    activeKnowledge(),
    activeEffects(),
    pendingEffects(),
    passiveEffects()
{
    // Nothing to do.
}

EffectManager::~EffectManager()
{
    // Nothing to do.
}

void EffectManager::addPassiveEffect(const Effect & effect)
{
    // First check if the same effect is already active.
    for (auto & iterator : passiveEffects)
    {
        // Check the equality between the effects.
        if (iterator == effect)
        {
            // If the effect is already active, set the remaining time to the
            // longest one.
            if (iterator.remainingTic < effect.remainingTic)
            {
                iterator.remainingTic = effect.remainingTic;
            }
            return;
        }
    }
    // Add the effect to the active effects.
    passiveEffects.push_back(effect);
    // Activate the effect.
    this->activateEffect(effect);
}

void EffectManager::removePassiveEffect(const Effect & effect)
{
    // Iterate trough the active effects.
    for (auto it = passiveEffects.begin(); it != passiveEffects.end(); ++it)
    {
        if (it->name == effect.name)
        {
            // Deactivate the effect.
            this->deactivateEffect(*it);
            // Remove the effect from the list of active effects.
            passiveEffects.erase(it);
            // Stop the loop.
            break;
        }
    }
}

void EffectManager::removeAllPassiveEffect()
{
    // Iterate trough the active effects.
    for (auto it = passiveEffects.begin(); it != passiveEffects.end();)
    {
        // Deactivate the effect.
        this->deactivateEffect(*it);
        // Remove the effect from the list of active effects.
        it = passiveEffects.erase(it);
    }
}

void EffectManager::forceAddEffect(const Effect & effect)
{
    // First check if the same effect is already active.
    for (auto & iterator : pendingEffects)
    {
        // Check the equality between the effects.
        if (iterator == effect)
        {
            // If the effect is already active, set the remaining time to the
            // longest one.
            if (iterator.remainingTic < effect.remainingTic)
            {
                iterator.remainingTic = effect.remainingTic;
            }
            return;
        }
    }
    // Add the effect to the active effects.
    activeEffects.push_back(effect);
    // Activate the effect.
    this->activateEffect(effect);
    // Sort the list of active effects.
    this->sortList();
}

void EffectManager::addPendingEffect(const Effect & effect)
{
    // First check if the same effect is already in the pending queue.
    for (auto & iterator : pendingEffects)
    {
        // Check the equality between the effects.
        if (iterator == effect)
        {
            // If the effect is already active, set the remaining time to the
            // longest one.
            if (iterator.remainingTic < effect.remainingTic)
            {
                iterator.remainingTic = effect.remainingTic;
            }
            return;
        }
    }
    // Add the effect to the pending effects.
    pendingEffects.push_back(effect);
}

bool EffectManager::effectActivate(std::vector<std::string> & messages)
{
    // Define a function which checks if an effect is already active.
    auto CheckEffectPresence = [&](Effect & effect)
    {
        for (auto & activeEffect : activeEffects)
        {
            if (effect == activeEffect)
            {
                return true;
            }
        }
        return false;
    };
    // First check if the same effect is already active.
    for (auto & pendingEffect : pendingEffects)
    {
        if (!CheckEffectPresence(pendingEffect))
        {
            if (!pendingEffect.messageActivate.empty())
            {
                messages.push_back(pendingEffect.messageActivate);
            }
            // Add the effect to the active effects.
            activeEffects.push_back(pendingEffect);
            // Activate the effect.
            this->activateEffect(pendingEffect);
            // Sort the list of active effects.
            this->sortList();
        }
    }
    // Empty out the list of pending effects.
    pendingEffects.clear();
    return !messages.empty();
}

bool EffectManager::effectUpdate(std::vector<std::string> & messages)
{
    // Iterate trough the active effects.
    auto it = activeEffects.begin();
    while (it != activeEffects.end())
    {
        // If the effect is expired, remove it.
        if (it->update())
        {
            // If the effect has an expiration message, add it to the list of
            // output messages.
            if (!it->messageExpire.empty())
            {
                messages.push_back(it->messageExpire);
            }
            // Deactivate the effect.
            this->deactivateEffect(*it);
            // Remove the effect from the list of active effects.
            it = activeEffects.erase(it);
            // Sort the list of active effects.
            this->sortList();
            continue;
        }
        ++it;
    }
    return !messages.empty();
}

std::vector<Effect>::iterator EffectManager::begin()
{
    return activeEffects.begin();
}

std::vector<Effect>::const_iterator EffectManager::begin() const
{
    return activeEffects.begin();
}

std::vector<Effect>::iterator EffectManager::end()
{
    return activeEffects.end();
}

std::vector<Effect>::const_iterator EffectManager::end() const
{
    return activeEffects.end();
}

int EffectManager::getAbilityModifier(const Ability & modifier) const
{
    auto it = activeAbilityModifier.find(modifier);
    if (it != activeAbilityModifier.end())
    {
        return it->second;
    }
    return 0;
}

int EffectManager::getCombatModifier(const CombatModifier & modifier) const
{
    auto it = activeCombatModifier.find(modifier);
    if (it != activeCombatModifier.end())
    {
        return it->second;
    }
    return 0;
}

int EffectManager::getStatusModifier(const StatusModifier & modifier) const
{
    auto it = activeStatusModifier.find(modifier);
    if (it != activeStatusModifier.end())
    {
        return it->second;
    }
    return 0;
}

int EffectManager::getKnowledge(const Knowledge & knowledge) const
{
    auto it = activeKnowledge.find(knowledge);
    if (it != activeKnowledge.end())
    {
        return it->second;
    }
    return 0;
}

void EffectManager::activateEffect(const Effect & effect)
{
    // Add the ability modifier of the effect to the pool.
    activeAbilityModifier += effect.effectAbilityModifier;
    // Add the combat modifier of the effect to the pool.
    activeCombatModifier += effect.effectCombatModifier;
    // Add the status modifier of the effect to the pool.
    activeStatusModifier += effect.effectStatusModifier;
    // Add the knowledge of the effect to the pool.
    activeKnowledge += effect.effectKnowledge;
}

void EffectManager::deactivateEffect(const Effect & effect)
{
    // Remove the ability modifier of the effect to the pool.
    activeAbilityModifier -= effect.effectAbilityModifier;
    // Remove the combat modifier of the effect to the pool.
    activeCombatModifier -= effect.effectCombatModifier;
    // Remove the status modifier of the effect to the pool.
    activeStatusModifier -= effect.effectStatusModifier;
    // Remove the knowledge of the effect to the pool.
    activeKnowledge -= effect.effectKnowledge;
}

void EffectManager::sortList()
{
    std::sort(activeEffects.begin(), activeEffects.end(), std::less<Effect>());
}
