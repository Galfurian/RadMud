/// @file   race.cpp
/// @brief  Implement race methods.
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
#include "race.hpp"

#include "constants.hpp"
// Local Includes.

// Other Include.
#include "luabridge/LuaBridge.h"
#include "utils.hpp"
#include "formatter.hpp"

Race::Race() :
    vnum(),
    name(),
    description(),
    material(),
    strength(),
    agility(),
    perception(),
    constitution(),
    intelligence(),
    available_faction(),
    player_allow(),
    tileSet(),
    tileId(),
    corpse()
{
    // Nothing to do.
}

Race::~Race()
{
    Logger::log(LogLevel::Debug, "Deleted race\t\t[%s]\t\t(%s)", ToString(this->vnum), this->name);
}

void Race::initializeCorpse(const std::string & corpseDescription)
{
    corpse.vnum = 0;
    corpse.name = "corpse";
    corpse.article = "a";
    corpse.shortdesc = "the corpse of " + this->getShortDescription();
    corpse.keys.push_back("corpse");
    corpse.keys.push_back(name);
    corpse.description = corpseDescription;
    corpse.type = ModelType::Corpse;
    corpse.slot = EquipmentSlot::None;
    corpse.flags = 0;
    corpse.weight = 50;
    corpse.price = 0;
    corpse.condition = 10;
    corpse.decay = 1;
    corpse.material = this->material->type;
    corpse.tileSet = this->tileSet;
    corpse.tileId = this->tileId;
}

bool Race::check()
{
    assert(vnum > 0);
    assert(!name.empty());
    assert(!description.empty());
    assert(material != nullptr);
    assert(strength > 0);
    assert(agility > 0);
    assert(perception > 0);
    assert(constitution > 0);
    assert(intelligence > 0);
    assert(!available_faction.empty());
    //assert(!corpseDescription.empty());
    return true;
}

std::string Race::getShortDescription(bool capital)
{
    std::string shortDescription = this->article + " " + this->name;
    if (capital)
    {
        shortDescription[0] = static_cast<char>(toupper(shortDescription[0]));
    }
    return shortDescription;
}

bool Race::setCharacteristic(std::string source)
{
    if (source.empty())
    {
        return false;
    }
    std::vector<std::string> charList = SplitString(source, ";");
    if (charList.size() != 5)
    {
        return false;
    }
    int value = ToNumber<int>(charList[0]);
    if (value < 0)
    {
        Logger::log(LogLevel::Error, "The list of characteristics contains a negative strength.");
        return false;
    }
    this->strength = static_cast<unsigned int>(value);
    value = ToNumber<int>(charList[1]);
    if (value < 0)
    {
        Logger::log(LogLevel::Error, "The list of characteristics contains a negative agility.");
        return false;
    }
    this->agility = static_cast<unsigned int>(value);
    value = ToNumber<int>(charList[2]);
    if (value < 0)
    {
        Logger::log(LogLevel::Error, "The list of characteristics contains a negative perception.");
        return false;
    }
    this->perception = static_cast<unsigned int>(value);
    value = ToNumber<int>(charList[3]);
    if (value < 0)
    {
        Logger::log(LogLevel::Error, "The list of characteristics contains a negative constitution.");
        return false;
    }
    this->constitution = static_cast<unsigned int>(value);
    value = ToNumber<int>(charList[4]);
    if (value < 0)
    {
        Logger::log(LogLevel::Error, "The list of characteristics contains a negative intelligence.");
        return false;
    }
    this->intelligence = static_cast<unsigned int>(value);
    return true;
}

bool Race::factionAllowed(int factionVnum)
{
    for (unsigned int i = 0; i < available_faction.size(); i++)
    {
        if (available_faction[i] == factionVnum)
        {
            return true;
        }
    }
    return false;
}

void Race::luaRegister(lua_State * L)
{
    luabridge::getGlobalNamespace(L) //
    .beginClass<Race>("Race") //
    .addData("vnum", &Race::vnum) //
    .addData("name", &Race::name) //
    .addData("material", &Race::material) //
    .addData("strength", &Race::strength) //
    .addData("agility", &Race::agility) //
    .addData("perception", &Race::perception) //
    .addData("constitution", &Race::constitution) //
    .addData("intelligence", &Race::intelligence) //
    .addData("available_faction", &Race::available_faction) //
    .endClass();
}

std::string Race::getTile()
{
    if (Formatter::getFormat() == Formatter::TELNET)
    {
        return ToString(tileSet) + ":" + ToString(tileId);
    }
    else
    {
        return "c";
    }
}
