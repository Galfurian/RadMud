/// @file   characterContainer.cpp
/// @author Enrico Fraccaroli
/// @date   Oct 11 2016
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
    for (const_iterator it = this->begin(); it != this->end(); ++it)
    {
        Character * character = (*it);
        if (skipMobile && character->isMobile())
        {
            continue;
        }
        if (skipPlayer && character->isPlayer())
        {
            continue;
        }
        // Check exceptions.
        if (!exceptions.empty())
        {
            for (auto exception : exceptions)
            {
                if (exception->getName() == character->getName())
                {
                    continue;
                }
            }
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

void CharacterContainer::addUnique(const CharacterContainer & others)
{
    for (auto other : others)
    {
        this->emplace_back_character(other);
    }
}
