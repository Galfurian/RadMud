/// @file   itemUtils.cpp
/// @brief  Implement utility function for items.
/// @author Enrico Fraccaroli
/// @date   23 02 2018
/// @copyright
/// Copyright (c) 2018 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
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

#include "itemUtils.hpp"
#include "toolModel.hpp"
#include "foodModel.hpp"
#include "lightModel.hpp"
#include "resourceModel.hpp"

namespace ItemUtils
{

/// @brief Search for the item inside the given container.
/// @param container The container.
/// @param key       The key of the item to search.
/// @param number    Position of the item we want to look for.
/// @return The item, if it's in the character's inventory.
Item * FindItemIn(std::vector<Item *> const & container,
                  std::string const & key,
                  int & number)
{
    for (auto item : container)
    {
        if (!item->hasKey(ToLower(key))) continue;
        if (number == 1) return item;
        --number;
    }
    return nullptr;
}

bool IsValidTool(Item * item,
                 ItemVector const & exceptions,
                 ToolType const & toolType)
{
    // Check the pointer to the model.
    if (item->model == nullptr)
    {
        return false;
    }
    // Check if the item is inside the exception list.
    if (std::find_if(exceptions.begin(), exceptions.end(),
                     [item](Item * exception)
                     {
                         return (item->vnum == exception->vnum);
                     }) != exceptions.end())
    {
        return false;
    }
    // Check if the item is actually a tool.
    if (item->model->getType() == ModelType::Tool)
    {
        // Check if the type of tool is the same.
        return item->model->to<ToolModel>()->toolType == toolType;
    }
    if ((toolType == ToolType::CookingFire) &&
        (item->model->getType() == ModelType::Light))
    {
        // Check if the light source can be used to cook.
        return HasFlag(item->model->to<LightModel>()->lightSourceFlags,
                       LightModelFlags::CanUseToCook);
    }
    return false;
}

bool IsValidResource(Item * item,
                     const ResourceType & resourceType)
{
    // Check the pointer to the model.
    if (item->model == nullptr)
    {
        return false;
    }
    // Check if the item is a resource.
    if (item->model->getType() == ModelType::Resource)
    {
        // Check if the type of resource is the same.
        return (item->model->to<ResourceModel>()->resourceType == resourceType);
    }
    else if ((resourceType == ResourceType::Meat) &&
             (item->model->getType() == ModelType::Food))
    {
        // Check if the type of resource is the same.
        return HasFlag(item->model->to<FoodModel>()->foodFlags, FoodFlag::Raw);
    }
    return false;
}

}