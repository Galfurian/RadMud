/// @file   building.cpp
/// @brief  Implmement building variables and methods.
/// @author Enrico Fraccaroli
/// @date   Feb 24 2016
/// @copyright
/// Copyright (c) 2016 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
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
#include "building.hpp"

#include "structure/room.hpp"

Building::Building() :
    vnum(),
    name(),
    difficulty(),
    time(),
    assisted(),
    tools(),
    buildingModel(),
    ingredients(),
    unique()
{
}

Building::Building(const Building & source) :
    vnum(source.vnum),
    name(source.name),
    difficulty(source.difficulty),
    time(source.time),
    assisted(source.assisted),
    tools(source.tools),
    buildingModel(source.buildingModel),
    ingredients(source.ingredients),
    unique(source.unique)
{
}

Building::~Building()
{
}

bool Building::check()
{
    assert(vnum > 0);
    assert(!name.empty());
    assert(difficulty > 0);
    assert(time > 0);
    assert(buildingModel != nullptr);
    assert(!tools.empty());
    for (auto it : tools)
    {
        assert(it != ToolType::NoType);
    }
    for (auto it : ingredients)
    {
        assert(it.first != ResourceType::NoType);
        assert(it.second > 0);
    }
    return true;
}

std::string Building::getName()
{
    return ToLower(name);
}

std::string Building::getNameCapital()
{
    return name;
}

bool Building::setTool(const std::string & source)
{
    if (source.empty())
    {
        Logger::log(LogLevel::Error, "Tool list is empty.");
        return false;
    }
    std::vector<std::string> toolList = SplitString(source, " ");
    for (auto it : toolList)
    {
        int value = ToNumber<int>(it);
        if (value < 0)
        {
            Logger::log(LogLevel::Error, "Tool list contains a negative value.");
            return false;
        }
        ToolType toolType = static_cast<ToolType>(static_cast<unsigned int>(value));
        if (toolType == ToolType::NoType)
        {
            Logger::log(LogLevel::Error, "Can't find the Tool :" + it);
            return false;
        }
        tools.insert(toolType);
    }
    return true;
}

bool Building::setIngredient(const std::string & source)
{
    if (source.empty())
    {
        return false;
    }
    // Split the string of ingredients.
    std::vector<std::string> ingredientList = SplitString(source, ";");
    for (auto iterator : ingredientList)
    {
        // Split the string with the information about the ingredient.
        std::vector<std::string> ingredientInfo = SplitString(iterator, "*");
        if (ingredientInfo.size() != 2)
        {
            Logger::log(LogLevel::Error, "Ingredient is not composed by [Ingredient*Quantity]");
            return false;
        }
        ResourceType ingredient = static_cast<ResourceType>(ToNumber<int>(ingredientInfo[0]));
        if (ingredient == ResourceType::NoType)
        {
            Logger::log(LogLevel::Error, "Can't find the Ingredient :" + ingredientInfo[0]);
            return false;
        }
        int quantity = ToNumber<int>(ingredientInfo[1]);
        if (quantity <= 0)
        {
            Logger::log(
                LogLevel::Error,
                "Can't find the quantity of the Outcome :" + ingredientInfo[0]);
            return false;
        }
        this->ingredients[ingredient] = static_cast<unsigned int>(quantity);
    }
    return true;
}
