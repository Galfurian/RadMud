/// @file   loadAction.cpp
/// @brief  Implementation of the class for loading activities.
/// @author Enrico Fraccaroli
/// @date   Oct 10 2016
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

#include "loadAction.hpp"
#include "../character/character.hpp"

LoadAction::LoadAction(Item * _itemToBeLoaded, Character * _actor, unsigned int _cooldown) :
    GeneralAction(_actor, std::chrono::system_clock::now() + std::chrono::seconds(_cooldown)),
    itemToBeLoaded(_itemToBeLoaded)
{
    Logger::log(LogLevel::Debug, "Created reload action.");
}

LoadAction::~LoadAction()
{
    Logger::log(LogLevel::Debug, "Deleted reload action.");
}

bool LoadAction::check() const
{
    bool correct = GeneralAction::check();
    correct &= this->checkItem();
    return correct;
}

ActionType LoadAction::getType() const
{
    return ActionType::Load;
}

std::string LoadAction::getDescription() const
{
    return "loading";
}

std::string LoadAction::stop()
{
    return "You stop loading " + itemToBeLoaded->getName(true) + ".";
}

ActionStatus LoadAction::perform()
{
    // Check if the cooldown is ended.
    if (!this->checkElapsed())
    {
        return ActionStatus::Running;
    }
    actor->sendMsg("You have finished loading %s...\n\n", this->itemToBeLoaded->getName(true));
    return ActionStatus::Finished;
}

bool LoadAction::checkItem() const
{
    if (itemToBeLoaded == nullptr)
    {
        Logger::log(LogLevel::Error, "The item is a null pointer.");
        return false;
    }
    return true;
}
