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
    abilityModifier(),
    combatModifier(),
    statusModifier(),
    knowledge(),
    activeEffects(),
    pendingEffects()
{
    // Nothing to do.
}

EffectManager::~EffectManager()
{
    // Nothing to do.
}

void EffectManager::forceAddEffect(const Effect & effect)
{
    bool present = false;
    for (auto & iterator : pendingEffects)
    {
        if (iterator.name == effect.name)
        {
            if (iterator.remainingTic < effect.remainingTic)
            {
                iterator.remainingTic = effect.remainingTic;
            }
            present = true;
            break;
        }
    }
    if (!present)
    {
        activeEffects.push_back(effect);
        this->sortList();
    }
}

void EffectManager::addPendingEffect(const Effect & effect)
{
    bool present = false;
    for (auto & iterator : pendingEffects)
    {
        if (iterator.name == effect.name)
        {
            if (iterator.remainingTic < effect.remainingTic)
            {
                iterator.remainingTic = effect.remainingTic;
            }
            present = true;
            break;
        }
    }
    if (!present)
    {
        pendingEffects.push_back(effect);
        this->sortList();
    }
}

bool EffectManager::effectActivate(std::vector<std::string> & messages)
{
    for (auto & iterator : pendingEffects)
    {
        bool present = false;
        for (auto & iterator2 : activeEffects)
        {
            if (iterator.name == iterator2.name)
            {
                messages.push_back("An effect cannot be activated.");
                present = true;
                break;
            }
        }
        if (!present)
        {
            if (!iterator.messageActivate.empty())
            {
                messages.push_back(iterator.messageActivate);
            }
            activeEffects.push_back(iterator);
            this->sortList();
        }
    }
    pendingEffects.clear();
    return !messages.empty();
}

bool EffectManager::effectUpdate(std::vector<std::string> & messages)
{
    for (auto it = activeEffects.begin(); it != activeEffects.end();)
    {
        if (it->update())
        {
            if (!it->messageExpire.empty())
            {
                messages.push_back(it->messageExpire);
            }
            it = activeEffects.erase(it);
            this->sortList();
        }
        else
        {
            ++it;
        }
    }
    return !messages.empty();
}

void EffectManager::sortList()
{
    std::sort(activeEffects.begin(), activeEffects.end(), std::less<Effect>());
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

int EffectManager::getHealthMod() const
{
    return 0;
}

int EffectManager::getStaminaMod() const
{
    return 0;
}

int EffectManager::getMeleeHitMod() const
{
    return 0;
}

int EffectManager::getMeleeDamMod() const
{
    return 0;
}

int EffectManager::getRangedHitMod() const
{
    return 0;
}

int EffectManager::getRangedDamMod() const
{
    return 0;
}

int EffectManager::getAbilityModifier(const Ability &) const
{
    return 0;
}
