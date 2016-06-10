/// @file   exit.cpp
/// @brief  Implements all the methods needed to interract with an exit.
/// @author Enrico Fraccaroli
/// @date   Mar 10 2015
/// @copyright
/// Copyright (c) 2015, 2016 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
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
#include "exit.hpp"

#include "constants.hpp"
// Local Includes.

// Other Include.
#include "luabridge/LuaBridge.h"
#include "room.hpp"
#include "utils.hpp"

using namespace std;

Exit::Exit() :
    source(),
    destination(),
    direction(Direction::None),
    flags()
{
}

Exit::~Exit()
{
}

Exit::Exit(Room * _source, Room * _destination, Direction _direction, unsigned int _flags)
{
    source = _source;
    destination = _destination;
    direction = _direction;
    flags = _flags;
}

bool Exit::check()
{
    assert(source != nullptr);
    assert(destination != nullptr);
    assert(direction != Direction::None);
    return true;
}

Direction Exit::getOppositeDirection()
{
    return InverDirection(direction);
}

std::string Exit::getDirection()
{
    return GetDirectionName(direction);
}

void Exit::luaRegister(lua_State * L)
{
    luabridge::getGlobalNamespace(L) //
    .beginClass<Exit>("Exit") //
    .addData("source", &Exit::source) //
    .addData("destination", &Exit::destination) //
    .addFunction("getDirection", &Exit::getDirection) //
    .endClass();
}
