/// @file   mapWrapper.cpp
/// @author Enrico Fraccaroli
/// @date   feb 05 2017
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

#include "mapWrapper.hpp"
#include "sqliteWriteFunctions.hpp"
#include "roomFactory.hpp"
#include "mud.hpp"

MapWrapper::MapWrapper() :
    vnum(),
    width(),
    height(),
    elevation(),
    map()
{
    // Nothing to do.
}

MapWrapper::~MapWrapper()
{
    Logger::log(LogLevel::Trace, "Deleting map wrapper:%s", vnum);
}

void MapWrapper::initialize(const int & _width,
                            const int & _height,
                            const int & _elevation)
{
    width = _width;
    height = _height;
    elevation = _elevation;
    // Set the coordinates.
    for (int x = 0; x < width; ++x)
    {
        for (int y = 0; y < height; ++y)
        {
            for (int z = 0; z < elevation; ++z)
            {
                auto cell = getCell(x, y, z);
                // Set the coordinates.
                cell->coordinates = Coordinates(x, y, z);
                // Set neighbours.
                auto n_west = getCell(x - 1, y, z);
                if (n_west != nullptr)
                {
                    cell->addNeighbour(Direction::West, n_west);
                }
                auto n_east = getCell(x + 1, y, z);
                if (n_east != nullptr)
                {
                    cell->addNeighbour(Direction::East, n_east);
                }
                auto n_south = getCell(x, y - 1, z);
                if (n_south != nullptr)
                {
                    cell->addNeighbour(Direction::South, n_south);
                }
                auto n_north = getCell(x, y + 1, z);
                if (n_north != nullptr)
                {
                    cell->addNeighbour(Direction::North, n_north);
                }
#if 0
                auto n_up = getCell(x, y, z + 1);
                if (n_up != nullptr)
                {
                    cell->addNeighbour(Direction::Up, n_up);
                }
                auto n_down = getCell(x, y, z - 1);
                if (n_down != nullptr)
                {
                    cell->addNeighbour(Direction::Down, n_down);
                }
#endif
            }
        }
    }
}

void MapWrapper::destroy()
{
    for (int x = 0; x < width; ++x)
    {
        for (int y = 0; y < height; ++y)
        {
            for (int z = 0; z < elevation; ++z)
            {
                auto cell = this->getCell(x, y, z);
                cell->coordinates = Coordinates(0, 0, 0);
                cell->terrain = nullptr;
                cell->neighbours.clear();
            }
        }
    }
}

bool MapWrapper::buildMap(const std::string & /*mapName*/,
                          const std::string & /*builder*/)
{
#if 0
    // -------------------------------------------------------------------------
    // First create a new area.
    auto area = new Area();
    area->vnum = Mud::instance().getUniqueAreaVnum();
    area->name = mapName;
    area->builder = builder;
    area->width = width;
    area->height = height;
    area->elevation = elevation;
    area->tileSet = 1;
    area->type = AreaType::Normal;
    area->status = AreaStatus::Normal;
    if (!Mud::instance().addArea(area))
    {
        Logger::log(LogLevel::Error, "While adding the area to the MUD.\n");
        return false;
    }
    if (!SaveArea(area))
    {
        Logger::log(LogLevel::Error, "While saving the area on the DB.\n");
        return false;
    }
    // Generate the normal rooms.
    for (int x = 0; x < width; ++x)
    {
        for (int y = 0; y < height; ++y)
        {
            auto cell = this->getCell(x, y);
            // Create the room.
            cell->room = new Room();
            cell->room->vnum = Mud::instance().getMaxVnumRoom() + 1;
            cell->room->area = area;
            cell->room->coord = cell->coordinates;
            cell->room->terrain = cell->terrain;
            cell->room->name = cell->terrain->name;
            cell->room->liquidContent = cell->liquidContent;
            cell->room->description = "";
            cell->room->flags = cell->flags;
            // Add the created room to the room_map.
            if (!Mud::instance().addRoom(cell->room))
            {
                Logger::log(LogLevel::Error,
                            "Cannot add the room to the mud.\n");
                return false;
            }
            if (!cell->room->area->addRoom(cell->room))
            {
                Logger::log(LogLevel::Error,
                            "Cannot add the room to the area.\n");
                return false;
            }
            if (!SaveRoom(cell->room))
            {
                Logger::log(LogLevel::Error,
                            "While saving the room on DB.\n");
                return false;
            }
            if (!SaveAreaList(area, cell->room))
            {
                Logger::log(LogLevel::Error,
                            "While saving the Area List.\n");
                return false;
            }
            Logger::log(LogLevel::Debug, "Created room at %s",
                        cell->room->coord.toString());
        }
    }
    // Generate the exits.
    for (int x = 0; x < width; ++x)
    {
        for (int y = 0; y < height; ++y)
        {
            auto cell = this->getCell(x, y);
            if (cell->room == nullptr)
            {
                Logger::log(LogLevel::Error,
                            "A cell has a nullptr room at %s.\n",
                            cell->coordinates.toString());
                return false;
            }
            for (auto neighbour : cell->neighbours)
            {
                // Get the room of the neighbour.
                if (neighbour.second->room == nullptr)
                {
                    Logger::log(LogLevel::Error,
                                "A neighbour has a nullptr room %s->%s.\n",
                                cell->coordinates.toString(),
                                neighbour.second->coordinates.toString());
                    return false;
                }
                // Create the two exits.
                auto forward = std::make_shared<Exit>(
                    cell->room,
                    neighbour.second->room,
                    neighbour.first,
                    0);
                auto backward = std::make_shared<Exit>(
                    neighbour.second->room,
                    cell->room,
                    neighbour.first.getOpposite(),
                    0);
                // Insert in both the rooms exits the connection.
                if (cell->room->addExit(forward))
                {
                    if (!SaveRoomExit(forward))
                    {
                        Logger::log(LogLevel::Error,
                                    "While saving the exit on DB.\n");
                        return false;
                    }
                }
                if (neighbour.second->room->addExit(backward))
                {
                    if (!SaveRoomExit(backward))
                    {
                        Logger::log(LogLevel::Error,
                                    "While saving the exit on DB.\n");
                        return false;
                    }
                }
            }
        }
    }
    // Generate the air
    for (int x = 0; x < width; ++x)
    {
        for (int y = 0; y < height; ++y)
        {
            auto cell = this->getCell(x, y);
            auto maxHeight = (cell->findHighestNearbyCell()->coordinates.z + 1);
            for (int z = (cell->coordinates.z + 1);
                 (z < maxHeight) && (z < area->elevation); ++z)
            {
                // Create the room.
                cell->room = RoomFactory::createRoomAir();
                cell->room->vnum = Mud::instance().getMaxVnumRoom() + 1;
                cell->room->area = area;
                cell->room->coord = cell->coordinates;
                cell->room->coord.z = z;
                // Add the created room to the room_map.
                if (!Mud::instance().addRoom(cell->room))
                {
                    Logger::log(LogLevel::Error,
                                "Cannot add the air room to the mud.\n");
                    return false;
                }
                if (!cell->room->area->addRoom(cell->room))
                {
                    Logger::log(LogLevel::Error,
                                "Cannot add the air room to the area.\n");
                    return false;
                }
                if (!SaveRoom(cell->room))
                {
                    Logger::log(LogLevel::Error,
                                "While saving the air room on DB.\n");
                    return false;
                }
                if (!SaveAreaList(area, cell->room))
                {
                    Logger::log(LogLevel::Error,
                                "While saving the air room in Area List.\n");
                    return false;
                }
                Logger::log(LogLevel::Debug, "Created air room at %s",
                            cell->room->coord.toString());
            }
        }
    }
#endif
    return true;
}