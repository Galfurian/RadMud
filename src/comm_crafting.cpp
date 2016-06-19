/// @file   comm_crafting.cpp
/// @brief  Implements the methods used by the player in order to <b>craft items</b>.
/// @author Enrico Fraccaroli
/// @date   Aug 23 2014
/// @copyright
/// Copyright (c) 2014, 2015, 2016 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
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
#include <istream>

#include "commands.hpp"
// Other Include.
#include "mud.hpp"

using namespace std;

void DoProfession(Character * character, Profession * profession, std::istream & sArgs)
{
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    if (arguments.size() != 1)
    {
        character->sendMsg("What do you want to produce?\n");
        return;
    }
    // Search the production.
    Production * production = Mud::instance().findProduction(arguments[0].first);
    if (production == nullptr)
    {
        character->sendMsg("%s '%s'.\n", profession->notFoundMessage, arguments[0].first);
        return;
    }
    if (production->profession != profession)
    {
        character->sendMsg("%s '%s'.\n", profession->notFoundMessage, arguments[0].first);
        return;
    }
    // Check if the actor has enough stamina to execute the action.
    unsigned int consumedStamina;
    if (!character->hasStaminaFor(consumedStamina, ActionType::Crafting))
    {
        character->sendMsg("You are too tired right now.\n");
        Logger::log(
            LogLevel::Debug,
            "[%s] Has %s stamina and needs %s.",
            character->getName(),
            ToString(character->getStamina()),
            ToString(consumedStamina));
        return;
    }
    // Search the needed tools.
    ItemVector usedTools;
    if (!character->findNearbyTools(production->tools, usedTools))
    {
        character->sendMsg("You don't have the right tools.\n");
        return;
    }
    // Search the needed ingredients.
    ItemVector usedIngredients;
    if (!character->findNearbyResouces(production->ingredients, usedIngredients))
    {
        character->sendMsg("You don't have enough material.\n");
        return;
    }
    // Search the needed workbench.
    if (production->workbench != ToolType::NoType)
    {
        bool found = false;
        for (auto it : character->room->items)
        {
            if (it->model->getToolFunc().type == production->workbench)
            {
                found = true;
                break;
            }
        }
        if (!found)
        {
            character->sendMsg("The proper workbench is not present.\n");
            return;
        }
    }

    // Search the production material among the selected ingredients.
    Material * craftMaterial = nullptr;
    for (auto iterator : usedIngredients)
    {
        ResourceType itemResourceType = iterator->model->getResourceFunc().type;
        if (itemResourceType == production->material)
        {
            craftMaterial = iterator->composition;
            break;
        }
    }
    if (craftMaterial == nullptr)
    {
        character->sendMsg("You cannot decide which will be the material.\n");
        return;
    }

    // //////////////////////////////////////////
    // Prepare the action.
    if (!character->getAction()->setCraft(production, usedTools, usedIngredients, craftMaterial, production->time))
    {
        character->sendMsg("You can't start the process of production.\n");
        return;
    }

    // //////////////////////////////////////////
    character->sendMsg(
        "%s %s.\n",
        profession->startMessage,
        Formatter::yellow() + production->outcome.first->getName() + Formatter::reset());

    // Set the list of exceptions.
    CharacterVector exceptions;
    exceptions.push_back(character);
    // Send the message inside the room.
    character->room->sendToAll(
        "%s has started %s something...\n",
        exceptions,
        character->getNameCapital(),
        production->profession->action);
}

void DoBuild(Character * character, std::istream & sArgs)
{
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    if (arguments.size() != 1)
    {
        character->sendMsg("What do you want to build?\n");
        return;
    }
    Building * schematics = Mud::instance().findBuilding(arguments[0].first);
    if (schematics == nullptr)
    {
        character->sendMsg("You don't know how to build '%s'.\n", arguments[0].first);
        return;
    }
    // Check if the actor has enough stamina to execute the action.
    unsigned int consumedStamina;
    if (!character->hasStaminaFor(consumedStamina, ActionType::Building))
    {
        character->sendMsg("You are too tired right now.\n");
        Logger::log(
            LogLevel::Debug,
            "[%s] Has %s stamina and needs %s.",
            character->getName(),
            ToString(character->getStamina()),
            ToString(consumedStamina));
        return;
    }
    // Search the needed tools.
    ItemVector usedTools;
    if (!character->findNearbyTools(schematics->tools, usedTools))
    {
        character->sendMsg("You don't have the right tools.\n");
        return;
    }
    // Search the needed ingredients.
    ItemVector usedIngredients;
    if (!character->findNearbyResouces(schematics->ingredients, usedIngredients))
    {
        character->sendMsg("You don't have enough material.\n");
        return;
    }
    // Search the model that has to be built.
    Item * building = nullptr;
    for (auto iterator : character->inventory)
    {
        if (iterator->model->vnum == schematics->buildingModel->vnum)
        {
            building = iterator;
            break;
        }
    }
    if (building == nullptr)
    {
        for (auto iterator : character->room->items)
        {
            if (iterator->model->vnum == schematics->buildingModel->vnum)
            {
                building = iterator;
                break;
            }
        }
        if (building == nullptr)
        {
            // Otherwise notify the missing item.
            character->sendMsg("You don't have the main building item.\n");
            return;
        }
    }
    for (auto iterator : character->room->items)
    {
        if (HasFlag(iterator->flags, ItemFlag::Built))
        {
            if (schematics->unique)
            {
                // Otherwise notify the missing item.
                character->sendMsg("There are already something built here.\n");
                return;
            }
            Building * builtSchematics = Mud::instance().findBuilding(iterator->model->vnum);
            if (builtSchematics)
            {
                if (builtSchematics->unique)
                {
                    // Otherwise notify the missing item.
                    character->sendMsg("You cannot build something here.\n");
                    return;
                }
            }
        }
    }

    // //////////////////////////////////////////
    // Prepare the action.
    if (!character->getAction()->setBuild(schematics, building, usedTools, usedIngredients, schematics->time))
    {
        character->sendMsg("You can't start the building.\n");
        return;
    }

    // //////////////////////////////////////////
    character->sendMsg(
        "You start building %s.\n",
        Formatter::yellow() + schematics->buildingModel->getName() + Formatter::reset());

    // //////////////////////////////////////////
    std::string roomMsg;
    roomMsg += character->name + " has started building something...\n";
    // Set the list of exceptions.
    CharacterVector exceptions;
    exceptions.push_back(character);
    // Send the message inside the room.
    character->room->sendToAll(roomMsg, exceptions);
}

void DoDeconstruct(Character * character, std::istream & sArgs)
{
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    if (arguments.size() != 1)
    {
        character->sendMsg("What do you want to deconstruct, sai?\n");
        return;
    }
    Item * item = character->findNearbyItem(arguments[0].first, arguments[0].second);
    if (item == nullptr)
    {
        character->sendMsg("You can't find want you to deconstruct.\n");
        return;
    }
    // Check if the actor has enough stamina to execute the action.
    unsigned int consumedStamina;
    if (!character->hasStaminaFor(consumedStamina, ActionType::Building))
    {
        character->sendMsg("You are too tired right now.\n");
        Logger::log(
            LogLevel::Debug,
            "[%s] Has %s stamina and needs %s.",
            character->getName(),
            ToString(character->getStamina()),
            ToString(consumedStamina));
        return;
    }
    if (HasFlag(item->flags, ItemFlag::Built))
    {
        character->sendMsg("You deconstruct %s.\n", item->getName());
        // Reset item flags.
        ClearFlag(item->flags, ItemFlag::Built);
        SQLiteDbms::instance().beginTransaction();
        item->updateOnDB();
        SQLiteDbms::instance().endTransaction();
    }
    else
    {
        character->sendMsg("%s is not built.\n", item->getNameCapital());
    }
}

void DoRead(Character * character, std::istream & sArgs)
{
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    if (arguments.size() != 1)
    {
        character->sendMsg("What do you want to read today, sai?\n");
        return;
    }
    Item * item = character->findNearbyItem(arguments[0].first, arguments[0].second);
    if (item == nullptr)
    {
        character->sendMsg("You can't find want you to read.\n");
        return;
    }
    Writing * writing = Mud::instance().findWriting(item->vnum);
    if (writing == nullptr)
    {
        character->sendMsg("There is nothing written on %s.\n", item->getName());
        return;
    }
    character->sendMsg("You start reading %s...\n", item->getName());
    if (!writing->title.empty())
    {
        character->sendMsg("The title is '%s'.\n", writing->title);
    }
    character->sendMsg(writing->content + "\n");
    if (!writing->author.empty())
    {
        character->sendMsg("The author apears to be '%s'.\n", writing->author);
    }
}

