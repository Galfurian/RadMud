/// @file   skill.cpp
/// @brief  Implements the skill functions.
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

#include "character/skill/skill.hpp"

Skill::Skill() :
    vnum(),
    name(),
    description(),
    ability(),
    stage(),
    requiredSkill(),
    usedForSkill(),
    modifierManager(std::make_shared<ModifierManager>())
{
    // Nothing to do.
}

Skill::~Skill()
{
//    Logger::log(LogLevel::Debug,
//                "Deleted skill\t\t[%s]\t\t(%s)",
//                ToString(this->vnum),
//                this->name);
}

bool Skill::check()
{
    if (vnum <= 0) return false;
    if (name.empty())return false;
    if (description.empty())return false;
    if (ability == Ability::None)return false;
    if (stage <= 0)return false;
    return true;
}

json::jnode_t &operator<<(json::jnode_t &lhs, const Skill &rhs)
{
    lhs.set_type(json::JTYPE_OBJECT);
    lhs["vnum"] << rhs.vnum;
    lhs["name"] << rhs.name;
    lhs["description"] << rhs.description;
    lhs["ability"] << static_cast<unsigned int>(rhs.ability);
    lhs["stage"] << rhs.stage;
    // Serialize the required skills.
    json::jnode_t requiredSkillsNode;
    requiredSkillsNode.set_type(json::JTYPE_ARRAY);
    for (auto const &it : rhs.requiredSkill) {
        requiredSkillsNode.add_element() << it;
    }
    lhs["required_skill"] = requiredSkillsNode;
    // Serialize the used for skills.
    json::jnode_t usedForSkillsNode;
    usedForSkillsNode.set_type(json::JTYPE_ARRAY);
    for (auto const &it : rhs.usedForSkill) {
        usedForSkillsNode.add_element() << it;
    }
    lhs["used_for_skill"] = usedForSkillsNode;
    // Serialize the modifier manager.
    json::jnode_t modifierManagerNode;
    modifierManagerNode.set_type(json::JTYPE_OBJECT);
    // Serialize the ability modifiers.
    json::jnode_t abilityModifiersNode;
    abilityModifiersNode.set_type(json::JTYPE_OBJECT);
    for (auto const &it : rhs.modifierManager->getAbilityMod()) {
        abilityModifiersNode[ability_to_string(it.first)] << it.second;
    }
    modifierManagerNode["ability_modifiers"] = abilityModifiersNode;
    // Serialize the combat modifiers.
    json::jnode_t combatModifiersNode;
    combatModifiersNode.set_type(json::JTYPE_OBJECT);
    for (auto const &it : rhs.modifierManager->getCombatMod()) {
        combatModifiersNode[combat_modifier_to_string(it.first)] << it.second;
    }
    modifierManagerNode["combat_modifiers"] = combatModifiersNode;
    // Serialize the status modifiers.
    json::jnode_t statusModifiersNode;
    statusModifiersNode.set_type(json::JTYPE_OBJECT);
    for (auto const &it : rhs.modifierManager->getStatusMod()) {
        statusModifiersNode[it.first.toString()] << it.second;
    }
    modifierManagerNode["status_modifiers"] = statusModifiersNode;
    // Serialize the knowledge.
    json::jnode_t knowledgeNode;
    knowledgeNode.set_type(json::JTYPE_OBJECT);
    for (auto const &it : rhs.modifierManager->getKnowledge()) {
        knowledgeNode[it.first.toString()] << it.second;
    }
    modifierManagerNode["knowledge"] = knowledgeNode;
    lhs["modifier_manager"] = modifierManagerNode;
    return lhs;
}

const json::jnode_t &operator>>(const json::jnode_t &lhs, Skill &rhs)
{
    lhs["vnum"] >> rhs.vnum;
    lhs["name"] >> rhs.name;
    lhs["description"] >> rhs.description;
    unsigned int ability;
    lhs["ability"] >> ability;
    rhs.ability = Ability(ability);
    lhs["stage"] >> rhs.stage;
    // Deserialize the required skills.
    if(lhs.has_property("required_skill")){
        json::jnode_t requiredSkillsNode = lhs["required_skill"];
        for (auto it = requiredSkillsNode.abegin(); it != requiredSkillsNode.aend(); ++it) {
            VnumType vnum;
            (*it) >> vnum;
            rhs.requiredSkill.emplace_back(vnum);
        }
    }
    // Deserialize the used for skills.
    if(lhs.has_property("used_for_skill")){
        json::jnode_t usedForSkillsNode = lhs["used_for_skill"];
        for (auto it = usedForSkillsNode.abegin(); it != usedForSkillsNode.aend(); ++it) {
            VnumType vnum;
            (*it) >> vnum;
            rhs.usedForSkill.emplace_back(vnum);
        }
    }
    // Deserialize the modifier manager.
    if(lhs.has_property("modifier_manager")){
        json::jnode_t modifierManagerNode = lhs["modifier_manager"];
        // Deserialize the ability modifiers.
        if(modifierManagerNode.has_property("ability_modifiers")){
            json::jnode_t abilityModifiersNode = modifierManagerNode["ability_modifiers"];
            for (auto it = abilityModifiersNode.pbegin(); it != abilityModifiersNode.pend(); ++it) {
                int modifier;
                it->second >> modifier;
                rhs.modifierManager->setAbilityMod(string_to_ability(it->first), modifier);
            }
        }
        // Deserialize the combat modifiers.
        if(modifierManagerNode.has_property("combat_modifiers")){
            json::jnode_t combatModifiersNode = modifierManagerNode["combat_modifiers"];
            for (auto it = combatModifiersNode.pbegin(); it != combatModifiersNode.pend(); ++it) {
                int modifier;
                it->second >> modifier;
                rhs.modifierManager->setCombatMod(string_to_combat_modifier(it->first), modifier);
            }
        }
        // Deserialize the status modifiers.
        if(modifierManagerNode.has_property("status_modifiers")){
            json::jnode_t statusModifiersNode = modifierManagerNode["status_modifiers"];
            for (auto it = statusModifiersNode.pbegin(); it != statusModifiersNode.pend(); ++it) {
                int modifier;
                it->second >> modifier;
                rhs.modifierManager->setStatusMod(StatusModifier(it->first), modifier);
            }
        }
        // Deserialize the knowledge.
        if(modifierManagerNode.has_property("knowledge")){
            json::jnode_t knowledgeNode = modifierManagerNode["knowledge"];
            for (auto it = knowledgeNode.pbegin(); it != knowledgeNode.pend(); ++it) {
                int modifier;
                it->second >> modifier;
                rhs.modifierManager->setKnowledge(Knowledge(it->first), modifier);
            }
        }
    }
    return lhs;
}