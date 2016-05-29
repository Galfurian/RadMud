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
        character->sendMsg(profession->notFoundMessage + " '" + arguments[0].first + "'.\n");
        return;
    }
    if (production->profession != profession)
    {
        character->sendMsg(profession->notFoundMessage + " '" + arguments[0].first + "'.\n");
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
    if (!character->action.setCraft(production, usedTools, usedIngredients, craftMaterial, production->time))
    {
        character->sendMsg("You can't start the process of production.\n");
        return;
    }

    // //////////////////////////////////////////
    std::string charMsg;
    charMsg += profession->startMessage;
    charMsg += " " + Telnet::yellow() + production->outcome.first->getName() + Telnet::reset() + ".\n";
    character->sendMsg(charMsg);

    std::string roomMsg;
    roomMsg += character->name + " has started " + production->profession->action + " something...\n";
    character->room->sendToAll(roomMsg, character);
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
        character->sendMsg("You don't know how to build '" + arguments[0].first + "'.\n");
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
    if (!character->action.setBuild(schematics, building, usedTools, usedIngredients, schematics->time))
    {
        character->sendMsg("You can't start the building.\n");
        return;
    }

    // //////////////////////////////////////////
    std::string charMsg;
    charMsg += "You start building";
    charMsg += " " + Telnet::yellow() + schematics->buildingModel->getName() + Telnet::reset() + ".\n";
    character->sendMsg(charMsg);

    // //////////////////////////////////////////
    std::string roomMsg;
    roomMsg += character->name + " has started building something...\n";
    character->room->sendToAll(roomMsg, character);
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
    if (HasFlag(item->flags, ItemFlag::Built))
    {
        character->sendMsg("You deconstruct " + item->getName() + ".\n");
        // Reset item flags.
        item->flags = 0;
        SQLiteDbms::instance().beginTransaction();
        item->updateOnDB();
        SQLiteDbms::instance().endTransaction();
    }
    else
    {
        character->sendMsg(item->getNameCapital() + " is not built.\n");
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
        character->sendMsg("There is nothing written on " + item->getName() + ".\n");
        return;
    }
    character->sendMsg("You start reading " + item->getName() + "...\n");
    if (!writing->title.empty())
    {
        character->sendMsg("The title is '" + writing->title + "'.\n");
    }
    character->sendMsg(writing->content + "\n");
    if (!writing->author.empty())
    {
        character->sendMsg("The author apears to be '" + writing->author + "'.\n");
    }
}

