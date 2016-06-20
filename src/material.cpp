/// @file   material.cpp
/// @brief  Implements material class methods.
/// @author Enrico Fraccaroli
/// @date   Aug 7 2015
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
#include "material.hpp"

#include "constants.hpp"
// Local Includes.

// Other Include.
#include "luabridge/LuaBridge.h"
#include "utils.hpp"

Material::Material() :
    vnum(),
    type(MaterialType::NoType),
    name(),
    article(),
    worth(),
    hardness(),
    lightness()
{
    // Nothing to do.
}

Material::~Material()
{
    Logger::log(LogLevel::Debug, "Deleted material\t[%s]\t\t(%s)", ToString(this->vnum), this->name);
}

bool Material::check()
{
    assert(vnum > 0);
    assert(name.empty() == false);
    assert(article.empty() == false);
    assert(type != MaterialType::NoType);
    assert(worth > 0);
    assert(hardness > 0);
    assert(lightness > 0);
    return true;
}

void Material::luaRegister(lua_State * L)
{
    luabridge::getGlobalNamespace(L) //
    .beginClass<Material>("Material") //
    .addData("vnum", &Material::vnum, false) //
    .addData("name", &Material::name, false) //
    .addData("article", &Material::article, false) //
    .addData("type", &Material::type, false) //
    .addData("worth", &Material::worth, false) //
    .addData("hardness", &Material::hardness, false) //
    .addData("lightness", &Material::lightness, false) //
    .endClass();
}
