/// @file   combatAction.hpp
/// @brief  Contais the definition of the virtual class for general combat actions.
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

#include "../generalAction.hpp"

/// The list of possible combat actions.
using CombatActionType = enum class CombatActionType_t
{
    NoAction,       ///< The combat move is to do nothing.
    BasicAttack,    ///< The action is a basic attack.
    Flee            ///< The character tries to flee.
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

    bool check() const override;

    ActionType getType() const override;

    std::string getDescription() const override;

    std::string stop() override;

    virtual ActionStatus perform() = 0;

    /// @brief Provides the type of combat action.
    /// @return The type of combat action.
    virtual CombatActionType getCombatActionType() const = 0;
};
