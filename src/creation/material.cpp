/// @file   material.cpp
/// @brief  Implements material class methods.
/// @author Enrico Fraccaroli
/// @date   Aug 7 2015
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

#include "material.hpp"

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
    Logger::log(
        LogLevel::Debug,
        "Deleted material\t[%s]\t\t(%s)",
        ToString(this->vnum),
        this->name);
}

bool Material::check()
{
    assert(vnum > 0);
    assert(!name.empty());
    assert(!article.empty());
    assert(type != MaterialType::NoType);
    assert(worth > 0);
    assert(hardness > 0);
    assert(lightness > 0);
    return true;
}

double Material::getWorthModifier() const
{
    return 1 + (0.1 * this->worth);
}

double Material::getHardnessModifier() const
{
    return 1 + (0.1 * this->hardness);
}

double Material::getLightnessModifier() const
{
    return 1 + (0.1 * this->lightness);
}

void Material::luaRegister(lua_State * L)
{
    luabridge::getGlobalNamespace(L)
        .beginClass<Material>("Material")
        .addData("vnum", &Material::vnum, false)
        .addData("name", &Material::name, false)
        .addData("article", &Material::article, false)
        .addData("type", &Material::type, false)
        .addData("worth", &Material::worth, false)
        .addData("hardness", &Material::hardness, false)
        .addData("lightness", &Material::lightness, false)
        .endClass();
}
