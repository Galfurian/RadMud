/// @file   race.cpp
/// @brief  Implement race methods.
/// @author Enrico Fraccaroli
/// @date   Aug 23 2014
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

#include "character/race/race.hpp"

#include "utilities/logger.hpp"
#include "mud.hpp"

Race::Race() :
    vnum(),
    name(),
    description(),
    abilities(),
    player_allow(),
    tileSet(),
    tileId(),
    corpse(),
    bodyParts(),
    skills()
{
    // Nothing to do.
}

Race::~Race()
{
//    Logger::log(LogLevel::Debug,
//                "Deleted race\t\t[%s]\t\t(%s)",
//                ToString(this->vnum),
//                this->name);
}

bool Race::check()
{
    if (vnum <= 0) return false;
    if (name.empty()) return false;
    if (description.empty()) return false;
    return true;
}

std::string Race::getShortDescription(bool capital)
{
    std::string shortDescription = this->article + " " + this->name;
    if (capital && !shortDescription.empty())
    {
        shortDescription[0] = static_cast<char>(toupper(shortDescription[0]));
    }
    return shortDescription;
}

unsigned int Race::getAbility(const Ability & ability) const
{
    for (auto it : abilities)
    {
        if (it.first == ability)
        {
            return it.second;
        }
    }
    return 0;
}

std::string Race::getTile()
{
    if (Formatter::getFormat() == Formatter::CLIENT)
    {
        return ToString(tileSet) + ":" + ToString(tileId);
    }
    return "c";
}
