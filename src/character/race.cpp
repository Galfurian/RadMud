/// @file   race.cpp
/// @brief  Implement race methods.
/// @author Enrico Fraccaroli
/// @date   Aug 23 2014
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

#include "race.hpp"

#include "logger.hpp"
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
    corpse(),
    naturalWeapon()
{
    // Nothing to do.
}

Race::~Race()
{
    //Logger::log(LogLevel::Debug, "Deleted race\t\t[%s]\t\t(%s)", ToString(this->vnum), this->name);
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
    assert(!naturalWeapon.empty());
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
    if (Ability::isValid(abilityNumber))
    {
        return this->getAbility(Ability(abilityNumber));
    }
    return 0;
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
    luabridge::getGlobalNamespace(L)
        .beginClass<Race>("Race")
        .addData("vnum", &Race::vnum)
        .addData("name", &Race::name)
        .addData("material", &Race::material)
        .addFunction("getAbility", &Race::getAbilityLua)
        .addData("available_faction", &Race::availableFaction)
        .endClass();
}

std::string Race::getTile()
{
    if (Formatter::getFormat() == Formatter::TELNET)
    {
        return ToString(tileSet) + ":" + ToString(tileId);
    }
    return "c";
}
