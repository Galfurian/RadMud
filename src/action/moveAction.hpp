/// @file   moveAction.hpp
/// @brief  Definition of the class for a move action.
/// @author Enrico Fraccaroli
/// @date   Jul 14 2016
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

#include "generalAction.hpp"
#include "direction.hpp"

class Room;

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
    MoveAction(Character * _actor, Room * _destination, Direction _direction);

    /// @brief Destructor.
    virtual ~MoveAction();

    bool check(std::string & error) const override;

    ActionType getType() const override;

    std::string getDescription() const override;

    std::string stop() override;

    ActionStatus perform() override;

    unsigned int getCooldown() override;

    /// @brief Given an action, it returns the stamina required to execute it.
    static unsigned int getConsumedStamina(Character * character);

    /// @brief Check if the character can move in the given direction.
    /// @param character     The character that wants to move.
    /// @param direction     The direction where the character whats to move.
    /// @param error         A reference to a string which will contain error
    ///                       message in case of failure.
    /// @param allowInCombat If true, the function does not check if the
    ///                       character is in close-combat.
    /// @return <b>True</b> if the character can move to the given direction,<br>
    ///         <b>False</b> otherwise.
    static bool canMoveTo(Character * character,
                          const Direction & direction,
                          std::string & error,
                          bool allowInCombat);
};
