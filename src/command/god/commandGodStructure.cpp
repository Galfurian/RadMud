/// @file   commandGodStructure.cpp
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

#include "commandGodStructure.hpp"
#include "aStar.hpp"
#include "mud.hpp"

bool DoFindPath(Character * character, ArgumentHandler & args)
{
    if (args.size() != 1)
    {
        character->sendMsg("You have to provide a room vnum.\n\n");
        return false;
    }
    auto roomVnum = ToNumber<int>(args[0].getContent());
    auto room = character->room->area->getRoom(roomVnum);
    if (room == nullptr)
    {
        character->sendMsg("The room %s doesn't exists.\n", roomVnum);
        return false;
    }
    AStar<Room *> aStar;
    std::vector<Room *> path;
    auto checkFunction = [&character](Room * from, Room * to)
    {
        // Get the direction.
        auto direction = Area::getDirection(from->coord, to->coord);
        // Get the exit;
        auto destExit = from->findExit(direction);
        // If the direction is upstairs, check if there is a stair.
        if (direction == Direction::Up)
        {
            if (!HasFlag(destExit->flags, ExitFlag::Stairs)) return false;
        }
        // Check if the destination is correct.
        if (destExit->destination == nullptr) return false;
        // Check if the destination is bocked by a door.
        auto door = destExit->destination->findDoor();
        if (door != nullptr)
        {
            if (HasFlag(door->flags, ItemFlag::Closed)) return false;
        }
        // Check if the destination has a floor.
        auto destDown = destExit->destination->findExit(Direction::Down);
        if (destDown != nullptr)
        {
            if (!HasFlag(destDown->flags, ExitFlag::Stairs)) return false;
        }
        // Check if the destination is forbidden for mobiles.
        return !(character->isMobile() &&
                 HasFlag(destExit->flags, ExitFlag::NoMob));
    };
    if (!aStar.findPath(character->room, room, path, checkFunction))
    {
        character->sendMsg("There is no path to that room.\n\n");
        return false;
    }
    character->sendMsg("You have to go:\n");
    Coordinates previous = character->room->coord;
    for (auto node : path)
    {
        auto direction = Area::getDirection(previous, node->coord);
        previous = node->coord;
        character->sendMsg("    %s\n", direction.toString());
    }
    character->sendMsg("\n");
    return true;
}

bool DoRoomCreate(Character * character, ArgumentHandler & args)
{
    // Stop any action the character is executing.
    StopAction(character);

    // Check the number of arguments.
    if (args.size() != 1)
    {
        character->sendMsg("Usage: [direction]\n");
        return false;
    }
    auto currentRoom = character->room;
    auto currentArea = currentRoom->area;
    if (currentArea == nullptr)
    {
        character->sendMsg("Your room's area has not been defined!\n");
        return false;
    }
    // Check if it's a direction.
    auto direction = Direction(args[0].getContent(), true);
    if (direction == Direction::None)
    {
        character->sendMsg("You must insert a valid direction!\n");
        return false;
    }
    // Get the coordinate modifier.
    auto targetCoord = currentRoom->coord + direction.getCoordinates();
    if (!currentArea->inBoundaries(targetCoord))
    {
        character->sendMsg("Its outside the boundaries.\n");
        return false;
    }
    // Find the room.
    auto targetRoom = currentArea->getRoom(targetCoord);
    if (targetRoom)
    {
        character->sendMsg("There is already a room.\n");
        return false;
    }
    if (!CreateRoom(targetCoord, currentRoom))
    {
        character->sendMsg("Sorry but you couldn't create the room.\n");
        return false;
    }
    character->sendMsg("Room create (%s)\n", targetCoord.toString());
    return true;
}

bool DoRoomDelete(Character * character, ArgumentHandler & args)
{
    // Stop any action the character is executing.
    StopAction(character);
    // Check the number of arguments.
    if (args.size() != 1)
    {
        character->sendMsg("Usage: [direction]\n");
        return false;
    }
    auto currentArea = character->room->area;
    if (currentArea == nullptr)
    {
        character->sendMsg("Your room's area has not been defined!\n");
        return false;
    }
    // Check if it's a direction.
    auto direction = Direction(args[0].getContent(), true);
    if (direction == Direction::None)
    {
        character->sendMsg("You must insert a valid direction!\n");
        return false;
    }
    // Get the coordinate modifier.
    auto coord = character->room->coord + direction.getCoordinates();
    if (!currentArea->inBoundaries(coord))
    {
        character->sendMsg("Outside the boundaries.\n");
        return false;
    }
    // Find the room.
    auto targetRoom = currentArea->getRoom(coord);
    if (targetRoom == nullptr)
    {
        character->sendMsg("Sorry but in that direction there is no room.\n");
        return false;
    }
    // Remove the room from the Database.
    if (!targetRoom->removeOnDB())
    {
        character->sendMsg("You couldn't delete the selected room.\n");
        character->sendMsg("There are items or characters in that room.\n");
        return false;
    }
    // Remove the room from the list of rooms.
    if (!Mud::instance().remRoom(targetRoom))
    {
        character->sendMsg("You cannot remove the room.\n");
        return false;
    }
    // Delete completely the room.
    delete (targetRoom);
    character->sendMsg("You have destroyed the room at coordinates :\n");
    character->sendMsg("%s\n", coord.toString());
    return true;
}

bool DoRoomEdit(Character * character, ArgumentHandler & args)
{
    if (args.empty())
    {
        character->sendMsg("What do you want to edit?\n");
        return false;
    }
    if (args[0].getContent() == "des")
    {
        auto input = args.substr(1);
        if (input.empty())
        {
            character->sendMsg("You can't set an empty description.\n");
            return false;
        }
        QueryList value = {std::make_pair("description", input)};
        QueryList where = {
            std::make_pair("vnum", ToString(character->room->vnum))};
        if (!SQLiteDbms::instance().updateInto("Room", value, where))
        {
            character->sendMsg("Command gone wrong.\n");
            return false;
        }
        character->room->description = input;
        character->sendMsg("Room description modified.\n");
        return true;
    }
    else if (args[0].getContent() == "nam")
    {
        auto input = args.substr(1);
        if (input.empty())
        {
            character->sendMsg("You can't set an empty name.\n");
            return false;
        }
        QueryList value = {std::make_pair("name", input)};
        QueryList where = {
            std::make_pair("vnum", ToString(character->room->vnum))};
        if (!SQLiteDbms::instance().updateInto("Room", value, where))
        {
            character->sendMsg("Command gone wrong.\n");
            return false;
        }
        character->room->name = input;
        character->sendMsg("Room name modified.\n");
        return true;
    }
    else if (args[0].getContent() == "sflag")
    {
        auto input = args.substr(1);
        if (input.empty())
        {
            character->sendMsg("Wrong flag.\n");
        }
        else
        {
            if (input == "R")
            {
                SetFlag(&character->room->flags, RoomFlag::Rent);
                return true;
            }
            else if (input == "P")
            {
                SetFlag(&character->room->flags, RoomFlag::Peaceful);
                return true;
            }
            character->sendMsg("Not a valid flag.\n");
        }
    }
    else if (args[0].getContent() == "cflag")
    {
        auto input = args.substr(1);
        if (input.empty())
        {
            character->sendMsg("Wrong flag.\n");
        }
        else
        {
            if (input == "R")
            {
                ClearFlag(&character->room->flags, RoomFlag::Rent);
                return true;
            }
            else if (input == "P")
            {
                ClearFlag(&character->room->flags, RoomFlag::Peaceful);
                return true;
            }
            character->sendMsg("Not a valid flag.\n");
        }
    }
    else
    {
        std::string msg;
        msg += "Usage:                           \n";
        msg += "|  Argument  |  Text            |\n";
        msg += "| ---------- | ---------------- |\n";
        msg += "|  nam       |  [name]          |\n";
        msg += "|  ter       |  [terrain]       |\n";
        msg += "|  des       |  [description]   |\n";
        msg += "|  sflag     |  [Set Flag]      |\n";
        msg += "|            |   R -> kRent     |\n";
        msg += "|            |   P -> kPeaceful |\n";
        msg += "|  cflag     |  [Clear Flag]    |\n";
        msg += "|            |   R -> kRent     |\n";
        msg += "|            |   P -> kPeaceful |\n";
        msg += "| ---------- | ---------------- |\n";
        character->sendMsg(msg);
    }
    return false;
}

bool DoRoomInfo(Character * character, ArgumentHandler & args)
{
    Room * room;
    if (args.empty())
    {
        room = character->room;
    }
    else if (args.size() == 1)
    {
        room = Mud::instance().findRoom(ToNumber<int>(args[0].getContent()));
        if (room == nullptr)
        {
            character->sendMsg("Can't find the desired room.\n");
            return false;
        }
    }
    else
    {
        character->sendMsg("CanYu can only provide nothing or a room vnum.\n");
        return false;
    }
    std::string msg;
    msg += "Room Informations:\n";
    msg += " Vnum        :" + ToString(room->vnum) + "\n";
    msg += " Area        :";
    msg += ((room->area) ? ToString(room->area->vnum) : "NULL") + "\n";
    msg += " X           :" + ToString(room->coord.x) + "\n";
    msg += " Y           :" + ToString(room->coord.y) + "\n";
    msg += " Z           :" + ToString(room->coord.z) + "\n";
    msg += " Name        :" + room->name + "\n";
    msg += " Description :" + room->description + "\n";
    msg += " Terrain     :" + room->terrain->name + "\n";
    msg += " Flags       :" + GetRoomFlagString(room->flags) + "\n";
    character->sendMsg(msg);
    return true;
}

bool DoRoomList(Character * character, ArgumentHandler & /*args*/)
{
    Table table;
    table.addColumn("VNUM", StringAlign::Center);
    table.addColumn("AREA", StringAlign::Left);
    table.addColumn("COORD", StringAlign::Center);
    table.addColumn("TERRAIN", StringAlign::Center);
    table.addColumn("NAME", StringAlign::Left);
    for (auto iterator : Mud::instance().mudRooms)
    {
        Room * room = iterator.second;
        // Prepare the row.
        TableRow row;
        row.push_back(ToString(room->vnum));
        if (room->area != nullptr)
        {
            row.push_back(room->area->name);
        }
        else
        {
            row.push_back("None");
        }
        row.push_back(
            ToString(room->coord.x) + ' ' + ToString(room->coord.y) + ' '
            + ToString(room->coord.z));
        row.push_back(room->terrain->name);
        row.push_back(room->name);
        // Add the row to the table.
        table.addRow(row);
    }
    character->sendMsg(table.getTable());
    return true;
}

bool DoAreaInfo(Character * character, ArgumentHandler & args)
{
    if (args.size() != 1)
    {
        character->sendMsg("You must instert an area vnum.\n");
        return false;
    }
    auto area = Mud::instance().findArea(ToNumber<int>(args[0].getContent()));
    if (area == nullptr)
    {
        character->sendMsg("The selected area does not exist.");
        return false;
    }
    std::string msg;
    msg += "Area Informations:\n";
    msg += " Vnum      :" + ToString(area->vnum) + "\n";
    msg += " Name      :" + area->name + "\n";
    msg += " Builder   :" + area->builder + "\n";
    msg += " Width     :" + ToString(area->width) + "\n";
    msg += " Height    :" + ToString(area->height) + "\n";
    msg += " Elevation :" + ToString(area->elevation) + "\n";
    msg += " N. Rooms  :" + ToString(area->map.size()) + "\n";
    character->sendMsg(msg);
    return true;
}

bool DoAreaList(Character * character, ArgumentHandler & /*args*/)
{
    Table table;
    table.addColumn("VNUM", StringAlign::Center);
    table.addColumn("NAME", StringAlign::Left);
    table.addColumn("BUILDER", StringAlign::Left);
    table.addColumn("ROOMS", StringAlign::Center);
    for (auto iterator : Mud::instance().mudAreas)
    {
        Area * area = iterator.second;
        // Prepare the row.
        TableRow row;
        row.push_back(ToString(area->vnum));
        row.push_back(area->name);
        row.push_back(area->builder);
        row.push_back(ToString(area->map.size()));
        // Add the row to the table.
        table.addRow(row);
    }
    character->sendMsg(table.getTable());
    return true;
}
