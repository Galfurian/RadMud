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

#include "logger.hpp"

Effect::Effect(Character * _affected,
               std::string _name,
               unsigned int _remainingTic,
               std::string _messageActivate,
               std::string _messageExpire,
               std::function<void(Character * character)> _expireFunction) :
    affected(_affected),
    name(_name),
    remainingTic(_remainingTic),
    messageActivate(_messageActivate),
    messageExpire(_messageExpire),
    expireFunction(_expireFunction),
    effectAbilityModifier(),
    effectCombatModifier(),
    effectStatusModifier(),
    effectKnowledge()
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

std::map<AbilityModifier, int> & operator+=(
    std::map<AbilityModifier, int> & left,
    const std::map<AbilityModifier, int> & right)
{
    for (auto & rightModifier : right)
    {
        auto leftModifier = left.find(rightModifier.first);
        if (leftModifier != left.end())
        {
            leftModifier->second += rightModifier.second;
        }
        else
        {
            left.insert(rightModifier);
        }
    }
    return left;
}

std::map<AbilityModifier, int> & operator-=(
    std::map<AbilityModifier, int> & left,
    const std::map<AbilityModifier, int> & right)
{
    for (auto & rightModifier : right)
    {
        auto leftModifier = left.find(rightModifier.first);
        if (leftModifier != left.end())
        {
            leftModifier->second -= rightModifier.second;
            if (leftModifier->second < 0)
            {
                leftModifier->second = 0;
            }
        }
    }
    return left;
}

std::map<CombatModifier, int> & operator+=(
    std::map<CombatModifier, int> & left,
    const std::map<CombatModifier, int> & right)
{
    for (auto & rightModifier : right)
    {
        auto leftModifier = left.find(rightModifier.first);
        if (leftModifier != left.end())
        {
            leftModifier->second += rightModifier.second;
        }
        else
        {
            left.insert(rightModifier);
        }
    }
    return left;
}

std::map<CombatModifier, int> & operator-=(
    std::map<CombatModifier, int> & left,
    const std::map<CombatModifier, int> & right)
{
    for (auto & rightModifier : right)
    {
        auto leftModifier = left.find(rightModifier.first);
        if (leftModifier != left.end())
        {
            leftModifier->second -= rightModifier.second;
            if (leftModifier->second < 0)
            {
                leftModifier->second = 0;
            }
        }
    }
    return left;
}

std::map<StatusModifier, int> & operator+=(
    std::map<StatusModifier, int> & left,
    const std::map<StatusModifier, int> & right)
{
    for (auto & rightModifier : right)
    {
        auto leftModifier = left.find(rightModifier.first);
        if (leftModifier != left.end())
        {
            leftModifier->second += rightModifier.second;
        }
        else
        {
            left.insert(rightModifier);
        }
    }
    return left;
}

std::map<StatusModifier, int> & operator-=(
    std::map<StatusModifier, int> & left,
    const std::map<StatusModifier, int> & right)
{
    for (auto & rightModifier : right)
    {
        auto leftModifier = left.find(rightModifier.first);
        if (leftModifier != left.end())
        {
            leftModifier->second -= rightModifier.second;
            if (leftModifier->second < 0)
            {
                leftModifier->second = 0;
            }
        }
    }
    return left;
}

std::map<Knowledge, bool> & operator+=(
    std::map<Knowledge, bool> & left,
    const std::map<Knowledge, bool> & right)
{
    for (auto & rightModifier : right)
    {
        auto leftModifier = left.find(rightModifier.first);
        if (leftModifier != left.end())
        {
            leftModifier->second = true;
        }
        else
        {
            left.insert(rightModifier);
        }
    }
    return left;
}

std::map<Knowledge, bool> & operator-=(
    std::map<Knowledge, bool> & left,
    const std::map<Knowledge, bool> & right)
{
    for (auto & rightModifier : right)
    {
        auto leftModifier = left.find(rightModifier.first);
        if (leftModifier != left.end())
        {
            leftModifier->second = false;
        }
    }
    return left;
}