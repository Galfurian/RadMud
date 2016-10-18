/// @file   moveAction.hpp
/// @brief  Definition of the class for a move action.
/// @author Enrico Fraccaroli
/// @date   Jul 14 2016
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

/// @brief An action which allows to move characters.
class MoveAction :
    public GeneralAction
{
private:
    /// The destination of the actor.
    Room * destination;
    /// The direction of the actor.
    Direction direction;

public:
    /// @brief Constructor.
    /// @param _actor       The actor who is doing the action.
    /// @param _destination The destionation of the movement.
    /// @param _direction   The direction of the movement.
    /// @param _cooldown    How many seconds are required to complete the movement.
    MoveAction(
        Character * _actor,
        Room * _destination,
        Direction _direction,
        unsigned int _cooldown);

    /// @brief Destructor.
    virtual ~MoveAction();

    bool check() const override;

    ActionType getType() const override;

    std::string getDescription() const override;

    std::string stop() override;

    ActionStatus perform() override;

    /// @brief Given an action, it returns the stamina required to execute it.
    static unsigned int getConsumedStamina(const Character * character, const CharacterPosture & posture);

private:
    /// @brief Checks the destination.
    /// @return <b>True</b> if the destination is set,<br>
    ///         <b>False</b> otherwise.
    bool checkDestination() const;

    /// @brief Checks the direction.
    /// @return <b>True</b> if the direction is set,<br>
    ///         <b>False</b> otherwise.
    bool checkDirection() const;
};
