/// @file   characterContainer.hpp
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

#pragma once

class Character;

class Mobile;

class Player;

#include <vector>
#include <string>

/// @brief Custom vector of characters.
class CharacterContainer :
    public std::vector<Character *>
{
public:
    /// @brief Constructor.
    CharacterContainer();

    /// Search the character.
    Character * findCharacter(
        const std::string & target,
        int & number,
        const std::vector<Character *> & exceptions = std::vector<Character *>(),
        bool skipMobile = false,
        bool skipPlayer = false) const;

    bool containsCharacter(Character * character) const;

    void emplace_back_character(Character * character);

    void addUnique(const CharacterContainer & others);
};
