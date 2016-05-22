/// @file   combat.cpp
/// @brief  Contains implementation of combat classes.
/// @author Enrico Fraccaroli
/// @date   May 8 2016
/// @copyright
/// Copyright (c) 2014, 2015, 2016 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
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

#include "combat.hpp"
#include "utils.hpp"
#include "character.hpp"

Aggression::Aggression(Character * _aggressor, unsigned int _aggression) :
    aggressor(_aggressor), aggression(_aggression)
{
    // Nothing to do.
}
bool Aggression::operator>(const Aggression & source) const
{
    return (this->aggression > source.aggression);
}
bool Aggression::operator<(const Aggression & source) const
{
    return (this->aggression < source.aggression);
}
bool Aggression::operator==(const Aggression & source) const
{
    return (this->aggressor->getName() == source.aggressor->getName());
}
bool Aggression::operator==(const Character * source) const
{
    return (this->aggressor->getName() == source->name);
}

OpponentsList::OpponentsList() :
    aggressionList()
{
    // Nothing to do.
}

bool OpponentsList::addOpponent(Character * opponent, unsigned int itialAggression)
{
    bool ret = false;
    auto iterator = std::find(aggressionList.begin(), aggressionList.end(), opponent);
    if (iterator == aggressionList.end())
    {
        // Add the aggressor.
        aggressionList.push_back(Aggression(opponent, itialAggression));
        // Sort the list.
        this->sortList();
        // Set return value to success.
        ret = true;
    }
    return ret;
}

bool OpponentsList::hasOpponents() const
{
    return (!aggressionList.empty());
}

const Aggression & OpponentsList::getTopAggro()
{
    return (*aggressionList.begin());
}

bool OpponentsList::setAggro(Character * opponent, unsigned int newAggression)
{
    bool ret = false;
    auto iterator = std::find(aggressionList.begin(), aggressionList.end(), opponent);
    if (iterator != aggressionList.end())
    {
        // Set the new aggro.
        iterator->aggression = newAggression;
        // Sort the list.
        this->sortList();
        // Set return value to success.
        ret = true;
    }
    return ret;
}

void OpponentsList::sortList()
{
    std::sort(aggressionList.begin(), aggressionList.end(), std::greater<Aggression>());
}

void OpponentsList::printList()
{
    std::cout << "Aggro List:" << std::endl;
    for (auto it : aggressionList)
    {
        std::cout << "  [" + it.aggressor->name + "] " + ToString(it.aggression) << std::endl;
    }
}
