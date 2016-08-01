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
#include "../action/combat/combatAction.hpp"
#include "../action/combat/basicAttack.hpp"
#include "../action/combat/flee.hpp"

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
    CharacterVector excpetions = { character };
    // Retrieve the target.
    Character * target = character->room->findCharacter(
        arguments[0].first,
        arguments[0].second,
        excpetions);
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

    // Check if the two characters are both already in combat.
    if (character->getAction()->getType() == ActionType::Combat)
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
    else if ((character->getAction()->getType() != ActionType::Combat)
        && (target->getAction()->getType() == ActionType::Combat))
    {
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
        // Notify the character.
        character->sendMsg("You attack %s.\n", target->getName());
        // Notify the target.
        target->sendMsg("%s attacks you.\n\n", character->getNameCapital());
        // Notify the others.
        CharacterVector exceptions;
        exceptions.push_back(character);
        exceptions.push_back(target);
        character->room->sendToAll(
            "%s attacks %s.\n",
            exceptions,
            character->getNameCapital(),
            target->getName());
        // Let the characters enter the combat.
        if (!character->setNextCombatAction(CombatActionType::BasicAttack))
        {
            character->sendMsg("You were not ablet to attack %s.\n", target->getName());
            character->opponents.remOpponent(target);
            target->opponents.remOpponent(character);
            return;
        }
    }
    else
    {
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
        // Notify the character.
        character->sendMsg("You attack %s.\n", target->getName());
        // Notify the target.
        target->sendMsg("%s attacks you.\n\n", character->getNameCapital());
        // Notify the others.
        CharacterVector exceptions;
        exceptions.push_back(character);
        exceptions.push_back(target);
        character->room->sendToAll(
            "%s attacks %s.\n",
            exceptions,
            character->getNameCapital(),
            target->getName());

        // Let the characters enter the combat.
        if (!character->setNextCombatAction(CombatActionType::BasicAttack))
        {
            character->sendMsg("You were not ablet to attack %s.\n", target->getName());
            character->opponents.remOpponent(target);
            target->opponents.remOpponent(character);
            return;
        }
        if (!target->setNextCombatAction(CombatActionType::BasicAttack))
        {
            character->sendMsg("You were not ablet to attack %s.\n", target->getName());
            character->opponents.remOpponent(target);
            target->opponents.remOpponent(character);
            return;
        }
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
    auto combatAction = character->getAction()->toCombatAction();
    // Check if the character is already trying to flee.
    if (combatAction->getCombatActionType() == CombatActionType::Flee)
    {
        character->sendMsg("You are already trying to flee.\n");
        return;
    }
    character->sendMsg("You prepare to flee...\n");
    character->setNextCombatAction(CombatActionType::Flee);
}

void DoAim(Character * character, std::istream & sArgs)
{
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    CharacterVector targets;
    if (!character->getCharactersInSight(targets))
    {
        character->sendMsg("There are no targets nearby!\n");
        return;
    }
    character->sendMsg("You see:\n");
    for (auto it : targets)
    {
        character->sendMsg("    %s\n", it->getName());
    }
}
