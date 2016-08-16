/// @file   crafting.cpp
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

#include "command.hpp"
#include "../mud.hpp"
#include "../model/resourceModel.hpp"
#include "../action/buildAction.hpp"
#include "../action/craftAction.hpp"

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
    if (character->getConsumedStaminaFor(ActionType::Crafting) > character->getStamina())
    {
        character->sendMsg("You are too tired right now.\n");
        return;
    }
    // Search the needed tools.
    std::vector<Item *> usedTools;
    if (!character->findNearbyTools(production->tools, usedTools, false, true, true))
    {
        character->sendMsg("You don't have the right tools.\n");
        return;
    }
    // Search the needed ingredients.
    std::vector<Item *> usedIngredients;
    if (!character->findNearbyResouces(production->ingredients, usedIngredients))
    {
        character->sendMsg("You don't have enough material.\n");
        return;
    }
    // Search the needed workbench.
    if (production->workbench != ToolType::NoType)
    {
        Item * workbench = character->findNearbyTool(production->workbench, std::vector<Item *>(),
        true,
        false,
        false);
        if (workbench == nullptr)
        {
            character->sendMsg("The proper workbench is not present.\n");
            return;
        }
    }
    // Search the production material among the selected ingredients.
    Material * craftMaterial = nullptr;
    for (auto iterator : usedIngredients)
    {
        if (iterator->model->getType() == ModelType::Resource)
        {
            ResourceModel * resourceModel = iterator->model->toResource();
            if (resourceModel->resourceType == production->material)
            {
                craftMaterial = iterator->composition;
                break;
            }
        }
    }
    if (craftMaterial == nullptr)
    {
        character->sendMsg("You cannot decide which will be the material.\n");
        return;
    }

    // //////////////////////////////////////////
    // Prepare the action.
    std::shared_ptr<CraftAction> craftAction = std::make_shared<CraftAction>(
        character,
        production,
        craftMaterial,
        usedTools,
        usedIngredients,
        production->time);
    // Check the new action.
    if (!craftAction->check())
    {
        character->sendMsg("You can't start the process of production.\n");
        return;
    }
    // Set the new action.
    character->setAction(craftAction);

    // //////////////////////////////////////////
    character->sendMsg(
        "%s %s.\n",
        profession->startMessage,
        Formatter::yellow() + production->outcome->getName() + Formatter::reset());

    // Send the message inside the room.
    character->room->sendToAll(
        "%s has started %s something...\n",
        { character },
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
    if (character->getConsumedStaminaFor(ActionType::Building) > character->getStamina())
    {
        character->sendMsg("You are too tired right now.\n");
        return;
    }
    // Search the needed tools.
    std::vector<Item *> usedTools;
    if (!character->findNearbyTools(schematics->tools, usedTools, true, true, true))
    {
        character->sendMsg("You don't have the right tools.\n");
        return;
    }
    // Search the needed ingredients.
    std::vector<Item *> usedIngredients;
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
    std::shared_ptr<BuildAction> buildAction = std::make_shared<BuildAction>(
        character,
        schematics,
        building,
        usedTools,
        usedIngredients,
        schematics->time);
    // Check the new action.
    if (!buildAction->check())
    {
        character->sendMsg("You can't start the building.\n");
        return;
    }
    // Set the new action.
    character->setAction(buildAction);

    // //////////////////////////////////////////
    character->sendMsg(
        "You start building %s.\n",
        Formatter::yellow() + schematics->buildingModel->getName() + Formatter::reset());

    // //////////////////////////////////////////
    std::string roomMsg;
    roomMsg += character->name + " has started building something...\n";
    // Send the message inside the room.
    character->room->sendToAll(roomMsg, { character });
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
    if (character->getConsumedStaminaFor(ActionType::Building) > character->getStamina())
    {
        character->sendMsg("You are too tired right now.\n");
        return;
    }
    std::string error;
    if (!item->canDeconstruct(error))
    {
        character->sendMsg(error + "\n");
    }
    else
    {
        character->sendMsg("You deconstruct %s.\n", item->getName());
        // Reset item flags.
        ClearFlag(item->flags, ItemFlag::Built);
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

