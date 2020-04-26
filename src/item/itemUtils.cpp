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

#include "item/itemUtils.hpp"
#include "model/submodel/toolModel.hpp"
#include "model/submodel/foodModel.hpp"
#include "model/submodel/lightModel.hpp"
#include "model/submodel/resourceModel.hpp"

namespace ItemUtils
{
/// @brief Search for the item inside the given container.
/// @param container The container.
/// @param key       The key of the item to search.
/// @param number    Position of the item we want to look for.
/// @return The item, if it's in the character's inventory.
Item *FindItemIn(std::vector<Item *> const &container, std::string const &key,
				 int &number)
{
	for (auto item : container) {
		if (!item->hasKey(ToLower(key)))
			continue;
		if (number == 1)
			return item;
		--number;
	}
	return nullptr;
}

bool IsValidTool(Item *item, ItemVector const &exceptions,
				 ToolType const &toolType)
{
	// Check the pointer to the model.
	if (item->model == nullptr) {
		return false;
	}
	// Check if the item is inside the exception list.
	if (std::find_if(exceptions.begin(), exceptions.end(),
					 [item](Item *exception) {
						 return (item->vnum == exception->vnum);
					 }) != exceptions.end()) {
		return false;
	}
	// Check if the item is actually a tool.
	if (item->getType() == ModelType::Tool) {
		// Check if the type of tool is the same.
		return item->model->to<ToolModel>()->toolType == toolType;
	}
	if ((toolType == ToolType::CookingFire) &&
		(item->getType() == ModelType::Light)) {
		// Check if the light source can be used to cook.
		return HasFlag(item->model->to<LightModel>()->lightSourceFlags,
					   LightModelFlags::CanUseToCook);
	}
	return false;
}

bool IsValidResource(Item *item, const ResourceType &type)
{
	// Check the pointer to the item.
	if (item == nullptr)
		return false;
	// Check the pointer to the model.
	if (item->model == nullptr)
		return false;
	auto modelType = item->getType();
	// Check if the item has the correct type.
	if (modelType == ModelType::Resource)
		return (item->model->to<ResourceModel>()->resourceType == type);
	// If the required type is meat, and the item is food, check if it is raw.
	if ((type == ResourceType::Meat) && (modelType == ModelType::Food))
		return HasFlag(item->model->to<FoodModel>()->foodFlags, FoodFlag::Raw);
	return false;
}

}