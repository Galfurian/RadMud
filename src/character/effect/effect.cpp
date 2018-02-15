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
               std::string _messageExpire,
               std::function<void(Character * character)> _expireFunction) :
    affected(_affected),
    name(std::move(_name)),
    remainingTic(_remainingTic),
    messageActivate(std::move(_messageActivate)),
    messageExpire(std::move(_messageExpire)),
    expireFunction(std::move(_expireFunction))
{
    Logger::log(LogLevel::Debug, "Created effect %s.", name);
}


Effect::~Effect()
{
    Logger::log(LogLevel::Debug, "Deleted effect %s.", name);
}
