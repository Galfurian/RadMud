/// @file   building.cpp
/// @brief  Implmement building variables and methods.
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

// Basic Include.
#include "building.hpp"

#include "logger.hpp"
#include "room.hpp"

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
    // Nothing to do.
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
    // Nothing to do.
}

Building::~Building()
{
    // Nothing to do.
}

bool Building::check()
{
    if (vnum <= 0) return false;
    if (name.empty()) return false;
    if (difficulty <= 0) return false;
    if (time <= 0) return false;
    if (buildingModel == nullptr) return false;
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
        ToolType toolType = ToolType(ToNumber<unsigned int>(it));
        if (toolType == ToolType::None)
        {
            Logger::log(LogLevel::Error, "Can't find the Tool :" + it);
            return false;
        }
        tools.emplace_back(toolType);
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
        std::vector<std::string> info = SplitString(iterator, "*");
        if (info.size() != 2)
        {
            Logger::log(LogLevel::Error, "Ingredient is not composed by"
                " [Ingredient*Quantity]");
            return false;
        }
        auto ingredient = ResourceType(
            ToNumber<unsigned int>(info[0]));
        if (ingredient == ResourceType::None)
        {
            Logger::log(LogLevel::Error,
                        "Can't find the Ingredient :%s",
                        info[0]);
            return false;
        }
        int quantity = ToNumber<int>(info[1]);
        if (quantity <= 0)
        {
            Logger::log(LogLevel::Error,
                        "Can't find the quantity of the Outcome :" +
                        info[0]);
            return false;
        }
        this->ingredients[ingredient] = static_cast<unsigned int>(quantity);
    }
    return true;
}
