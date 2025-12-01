/// @file   production.cpp
/// @brief  Implmement production variables and methods.
/// @author Enrico Fraccaroli
/// @date   Aug 11 2015
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

#include "creation/production.hpp"

#include "model/itemModel.hpp"
#include "character/character.hpp"
#include "creation/profession.hpp"
#include "mud.hpp"

Production::Production() :
    vnum(-1),
    name(),
    profession(),
    difficulty(),
    time(),
    assisted(),
    outcome(),
    quantity(),
    tools(),
    ingredients(),
    workbench(ToolType::None),
    requiredKnowledge()
{
    // Nothing to do.
}

Production::~Production()
{
//    Logger::log(LogLevel::Debug,
//                "Deleted production\t[%s]\t\t(%s)",
//                ToString(this->vnum),
//                this->name);
}

bool Production::check()
{
    if (vnum <= 0) return false;
    if (name.empty()) return false;
    if (profession == nullptr) return false;
    if (difficulty <= 0) return false;
    if (time <= 0) return false;
    if (outcome == nullptr) return false;
    if (quantity <= 0) return false;
    if (tools.empty()) return false;
    for (auto it : tools)
    {
        if (it == ToolType::None) return false;
    }
    for (auto it : ingredients)
    {
        if (it.first == ResourceType::None) return false;
        if (it.second <= 0) return false;
    }
    return true;
}

std::string Production::getName()
{
    return ToLower(name);
}

std::string Production::getNameCapital()
{
    return name;
}

json::jnode_t &operator<<(json::jnode_t &lhs, const Production &rhs)
{
    lhs.set_type(json::JTYPE_OBJECT);
    lhs["vnum"] << rhs.vnum;
    lhs["name"] << rhs.name;
    lhs["profession"] << rhs.profession->vnum;
    lhs["difficulty"] << rhs.difficulty;
    lhs["time"] << rhs.time;
    lhs["assisted"] << rhs.assisted;
    lhs["outcome"] << rhs.outcome->vnum;
    lhs["quantity"] << rhs.quantity;
    lhs["workbench"] << rhs.workbench.toUInt();
    json::jnode_t tools_node;
    tools_node.set_type(json::JTYPE_ARRAY);
    for (auto const &it : rhs.tools) {
        tools_node.add_element() << it.toUInt();
    }
    lhs["tools"] = tools_node;
    json::jnode_t ingredients_node;
    ingredients_node.set_type(json::JTYPE_OBJECT);
    for (auto const &it : rhs.ingredients) {
        ingredients_node[resource_type_to_string(it.first)] << it.second;
    }
    lhs["ingredients"] = ingredients_node;
    json::jnode_t knowledge_node;
    knowledge_node.set_type(json::JTYPE_ARRAY);
    for (auto const &it : rhs.requiredKnowledge) {
        knowledge_node.add_element() << static_cast<uint8_t>(it);
    }
    lhs["required_knowledge"] = knowledge_node;
    return lhs;
}

const json::jnode_t &operator>>(const json::jnode_t &lhs, Production &rhs)
{
    lhs["vnum"] >> rhs.vnum;
    lhs["name"] >> rhs.name;
    unsigned int profession_vnum;
    lhs["profession"] >> profession_vnum;
    rhs.profession = Mud::instance().findProfession(profession_vnum);
    lhs["difficulty"] >> rhs.difficulty;
    lhs["time"] >> rhs.time;
    lhs["assisted"] >> rhs.assisted;
    int outcome_vnum;
    lhs["outcome"] >> outcome_vnum;
    rhs.outcome = Mud::instance().findItemModel(outcome_vnum);
    lhs["quantity"] >> rhs.quantity;
    unsigned int workbench;
    lhs["workbench"] >> workbench;
    rhs.workbench = ToolType(workbench);
    if(lhs.has_property("tools")){
        json::jnode_t tools_node = lhs["tools"];
        for (auto it = tools_node.abegin(); it != tools_node.aend(); ++it) {
            unsigned int tool;
            (*it) >> tool;
            rhs.tools.emplace_back(ToolType(tool));
        }
    }
    if(lhs.has_property("ingredients")){
        json::jnode_t ingredients_node = lhs["ingredients"];
        for (auto it = ingredients_node.pbegin(); it != ingredients_node.pend(); ++it) {
            unsigned int quantity;
            it->second >> quantity;
            rhs.ingredients[string_to_resource_type(it->first)] = quantity;
        }
    }
    if(lhs.has_property("required_knowledge")){
        json::jnode_t knowledge_node = lhs["required_knowledge"];
        for (auto it = knowledge_node.abegin(); it != knowledge_node.aend(); ++it) {
            unsigned int knowledge;
            (*it) >> knowledge;
            rhs.requiredKnowledge.emplace_back(Knowledge(knowledge));
        }
    }
    return lhs;
}
