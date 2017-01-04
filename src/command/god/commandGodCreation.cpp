/// @file   commandGodCreation.cpp
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

#include "commandGodCreation.hpp"
#include "mud.hpp"

bool DoMaterialInfo(Character * character, ArgumentHandler & args)
{
    if (args.size() != 1)
    {
        character->sendMsg("You must insert a valide material vnum.\n");
        return false;
    }
    auto materialVnum = ToNumber<int>(args[0].getContent());
    auto material = Mud::instance().findMaterial(materialVnum);
    if (material == nullptr)
    {
        character->sendMsg("Can't find the desire material %s.\n",
                           materialVnum);
        return false;
    }
    std::string msg;
    msg += "Vnum      : " + ToString(material->vnum) + "\n";
    msg += "Type      : " + material->type.toString() + "\n";
    msg += "Name      : " + material->name + "\n";
    msg += "Worth     : " + ToString(material->worth) + "\n";
    msg += "Hardness  : " + ToString(material->hardness) + "\n";
    msg += "Lightness : " + ToString(material->lightness) + "\n";
    character->sendMsg(msg);
    return true;
}

bool DoMaterialList(Character * character, ArgumentHandler & /*args*/)
{
    Table table;
    table.addColumn("VNUM", StringAlign::Right);
    table.addColumn("TYPE", StringAlign::Center);
    table.addColumn("NAME", StringAlign::Left);
    table.addColumn("WORTH", StringAlign::Right);
    table.addColumn("HARDNESS", StringAlign::Right);
    table.addColumn("LIGHTNESS", StringAlign::Right);
    for (auto iterator : Mud::instance().mudMaterials)
    {
        // Prepare the row.
        TableRow row;
        row.push_back(ToString(iterator.second->vnum));
        row.push_back(iterator.second->type.toString());
        row.push_back(iterator.second->name);
        row.push_back(ToString(iterator.second->worth));
        row.push_back(ToString(iterator.second->hardness));
        row.push_back(ToString(iterator.second->lightness));
        // Add the row to the table.
        table.addRow(row);
    }
    character->sendMsg(table.getTable());
    return true;
}

bool DoBuildingInfo(Character * character, ArgumentHandler & args)
{
    if (args.size() != 1)
    {
        character->sendMsg("You must provide a building vnum.\n");
        return false;
    }
    auto buildingVnum = ToNumber<int>(args[0].getContent());
    auto building = Mud::instance().findBuilding(buildingVnum);
    if (building == nullptr)
    {
        character->sendMsg("Can't find the desire building %s.\n",
                           buildingVnum);
        return false;
    }
    std::string msg;
    msg += "Vnum        : " + ToString(building->vnum) + "\n";
    msg += "Name        : " + building->name + "\n";
    msg += "Difficulty  : " + ToString(building->difficulty) + "\n";
    msg += "Time        : " + ToString(building->time) + "\n";
    msg += "Assisted    : " + ToString(building->assisted) + "\n";
    msg += "Tools       :\n";
    for (auto iterator : building->tools)
    {
        msg += "                  " + GetToolTypeName(iterator) + "\n";
    }
    msg += "Building    : " + building->buildingModel->name + "\n";
    msg += "Ingredients :\n";
    for (auto iterator : building->ingredients)
    {
        msg += "    " + iterator.first.toString();
        msg += "(" + ToString(iterator.second) + ")\n";
    }
    msg += "Unique      : " + ToString(building->unique) + "\n";
    character->sendMsg(msg);
    return true;
}

bool DoBuildingList(Character * character, ArgumentHandler & /*args*/)
{
    Table table;
    table.addColumn("VNUM", StringAlign::Center);
    table.addColumn("NAME", StringAlign::Center);
    table.addColumn("DIFFICULTY", StringAlign::Left);
    table.addColumn("TIME", StringAlign::Center);
    table.addColumn("UNIQUE", StringAlign::Center);
    for (auto iterator : Mud::instance().mudBuildings)
    {
        auto building = &(iterator.second);
        // Prepare the row.
        TableRow row;
        row.push_back(ToString(building->vnum));
        row.push_back(building->name);
        row.push_back(ToString(building->difficulty));
        row.push_back(ToString(building->time));
        row.push_back(ToString(building->unique));
        // Add the row to the table.
        table.addRow(row);
    }
    character->sendMsg(table.getTable());
    return true;
}

bool DoProfessionInfo(Character * character, ArgumentHandler & args)
{
    if (args.size() != 1)
    {
        character->sendMsg("You must provide a profession name.\n");
        return false;
    }
    auto profession = Mud::instance().findProfession(args[0].getContent());
    if (profession == nullptr)
    {
        character->sendMsg("Can't find the desire profession.\n");
        return false;
    }
    std::string msg;
    msg += "Name          : " + profession->name + "\n";
    msg += "Description   : " + profession->description + "\n";
    msg += "Command       : " + profession->command + "\n";
    msg += "Posture       : " + profession->posture.toString() + "\n";
    msg += "Action        : " + profession->action + "\n";
    msg += "    Start     : " + profession->startMessage + "\n";
    msg += "    Finish    : " + profession->finishMessage + "\n";
    msg += "    Success   : " + profession->successMessage + "\n";
    msg += "    Failure   : " + profession->failureMessage + "\n";
    msg += "    Interrupt : " + profession->interruptMessage + "\n";
    msg += "    Not Found : " + profession->notFoundMessage + "\n";
    character->sendMsg(msg);
    return true;
}

bool DoProfessionList(Character * character, ArgumentHandler & /*args*/)
{
    Table table;
    table.addColumn("NAME", StringAlign::Center);
    table.addColumn("COMMAND", StringAlign::Center);
    table.addColumn("POSTURE", StringAlign::Center);
    table.addColumn("ACTION", StringAlign::Center);
    for (auto iterator : Mud::instance().mudProfessions)
    {
        // Prepare the row.
        TableRow row;
        row.push_back(iterator.second->name);
        row.push_back(iterator.second->command);
        row.push_back(iterator.second->posture.toString());
        row.push_back(iterator.second->action);
        // Add the row to the table.
        table.addRow(row);
    }
    character->sendMsg(table.getTable());
    return true;
}

bool DoProductionInfo(Character * character, ArgumentHandler & args)
{
    if (args.size() != 1)
    {
        character->sendMsg("You must provide a production vnum.\n");
        return false;
    }
    auto productionVnum = ToNumber<int>(args[0].getContent());
    auto production = Mud::instance().findProduction(productionVnum);
    if (production == nullptr)
    {
        character->sendMsg("Can't find the desire production %s.\n",
                           productionVnum);
        return false;
    }
    std::string msg;
    msg += "Vnum        : " + ToString(production->vnum) + "\n";
    msg += "Name        : " + production->name + "\n";
    msg += "Profession  : " + production->profession->command + "\n";
    msg += "Difficulty  : " + ToString(production->difficulty) + "\n";
    msg += "Time        : " + ToString(production->time) + "\n";
    msg += "Assisted    : " + ToString(production->assisted) + "\n";
    msg += "Outcome     : " + production->outcome->name + "*";
    msg += ToString(production->quantity) + "\n";
    msg += "Tools       :\n";
    for (auto iterator : production->tools)
    {
        msg += "                  " + GetToolTypeName(iterator) + "\n";
    }
    msg += "Ingredients :\n";
    for (auto iterator : production->ingredients)
    {
        msg += "    " + iterator.first.toString();
        msg += "(" + ToString(iterator.second) + ")\n";
    }
    msg += "Workbench   :" + GetToolTypeName(production->workbench) + "\n";
    character->sendMsg(msg);
    return true;
}

bool DoProductionList(Character * character, ArgumentHandler & /*args*/)
{
    Table table;
    table.addColumn("VNUM", StringAlign::Center);
    table.addColumn("NAME", StringAlign::Center);
    table.addColumn("PROFESSION", StringAlign::Center);
    table.addColumn("DIFFICULTY", StringAlign::Left);
    for (auto iterator : Mud::instance().mudProductions)
    {
        // Prepare the row.
        TableRow row;
        row.push_back(ToString(iterator.second->vnum));
        row.push_back(iterator.second->name);
        row.push_back(iterator.second->profession->command);
        row.push_back(ToString(iterator.second->difficulty));
        // Add the row to the table.
        table.addRow(row);
    }
    character->sendMsg(table.getTable());
    return true;
}
