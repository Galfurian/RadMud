/// @file   generalAction.hpp
/// @brief  Defines the base class for any kind of action.
/// @author Enrico Fraccaroli
/// @date   Jul 13 2016
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

#include "production.hpp"
#include "building.hpp"
#include "combatHandler.hpp"
#include "item.hpp"

#include <memory>
#include <chrono>

class BuildAction;

class CraftAction;

class MoveAction;

class CombatAction;

/// The list of possible actions.
using ActionType = enum class ActionType_t
{
    Wait,       ///< The character is doing common action.
    Move,       ///< The character is moving to another location.
    Crafting,   ///< The character is crafting something.
    Building,   ///< The character is crafting something.
    Combat,     ///< The character is fighting.
    Scout,      ///< The character is scouting the area.
    Reload,     ///< The character is reloading a firearm.
    Load,       ///< The character is loading something.
    Unload,     ///< The character is unloading something.
    Aim,        ///< The character is aiming a target.
    Dismember   ///< The character is dismembering a corpse.
};

/// The list of possible actions.
using ActionStatus = enum class ActionStatus_t
{
    Finished,   ///< The action is finished.
    Running,    ///< The action is still running.
    Error       ///< The action has encountered an error.
};

/// @brief A general action. Performing this action will result in doing nothing.
class GeneralAction :
    public std::enable_shared_from_this<GeneralAction>
{
protected:
    /// Actor of the action.
    Character * actor;
    /// The time point in the future needed by the action to complete.
    std::chrono::time_point<std::chrono::system_clock> actionCooldown;

public:
    /// @brief Constructor.
    GeneralAction(Character * _actor);

    /// @brief Destructor.
    virtual ~GeneralAction();

    /// @brief Check if the cooldown of the action is elapsed.
    /// @return <b>True</b> if the time has passed,<br>
    ///         <b>False</b> otherwise.
    bool checkElapsed() const;

    /// @brief Get the elapsed time.
    long int getElapsed() const;

    /// @brief Checks the correctness of the action's values.
    /// @param error A string which contains the error in case of a failed check.
    /// @return <b>True</b> if it has correct values,<br>
    ///         <b>False</b> otherwise.
    virtual bool check(std::string & error) const;

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
    /// @return the status after performing the action.
    virtual ActionStatus perform();

    /// @brief Provides the remaining time before the action can be triggered.
    virtual unsigned int getCooldown();

    /// @brief Allows to set the remaining time before the action can be triggered.
    /// @param _actionCooldown The cooldown that has to be set.
    void resetCooldown(const unsigned int & _actionCooldown = 0);

    /// @brief Returns the action <b>statically</b> casted to CombatAction.
    std::shared_ptr<CombatAction> toCombatAction();
};

inline bool operator==(const std::shared_ptr<GeneralAction> & _generalAction,
                       const ActionType & _actionType)
{
    return (_generalAction->getType() == _actionType);
}

inline bool operator!=(const std::shared_ptr<GeneralAction> & _generalAction,
                       const ActionType & _actionType)
{
    return (_generalAction->getType() != _actionType);
}