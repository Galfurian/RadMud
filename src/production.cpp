/// @file   production.cpp
/// @brief  Implmement production variables and methods.
/// @author Enrico Fraccaroli
/// @date   Aug 11 2015
/// @copyright
/// Copyright (c) 2015, 2016 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
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
#include "production.hpp"

#include <algorithm>

// Other Include.
#include "mud.hpp"
#include "room.hpp"
#include "utils.hpp"
#include "logger.hpp"
#include "character.hpp"
#include "constants.hpp"

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
        workbench(ToolType::NoType),
        material(ResourceType::NoType)
{
    // Nothing to do.
}

Production::~Production()
{
    Logger::log(
        LogLevel::Debug,
        "Deleted production\t[%s]\t\t(%s)",
        ToString(this->vnum),
        this->name);
}

bool Production::setOutcome(const std::string & source)
{
    if (source.empty())
    {
        Logger::log(LogLevel::Error, "No outcome set.");
        return false;
    }
    std::vector<std::string> outcomeList = SplitString(source, ";");
    // Flag used to determine if all goes well.
    bool correct = true;
    for (auto iterator : outcomeList)
    {
        std::vector<std::string> outcomeInfo = SplitString(iterator, "*");
        if (outcomeInfo.size() != 2)
        {
            correct = false;
        }

        outcome = Mud::instance().findItemModel(ToInt(outcomeInfo[0]));
        if (outcome == nullptr)
        {
            Logger::log(LogLevel::Error, "Can't find the Outcome :" + outcomeInfo[0]);
            correct = false;
            break;
        }
        int outcomeQuantity = ToInt(outcomeInfo[1]);
        if (outcomeQuantity <= 0)
        {
            Logger::log(
                LogLevel::Error,
                "Can't find the quantity of the Outcome :" + outcomeInfo[0]);
            correct = false;
            break;
        }
        quantity = static_cast<unsigned int>(outcomeQuantity);
    }
    return correct;
}

bool Production::setTool(const std::string & source)
{
    if (source.empty())
    {
        Logger::log(LogLevel::Error, "No tool set.");
        return false;
    }
    std::vector<std::string> toolList = SplitString(source, ";");
    // Flag used to determine if all goes well.
    bool correct = true;
    for (auto iterator : toolList)
    {
        ToolType toolType = static_cast<ToolType>(ToInt(iterator));
        if (toolType == ToolType::NoType)
        {
            Logger::log(LogLevel::Error, "Can't find the Tool Type:" + iterator);
            correct = false;
            break;
        }
        this->tools.insert(toolType);
    }
    return (!toolList.empty()) && correct;
}

bool Production::setIngredient(const std::string & source)
{
    if (source.empty())
    {
        Logger::log(LogLevel::Error, "No outcome set.");
        return true;
    }
    // Split the string of ingredients.
    std::vector<std::string> ingredientList = SplitString(source, ";");
    // Flag used to determine if all goes well.
    bool correct = true;
    for (auto iterator : ingredientList)
    {
        // Split the string with the information about the ingredient.
        std::vector<std::string> ingredientInfo = SplitString(iterator, "*");
        if (ingredientInfo.size() != 2)
        {
            correct = false;
            break;
        }
        ResourceType ingredient = static_cast<ResourceType>(ToInt(ingredientInfo[0]));
        if (ingredient == ResourceType::NoType)
        {
            Logger::log(LogLevel::Error, "Can't find the Ingredient :" + ingredientInfo[0]);
            correct = false;
            break;
        }

        int ingredientQuantity = ToInt(ingredientInfo[1]);
        if (ingredientQuantity == 0)
        {
            Logger::log(
                LogLevel::Error,
                "Can't find the quantity of the Outcome :" + ingredientInfo[0]);
            correct = false;
            break;
        }
        if (!this->ingredients.insert(std::make_pair(ingredient, ingredientQuantity)).second)
        {
            Logger::log(LogLevel::Error, "Cannot insert the ingredient");
            correct = false;
            break;
        }
    }
    return correct;
}

bool Production::check()
{
    assert(vnum >= 0);
    assert(!name.empty());
    assert(profession != nullptr);
    assert(difficulty > 0);
    assert(time > 0);
    assert(outcome != nullptr);
    assert(quantity > 0);
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
    assert(material != ResourceType::NoType);
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
