/// @file   building.cpp
/// @brief  Implmement building variables and methods.
/// @author Enrico Fraccaroli
/// @date   Feb 24 2016
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

// Basic Include.
#include "building.hpp"

#include "character.hpp"
#include "constants.hpp"
#include "luabridge/LuaBridge.h"
#include "room.hpp"
#include "utils.hpp"

Building::Building() :
        vnum(),
        name(),
        difficulty(),
        time(),
        assisted(),
        tools(),
        buildingModel(),
        ingredients(),
        unique()
{
}

Building::Building(const Building & source) :
        vnum(source.vnum),
        name(source.name),
        difficulty(source.difficulty),
        time(source.time),
        assisted(source.assisted),
        tools(source.tools),
        buildingModel(source.buildingModel),
        ingredients(source.ingredients),
        unique(source.unique)
{
}

Building::~Building()
{
}

bool Building::check()
{
    assert(vnum > 0);
    assert(!name.empty());
    assert(difficulty > 0);
    assert(time > 0);
    assert(buildingModel != nullptr);
    assert(!tools.empty());
    for (auto it : tools)
    {
        assert(it != ToolType::NoType);
    }
    for (auto it : ingredients)
    {
        assert(it.first != ResourceType::NoType);
        assert(it.second > 0);
    }
    return true;
}

std::string Building::getName()
{
    return ToLower(name);
}

std::string Building::getNameCapital()
{
    return name;
}
