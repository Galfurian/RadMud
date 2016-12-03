/// @file   crafting.cpp
/// @brief  Implements the methods used by the player in order to <b>craft items</b>.
/// @author Enrico Fraccaroli
/// @date   Aug 23 2014
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

#include "crafting.hpp"

#include "mud.hpp"
#include "buildAction.hpp"
#include "craftAction.hpp"

void LoadCraftingCommands()
{
    {
        Command command;
        command.name = "build";
        command.help = "Build something.";
        command.arguments = "(item)";
        command.hndl = DoBuild;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.name = "deconstruct";
        command.help = "Deconstruct a building.";
        command.arguments = "(building)";
        command.hndl = DoDeconstruct;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.name = "read";
        command.help = "Read an inscription from an item.";
        command.arguments = "(item)";
        command.hndl = DoRead;
        Mud::instance().addCommand(command);
    }
}

bool DoProfession(Character * character, Profession * profession, ArgumentHandler & args)
{
    // Stop any action the character is executing.
    StopAction(character);
    if (args.size() != 1)
    {
        character->sendMsg("What do you want to produce?\n");
        return false;
    }
    // Search the production.
    auto production = Mud::instance().findProduction(args[0].getContent());
    if (production == nullptr)
    {
        character->sendMsg("%s '%s'.\n", profession->notFoundMessage, args[0].getContent());
        return false;
    }
    if (production->profession != profession)
    {
        character->sendMsg("%s '%s'.\n", profession->notFoundMessage, args[0].getContent());
        return false;
    }
    // Check if the actor has enough stamina to execute the action.
    if (CraftAction::getConsumedStamina(character) > character->getStamina())
    {
        character->sendMsg("You are too tired right now.\n");
        return false;
    }
    // Search the needed tools.
    std::vector<Item *> usedTools;
    if (!character->findNearbyTools(production->tools, usedTools, false, true, true))
    {
        character->sendMsg("You don't have the right tools.\n");
        return false;
    }
    // Search the needed ingredients.
    std::vector<std::pair<Item *, unsigned int>> usedIngredients;
    if (!character->findNearbyResouces(production->ingredients, usedIngredients))
    {
        character->sendMsg("You don't have enough material.\n");
        return false;
    }
    // Search the needed workbench.
    if (production->workbench != ToolType::None)
    {
        auto workbench = character->findNearbyTool(production->workbench, std::vector<Item *>(), true, false, false);
        if (workbench == nullptr)
        {
            character->sendMsg("The proper workbench is not present.\n");
            return false;
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
        return false;
    }
    // Prepare the action.
    auto craftAction = std::make_shared<CraftAction>(character, production, craftMaterial, usedTools, usedIngredients);
    // Check the new action.
    std::string error;
    if (craftAction->check(error))
    {
        // Set the new action.
        character->setAction(craftAction);
        // Send the messages.
        character->sendMsg(
            "%s %s.\n",
            profession->startMessage,
            Formatter::yellow() + production->outcome->getName() + Formatter::reset());
        character->room->sendToAll(
            "%s has started %s something...\n",
            {character},
            character->getNameCapital(),
            production->profession->action);
        return true;
    }
    character->sendMsg("%s\n", error);
    return false;
}

bool DoBuild(Character * character, ArgumentHandler & args)
{
    // Stop any action the character is executing.
    StopAction(character);
    if (args.size() != 1)
    {
        character->sendMsg("What do you want to build?\n");
        return false;
    }
    auto schematics = Mud::instance().findBuilding(args[0].getContent());
    if (schematics == nullptr)
    {
        character->sendMsg("You don't know how to build '%s'.\n", args[0].getContent());
        return false;
    }
    // Check if the actor has enough stamina to execute the action.
    if (BuildAction::getConsumedStamina(character) > character->getStamina())
    {
        character->sendMsg("You are too tired right now.\n");
        return false;
    }
    // Search the needed tools.
    std::vector<Item *> usedTools;
    if (!character->findNearbyTools(schematics->tools, usedTools, true, true, true))
    {
        character->sendMsg("You don't have the right tools.\n");
        return false;
    }
    // Search the needed ingredients.
    std::vector<std::pair<Item *, unsigned int>> usedIngredients;
    if (!character->findNearbyResouces(schematics->ingredients, usedIngredients))
    {
        character->sendMsg("You don't have enough material.\n");
        return false;
    }
    // Search the model that has to be built.
    auto it = std::find_if(character->inventory.begin(),
                           character->inventory.end(), [&schematics](Item * item)
                           {
                               return (item->model->vnum == schematics->buildingModel->vnum);
                           });
    if (it == character->inventory.end())
    {
        it = std::find_if(character->room->items.begin(),
                          character->room->items.end(), [&schematics](Item * item)
                          {
                              return (item->model->vnum == schematics->buildingModel->vnum);
                          });
        if (it == character->room->items.end())
        {
            // Otherwise notify the missing item.
            character->sendMsg("You don't have the main building item.\n");
            return false;
        }
    }
    auto building = (*it);
    // Check if there is already something built inside the room with the Unique flag.
    for (auto iterator : character->room->items)
    {
        if (HasFlag(iterator->flags, ItemFlag::Built))
        {
            if (schematics->unique)
            {
                // Otherwise notify the missing item.
                character->sendMsg("There are already something built here.\n");
                return false;
            }
            auto builtSchematics = Mud::instance().findBuilding(iterator->model->vnum);
            if (builtSchematics)
            {
                if (builtSchematics->unique)
                {
                    // Otherwise notify the missing item.
                    character->sendMsg("You cannot build something here.\n");
                    return false;
                }
            }
        }
    }
    // Prepare the action.
    auto buildAction = std::make_shared<BuildAction>(character, schematics, building, usedTools, usedIngredients);
    // Check the new action.
    std::string error;
    if (buildAction->check(error))
    {
        // Set the new action.
        character->setAction(buildAction);
        character->sendMsg(
            "You start building %s.\n",
            Formatter::yellow() + schematics->buildingModel->getName() + Formatter::reset());
        // Send the message inside the room.
        character->room->sendToAll(
            "%s has started building something...\n",
            {character},
            character->getNameCapital());
        return true;
    }
    character->sendMsg("%s\n", error);
    return false;
}

bool DoDeconstruct(Character * character, ArgumentHandler & args)
{
    // Stop any action the character is executing.
    StopAction(character);
    if (args.size() != 1)
    {
        character->sendMsg("What do you want to deconstruct, sai?\n");
        return false;
    }
    auto item = character->findNearbyItem(args[0].getContent(), args[0].getIndex());
    if (item == nullptr)
    {
        character->sendMsg("You can't find want you to deconstruct.\n");
        return false;
    }
    // Check if the actor has enough stamina to execute the action.
    if (BuildAction::getConsumedStamina(character) > character->getStamina())
    {
        character->sendMsg("You are too tired right now.\n");
        return false;
    }
    std::string error;
    if (item->canDeconstruct(error))
    {
        character->sendMsg("You deconstruct %s.\n", item->getName(true));
        // Reset item flags.
        ClearFlag(&item->flags, ItemFlag::Built);
        return true;
    }
    character->sendMsg(error + "\n");
    return false;
}

bool DoRead(Character * character, ArgumentHandler & args)
{
    // Stop any action the character is executing.
    StopAction(character);
    if (args.size() != 1)
    {
        character->sendMsg("What do you want to read today, sai?\n");
        return false;
    }
    auto item = character->findNearbyItem(args[0].getContent(), args[0].getIndex());
    if (item == nullptr)
    {
        character->sendMsg("You can't find want you to read.\n");
        return false;
    }
    auto writing = Mud::instance().findWriting(item->vnum);
    if (writing == nullptr)
    {
        character->sendMsg("There is nothing written on %s.\n", item->getName(true));
        return false;
    }
    character->sendMsg("You start reading %s...\n", item->getName(true));
    if (!writing->title.empty())
    {
        character->sendMsg("The title is '%s'.\n", writing->title);
    }
    character->sendMsg(writing->content + "\n");
    if (!writing->author.empty())
    {
        character->sendMsg("The author appears to be '%s'.\n", writing->author);
    }
    return true;
}

