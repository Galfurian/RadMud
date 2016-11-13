/// @file   basicAttack.hpp
/// @brief  Contais the definition of the class for the basic attacks.
/// @author Enrico Fraccaroli
/// @date   Nov 2 2016
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

/// @brief An action executed by characters when fighting at close range.
class BasicAttack :
    public CombatAction
{
public:
    /// @brief Constructor.
    /// @param _actor The actor who is doing the action.
    BasicAttack(Character * _actor);

    /// @brief Destructor.
    virtual ~BasicAttack();

    bool check(std::string & error) const override;

    ActionType getType() const override;

    std::string getDescription() const override;

    std::string stop() override;

    ActionStatus perform() override;

    CombatActionType getCombatActionType() const override;

    /// @brief Returns the stamina required to execute the action.
    /// @param character The actor.
    /// @param weapon    The weapon used to performe the action.
    /// @return The required stamina.
    static unsigned int getConsumedStamina(Character * character, Item * weapon);

private:

    /// @brief Sets the predefined target. If there is already one, it checks if it is a valid target.
    /// @return <b>True</b> if there is a valid target,<br>
    ///         <b>False</b> otherwise.
    bool setPredefinedTarget();

    /// @brief Checks if the given target is a valid target for either close or long range combat.
    /// @param target The character to check.
    /// @return <b>True</b> if the character is a valid target,<br>
    ///         <b>False</b> otherwise.
    bool checkTarget(Character * target);

    /// @brief Unset all the variables used for combat.
    void handleStop();

    /// @brief Performs a melee attack with the given weapon.
    /// @param target The character to attack.
    /// @param weapon The melee weapon used to attack.
    /// @param dualWielding If the actor is dual wielding.
    void performMeleeAttack(Character * target, MeleeWeaponItem * weapon, const bool dualWielding);

    /// @brief Performs a ranged attack with the given weapon.
    /// @param target The character to attack.
    /// @param weapon The ranged weapon used to attack.
    /// @param dualWielding If the actor is dual wielding.
    void performRangedAttack(Character * target, RangedWeaponItem * weapon, const bool dualWielding);

    /// @brief Send the messages when the actor hits with a close ranged attack.
    /// @param target The character which is involved in the attack.
    /// @param weapon The weapon used to attack.
    void handleMeleeHit(Character * target, MeleeWeaponItem * weapon);

    /// @brief Send the messages when the actor hits with a long range attack.
    /// @param target The character which is involved in the attack.
    /// @param weapon The weapon used to attack.
    void handleRangedHit(Character * target, RangedWeaponItem * weapon);

    /// @brief Send the messages when the actor misses the target with a close ranged attack.
    /// @param target The character which is involved in the attack.
    /// @param weapon The weapon used to attack.
    void handleMeleeMiss(Character * target, MeleeWeaponItem * weapon);

    /// @brief Send the messages when the actor misses the target with a long range attack.
    /// @param target The character which is involved in the attack.
    /// @param weapon The weapon used to attack.
    void handleRangedMiss(Character * target, RangedWeaponItem * weapon);
};
