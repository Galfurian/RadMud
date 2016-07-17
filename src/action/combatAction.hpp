/// @file   combatAction.hpp
/// @brief  Definition of the class for a combat action.
/// @author Enrico Fraccaroli
/// @date   Jul 16 2016
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

#include "generalAction.hpp"

/// The list of possible combat actions.
typedef enum class CombatActionTypes
{
    /// The combat move is to do nothing.
    NoAction,
    /// The action is a basic attack.
    BasicAttack,
    /// The character tries to flee.
    Flee
} CombatActionType;

class CombatAction: public GeneralAction
{
    private:
        /// The current combat action.
        CombatActionType combatAction;

    public:
        /// @brief Constructor.
        /// @param _cooldown    How many seconds are required to complete the movement.
        CombatAction(Character * _actor);

        /// @brief Destructor.
        virtual ~CombatAction();

        /// @brief Checks the correctness of the action's values.
        /// @return <b>True</b> if it has correct values,<br>
        ///         <b>False</b> otherwise.
        virtual bool check() const;

        /// @brief Provides the type of the action.
        /// @return The type of action.
        virtual ActionType getType() const;

        /// @brief Provides the description of the action.
        /// @return The string which describe the current action.
        virtual std::string getDescription() const;

        /// @brief Stops the current action and returns a string which describe the intterruption.
        /// @return The stopping description.
        virtual std::string stop();

        /// @brief Performs the current action.
        /// @return <b>True</b> if the action is finished,<br>
        ///         <b>False</b> otherwise.
        virtual bool perform();

        /// @brief Provides the type of combat action.
        /// @return The type of combat action.
        CombatActionType getCombatActionType() const;

        /// @brief Allows to set a combat action.
        /// @param nextAction The next action to execute in combat.
        /// @return <b>True</b> if correct values have been provided,<br>
        ///         <b>False</b> otherwise.
        bool setNextCombatAction(const CombatActionType & nextAction);

    private:

        /// @brief Returns the attack damage.
        /// @return The value of the attack.
        void performCombatAction(const CombatActionType & move);
};
