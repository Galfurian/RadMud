/// @file   building.cpp
/// @brief  Implement a building.
/// @author Enrico Fraccaroli
/// @date   Feb 24 2016
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

#include "creation/building.hpp"

#include "model/submodel/resourceModel.hpp"
#include "enumerators/knowledge.hpp"
#include "model/submodel/toolModel.hpp"
#include "utilities/logger.hpp"
#include "mud.hpp"

Building::Building() :
    vnum(),
    name(),
    difficulty(),
    time(),
    assisted(),
    buildingModel(),
    tools(),
    ingredients(),
    requiredKnowledge()
{
    // Nothing to do.
}

Building::~Building()
{
    // Nothing to do.
}

std::string Building::getName()
{
    return ToLower(name);
}

std::string Building::getNameCapital()
{
    return name;
}

json::jnode_t &operator<<(json::jnode_t &lhs, const Building &rhs)
{
    lhs.set_type(json::JTYPE_OBJECT);
    lhs["vnum"] << rhs.vnum;
    lhs["name"] << rhs.name;
    lhs["difficulty"] << rhs.difficulty;
    lhs["time"] << rhs.time;
    lhs["assisted"] << rhs.assisted;
    lhs["building_model"] << rhs.buildingModel->vnum;
    // Serialize the tools.
    json::jnode_t tools_node;
    tools_node.set_type(json::JTYPE_ARRAY);
    for (auto const &it : rhs.tools) {
        tools_node.add_element() << it.toUInt();
    }
    lhs["tools"] = tools_node;
    // Serialize the ingredients.
    json::jnode_t ingredients_node;
    ingredients_node.set_type(json::JTYPE_OBJECT);
    for (auto const &it : rhs.ingredients) {
        ingredients_node[it.first.toString()] << it.second;
    }
    lhs["ingredients"] = ingredients_node;
    // Serialize the required knowledge.
    json::jnode_t knowledge_node;
    knowledge_node.set_type(json::JTYPE_ARRAY);
    for (auto const &it : rhs.requiredKnowledge) {
        knowledge_node.add_element() << it.toUInt();
    }
    lhs["required_knowledge"] = knowledge_node;
    return lhs;
}

const json::jnode_t &operator>>(const json::jnode_t &lhs, Building &rhs)
{
    lhs["vnum"] >> rhs.vnum;
    lhs["name"] >> rhs.name;
    lhs["difficulty"] >> rhs.difficulty;
    lhs["time"] >> rhs.time;
    lhs["assisted"] >> rhs.assisted;
    int building_model_vnum;
    lhs["building_model"] >> building_model_vnum;
    rhs.buildingModel = Mud::instance().findItemModel(building_model_vnum);
    // Deserialize the tools.
    if(lhs.has_property("tools")){
        json::jnode_t tools_node = lhs["tools"];
        for (auto it = tools_node.abegin(); it != tools_node.aend(); ++it) {
            unsigned int tool;
            (*it) >> tool;
            rhs.tools.emplace_back(ToolType(tool));
        }
    }
    // Deserialize the ingredients.
    if(lhs.has_property("ingredients")){
        json::jnode_t ingredients_node = lhs["ingredients"];
        for (auto it = ingredients_node.pbegin(); it != ingredients_node.pend(); ++it) {
            unsigned int quantity;
            it->second >> quantity;
            rhs.ingredients[ResourceType(it->first)] = quantity;
        }
    }
    // Deserialize the required knowledge.
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
