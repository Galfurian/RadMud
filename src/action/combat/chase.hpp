/// @file   chase.hpp
/// @brief  Contais the Chase class.
/// @author Enrico Fraccaroli
/// @date   Nov 11 2016
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

#include "combatAction.hpp"

#include <functional>
#include <vector>

class Room;

/// @brief An action executed by characters when fighting at close range.
class Chase :
    public CombatAction
{
private:
    /// The chased target.
    Character * target;
    /// The room to which the path leads.
    Room * lastRoom;
    /// The path which leads to the target.
    std::vector<Room *> path;
    /// Checking function.
    std::function<bool(Room *, Room *)> RoomCheckFunction;

public:
    /// @brief Constructor.
    /// @param _actor  The actor who is doing the action.
    /// @param _target The target of the action.
    Chase(Character * _actor, Character * _target);

    /// @brief Destructor.
    virtual ~Chase();

    bool check(std::string & error) const override;

    ActionType getType() const override;

    std::string getDescription() const override;

    std::string stop() override;

    ActionStatus perform() override;

    unsigned int getCooldown() override;

    CombatActionType getCombatActionType() const override;

    /// @brief Returns the stamina required to execute the action.
    /// @param character The character.
    /// @return The required stamina.
    static unsigned int getConsumedStamina(Character * character);

private:

    /// @brief Updates the path between the actor and the target.
    /// @return If there is a valid path.
    bool updatePath();

    /// @brief Advance on the next tile towards the target.
    /// @return If the move has been performed.
    bool moveTowardsTarget();
};
