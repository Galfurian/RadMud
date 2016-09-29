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

#include "crafting.hpp"

#include "../mud.hpp"
#include "../model/resourceModel.hpp"
#include "../action/buildAction.hpp"
#include "../action/craftAction.hpp"

void LoadCraftingCommands()
{
    Command command;
    command.level = 0;
    {
        command.name = "build";
        command.help = "Build something.";
        command.args = "(item)";
        command.hndl = DoBuild;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "deconstruct";
        command.help = "Deconstruct a building.";
        command.args = "(building)";
        command.hndl = DoDeconstruct;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "read";
        command.help = "Read an inscription from an item.";
        command.args = "(item)";
        command.hndl = DoRead;
        Mud::instance().addCommand(command);
    }
}

void DoProfession(Character * character, Profession * profession, ArgumentHandler & args)
{
    if (args.size() != 1)
    {
        character->sendMsg("What do you want to produce?\n");
        return;
    }
    // Search the production.
    auto production = Mud::instance().findProduction(args[0].getContent());
    if (production == nullptr)
    {
        character->sendMsg("%s '%s'.\n", profession->notFoundMessage, args[0].getContent());
        return;
    }
    if (production->profession != profession)
    {
        character->sendMsg("%s '%s'.\n", profession->notFoundMessage, args[0].getContent());
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
    std::vector<std::pair<Item *, unsigned int>> usedIngredients;
    if (!character->findNearbyResouces(production->ingredients, usedIngredients))
    {
        character->sendMsg("You don't have enough material.\n");
        return;
    }
    // Search the needed workbench.
    if (production->workbench != ToolType::NoType)
    {
        Item * workbench = character->findNearbyTool(
            production->workbench,
            std::vector<Item *>(),
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
        auto item = iterator.first;
        if (item->model->getType() == ModelType::Resource)
        {
            ResourceModel * resourceModel = item->model->toResource();
            if (resourceModel->resourceType == production->material)
            {
                craftMaterial = item->composition;
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
    auto craftAction = std::make_shared<CraftAction>(
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
    // Send the messages.
    character->sendMsg(
        "%s %s.\n",
        profession->startMessage,
        Formatter::yellow() + production->outcome->getName() + Formatter::reset());
    character->room->sendToAll(
        "%s has started %s something...\n",
        { character },
        character->getNameCapital(),
        production->profession->action);
}

void DoBuild(Character * character, ArgumentHandler & args)
{
    if (args.size() != 1)
    {
        character->sendMsg("What do you want to build?\n");
        return;
    }
    auto schematics = Mud::instance().findBuilding(args[0].getContent());
    if (schematics == nullptr)
    {
        character->sendMsg("You don't know how to build '%s'.\n", args[0].getContent());
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
    std::vector<std::pair<Item *, unsigned int>> usedIngredients;
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
    auto buildAction = std::make_shared<BuildAction>(
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
    // Send the message inside the room.
    character->room->sendToAll(
        "%s has started building something...\n",
        { character },
        character->getNameCapital());
}

void DoDeconstruct(Character * character, ArgumentHandler & args)
{
    if (args.size() != 1)
    {
        character->sendMsg("What do you want to deconstruct, sai?\n");
        return;
    }
    auto item = character->findNearbyItem(args[0].getContent(), args[0].getIndex());
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
        character->sendMsg("You deconstruct %s.\n", item->getName(true));
        // Reset item flags.
        ClearFlag(item->flags, ItemFlag::Built);
    }
}

void DoRead(Character * character, ArgumentHandler & args)
{
    if (args.size() != 1)
    {
        character->sendMsg("What do you want to read today, sai?\n");
        return;
    }
    auto item = character->findNearbyItem(args[0].getContent(), args[0].getIndex());
    if (item == nullptr)
    {
        character->sendMsg("You can't find want you to read.\n");
        return;
    }
    auto writing = Mud::instance().findWriting(item->vnum);
    if (writing == nullptr)
    {
        character->sendMsg("There is nothing written on %s.\n", item->getName(true));
        return;
    }
    character->sendMsg("You start reading %s...\n", item->getName(true));
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

