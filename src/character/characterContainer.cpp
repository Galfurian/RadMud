/// @file   characterContainer.cpp
/// @author Enrico Fraccaroli
/// @date   Oct 11 2016
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

#include "characterContainer.hpp"
#include "character.hpp"
#include "player.hpp"
#include "mobile.hpp"

CharacterContainer::CharacterContainer()
{
    // Nothing to do.
}

Character * CharacterContainer::findCharacter(const std::string & target,
                                              int & number,
                                              const std::vector<Character *> & exceptions,
                                              bool skipMobile,
                                              bool skipPlayer) const
{
    auto isAnException = [&](Character * character)
    {
        // Check exceptions.
        if (!exceptions.empty())
        {
            for (auto exception : exceptions)
            {
                if (exception->getName() == character->getName())
                {
                    return true;
                }
            }
        }
        return false;
    };

    for (const_iterator it = this->begin(); it != this->end(); ++it)
    {
        Character * character = (*it);
        if ((skipMobile && character->isMobile()) || (skipPlayer && character->isPlayer()))
        {
            continue;
        }
        if (isAnException(character))
        {
            continue;
        }
        // Check if the character is a mobile or a player.
        if (character->isMobile())
        {
            if (character->toMobile()->hasKey(ToLower(target)))
            {
                if (number > 1)
                {
                    number -= 1;
                }
                else
                {
                    return character->toMobile();
                }
            }
        }
        else
        {
            if (character->toPlayer()->isPlaying())
            {
                if (BeginWith(character->toPlayer()->getName(), ToLower(target)))
                {
                    if (number > 1)
                    {
                        number -= 1;
                    }
                    else
                    {
                        return character->toPlayer();
                    }
                }
            }
        }
    }
    return nullptr;
}

bool CharacterContainer::containsCharacter(Character * character) const
{
    for (const_iterator it = this->begin(); it != this->end(); ++it)
    {
        Character * contained = (*it);
        if (contained != nullptr)
        {
            if (contained->getName() == character->getName())
            {
                return true;
            }
        }
    }
    return false;
}


void CharacterContainer::emplace_back_character(Character * character)
{
    for (const_iterator it = this->begin(); it != this->end(); ++it)
    {
        Character * contained = (*it);
        if (contained != nullptr)
        {
            if (contained->getName() == character->getName())
            {
                return;
            }
        }
    }
    this->emplace_back(character);
}

void CharacterContainer::addUnique(const CharacterContainer & other)
{
    for (auto character : other)
    {
        this->emplace_back_character(character);
    }
}
