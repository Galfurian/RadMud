/// @file   commandGodLiquid.cpp
/// @author Enrico Fraccaroli
/// @date   Jan 03 2017
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

#include "commandGodLiquid.hpp"
#include "liquidContainerItem.hpp"
#include "mud.hpp"

bool DoLiquidInfo(Character * character, ArgumentHandler & args)
{
    if (args.size() != 1)
    {
        character->sendMsg("You must provide a liquid vnum.\n");
        return false;
    }
    auto liquidVnum = ToNumber<unsigned int>(args[0].getContent());
    auto liquid = Mud::instance().findLiquid(liquidVnum);
    if (liquid == nullptr)
    {
        character->sendMsg("Can't find the desire liquid '%s'.\n", liquidVnum);
        return false;
    }
    std::string msg;
    msg += "Vnum  : " + ToString(liquid->vnum) + "\n";
    msg += "Type  : " + liquid->type.toString() + "\n";
    msg += "Name  : " + liquid->getNameCapital() + "\n";
    msg += "Descr : " + liquid->description + "\n";
    msg += "Worth : " + ToString(liquid->worth) + "\n";
    character->sendMsg(msg);
    return true;
}

bool DoLiquidList(Character * character, ArgumentHandler & /*args*/)
{
    Table table;
    table.addColumn("VNUM", align::center);
    table.addColumn("NAME", align::right);
    table.addColumn("WORTH", align::right);
    for (auto iterator : Mud::instance().mudLiquids)
    {
        // Prepare the row.
        TableRow row;
        row.emplace_back(ToString(iterator.second->vnum));
        row.emplace_back(iterator.second->getNameCapital());
        row.emplace_back(ToString(iterator.second->worth));
        // Add the row to the table.
        table.addRow(row);
    }
    character->sendMsg(table.getTable());
    return true;
}

bool DoLiquidCreate(Character * character, ArgumentHandler & args)
{
    if (args.empty())
    {
        character->sendMsg("Usage: [Container] [Liquid Vnum] [Quantity].\n");
        return false;
    }
    // Get the number of arguments.
    auto argv(args.size());
    // Find the container.
    auto item = character->findNearbyItem(args[0].getContent(),
                                          args[0].getIndex());
    if (item == nullptr)
    {
        character->sendMsg("Can't find the desire container '%s'.\n",
                           args[0].getContent());
        return false;
    }
    if (item->model->getType() != ModelType::LiquidContainer)
    {
        character->sendMsg("The item is not a container of liquids.\n");
        return false;
    }
    if (argv == 1)
    {
        character->sendMsg("You can fill %s with:\n", item->getName());
        for (auto const & it : Mud::instance().mudLiquids)
        {
            character->sendMsg("    [%s] %s\n",
                Align(it.first, align::right, 4),
                it.second->name);
        }
        return false;
    }
    // Find the liquid.
    auto liquidVnum = ToNumber<unsigned int>(args[1].getContent());
    auto liquid = Mud::instance().findLiquid(liquidVnum);
    if (liquid == nullptr)
    {
        character->sendMsg("Can't find the desire liquid '%s'.\n", liquidVnum);
        return false;
    }
    if (argv == 2)
    {
        character->sendMsg("You should provide the amount:\n");
        character->sendMsg("    [ 0 ] Fill completely.\n");
        character->sendMsg("    [Any] Fill the amount specified.\n");
        return false;
    }
    // Get the quantity.
    auto quantity = ToNumber<double>(args[2].getContent());
    if ((quantity < 0) || (quantity >= 100))
    {
        character->sendMsg("Accepted quantity of liquids (from 0 to 99).\n");
        return false;
    }
    // Cast the item to liquid container.
    auto liquidContainer = dynamic_cast<LiquidContainerItem *>(item);
    if (static_cast<long>(quantity) == 0)
        quantity = liquidContainer->getFreeSpace();
    if (!liquidContainer->pourIn(liquid, quantity))
    {
        character->sendMsg("Item can't contain that quantity of liquid.\n");
        return false;
    }
    character->sendMsg("You materialise %s units of %s inside %s.\n",
                       ToString(quantity),
                       liquid->getName(),
                       item->getName(true));
    return true;
}
