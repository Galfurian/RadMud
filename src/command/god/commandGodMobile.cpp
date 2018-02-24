/// @file   commandGodMobile.cpp
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

#include "commandGodMobile.hpp"
#include "mud.hpp"

bool DoMobileKill(Character * character, ArgumentHandler & args)
{
    if (args.size() != 1)
    {
        character->sendMsg("You must provide a target mobile.\n");
        return false;
    }
    auto mobile = character->room->findMobile(args[0].getContent(),
                                              args[0].getIndex());
    if (mobile == nullptr)
    {
        character->sendMsg("Mobile not found.\n");
        return false;
    }
    // Kill the mob.
    mobile->kill();
    // Notify the death.
    character->sendMsg("You snap your fingers.\n");
    character->room->sendToAll("%s fall to the ground dead.",
                               {}, mobile->getNameCapital());
    return true;
}

bool DoMobileList(Character * character, ArgumentHandler & /*args*/)
{
    Table table;
    table.addColumn("ALIVE", align::center);
    table.addColumn("VNUM", align::left);
    table.addColumn("NAME", align::left);
    table.addColumn("LOCATION", align::right);
    for (auto mobile : Mud::instance().mudMobiles)
    {
        // Prepare the row.
        TableRow row;
        row.emplace_back((mobile->isAlive()) ? "Yes" : "No");
        row.emplace_back(ToString(mobile->vnum));
        row.emplace_back(mobile->getName());
        if (mobile->room != nullptr)
        {
            row.emplace_back("Room : " + ToString(mobile->room->vnum));
        }
        else
        {
            row.emplace_back("Is nowhere.");
        }
        // Add the row to the table.
        table.addRow(row);
    }
    character->sendMsg(table.getTable());
    return true;
}

bool DoMobileReload(Character * character, ArgumentHandler & args)
{
    if (args.size() != 1)
    {
        character->sendMsg("You must provide a target mobile.\n");
        return false;
    }
    auto mobile = character->room->findMobile(args[0].getContent(),
                                              args[0].getIndex());
    if (mobile == nullptr)
    {
        character->sendMsg("Mobile not found.\n");
        return false;
    }
    // Reload the mob.
    mobile->reloadLua();
    // Notify.
    character->sendMsg("Target(%s) Script(%s)\n",
                       mobile->getName(), mobile->lua_script);
    return true;
}

bool DoMobileTrigger(Character * character, ArgumentHandler & args)
{
    if (args.size() != 1)
    {
        character->sendMsg("You must provide a target mobile.\n");
    }
    else
    {
        auto mobile = character->room->findMobile(args[0].getContent(),
                                                  args[0].getIndex());
        if (mobile == nullptr)
        {
            character->sendMsg("Mobile not found.\n");
        }
        else
        {
            mobile->triggerEventMain();
            character->sendMsg("%s::triggerEventMain()\n",
                               mobile->getNameCapital());
            return true;
        }
    }
    return false;
}

bool DoMobileLog(Character * character, ArgumentHandler & args)
{
    if (args.size() != 1)
    {
        character->sendMsg("You must provide a mobile id.\n");
        return false;
    }
    auto mobile = Mud::instance().findMobile(
        ToNumber<unsigned int>(args[0].getContent()));
    if (mobile == nullptr)
    {
        character->sendMsg("Mobile not found.\n");
        return false;
    }
    // Notify.
    character->sendMsg("Target(%s)\n", mobile->getName());
    character->sendMsg("Log:\n%s\n", mobile->outbuffer);
    return true;
}
