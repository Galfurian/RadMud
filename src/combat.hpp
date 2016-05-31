/// @file   combat.hpp
/// @brief  Contains definition of combat classes.
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

#ifndef COMBAT_HPP
#define COMBAT_HPP

class Character;

#include <set>
#include <vector>
#include <string>

/// @brief Allows to store information about an aggressor.
class Aggression
{
    public:
        /// The aggressor.
        Character * aggressor;
        /// The level of aggression.
        mutable unsigned int aggression;

        /// @brief Constructor.
        Aggression(Character * _aggressor, unsigned int _aggression);

        /// @brief Operator used to order the aggressors based on the level of aggression.
        bool operator>(const Aggression & source) const;

        /// @brief Operator used to order the aggressors based on the level of aggression.
        bool operator<(const Aggression & source) const;

        /// @brief Operator used to order the aggressors based on the level of aggression.
        bool operator==(const Aggression & source) const;

        /// @brief Operator used to order the aggressors based on the level of aggression.
        bool operator==(const Character * source) const;
};

/// @brief Data structure used to store an ordered list of opponents during a combat.
class OpponentsList
{
    private:
        /// Owner of the list.
        Character * owner;
        /// List of aggressors.
        std::vector<Aggression> aggressionList;

    public:
        /// @brief Constructor.
        OpponentsList(Character * _owner);

        /// @brief Add an opponent to the list.
        /// @param opponent
        /// @param itialAggression
        /// @return <b>True</b> if the operation concluded successfuly,<br>
        ///         <b>False</b> otherwise.
        bool addOpponent(Character * opponent, unsigned int itialAggression);

        /// @brief Check if the list has some opponents.
        /// @return <b>True</b> if there are opponents,<br>
        ///         <b>False</b> otherwise.
        bool hasOpponents() const;

        /// @brief Check if the list contains a specific opponent.
        /// @param opponent The opponent to search.
        /// @return <b>True</b> if the opponent has been found,<br>
        ///         <b>False</b> otherwise.
        bool hasOpponent(Character * opponent);

        /// @brief Returns the current top aggressor.
        /// @return The top aggressor.
        const Aggression & getTopAggro();

        /// @brief Allows to the a new aggression level to the given opponent.
        /// @param opponent      The opponent
        /// @param newAggression The new aggression level.
        /// @return <b>True</b> if the operation concluded successfuly,<br>
        ///         <b>False</b> otherwise.
        bool setAggro(Character * opponent, unsigned int newAggression);

        /// @brief Allows to elect the given character as the opponent with
        ///         the top level of aggro.
        /// @param character The character to move on top of aggro list.
        /// @return <b>True</b> if the operation concluded successfuly,<br>
        ///         <b>False</b> otherwise.
        bool moveToTopAggro(Character * character);

        unsigned int getInitialAggro(Character * character);

        unsigned int getAggro(Character * character);

    private:
        /// @brief Sort the list of opponents.
        void sortList();

        /// @brief Print the list of opponents.
        void printList();
};

#endif
