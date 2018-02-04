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
#include "characterUtilities.hpp"
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
    MoveCharacterTo(
        character,
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
    table.addColumn("VNUM", align::center);
    table.addColumn("NAME", align::left);
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
    table.addColumn("VNUM", align::center);
    table.addColumn("NAME", align::left);
    table.addColumn("ALLOWED", align::left);
    table.addColumn("STRENGTH", align::right);
    table.addColumn("AGILITY", align::right);
    table.addColumn("PERCEPTION", align::right);
    table.addColumn("CONSTITUTION", align::right);
    table.addColumn("INTELLIGENCE", align::right);
    for (auto iterator : Mud::instance().mudRaces)
    {
        Race * race = iterator.second;
        // Prepare the row.
        TableRow row;
        row.push_back(ToString(race->vnum));
        row.push_back(race->name);
        row.push_back(ToString(race->player_allow));
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
    table.addColumn("VNUM", align::center);
    table.addColumn("NAME", align::left);
    table.addColumn("ATTRIBUTE", align::left);
    for (auto skill : Mud::instance().mudSkills)
    {
        // Prepare the row.
        TableRow row;
        row.push_back(ToString(skill->vnum));
        row.push_back(skill->name);
        row.push_back(skill->ability.toString());
        // Add the row to the table.
        table.addRow(row);
    }
    character->sendMsg(table.getTable());
    return true;
}

bool DoGenerateMap(Character * character, ArgumentHandler & args)
{
    std::shared_ptr<HeightMap> heightMap = nullptr;
    if (args.size() == 1)
    {
        heightMap = Mud::instance().findHeightMap(
            ToNumber<unsigned int>(args[0].getContent()));
    }
    if (heightMap == nullptr)
    {
        character->sendMsg("You must provide a valid height map vnum:\n");
        for (auto it : Mud::instance().mudHeightMaps)
        {
            character->sendMsg("    [%s] %s\n", it.first, it.second->name);
        }
        return false;
    }
    MapGeneratorConfiguration configuration;
    // Instantiate the map generator.
    MapGenerator mapGenerator(configuration, heightMap);
    // Generate the map.
    auto map = std::make_shared<MapWrapper>();
    if (!mapGenerator.generateMap(map))
    {
        character->sendMsg("Error while generating the map.");
        return false;
    }
    // Draw the map.
    std::string drawnMap;
    for (int y = 0; y < map->getHeight(); ++y)
    {
        for (int x = 0; x < map->getWidth(); ++x)
        {
            drawnMap += map->getCell(x, y)->getTile();
        }
        drawnMap += "\n";
    }
    // First send the configuration.
    character->sendMsg(configuration.toString());
    character->sendMsg(drawnMap);
    character->sendMsg(Formatter::reset());
    // Add the map to the list of generated maps.
    Mud::instance().addGeneratedMap(map);
    return true;
}

bool DoShowGenerateMap(Character * character, ArgumentHandler & args)
{
    if (args.empty())
    {
        character->sendMsg("List of generated maps:\n");
        for (auto generatedMap : Mud::instance().mudGeneratedMaps)
        {
            character->sendMsg("    %s\n", generatedMap.second->vnum);
        }
        return true;
    }
    if (args.size() != 1)
    {
        character->sendMsg("You must provide the vnum of a generated map.");
        return false;
    }
    auto vnum = ToNumber<unsigned int>(args[0].getContent());
    auto generatedMap = Mud::instance().mudGeneratedMaps.find(vnum);
    if (generatedMap == Mud::instance().mudGeneratedMaps.end())
    {
        character->sendMsg("Can't find the generated map '%s'.", vnum);
        return false;
    }
    auto map = generatedMap->second;
    // Draw the map.
    std::string drawnMap;
    for (int y = 0; y < map->getHeight(); ++y)
    {
        for (int x = 0; x < map->getWidth(); ++x)
        {
            drawnMap += map->getCell(x, y)->getTile();
        }
        drawnMap += "\n";
    }
    // First send the configuration.
    character->sendMsg(drawnMap);
    character->sendMsg(Formatter::reset());
    return true;
}

bool DoDeleteGenerateMap(Character * character, ArgumentHandler & args)
{
    if (args.size() != 1)
    {
        character->sendMsg("You must provide the vnum of a generated map.");
        return false;
    }
    auto vnum = ToNumber<unsigned int>(args[0].getContent());
    auto generatedMap = Mud::instance().mudGeneratedMaps.find(vnum);
    if (generatedMap == Mud::instance().mudGeneratedMaps.end())
    {
        character->sendMsg("Can't find the generated map '%s'.", vnum);
        return false;
    }
    auto map = generatedMap->second;
    Mud::instance().mudGeneratedMaps.erase(generatedMap);
    map->destroy();
    return true;
}

bool DoBuildGenerateMap(Character * character, ArgumentHandler & args)
{
    if (args.size() != 2)
    {
        character->sendMsg("You must provide the vnum of a generated map and "
                               "the name of the new map.");
        return false;
    }
    auto vnum = ToNumber<unsigned int>(args[0].getContent());
    auto mapName = args[1].getContent();
    auto generatedMap = Mud::instance().mudGeneratedMaps.find(vnum);
    if (generatedMap == Mud::instance().mudGeneratedMaps.end())
    {
        character->sendMsg("Can't find the generated map '%s'.", vnum);
        return false;
    }
    if (!generatedMap->second->buildMap(mapName, character->getNameCapital()))
    {
        character->sendMsg("Can't build the map '%s'.", vnum);
        return false;
    }
    return true;
}
