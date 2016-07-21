/// @file   opponent.hpp
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

#pragma once

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

        ~Aggression();

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
        friend class Character;
    public:
        /// Type of structure which contains aggressors.
        typedef typename std::vector<Aggression> AggressorVector;
        /// Iterator for an aggressor vector.
        typedef typename std::vector<Aggression>::iterator iterator;
        /// Constant iterator for an aggressor vector.
        typedef typename std::vector<Aggression>::const_iterator const_iterator;
    private:
        /// Owner of the list.
        Character * owner;
        /// List of aggressors.
        AggressorVector aggressionList;

    public:
        /// @brief Constructor.
        OpponentsList(Character * _owner);

        ~OpponentsList();

        /// @brief Tries to add the given character to the list of opponents.
        /// @param character The opponent to add.
        /// @param initAggro The initial value of aggression against the given opponent.
        /// @return <b>True</b> if the operation concluded successfuly,<br>
        ///         <b>False</b> otherwise.
        bool addOpponent(Character * character, unsigned int initAggro = 0);

        /// @brief Tries to remove the given character from the list of opponents.
        /// @param character The opponent to remove.
        /// @return <b>True</b> if the operation concluded successfuly,<br>
        ///         <b>False</b> otherwise.
        bool remOpponent(Character * character);

        /// @brief Check if the list of opponents contains the given character.
        /// @param character The opponent to search.
        /// @return <b>True</b> if the opponent has been found,<br>
        ///         <b>False</b> otherwise.
        bool hasOpponent(Character * character);

        /// @brief Check if the list has some opponents.
        /// @return <b>True</b> if there are opponents,<br>
        ///         <b>False</b> otherwise.
        bool hasOpponents() const;

        /// @brief Allows to the a new aggression level to the given opponent.
        /// @param character      The opponent
        /// @param newAggression The new aggression level.
        /// @return <b>True</b> if the operation concluded successfuly,<br>
        ///         <b>False</b> otherwise.
        bool setAggro(Character * character, unsigned int newAggression);

        /// @brief Returns the current top aggressor.
        /// @return The top aggressor.
        Aggression * getTopAggro();

        /// @brief Allows to elect the given character as the opponent with
        ///         the top level of aggro.
        /// @param character The character to move on top of aggro list.
        /// @return <b>True</b> if the operation concluded successfuly,<br>
        ///         <b>False</b> otherwise.
        bool moveToTopAggro(Character * character);

        /// @brief Given a character it returns the intial value of aggression against him.
        /// @param character The other character.
        /// @return The initial value of aggression.
        unsigned int getInitialAggro(Character * character);

        /// @brief Provides the value of aggression againts a given character.
        /// @param character The other character.
        /// @return The value of aggression.
        unsigned int getAggro(Character * character);

        /// @brief Provides the size of the aggessors list.
        std::size_t getSize();

        /// @brief Check and clear the list from possible disconnected players.
        void checkList();

        /// @brief Reset this list and all the ones linked to it.
        /// @details
        /// Traverses the internal list of opponents and for each element
        ///  removes from its list of opponents the actor which owns this list.
        /// Then the element is removed from the list.
        void resetList();

        /// @brief Provides an iterator to the begin of the list of aggressors.
        /// @return An iterator to the begin of the vector of aggressors.
        iterator begin();

        /// @brief Provides an iterator to the end of the list of aggressors.
        /// @return An iterator to the end of the vector of aggressors.
        iterator end();

    private:
        /// @brief Sort the list of opponents.
        void sortList();

        /// @brief Print the list of opponents.
        void printList();
};
