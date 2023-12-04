/// @file   combatAction.hpp
/// @brief  Contais the definition of the virtual class for
///          general combat actions.
/// @author Enrico Fraccaroli
/// @date   Jul 16 2016
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

#include "action/generalAction.hpp"

/// The list of possible combat actions.
using CombatActionType = enum class CombatActionType_t
{
    NoAction,    ///< The combat move is to do nothing.
    BasicAttack, ///< The action is a basic melee attack.
    Flee,        ///< The character tries to flee.
    Chase        ///< The character is chasing someone.
};

/// @brief An action executed by characters when fighting.
class CombatAction :
    public GeneralAction
{
public:
    /// @brief Constructor.
    /// @param _actor The actor who is doing the action.
    CombatAction(Character * _actor);

    /// @brief Destructor.
    virtual ~CombatAction();

    bool check(std::string & error) const override;

    ActionType getType() const override;

    std::string getDescription() const override;

    std::string stop() override;

    ActionStatus perform() override;

    unsigned int getCooldown() override;

    /// @brief Unset all the variables used for combat.
    void handleStop();

    /// @brief Provides the type of combat action.
    /// @return The type of combat action.
    virtual CombatActionType getCombatActionType() const = 0;
};
