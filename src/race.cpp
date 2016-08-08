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
#include "formatter.hpp"
#include "utils.hpp"
#include "mud.hpp"

Race::Race() :
        vnum(),
        name(),
        description(),
        material(),
        abilities(),
        availableFaction(),
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
    corpse.slot = EquipmentSlot::None;
    corpse.modelFlags = 0;
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

bool Race::setAbilities(std::string source)
{
    if (source.empty())
    {
        return false;
    }
    std::vector<std::string> abilityList = SplitString(source, ";");
    if (abilityList.size() != 5)
    {
        return false;
    }
    for (unsigned int it = 0; it < abilityList.size(); ++it)
    {
        Ability ability = static_cast<Ability>(it);
        abilities[ability] = ToNumber<unsigned int>(abilityList[it]);
    }
    return true;
}

bool Race::setAvailableFactions(const std::string & source)
{
    if (source.empty())
    {
        return true;
    }
    std::vector<std::string> factionList = SplitString(source, ";");
    if (factionList.empty())
    {
        return false;
    }
    for (unsigned int it = 0; it < factionList.size(); ++it)
    {
        int factionVnum = ToNumber<int>(factionList[it]);
        Faction * faction = Mud::instance().findFaction(factionVnum);
        if (faction == nullptr)
        {
            Logger::log(LogLevel::Error, "Can't find the faction: %s.", factionList[it]);
            return false;
        }
        availableFaction.push_back(faction);
    }
    return true;
}

unsigned int Race::getAbility(const Ability & ability) const
{
    auto it = abilities.find(ability);
    if (it != abilities.end())
    {
        return it->second;
    }
    return 0;
}

unsigned int Race::getAbilityLua(const unsigned int & abilityNumber)
{
    if (AbilityTest::is_value(abilityNumber))
    {
        return this->getAbility(AbilityTest::convert(abilityNumber));
    }
    else
    {
        return 0;
    }
}

bool Race::factionAllowed(int factionVnum)
{
    for (unsigned int it = 0; it < availableFaction.size(); ++it)
    {
        if (availableFaction.at(it)->vnum == factionVnum)
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
    .addFunction("getAbility", &Race::getAbilityLua) //
    .addData("available_faction", &Race::availableFaction) //
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
