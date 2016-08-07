/// @file   combatAction.cpp
/// @brief  Implementation of the class for a move action.
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

#include "combatAction.hpp"
#include "../../room.hpp"
#include "../../character.hpp"
#include "../../model/weaponModel.hpp"
#include "../../utilities/logger.hpp"
#include "basicAttack.hpp"
#include "flee.hpp"

using namespace std::chrono;

CombatAction::CombatAction(Character * _actor) :
        GeneralAction(_actor)
{
    Logger::log(LogLevel::Debug, "Created CombatAction.");
}

CombatAction::~CombatAction()
{
    Logger::log(LogLevel::Debug, "Deleted CombatAction.");
}

bool CombatAction::check() const
{
    bool correct = GeneralAction::check();
    return correct;
}

ActionType CombatAction::getType() const
{
    return ActionType::Combat;
}

std::string CombatAction::getDescription() const
{
    return "fighting";
}

std::string CombatAction::stop()
{
    return "You stop fighting.";
}
