/// @file   characterContainer.hpp
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

    /// @brief Search the character.
    /// @param target       The character to search.
    /// @param number       The index of the character to search.
    /// @param exceptions   The vector of exceptions.
    /// @param skipMobile   <b>True</b> skips mobiles,<br> <b>False</b> count also the mobiles.
    /// @param skipPlayer   <b>True</b> skips players,<br> <b>False</b> count also the players.
    /// @return The character if it has been found.
    Character * findCharacter(
        const std::string & target,
        int & number,
        const std::vector<Character *> & exceptions = std::vector<Character *>(),
        bool skipMobile = false,
        bool skipPlayer = false) const;

    /// @brief Checks if the container actually contains the given character.
    /// @param character The character to search.
    /// @return <b>True</b> if the container contains the given character,<br>
    ///         <b>False</b> otherwise.
    bool containsCharacter(Character * character) const;

    /// @brief Allows to add a character to the vector, if it is not already inside the vector.
    /// @param character The character to add.
    void emplace_back_character(Character * character);

    /// @brief Allows to add the characters contained inside another container into this one.
    /// @param other The other container.
    void addUnique(const CharacterContainer & other);
};
