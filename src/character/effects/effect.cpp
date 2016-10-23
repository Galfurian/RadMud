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

Effect::Effect(Character * _affected,
               std::string _name,
               unsigned int _remainingTic,
               std::string _messageActivate,
               std::string _messageFade,
               std::function<void(Character * character)> _expireFunction,
               int _health,
               int _stamina,
               std::map<Ability, int> _abilities,
               int _meleeHit,
               int _meleeDamage,
               int _rangedHit,
               int _rangedDamage) :
    affected(_affected),
    name(_name),
    remainingTic(_remainingTic),
    messageActivate(_messageActivate),
    messageExpire(_messageFade),
    expireFunction(_expireFunction),
    health(_health),
    stamina(_stamina),
    abilities(_abilities),
    meleeHit(_meleeHit),
    meleeDamage(_meleeDamage),
    rangedHit(_rangedHit),
    rangedDamage(_rangedDamage)
{
    Logger::log(LogLevel::Debug, "Created effect %s.", name);
    // Nothing to do.
}

bool Effect::update()
{
    remainingTic--;
    if (remainingTic == 0)
    {
        if (expireFunction)
        {
            expireFunction(affected);
        }
        return true;
    }
    return false;
}

bool Effect::operator<(const Effect & right) const
{
    return remainingTic < right.remainingTic;
}
