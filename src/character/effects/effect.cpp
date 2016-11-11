/// @file   effect.cpp
/// @brief  Implement the effect class.
/// @author Enrico Fraccaroli
/// @date   May 2 2016
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
