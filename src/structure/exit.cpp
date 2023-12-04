/// @file   exit.cpp
/// @brief  Implements all the methods needed to interract with an exit.
/// @author Enrico Fraccaroli
/// @date   Mar 10 2015
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

#include "structure/exit.hpp"
#include "structure/room.hpp"
#include "utilities/logger.hpp"

Exit::Exit() :
    source(),
    destination(),
    direction(Direction::None),
    flags()
{
    // Nothing to do.
}

Exit::Exit(Room * _source,
           Room * _destination,
           Direction _direction,
           unsigned int _flags) :
    source(_source),
    destination(_destination),
    direction(_direction),
    flags(_flags)
{
    // Nothing to do.
}

Exit::~Exit()
{
//    Logger::log(LogLevel::Debug, "Deleted: Exit.");
}

bool Exit::check() const
{
    if (source == nullptr) return false;
    if (destination == nullptr) return false;
    if (direction == Direction::None) return false;
    return true;
}

std::shared_ptr<Exit> Exit::getOppositeExit() const
{
    if (destination != nullptr)
    {
        for (auto it : destination->exits)
        {
            if (it->destination == this->source)
            {
                return it;
            }
        }
    }
    return nullptr;
}

bool Exit::unlink() const
{
    if (source != nullptr)
    {
        if (source->removeExit(this->direction))
        {
            return true;
        }
    }
    return false;
}

bool Exit::operator==(const Exit & right) const
{
    return (this->direction == right.direction);
}

std::string GetExitFlagString(unsigned int flags)
{
    std::string flagList;
    if (HasFlag(flags, ExitFlag::NoMob)) flagList += "|NoMob";
    if (HasFlag(flags, ExitFlag::Hidden)) flagList += "|Hidden";
    if (HasFlag(flags, ExitFlag::Stairs)) flagList += "|Stairs";
    flagList += "|";
    return flagList;
}
