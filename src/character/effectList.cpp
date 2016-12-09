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

#include "effectList.hpp"


EffectList::EffectList() :
    activeEffects(),
    pendingEffects()
{
    // Nothing to do.
}

EffectList::~EffectList()
{
    // Nothing to do.
}

int EffectList::getHealthMod() const
{
    int result = 0;
    for (auto & it : activeEffects) result += it.health;
    return result;
}

int EffectList::getStaminaMod() const
{
    int result = 0;
    for (auto & it : activeEffects) result += it.stamina;
    return result;
}

int EffectList::getMeleeHitMod() const
{
    int result = 0;
    for (auto & it : activeEffects) result += it.meleeHit;
    return result;
}

int EffectList::getMeleeDamMod() const
{
    int result = 0;
    for (auto & it : activeEffects) result += it.meleeDamage;
    return result;
}

int EffectList::getRangedHitMod() const
{
    int result = 0;
    for (auto & it : activeEffects) result += it.rangedHit;
    return result;
}

int EffectList::getRangedDamMod() const
{
    int result = 0;
    for (auto & it : activeEffects) result += it.rangedDamage;
    return result;
}

int EffectList::getAbilityModifier(const Ability & ability) const
{
    int result = 0;
    for (const_iterator it = activeEffects.begin(); it != activeEffects.end(); ++it)
    {
        auto abilityIt = it->abilities.find(ability);
        if (abilityIt != it->abilities.end())
        {
            result += abilityIt->second;
        }
    }
    return result;
}

void EffectList::forceAddEffect(const Effect & effect)
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

void EffectList::addPendingEffect(const Effect & effect)
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

bool EffectList::effectActivate(std::vector<std::string> & messages)
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

bool EffectList::effectUpdate(std::vector<std::string> & messages)
{
    for (auto iterator = activeEffects.begin(); iterator != activeEffects.end();)
    {
        if (iterator->update())
        {
            if (!iterator->messageExpire.empty())
            {
                messages.push_back(iterator->messageExpire);
            }
            iterator = activeEffects.erase(iterator);
            this->sortList();
        }
        else
        {
            ++iterator;
        }
    }
    return !messages.empty();
}

void EffectList::sortList()
{
    std::sort(activeEffects.begin(), activeEffects.end(), std::less<Effect>());
}

EffectList::iterator EffectList::begin()
{
    return activeEffects.begin();
}

EffectList::const_iterator EffectList::begin() const
{
    return activeEffects.begin();
}

EffectList::iterator EffectList::end()
{
    return activeEffects.end();
}

EffectList::const_iterator EffectList::end() const
{
    return activeEffects.end();
}