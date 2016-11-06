/// @file   BasicAttack.hpp
/// @brief  Contais the definition of the class for the basic attacks.
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

    void findPredefinedTarget();

    bool checkTarget(Character * target);

    ActionStatus handleStop();

    void performMeleeAttack(Character * target, MeleeWeaponItem * weapon, const bool dualWielding);

    void performRangedAttack(Character * target, RangedWeaponItem * weapon, const bool dualWielding);

    void handleMeleeHit(Character * target, MeleeWeaponItem * weapon);

    void handleRangedHit(Character * target, RangedWeaponItem * weapon);

    void handleMeleeMiss(Character * target, MeleeWeaponItem * weapon);

    void handleRangedMiss(Character * target, RangedWeaponItem * weapon);
};
