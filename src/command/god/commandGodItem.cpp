/// @file   commandGodItem.cpp
/// @author Enrico Fraccaroli
/// @date   Jan 02 2017
/// @copyright
/// Copyright (c) 2017 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
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

#include "commandGodItem.hpp"
#include "mud.hpp"

bool DoItemCreate(Character * character, ArgumentHandler & args)
{
    // Stop any ongoing action.
    StopAction(character);
    // Check the number of arguments.
    if ((args.size() != 2) && (args.size() != 3))
    {
        character->sendMsg("What do you want to create?\n");
        return false;
    }
    // Get the quantity.
    auto quantity = args[0].getMultiplier();
    // Get the model.
    auto modeVnum = ToNumber<int>(args[0].getContent());
    auto itemModel = Mud::instance().findItemModel(modeVnum);
    if (itemModel == nullptr)
    {
        character->sendMsg("Cannot find model '%s'.\n", args[0].getContent());
        return false;
    }
    // Get the material.
    auto materialVnum = ToNumber<int>(args[1].getContent());
    auto material = Mud::instance().findMaterial(materialVnum);
    if (material == nullptr)
    {
        character->sendMsg("Cannot find material '%s'.\n",
                           args[1].getContent());
        return false;
    }
    // Get the quality.
    auto quality = ItemQuality(ItemQuality::Normal);
    if (args.size() == 3)
    {
        auto itemQualityValue = ToNumber<unsigned int>(args[2].getContent());
        if (!ItemQuality::isValid(itemQualityValue))
        {
            character->sendMsg("Not a valid quality.\n");
            return false;
        }
        quality = ItemQuality(itemQualityValue);
    }
    // Create the item.
    auto item = itemModel->createItem(character->getName(), material, false,
                                      quality, quantity);
    if (item == nullptr)
    {
        character->sendMsg("Creation failed.\n");
        return false;
    }
    character->addInventoryItem(item);
    character->sendMsg(
        "You produce '%s' out of your apparently empty top hat.\n",
        item->getName(true));
    return true;
}

bool DoItemDestroy(Character * character, ArgumentHandler & args)
{
    if (args.size() != 1)
    {
        character->sendMsg("You must instert an item vnum.\n");
        return false;
    }
    auto item = Mud::instance().findItem(ToNumber<int>(args[0].getContent()));
    if (item == nullptr)
    {
        character->sendMsg("Invalid vnum.\n");
        return false;
    }
    if (!item->isEmpty())
    {
        character->sendMsg("You cannot destroy a non-empty item.\n");
        return false;
    }
    character->sendMsg("You have destroyed the desired object.\n");
    MudUpdater::instance().addItemToDestroy(item);
    return true;
}

bool DoItemGet(Character * character, ArgumentHandler & args)
{
    if (args.size() != 1)
    {
        character->sendMsg("You must instert an item vnum.\n");
        return false;
    }
    auto itemVnum = ToNumber<int>(args[0].getContent());
    auto item = Mud::instance().findItem(itemVnum);
    if (item == nullptr)
    {
        character->sendMsg("Invalid vnum (%s).\n", ToString(itemVnum));
        return false;
    }
    // Check if the God player can carry the item.
    if (!character->canCarry(item, item->quantity))
    {
        character->sendMsg("You can't carry anymore items.\n");
        return false;
    }
    if (item->room != nullptr)
    {
        character->sendMsg("The item was inside the room '%s' (%s)\n",
                           item->room->name, ToString(item->room->vnum));
        item->room->removeItem(item);
    }
    else if (item->owner != nullptr)
    {
        character->sendMsg("The item was possessed by '%s'\n",
                           item->owner->getName());
        if (!character->remEquipmentItem(item))
        {
            if (!character->remInventoryItem(item))
            {
                character->sendMsg("Cannot take the item from the owner!");
                return false;
            }
        }
    }
    else if (item->container != nullptr)
    {
        character->sendMsg("The item was inside the container '%s'\n",
                           item->container->getName(true));
        item->container->takeOut(item);
    }
    else
    {
        character->sendMsg("The item was nowhere!");
    }
    character->sendMsg("You materialize the desired object in your hands.\n");
    character->addInventoryItem(item);
    return true;
}

bool DoItemInfo(Character * character, ArgumentHandler & args)
{
    if (args.size() != 1)
    {
        character->sendMsg("You must insert the item vnum or the name"
                               " of the item inside the room.\n");
        return false;
    }
    auto item = Mud::instance().findItem(ToNumber<int>(args[0].getContent()));
    if (item == nullptr)
    {
        item = character->findNearbyItem(args[0].getContent(),
                                         args[0].getIndex());
        if (item == nullptr)
        {

            character->sendMsg("Cannot find the target"
                                   " item (provide vnum or item name).\n");
            return false;
        }
    }
    // Create a table.
    Table sheet(item->getNameCapital());
    // Get the sheet.
    item->getSheet(sheet);
    // Show the seet to character.
    character->sendMsg(sheet.getTable());
    return true;
}

bool DoItemList(Character * character, ArgumentHandler & args)
{
    // Variables used to filter the listed items.
    std::string itemName;
    std::string typeName;
    int modelVnum = -1;
    for (size_t argIt = 0; argIt < args.size(); ++argIt)
    {
        if (args[argIt].getContent() == "--help")
        {
            std::string help = "Usage:\n";
            help += "    item_list [options]\n";
            help += "Options:\n";
            help += "    -n [string]     Search items with the name which";
            help += " contains the given string.\n";
            help += "    -t [type_name]  Search items of the given type.\n";
            help += "    -m [model_vnum] Search the items of the model";
            help += " having the given vnum.\n";
            character->sendMsg(help);
            return true;
        }
        if ((argIt + 1) < args.size())
        {
            if (args[argIt].getContent() == "-n")
            {
                itemName = args[argIt + 1].getContent();
            }
            if (args[argIt].getContent() == "-t")
            {
                typeName = args[argIt + 1].getContent();
            }
            if (args[argIt].getContent() == "-m")
            {
                modelVnum = ToNumber<int>(args[argIt + 1].getContent());
            }
        }
    }
    Table table;
    table.addColumn("Vnum", StringAlign::Right);
    table.addColumn("Name", StringAlign::Left);
    table.addColumn("Type", StringAlign::Left);
    table.addColumn("Model", StringAlign::Left);
    table.addColumn("Location", StringAlign::Left);
    for (auto iterator : Mud::instance().mudItems)
    {
        auto item = iterator.second;
        if (!itemName.empty())
        {
            if (item->getName(false).find(itemName) == std::string::npos)
            {
                continue;
            }
        }
        if (!typeName.empty())
        {
            if (!BeginWith(ToLower(item->getTypeName()), ToLower(typeName)))
            {
                continue;
            }
        }
        if (modelVnum != -1)
        {
            if (item->model->vnum != modelVnum) continue;
        }
        // Prepare the row.
        TableRow row;
        row.push_back(ToString(item->vnum));
        row.push_back(item->getNameCapital());
        row.push_back(item->getTypeName());
        row.push_back(ToString(item->model->vnum));
        if (item->owner != nullptr)
        {
            row.push_back(" Owner  : " + item->owner->getName());
        }
        else if (item->room != nullptr)
        {
            row.push_back(" Room   : " + ToString(item->room->vnum));
        }
        else if (item->container != nullptr)
        {
            row.push_back(" Inside : " + ToString(item->container->vnum));
        }
        else
        {
            row.push_back(" Is nowhere.");
        }
        // Add the row to the table.
        table.addRow(row);
    }
    character->sendMsg(table.getTable());
    return true;
}

bool DoModelInfo(Character * character, ArgumentHandler & args)
{
    if (args.size() != 1)
    {
        character->sendMsg("You must insert a model vnum.\n");
        return false;
    }
    auto modelVnum = ToNumber<int>(args[0].getContent());
    auto itemModel = Mud::instance().findItemModel(modelVnum);
    if (itemModel == nullptr)
    {
        character->sendMsg("Item model not found %s.\n", modelVnum);
        return false;
    }
    // Create a table.
    Table sheet;
    // Get the sheet.
    itemModel->getSheet(sheet);
    // Show the seet to character.
    character->sendMsg(sheet.getTable());
    return true;
}

bool DoModelList(Character * character, ArgumentHandler & args)
{
    // Variables used to filter the listed items.
    std::string modelName;
    std::string modelType;
    int modelVnum = -1;
    for (size_t argIt = 0; argIt < args.size(); ++argIt)
    {
        if (args[argIt].getContent() == "--help")
        {
            std::string help = "Usage:\n";
            help += "    model_list [options]\n";
            help += "Options:\n";
            help += "    -n [string]     Search models with the name which";
            help += " contains the given string.\n";
            help += "    -t [type_name]  Search models of the given type.\n";
            help += "    -m [model_vnum] Search models with the given vnum.\n";
            character->sendMsg(help);
            return true;
        }
        if ((argIt + 1) < args.size())
        {
            if (args[argIt].getContent() == "-n")
            {
                modelName = args[argIt + 1].getContent();
            }
            if (args[argIt].getContent() == "-t")
            {
                modelType = args[argIt + 1].getContent();
            }
            if (args[argIt].getContent() == "-m")
            {
                modelVnum = ToNumber<int>(args[argIt + 1].getContent());
            }
        }
    }
    Table table;
    table.addColumn("VNUM", StringAlign::Right);
    table.addColumn("NAME", StringAlign::Left);
    table.addColumn("TYPE", StringAlign::Left);
    table.addColumn("SLOT", StringAlign::Left);
    table.addColumn("FLAGS", StringAlign::Right);
    for (auto iterator : Mud::instance().mudItemModels)
    {
        auto itemModel = iterator.second;
        if (!modelName.empty())
        {
            if (itemModel->name.find(modelName) == std::string::npos)
            {
                continue;
            }
        }
        if (!modelType.empty())
        {
            if (!BeginWith(ToLower(itemModel->getTypeName()),
                           ToLower(modelType)))
            {
                continue;
            }
        }
        if (modelVnum != -1)
        {
            if (itemModel->vnum != modelVnum) continue;
        }
        // Prepare the row.
        TableRow row;
        row.push_back(ToString(itemModel->vnum));
        row.push_back(itemModel->name);
        row.push_back(itemModel->getTypeName());
        std::string bodyParts;
        for (auto bodyPart : itemModel->bodyParts)
        {
            bodyParts += bodyPart->name + " ";
        }
        row.push_back(bodyParts);
        row.push_back(ToString(itemModel->modelFlags));
        // Add the row to the table.
        table.addRow(row);
    }
    character->sendMsg(table.getTable());
    return true;
}

bool DoWritingList(Character * character, ArgumentHandler & /*args*/)
{
    Table table;
    table.addColumn("VNUM", StringAlign::Center);
    table.addColumn("AUTHOR", StringAlign::Left);
    table.addColumn("TITLE", StringAlign::Left);
    for (auto iterator : Mud::instance().mudWritings)
    {
        Writing * writing = iterator.second;
        // Prepare the row.
        TableRow row;
        row.push_back(ToString(writing->vnum));
        row.push_back(writing->author);
        row.push_back(writing->title);
        // Add the row to the table.
        table.addRow(row);
    }
    character->sendMsg(table.getTable());
    return true;
}

