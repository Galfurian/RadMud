/// @file   combatHandler.hpp
/// @brief  Contains definition of combat classes.
/// @author Enrico Fraccaroli
/// @date   May 8 2016
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

#include <set>
#include <vector>
#include <string>
#include <memory>

#include "characterContainer.hpp"

/// @brief Data structure used to store an ordered list of opponents
///         during a combat.
class CombatHandler
{
    friend class Character;

public:
    /// @brief Allows to store information about an aggressor.
    struct Aggression
    {
        /// @brief Constructor.
        /// @param _aggressor  The aggressor.
        /// @param _aggression The aggro level.
        Aggression(Character * _aggressor,
                   unsigned int _aggression) :
            aggressor(_aggressor),
            aggression(_aggression)
        {
            /// Nothing to do.
        }

        /// The aggressor.
        Character * aggressor;
        /// The level of aggression.
        mutable unsigned int aggression;
    };
private:
    /// Owner of the list.
    Character * owner;

    /// List of opponents.
    std::vector<std::shared_ptr<Aggression> > opponents;

    /// The predefined target.
    Character * predefinedTarget;

    /// A pointer to the currently aimed character.
    Character * aimedCharacter;

public:
    /// List of characters in sight.
    CharacterContainer charactersInSight;

    /// @brief Constructor.
    CombatHandler(Character * _owner);

    /// @brief Destructor.
    ~CombatHandler();

    /// @brief Tries to add the given character to the list of opponents.
    /// @param character The opponent to add.
    /// @param initAggro The initial value of aggression given an opponent.
    /// @return <b>True</b> if the operation concluded successfully,<br>
    ///         <b>False</b> otherwise.
    bool addOpponent(Character * character, unsigned int initAggro = 0);

    /// @brief Tries to remove the given character from the list of opponents.
    /// @param character The opponent to remove.
    /// @return <b>True</b> if the operation concluded successfully,<br>
    ///         <b>False</b> otherwise.
    bool remOpponent(Character * character);

    /// @brief Check if the list of opponents contains the given character.
    /// @param character The opponent to search.
    /// @return <b>True</b> if the opponent has been found,<br>
    ///         <b>False</b> otherwise.
    bool hasOpponent(Character * character);

    /// @brief Updates the list of characters in sight.
    void updateCharactersInSight();

    /// @brief Allows to elect the given character as predefined target.
    /// @param character The character to set as predefined target.
    void setPredefinedTarget(Character * character);

    /// @brief Provides the predefined target.
    Character * getPredefinedTarget();

    /// @brief Allows to set the given character as aimed target.
    /// @param character The character to set as aimed target.
    void setAimedTarget(Character * character);

    /// @brief Provides the aimed target.
    Character * getAimedTarget();

    /// @brief Allows to the a new aggression level to the given opponent.
    /// @param character      The opponent
    /// @param newAggression The new aggression level.
    /// @return <b>True</b> if the operation concluded successfully,<br>
    ///         <b>False</b> otherwise.
    bool setAggro(Character * character, unsigned int newAggression);

    /// @brief Returns the current top aggressor.
    /// @return The top aggressor.
    std::shared_ptr<Aggression> getTopAggro();

    /// @brief Allows to elect the given character as the opponent with
    ///         the top level of aggro.
    /// @param character The character to move on top of aggro list.
    /// @return <b>True</b> if the operation concluded successfully,<br>
    ///         <b>False</b> otherwise.
    bool moveToTopAggro(Character * character);

    /// @brief Given a character it returns the initial value of aggression.
    /// @param character The other character.
    /// @return The initial value of aggression.
    unsigned int getInitialAggro(Character * character);

    /// @brief Provides the value of aggression against a given character.
    /// @param character The other character.
    /// @return The value of aggression.
    unsigned int getAggro(Character * character);

    /// @brief Provides the size of the aggressors list.
    std::size_t getSize();

    /// @brief Returns true if the vector is empty.
    bool empty();

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
    std::vector<std::shared_ptr<Aggression> >::iterator begin();

    /// @brief Provides an iterator to the end of the list of aggressors.
    /// @return An iterator to the end of the vector of aggressors.
    std::vector<std::shared_ptr<Aggression> >::iterator end();

private:
    /// @brief Sort the list of opponents.
    void sortList();

    /// @brief Print the list of opponents.
    void printList();
};
