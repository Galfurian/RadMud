/// @file   faction.cpp
/// @brief  Implements the faction functions.
/// @author Enrico Fraccaroli
/// @date   Sep 30 2014
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

#include "faction.hpp"

#include "utils.hpp"
#include "utilities/logger.hpp"
#include "model/currencyModel.hpp"
#include "lua/luabridge/LuaBridge.h"

Faction::Faction() :
        vnum(),
        name(),
        description(),
        currency()
{
    // Nothing to do.
}

Faction::~Faction()
{
    Logger::log(LogLevel::Debug, "Deleted faction\t[%s]\t\t(%s)", ToString(this->vnum), this->name);
}

bool Faction::check()
{
    assert(vnum > 0);
    assert(!name.empty());
    assert(!description.empty());
    assert(currency != nullptr);
    return true;
}

void Faction::getSheet(Table & sheet) const
{
    // Add the columns.
    sheet.addColumn("Attribute", StringAlign::Left);
    sheet.addColumn("Value", StringAlign::Left);
    // Set the values.
    sheet.addRow( { "Vnum", ToString(this->vnum) });
    sheet.addRow( { "Name", this->name });
    sheet.addRow( { "Description", this->description });
    sheet.addRow( { "Currency", currency->getName() + " (" + ToString(currency->vnum) + ")" });
}

std::string Faction::getName()
{
    return ToLower(name);
}

std::string Faction::getNameCapital()
{
    return name;
}

void Faction::luaRegister(lua_State * L)
{
    luabridge::getGlobalNamespace(L) //
    .beginClass<Faction>("Faction") //
    .addData("vnum", &Faction::vnum) //
    .addData("name", &Faction::name) //
    .endClass();
}
