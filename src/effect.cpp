/// @file   effect.cpp
/// @brief  Implement the effect class.
/// @author Enrico Fraccaroli
/// @date   May 2 2016
/// @copyright
/// Copyright (c) 2016 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
/// Permission to use, copy, modify, and distribute this software for any
/// purpose with or without fee is hereby granted, provided that the above
/// copyright notice and this permission notice appear in all copies.
///
/// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
/// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
/// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
/// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
/// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
/// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
/// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

#include "effect.hpp"

Effect::Effect(
    std::string _name,
    int _expires,
    std::string _messageActivate,
    std::string _messageFade) :
    name(_name),
    expires(_expires),
    messageActivate(_messageActivate),
    messageExpire(_messageFade),
    health(),
    stamina(),
    abilities(),
    hit(),
    damage()
{
    // Nothing to do.
}

Effect::~Effect()
{
    // Nothing to do.
}

bool Effect::update()
{
    expires--;
    return (expires <= 0);
}

bool Effect::operator<(const Effect & right) const
{
    return expires < right.expires;
}

EffectList::EffectList() :
    activeEffects(),
    pendingEffects()
{
}

EffectList::~EffectList()
{

}

int EffectList::getHealthMod() const
{
    int result = 0;
    for (const_iterator it = activeEffects.begin(); it != activeEffects.end(); ++it)
    {
        result += it->health;
    }
    return result;
}

int EffectList::getStaminaMod() const
{
    int result = 0;
    for (const_iterator it = activeEffects.begin(); it != activeEffects.end(); ++it)
    {
        result += it->stamina;
    }
    return result;
}

int EffectList::getHitMod() const
{
    int result = 0;
    for (const_iterator it = activeEffects.begin(); it != activeEffects.end(); ++it)
    {
        result += it->hit;
    }
    return result;
}

int EffectList::getDamMod() const
{
    int result = 0;
    for (const_iterator it = activeEffects.begin(); it != activeEffects.end(); ++it)
    {
        result += it->damage;
    }
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

void EffectList::addPendingEffect(const Effect & effect)
{
    bool present = false;
    for (auto iterator : pendingEffects)
    {
        if (iterator.name == effect.name)
        {
            if (iterator.expires < effect.expires)
            {
                iterator.expires = effect.expires;
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
    for (auto iterator : pendingEffects)
    {
        bool present = false;
        for (auto iterator2 : activeEffects)
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
            messages.push_back(iterator.messageActivate);
            activeEffects.push_back(iterator);
            this->sortList();
        }
    }
    pendingEffects.clear();
    return (!messages.empty());
}

bool EffectList::effectUpdate(std::vector<std::string> & messages)
{
    for (auto iterator = activeEffects.begin(); iterator != activeEffects.end();)
    {
        if (iterator->update())
        {
            messages.push_back(iterator->messageExpire);
            iterator = activeEffects.erase(iterator);
            this->sortList();
        }
        else
        {
            ++iterator;
        }
    }
    return (!messages.empty());
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
