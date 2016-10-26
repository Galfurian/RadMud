/// @file   aggressionList.cpp
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

#include "aggressionList.hpp"

#include "utils.hpp"
#include "logger.hpp"
#include "character.hpp"

AggressionList::AggressionList(Character * _owner) :
    owner(_owner),
    aggressionList()
{
    // Nothing to do.
}

AggressionList::~AggressionList()
{
    // Nothing to do.
}

bool AggressionList::addOpponent(Character * character, unsigned int initAggro)
{
    if (!this->hasOpponent(character))
    {
        if (initAggro == 0)
        {
            initAggro = this->getInitialAggro(character);
        }
        // Add the aggressor.
        aggressionList.push_back(std::make_shared<Aggression>(character, initAggro));
        // Sort the list.
        this->sortList();
        Logger::log(LogLevel::Debug, "%s engage %s with %s.", owner->getNameCapital(), character->getName(), initAggro);
        return true;
    }
    return false;
}

bool AggressionList::remOpponent(Character * character)
{
    auto it = std::find_if(aggressionList.begin(), aggressionList.end(),
                           [&](std::shared_ptr<Aggression> const & element)
                           {
                               return element->aggressor->name == character->name;
                           });
    if (it != aggressionList.end())
    {
        Logger::log(LogLevel::Debug, "%s disengages %s", owner->getNameCapital(), character->getName());
        aggressionList.erase(it, aggressionList.end());
        return true;
    }
    return false;
}

bool AggressionList::hasOpponent(Character * character)
{
    return std::find_if(aggressionList.begin(), aggressionList.end(),
                        [&](std::shared_ptr<Aggression> const & element)
                        {
                            return element->aggressor->name == character->name;
                        }) != aggressionList.end();
}

bool AggressionList::setAggro(Character * character, unsigned int newAggression)
{
    bool ret = false;
    auto it = std::find_if(aggressionList.begin(), aggressionList.end(), [&](std::shared_ptr<Aggression> const & p)
    {
        return p->aggressor->name == character->name;
    });
    if (it != aggressionList.end())
    {
        // Set the new aggro.
        (*it)->aggression = newAggression;
        // Sort the list.
        this->sortList();
        // Set return value to success.
        ret = true;
    }
    return ret;
}

std::shared_ptr<Aggression> AggressionList::getTopAggro()
{
    if (aggressionList.empty())
    {
        return nullptr;
    }
    return *(aggressionList.begin());
}

bool AggressionList::moveToTopAggro(Character * character)
{
    // Check if the character is a valid opponent.
    if (!this->hasOpponent(character))
    {
        return false;
    }
    auto topAggressor = this->getTopAggro();
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
    auto currentAggro = this->getAggro(character);
    auto topAggro = currentAggro;
    for (auto it : aggressionList)
    {
        if (it->aggression > topAggro)
        {
            topAggro = it->aggression;
        }
    }
    // Just set the aggro of the character enough to be put on the first place.
    this->setAggro(character, topAggro + 1);
    return true;
}

unsigned int AggressionList::getInitialAggro(Character * character)
{
    if (owner->level > character->level)
    {
        return (owner->level - character->level);
    }
    return (character->level - owner->level);
}

unsigned int AggressionList::getAggro(Character * character)
{
    for (auto it : aggressionList)
    {
        if (it->aggressor == character)
        {
            return it->aggression;
        }
    }
    return 0;
}

std::size_t AggressionList::getSize()
{
    return aggressionList.size();
}

bool AggressionList::empty()
{
    return aggressionList.empty();
}

void AggressionList::checkList()
{
    auto temporaryList = aggressionList;
    for (auto it = temporaryList.begin(); it != temporaryList.end(); ++it)
    {
        // Check if the aggressor is null.
        if ((*it)->aggressor == nullptr)
        {
            this->remOpponent((*it)->aggressor);
        }
            // Check if the aggressor is nowhere.
        else if ((*it)->aggressor->room == nullptr)
        {
            this->remOpponent((*it)->aggressor);
        }
    }
}

void AggressionList::resetList()
{
    auto temporaryList = aggressionList;
    for (auto it = temporaryList.begin(); it != temporaryList.end(); ++it)
    {
        // Check if the aggressor is null.
        if ((*it)->aggressor == nullptr)
        {
            continue;
        }
        // Remove the owner from its list.
        if (!(*it)->aggressor->aggressionList.remOpponent(owner))
        {
            Logger::log(LogLevel::Error, "Could not remove %s from opponents of %s.",
                        owner->getName(), (*it)->aggressor->getName());
        }
        // Remove from this list the opponent.
        if (!this->remOpponent((*it)->aggressor))
        {
            Logger::log(LogLevel::Error, "Could not remove %s from opponents of %s.",
                        (*it)->aggressor->getName(), owner->getName());
        }
    }
    aggressionList.clear();
}

AggressionList::iterator AggressionList::begin()
{
    return aggressionList.begin();
}

AggressionList::iterator AggressionList::end()
{
    return aggressionList.end();
}

void AggressionList::sortList()
{
    std::sort(aggressionList.begin(), aggressionList.end(),
              [](std::shared_ptr<Aggression> a, std::shared_ptr<Aggression> b)
              { return a->aggression > b->aggression; });
}

void AggressionList::printList()
{
    std::cout << "Aggro List:" << std::endl;
    for (auto it : aggressionList)
    {
        std::cout << "  [" + (*it).aggressor->name + "] " + ToString((*it).aggression) << std::endl;
    }
}
