/// @file   production.cpp
/// @brief  Implmement production variables and methods.
/// @author Enrico Fraccaroli
/// @date   Aug 11 2015
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

#include "production.hpp"

#include "character.hpp"
#include "logger.hpp"
#include "mud.hpp"

Production::Production() :
    vnum(-1),
    name(),
    profession(),
    difficulty(),
    time(),
    assisted(),
    outcome(),
    quantity(),
    tools(),
    ingredients(),
    workbench(ToolType::None),
    requiredKnowledge()
{
    // Nothing to do.
}

Production::~Production()
{
//    Logger::log(LogLevel::Debug,
//                "Deleted production\t[%s]\t\t(%s)",
//                ToString(this->vnum),
//                this->name);
}

bool Production::check()
{
    if (vnum <= 0) return false;
    if (name.empty()) return false;
    if (profession == nullptr) return false;
    if (difficulty <= 0) return false;
    if (time <= 0) return false;
    if (outcome == nullptr) return false;
    if (quantity <= 0) return false;
    if (tools.empty()) return false;
    for (auto it : tools)
    {
        if (it == ToolType::None) return false;
    }
    for (auto it : ingredients)
    {
        if (it.first == ResourceType::None) return false;
        if (it.second <= 0) return false;
    }
    return true;
}

std::string Production::getName()
{
    return ToLower(name);
}

std::string Production::getNameCapital()
{
    return name;
}

bool Production::hasRequiredKnowledge(Character * character)
{
    for (auto knowledge : requiredKnowledge)
    {
        Logger::log(LogLevel::Debug, "Required :%s", knowledge.toString());
        auto knowledgeLevel = character->effects.getKnowledge(knowledge);
        if (knowledgeLevel <= 0)
        {
            return false;
        }
    }
    return true;
}
