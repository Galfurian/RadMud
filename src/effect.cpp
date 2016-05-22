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

Effect::Effect(std::string _name, int _expires, std::string _messageActivate, std::string _messageFade) :
        name(_name),
        expires(_expires),
        messageActivate(_messageActivate),
        messageExpire(_messageFade),
        health_mod(),
        stamina_mod(),
        str_mod(),
        agi_mod(),
        per_mod(),
        con_mod(),
        int_mod(),
        hit_mod(),
        dam_mod()
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

EffectList::EffectList() :
        activeEffects(),
        pendingEffects()
{
}
EffectList::~EffectList()
{

}

int EffectList::getHealthMod()
{
    int result = 0;
    for (auto iterator : activeEffects)
    {
        result += iterator.health_mod;
    }
    return result;
}

int EffectList::getStaminaMod()
{
    int result = 0;
    for (auto iterator : activeEffects)
    {
        result += iterator.stamina_mod;
    }
    return result;
}

int EffectList::getStrMod()
{
    int result = 0;
    for (auto iterator : activeEffects)
    {
        result += iterator.str_mod;
    }
    return result;
}

int EffectList::getAgiMod()
{
    int result = 0;
    for (auto iterator : activeEffects)
    {
        result += iterator.agi_mod;
    }
    return result;
}

int EffectList::getPerMod()
{
    int result = 0;
    for (auto iterator : activeEffects)
    {
        result += iterator.per_mod;
    }
    return result;
}

int EffectList::getConMod()
{
    int result = 0;
    for (auto iterator : activeEffects)
    {
        result += iterator.con_mod;
    }
    return result;
}

int EffectList::getIntMod()
{
    int result = 0;
    for (auto iterator : activeEffects)
    {
        result += iterator.int_mod;
    }
    return result;
}

int EffectList::getHitMod()
{
    int result = 0;
    for (auto iterator : activeEffects)
    {
        result += iterator.hit_mod;
    }
    return result;
}

int EffectList::getDamMod()
{
    int result = 0;
    for (auto iterator : activeEffects)
    {
        result += iterator.dam_mod;
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
            activeEffects.push_back(iterator);
            messages.push_back(iterator.messageActivate);
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
        }
        else
        {
            ++iterator;
        }
    }
    return (!messages.empty());
}
