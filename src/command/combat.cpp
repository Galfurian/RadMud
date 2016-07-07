/// @file   combat.cpp
/// @brief  Implements the methods used by the character in order to <b>fight</b>.
/// @author Enrico Fraccaroli
/// @date   Aug 23 2014
/// @copyright
/// Copyright (c) 2014, 2015, 2016 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
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

// Basic Include.
#include "command.hpp"
#include "../room.hpp"
#include "../constants.hpp"

using namespace std;

void DoKill(Character * character, std::istream & sArgs)
{
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    // If there are no arguments, show the room.
    if (arguments.size() != 1)
    {
        character->sendMsg("You have to specify whom to kill.\n");
    }
    // Retrieve the target.
    Character * target = character->room->findCharacter(arguments[0].first, arguments[0].second, character);
    if (!target)
    {
        character->sendMsg("You don't see '%s' anywhere.\n", arguments[0].first);
        return;
    }
    // Check if the attacker can see the target.
    if (!character->canSee(target))
    {
        character->sendMsg("You don't see '%s' anywhere.\n", arguments[0].first);
        return;
    }
    ActionType characAction = character->getAction()->getType();
    ActionType targetAction = target->getAction()->getType();

    // Check if the two characters are both already in combat.
    if (characAction == ActionType::Combat)
    {
        // Check if the character is trying to attack a target with which is not in combat.
        if (!character->opponents.hasOpponent(target))
        {
            character->sendMsg("You have already your share of troubles!\n");
            return;
        }
        // Set the target as a top aggro.
        if (character->opponents.moveToTopAggro(target))
        {
            character->sendMsg("You focus your attacks on %s!\n", target->getName());
        }
        else
        {
            character->sendMsg("You are already doing your best to kill %s!\n", target->getName());
        }
    }
    // Check if the character is attacking a target which is already in combat.
    else if ((characAction != ActionType::Combat) && (targetAction == ActionType::Combat))
    {
        // Let the characters enter the combat.
        character->getAction()->setInCombat();
        // Set the opponents.
        if (!character->opponents.addOpponent(target))
        {
            character->sendMsg("You are already fighting againts %s.\n", target->getName());
            return;
        }
        if (!target->opponents.addOpponent(character))
        {
            character->sendMsg("You were not ablet to attack %s.\n", target->getName());
            character->opponents.remOpponent(target);
            return;
        }
        // Try to set the next combat action.
        if (!character->getAction()->setNextCombatAction(CombatAction::BasicAttack))
        {
            character->sendMsg("You were not ablet to attack %s.\n", target->getName());
            character->opponents.remOpponent(target);
            target->opponents.remOpponent(character);
            character->getAction()->stop();
        }
        character->sendMsg("You attack %s.\n", target->getName());
        target->sendMsg("%s attacks you.\n\n", character->getNameCapital());
    }
    else
    {
        // Let the characters enter the combat.
        character->getAction()->setInCombat();
        target->getAction()->setInCombat();

        // Set the opponents.
        if (!character->opponents.addOpponent(target))
        {
            character->sendMsg("You are already fighting againts %s.\n", target->getName());
            return;
        }
        if (!target->opponents.addOpponent(character))
        {
            character->sendMsg("You were not ablet to attack %s.\n", target->getName());
            character->opponents.remOpponent(target);
            return;
        }
        // Try to set the next combat action.
        if (!character->getAction()->setNextCombatAction(CombatAction::BasicAttack))
        {
            character->sendMsg("You were not ablet to attack %s.\n", target->getName());
            character->opponents.remOpponent(target);
            target->opponents.remOpponent(character);
            character->getAction()->stop();
            target->getAction()->stop();
            return;
        }
        if (!target->getAction()->setNextCombatAction(CombatAction::BasicAttack))
        {
            character->sendMsg("You were not ablet to attack %s.\n", target->getName());
            character->opponents.remOpponent(target);
            target->opponents.remOpponent(character);
            character->getAction()->stop();
            target->getAction()->stop();
        }
        character->sendMsg("You attack %s.\n", target->getName());
        target->sendMsg("%s attacks you.\n\n", character->getNameCapital());
    }
}

void DoFlee(Character * character, std::istream & sArgs)
{
    // Check that there are no more arguments.
    NoMore(character, sArgs);
    // Check if the character is in combat.
    if (character->getAction()->getType() != ActionType::Combat)
    {
        character->sendMsg("You are not fighting.\n");
        return;
    }
    // Check if the character is already trying to flee.
    if (character->getAction()->getCombatAction() == CombatAction::Flee)
    {
        character->sendMsg("You are already trying to flee.\n");
        return;
    }
    character->sendMsg("You prepare to flee...\n");
    character->getAction()->setNextCombatAction(CombatAction::Flee);
}