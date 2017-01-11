/// @file   commandGodMud.cpp
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

#include "commandGodMud.hpp"
#include "mapGenerator.hpp"
#include "mud.hpp"

bool DoShutdown(Character * character, ArgumentHandler &)
{
    // Send message to all the players.
    Mud::instance().broadcastMsg(0, character->getNameCapital() +
                                    " has shut down the game!");
    Mud::instance().shutDownSignal();
    return true;
}

bool DoMudSave(Character * character, ArgumentHandler &)
{
    if (!Mud::instance().saveMud())
    {
        character->sendMsg("Something gone wrong during the saving process.\n");
        return false;
    }
    // Send message to all the players.
    Mud::instance().broadcastMsg(0, character->getNameCapital() +
                                    " is writing the history...");
    character->sendMsg("Ok.\n");
    return true;
}

bool DoGoTo(Character * character, ArgumentHandler & args)
{
    if (args.size() != 1)
    {
        character->sendMsg("You have to provide a room vnum.");
        return false;
    }
    auto roomVnum = ToNumber<int>(args[0].getContent());
    auto destination = Mud::instance().findRoom(roomVnum);
    if (destination == nullptr)
    {
        character->sendMsg("The room %s doesn't exists.\n", roomVnum);
        return false;
    }
    // Stop any action the character is executing.
    StopAction(character);
    // Move player.
    character->moveTo(
        destination,
        character->getNameCapital() + " disappears in a puff of smoke!\n",
        character->getNameCapital() + " appears in a puff of smoke!\n",
        "You go to room " + destination->name + ".\n");
    return true;
}

bool DoFactionInfo(Character * character, ArgumentHandler & args)
{
    if (args.size() != 1)
    {
        character->sendMsg("You must insert a valide faction vnum.\n");
        return false;
    }
    auto factionVnum = ToNumber<int>(args[0].getContent());
    auto faction = Mud::instance().findFaction(factionVnum);
    if (faction == nullptr)
    {
        character->sendMsg("Faction not found.\n");
        return false;
    }
    // Create a table.
    Table sheet;
    // Get the sheet.
    faction->getSheet(sheet);
    // Show the seet to character.
    character->sendMsg(sheet.getTable());
    return true;
}

bool DoFactionList(Character * character, ArgumentHandler & /*args*/)
{
    Table table;
    table.addColumn("VNUM", StringAlign::Center);
    table.addColumn("NAME", StringAlign::Left);
    for (auto iterator : Mud::instance().mudFactions)
    {
        // Prepare the row.
        TableRow row;
        row.push_back(ToString(iterator.second->vnum));
        row.push_back(iterator.second->name);
        // Add the row to the table.
        table.addRow(row);
    }
    character->sendMsg(table.getTable());
    return true;
}

bool DoRaceList(Character * character, ArgumentHandler & /*args*/)
{
    Table table;
    table.addColumn("VNUM", StringAlign::Center);
    table.addColumn("NAME", StringAlign::Left);
    table.addColumn("ALLOWED", StringAlign::Left);
    table.addColumn("STRENGTH", StringAlign::Right);
    table.addColumn("AGILITY", StringAlign::Right);
    table.addColumn("PERCEPTION", StringAlign::Right);
    table.addColumn("CONSTITUTION", StringAlign::Right);
    table.addColumn("INTELLIGENCE", StringAlign::Right);
    for (auto iterator : Mud::instance().mudRaces)
    {
        Race * race = iterator.second;
        // Prepare the row.
        TableRow row;
        row.push_back(ToString(race->vnum));
        row.push_back(race->name);
        row.push_back(BoolToString(race->player_allow));
        row.push_back(ToString(race->getAbility(Ability::Strength)));
        row.push_back(ToString(race->getAbility(Ability::Agility)));
        row.push_back(ToString(race->getAbility(Ability::Perception)));
        row.push_back(ToString(race->getAbility(Ability::Constitution)));
        row.push_back(ToString(race->getAbility(Ability::Intelligence)));
        // Add the row to the table.
        table.addRow(row);
    }
    character->sendMsg(table.getTable());
    return true;
}

bool DoSkillList(Character * character, ArgumentHandler & /*args*/)
{
    Table table;
    table.addColumn("VNUM", StringAlign::Center);
    table.addColumn("NAME", StringAlign::Left);
    table.addColumn("ATTRIBUTE", StringAlign::Left);
    for (auto iterator : Mud::instance().mudSkills)
    {
        // Prepare the row.
        TableRow row;
        row.push_back(ToString(iterator->vnum));
        row.push_back(iterator->name);
        row.push_back(GetAttributeName(iterator->attribute, true));
        // Add the row to the table.
        table.addRow(row);
    }
    character->sendMsg(table.getTable());
    return true;
}

bool DoGenerateMap(Character * character, ArgumentHandler & args)
{
    MapGeneratorConfiguration configuration;
    HeightMapper heightMapper;
    if (args.size() >= 1)
    {
        if (IsNumber(args[0].getContent()))
        {
            configuration.width = ToNumber<int>(args[0].getContent());
        }
    }
    if (args.size() >= 2)
    {
        if (IsNumber(args[1].getContent()))
        {
            configuration.height = ToNumber<int>(args[1].getContent());
        }
    }
    if (args.size() >= 3)
    {
        if (IsNumber(args[2].getContent()))
        {
            configuration.numMountains = ToNumber<int>(args[2].getContent());
        }
    }
    if (args.size() >= 4)
    {
        if (IsNumber(args[3].getContent()))
        {
            configuration.minMountainRadius = ToNumber<int>(
                args[3].getContent());
        }
    }
    if (args.size() >= 5)
    {
        if (IsNumber(args[4].getContent()))
        {
            configuration.maxMountainRadius = ToNumber<int>(
                args[4].getContent());
        }
    }
    if (args.size() >= 6)
    {
        if (IsNumber(args[5].getContent()))
        {
            configuration.numRivers = ToNumber<int>(args[5].getContent());
        }
    }
    if (args.size() >= 7)
    {
        if (IsNumber(args[6].getContent()))
        {
            configuration.minRiverDistance = ToNumber<int>(
                args[6].getContent());
        }
    }
    // Instantiate the map generator.
    MapGenerator mapGenerator(configuration, heightMapper);
    // Generate the map.
    auto map = mapGenerator.generateMap();
    // Draw the map.
    std::string drawnMap;
    for (int y = 0; y < map.getHeight(); ++y)
    {
        for (int x = 0; x < map.getWidth(); ++x)
        {
            drawnMap += map.get(x, y).getTile();
        }
        drawnMap += "\n";
    }
    // First send the configuration.
    character->sendMsg(configuration.toString());
    character->sendMsg(drawnMap);
    character->sendMsg(Formatter::reset());
    return true;
}
