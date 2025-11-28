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

#include "character/race/race.hpp"

#include "utilities/logger.hpp"
#include "mud.hpp"

Race::Race() :
    vnum(),
    name(),
    description(),
    abilities(),
    player_allow(),
    tileSet(),
    tileId(),
    corpse(),
    bodyParts(),
    skills()
{
    // Nothing to do.
}

Race::~Race()
{
//    Logger::log(LogLevel::Debug,
//                "Deleted race\t\t[%s]\t\t(%s)",
//                ToString(this->vnum),
//                this->name);
}

bool Race::check()
{
    if (vnum <= 0) return false;
    if (name.empty()) return false;
    if (description.empty()) return false;
    return true;
}

std::string Race::getShortDescription(bool capital)
{
    std::string shortDescription = this->article + " " + this->name;
    if (capital && !shortDescription.empty())
    {
        shortDescription[0] = static_cast<char>(toupper(shortDescription[0]));
    }
    return shortDescription;
}

unsigned int Race::getAbility(const Ability & ability) const
{
    for (auto it : abilities)
    {
        if (it.first == ability)
        {
            return it.second;
        }
    }
    return 0;
}

std::string Race::getTile()
{
    if (Formatter::getFormat() == Formatter::CLIENT)
    {
        return ToString(tileSet) + ":" + ToString(tileId);
    }
    return "c";
}

json::jnode_t &operator<<(json::jnode_t &lhs, const Race &rhs)
{
    lhs.set_type(json::JTYPE_OBJECT);
    lhs["vnum"] << rhs.vnum;
    lhs["name"] << rhs.name;
    lhs["description"] << rhs.description;
    lhs["player_allow"] << rhs.player_allow;
    lhs["tile_set"] << rhs.tileSet;
    lhs["tile_id"] << rhs.tileId;
    // Serialize the abilities.
    json::jnode_t abilities_node;
    abilities_node.set_type(json::JTYPE_OBJECT);
    for (auto const &it : rhs.abilities) {
        abilities_node[it.first.toString()] << it.second;
    }
    lhs["abilities"] = abilities_node;
    // Serialize the body parts.
    json::jnode_t bodyparts_node;
    bodyparts_node.set_type(json::JTYPE_ARRAY);
    for (auto const &it : rhs.bodyParts) {
        bodyparts_node.add_element() << it->vnum;
    }
    lhs["body_parts"] = bodyparts_node;
    // Serialize the skills.
    json::jnode_t skills_node;
    skills_node.set_type(json::JTYPE_ARRAY);
    for (auto const &it : rhs.skills) {
        json::jnode_t skill_node;
        skill_node.set_type(json::JTYPE_OBJECT);
        skill_node["vnum"] << it->skillVnum;
        skill_node["level"] << it->skillLevel;
        skills_node.add_element(skill_node);
    }
    lhs["skills"] = skills_node;
    return lhs;
}

const json::jnode_t &operator>>(const json::jnode_t &lhs, Race &rhs)
{
    lhs["vnum"] >> rhs.vnum;
    lhs["name"] >> rhs.name;
    lhs["description"] >> rhs.description;
    lhs["player_allow"] >> rhs.player_allow;
    lhs["tile_set"] >> rhs.tileSet;
    lhs["tile_id"] >> rhs.tileId;
    // Deserialize the abilities.
    if(lhs.has_property("abilities")){
        json::jnode_t abilities_node = lhs["abilities"];
        for (auto it = abilities_node.pbegin(); it != abilities_node.pend(); ++it) {
            unsigned int ability_vnum;
            it->second >> ability_vnum;
            rhs.abilities[Ability(it->first)] = ability_vnum;
        }
    }
    // Deserialize the body parts.
    if(lhs.has_property("body_parts")){
        json::jnode_t bodyparts_node = lhs["body_parts"];
        for (auto it = bodyparts_node.abegin(); it != bodyparts_node.aend(); ++it) {
            unsigned int bodypart_vnum;
            (*it) >> bodypart_vnum;
            rhs.bodyParts.emplace_back(Mud::instance().findBodyPart(bodypart_vnum));
        }
    }
    // Deserialize the skills.
    if(lhs.has_property("skills")){
        json::jnode_t skills_node = lhs["skills"];
        for (auto it = skills_node.abegin(); it != skills_node.aend(); ++it) {
            unsigned int skill_vnum;
            unsigned int skill_level;
            (*it)["vnum"] >> skill_vnum;
            (*it)["level"] >> skill_level;
            rhs.skills.emplace_back(std::make_shared<SkillData>(Mud::instance().findSkill(skill_vnum), skill_level));
        }
    }
    return lhs;
}
