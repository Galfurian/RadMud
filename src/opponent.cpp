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

#include "utils.hpp"
#include "character.hpp"
#include "logger.hpp"
#include "opponent.hpp"

Aggression::Aggression(Character * _aggressor, unsigned int _aggression) :
        aggressor(_aggressor),
        aggression(_aggression)
{
    // Nothing to do.
}
Aggression::~Aggression()
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
    return (this->aggressor->name == source.aggressor->name);
}
bool Aggression::operator==(const Character * source) const
{
    return (this->aggressor->name == source->name);
}

OpponentsList::OpponentsList(Character * _owner) :
        owner(_owner),
        aggressionList()
{
    // Nothing to do.
}

OpponentsList::~OpponentsList()
{
    // Nothing to do.
}

bool OpponentsList::addOpponent(Character * character, unsigned int initAggro)
{
    bool ret = false;
    auto iterator = std::find(aggressionList.begin(), aggressionList.end(), character);
    if (iterator == aggressionList.end())
    {
        if (initAggro == 0)
        {
            initAggro = this->getInitialAggro(character);
        }
        // Add the aggressor.
        aggressionList.push_back(Aggression(character, initAggro));
        // Sort the list.
        this->sortList();
        // Set return value to success.
        ret = true;
        Logger::log(
            LogLevel::Debug,
            "%s engage %s with %s.",
            owner->getNameCapital(),
            character->getName(),
            ToString(initAggro));
    }
    return ret;
}

bool OpponentsList::remOpponent(Character * character)
{
    auto iterator = std::find(aggressionList.begin(), aggressionList.end(), character);
    if (iterator != aggressionList.end())
    {
        Logger::log(
            LogLevel::Debug,
            "%s disengage %s",
            owner->getNameCapital(),
            character->getName());
        iterator = aggressionList.erase(iterator, aggressionList.end());
        return true;
    }
    return false;
}

bool OpponentsList::hasOpponent(Character * character)
{
    bool ret = false;
    for (auto it : aggressionList)
    {
        if (it == character)
        {
            ret = true;
            break;
        }
    }
    return ret;
}

bool OpponentsList::hasOpponents() const
{
    return (!aggressionList.empty());
}

bool OpponentsList::setAggro(Character * character, unsigned int newAggression)
{
    bool ret = false;
    auto iterator = std::find(aggressionList.begin(), aggressionList.end(), character);
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

Aggression * OpponentsList::getTopAggro()
{
    if (!aggressionList.empty())
    {
        return &(*aggressionList.begin());
    }
    else
    {
        return nullptr;
    }
}

bool OpponentsList::moveToTopAggro(Character * character)
{
    // Check if the character is a valid opponent.
    if (!this->hasOpponent(character))
    {
        return false;
    }
    Aggression * topAggressor = this->getTopAggro();
    if (topAggressor == nullptr)
    {
        return false;
    }
    // Check if the character is already the top aggro.
    if (topAggressor->aggressor == character)
    {
        return false;
    }
    // Retrieve the top aggro.
    unsigned int currentAggro = this->getAggro(character);
    unsigned int topAggro = currentAggro;
    for (auto it : this->aggressionList)
    {
        if (it.aggression > topAggro)
        {
            topAggro = it.aggression;
        }
    }
    // Just set the aggro of the character enough to be put on the first place.
    this->setAggro(character, topAggro + 1);
    return true;
}

unsigned int OpponentsList::getInitialAggro(Character * character)
{
    if (owner->level > character->level)
    {
        return (owner->level - character->level);
    }
    else
    {
        return (character->level - owner->level);
    }
}

unsigned int OpponentsList::getAggro(Character * character)
{
    unsigned int ret = 0;
    for (auto it : aggressionList)
    {
        if (it == character)
        {
            ret = it.aggression;
            break;
        }
    }
    return ret;
}

std::size_t OpponentsList::getSize()
{
    return this->aggressionList.size();
}

void OpponentsList::checkList()
{
    for (std::vector<Aggression>::iterator it = aggressionList.begin(); it != aggressionList.end();)
    {
        // Check if the aggressor is null.
        if (it->aggressor == nullptr)
        {
            it = aggressionList.erase(it);
        }
        // Check if the aggressor is nowhere.
        else if (it->aggressor->room == nullptr)
        {
            it = aggressionList.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

OpponentsList::iterator OpponentsList::begin()
{
    return this->aggressionList.begin();
}

OpponentsList::iterator OpponentsList::end()
{
    return this->aggressionList.end();
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
