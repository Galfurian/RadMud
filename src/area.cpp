/// @file   area.cpp
/// @brief  This file implements all the methods needed to interact with an area.
/// @author Enrico Fraccaroli
/// @date   Mar 10 2015
/// @copyright
/// Copyright (c) 2015, 2016 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
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
#include "area.hpp"

#include <algorithm>
#include <cmath>
#include <iosfwd>
#include <ostream>

// Other Include.
#include "character.hpp"
#include "constants.hpp"
#include "item.hpp"
#include "luabridge/LuaBridge.h"
#include "logger.hpp"
#include "room.hpp"

using namespace std;

Area::Area() :
    vnum(),
    name(),
    builder(),
    continent(),
    areaMap(),
    width(),
    height(),
    elevation(),
    tileSet(),
    type(),
    status()
{
}

Area::~Area()
{
    Logger::log(LogLevel::Debug, "Deleted: Area.");
}

bool Area::check()
{
    assert(vnum > 0);
    assert(!name.empty());
    assert(!builder.empty());
    assert(!continent.empty());
    assert(width > 0);
    assert(height > 0);
    assert(elevation > 0);
    return true;
}

bool Area::inBoundaries(const int & x, const int & y, const int & z)
{
    bool result = true;
    result &= (x >= 0) && (x <= width);
    result &= (y >= 0) && (y <= height);
    result &= (z >= 0) && (z <= elevation);
    return result;
}

bool Area::inBoundaries(const Coordinates<int> & coord)
{
    return this->inBoundaries(coord.x, coord.y, coord.z);
}

bool Area::addRoom(Room * room)
{
    if (this->inBoundaries(room->coord))
    {
        if (areaMap.set(room->coord.x, room->coord.y, room->coord.z, room))
        {
            // Set the room area to be this one.
            room->area = this;
            return true;
        }
        else
        {
            Logger::log(LogLevel::Error, "Room's insertion could not be completed %s.", room->coord.toString());
        }
    }
    else
    {
        Logger::log(LogLevel::Error, "Room's coordiantes are not inside the boundaries %s.", room->coord.toString());
    }
    return false;
}

bool Area::remRoom(Room * room)
{
    return (areaMap.erase(room->coord.x, room->coord.y, room->coord.z) != areaMap.end());
}

Room * Area::getRoom(int room_vnum)
{
    for (Map3D<Room *>::iterator it = areaMap.begin(); it != areaMap.end(); ++it)
    {
        if (it->second->vnum == room_vnum)
        {
            return it->second;
        }
    }
    return nullptr;
}

Room * Area::getRoom(int x, int y, int z)
{
    return areaMap.get(x, y, z);
}

Room * Area::getRoom(Coordinates<int> coord)
{
    if (this->inBoundaries(coord))
    {
        return this->getRoom(coord.x, coord.y, coord.z);
    }
    return nullptr;
}

std::vector<std::string> Area::drawFov(Room * centerRoom, int radius)
{
    std::vector<std::string> layers(3);
    if (!this->inBoundaries(centerRoom->coord))
    {
        return layers;
    }
    // Retrieve the coordinates of the room.
    int origin_x = centerRoom->coord.x;
    int origin_y = centerRoom->coord.y;
    int origin_z = centerRoom->coord.z;
    // Evaluate the minimum and maximum value for x and y.
    int min_x = (origin_x < radius) ? 0 : (origin_x - radius);
    int max_x = ((origin_x + radius) > this->width) ? this->width : (origin_x + radius);
    int min_y = (origin_y < radius) ? 0 : (origin_y - radius);
    int max_y = ((origin_y + radius - 1) > this->height) ? this->height : (origin_y + radius - 1);
    // Create a 2D map of chararacters.
    Map2D<MapTile> map(radius * 2, radius * 2);
    // Evaluate the field of view.
    this->fov(map, origin_x, origin_y, origin_z, radius);
    // Prepare Enviroment layer.
    for (int y = max_y; y >= min_y; --y)
    {
        for (int x = min_x; x < max_x; ++x)
        {
            std::string tileCode = " : ";
            if (map.get(x, y) == MapTile::Walkable)
            {
                Room * room = this->getRoom(x, y, origin_z);
                Exit * up = room->findExit(Direction::Up);
                Exit * down = room->findExit(Direction::Down);

                // By default set it to walkable tile.
                tileCode = ToString(15) + ":" + ToString(this->tileSet + 0);

                if ((up != nullptr) && (down != nullptr))
                {
                    if (HasFlag(up->flags, ExitFlag::Stairs) && HasFlag(down->flags, ExitFlag::Stairs))
                    {
                        tileCode = ToString(18) + ":" + ToString(this->tileSet + 1);
                    }
                }
                else if (up != nullptr)
                {
                    if (HasFlag(up->flags, ExitFlag::Stairs))
                    {
                        tileCode = ToString(18) + ":" + ToString(this->tileSet + 1);
                    }
                }
                else if (down != nullptr)
                {
                    if (HasFlag(down->flags, ExitFlag::Stairs))
                    {
                        tileCode = ToString(18) + ":" + ToString(this->tileSet + 0);
                    }
                    else
                    {
                        tileCode = ToString(18) + ":" + ToString(this->tileSet + 4);
                    }
                }
            }
            layers[0] += tileCode;
            if (x != (origin_x + radius - 1))
            {
                layers[0] += ",";
            }
        }
        layers[0] += ";";
    }

    // Prepare Objects layer.
    for (int y = max_y; y > min_y; --y)
    {
        for (int x = min_x; x < max_x; ++x)
        {
            std::string tileCode = " : ";
            Room * room = this->getRoom(x, y, origin_z);
            if ((map.get(x, y) != MapTile::Void) && (room != nullptr))
            {
                Item * door = room->findDoor();
                if (!room->items.empty())
                {
                    tileCode = room->items.back()->model->getTile();
                }
                else if (door != nullptr)
                {
                    if (HasFlag(door->flags, ItemFlag::Closed))
                    {
                        tileCode = door->model->getTile(+1);
                    }
                    else
                    {
                        tileCode = door->model->getTile(+3);
                    }
                }
            }
            layers[1] += tileCode;
            if (x != (origin_x + radius - 1))
            {
                layers[1] += ",";
            }
        }
        layers[1] += ";";
    }

    // Prepare Living Creatures layer.
    for (int y = max_y; y > min_y; --y)
    {
        for (int x = min_x; x < max_x; ++x)
        {
            std::string tileCode = " : ";
            if ((origin_x == x) && (origin_y == y))
            {
                tileCode = ToString(1) + ":" + ToString(480);
            }
            else if (map.get(x, y) == MapTile::Walkable)
            {
                Room * room = this->getRoom(x, y, origin_z);
                // Check if there are creatures in the tile.
                if (!room->characters.empty())
                {
                    for (auto iterator : room->characters)
                    {
                        if (!HasFlag(iterator->flags, CharacterFlag::Invisible))
                        {
                            tileCode = iterator->race->getTile();
                            break;
                        }
                    }
                }
            }
            layers[2] += tileCode;
            if (x != (origin_x + radius - 1))
            {
                layers[2] += ",";
            }
        }
        layers[2] += ";";
    }
    return layers;
}

std::string Area::drawASCIIFov(Room * centerRoom, int radius)
{
    std::string result;
    if (!this->inBoundaries(centerRoom->coord))
    {
        return result;
    }
    // Retrieve the coordinates of the room.
    int origin_x = centerRoom->coord.x;
    int origin_y = centerRoom->coord.y;
    int origin_z = centerRoom->coord.z;
    // Evaluate the minimum and maximum value for x and y.
    int min_x = (origin_x < radius) ? 0 : (origin_x - radius);
    int max_x = ((origin_x + radius) > this->width) ? this->width : (origin_x + radius);
    int min_y = (origin_y < radius) ? 0 : (origin_y - radius);
    int max_y = ((origin_y + radius - 1) > this->height) ? this->height : (origin_y + radius - 1);
    // Create a 2D map of chararacters.
    Map2D<MapTile> map(radius * 2, radius * 2);
    // Evaluate the field of view.
    this->fov(map, origin_x, origin_y, origin_z, radius);
    // Prepare Living Creatures layer.
    for (int y = max_y; y >= min_y; --y)
    {
        for (int x = min_x; x < max_x; ++x)
        {
            // The tile which has to be placed.
            std::string tile = " ";
            Room * room = this->getRoom(x, y, origin_z);
            if ((map.get(x, y) != MapTile::Void) && (room != nullptr))
            {
                Exit * up = room->findExit(Direction::Up);
                Exit * down = room->findExit(Direction::Down);
                // VI  - WALKABLE
                tile = '.';
                // V   - OPEN DOOR
                Item * door = room->findDoor();
                if (door != nullptr)
                {
                    if (HasFlag(door->flags, ItemFlag::Closed))
                    {
                        tile = 'D';
                    }
                    else
                    {
                        tile = 'O';
                    }
                }
                // IV  - STAIRS
                if ((up != nullptr) && (down != nullptr))
                {
                    if (HasFlag(up->flags, ExitFlag::Stairs) && HasFlag(down->flags, ExitFlag::Stairs))
                    {
                        tile = "X";
                    }
                }
                else if (up != nullptr)
                {
                    if (HasFlag(up->flags, ExitFlag::Stairs))
                    {
                        tile = ">";
                    }
                }
                else if (down != nullptr)
                {
                    if (HasFlag(down->flags, ExitFlag::Stairs))
                    {
                        tile = "<";
                    }
                    else
                    {
                        tile = ' ';
                    }
                }
                // III - ITEMS
                if (room->items.size() > 0)
                {
                    tile = room->items.back()->model->getTile();
                }
                // II  - CHARACTERS
                if (room->characters.size() > 0)
                {
                    for (auto iterator : room->characters)
                    {
                        if (!HasFlag(iterator->flags, CharacterFlag::Invisible))
                        {
                            tile = iterator->race->getTile();
                        }
                    }
                }
                // I   - PLAYER
                if ((origin_x == x) && (origin_y == y))
                {
                    tile = "@";
                }
            }
            result += tile;
        }
        result += "\n";
    }
    return result;
}

void Area::fov(Map2D<MapTile> & map, int origin_x, int origin_y, int origin_z, int radius)
{
    double incr_x = 0.0;
    double incr_y = 0.0;
    double incr_z = 0.0;
    for (float i = 0.0; i < 360; ++i)
    {
        // The x and y coordinates used by LOS Algorithm.
        incr_x = cos(static_cast<double>(i * 0.0174533f));
        incr_y = sin(static_cast<double>(i * 0.0174533f));
        this->los(map, origin_x, origin_y, origin_z, incr_x, incr_y, incr_z, radius);
    }
}

void Area::los(
    Map2D<MapTile> & map,
    int origin_x,
    int origin_y,
    int origin_z,
    double incr_x,
    double incr_y,
    double incr_z,
    int radius)
{
    double ox = origin_x + static_cast<double>(0.5f);
    double oy = origin_y + static_cast<double>(0.5f);
    double oz = origin_z + static_cast<double>(0.5f);
    for (int i = 0; i < radius; ++i)
    {
        // Transform into integer the coordinates.
        int curr_x = static_cast<int>(ox);
        int curr_y = static_cast<int>(oy);
        int curr_z = static_cast<int>(oz);
        // Check if out of boundaries.
        if (!this->inBoundaries(curr_x, curr_y, curr_z))
        {
            map.set(curr_x, curr_y, MapTile::Void);
            break;
        }
        // Get the room at the current coordinates.
        Room * currentRoom = this->getRoom(curr_x, curr_y, curr_z);
        if (currentRoom == nullptr)
        {
            map.set(curr_x, curr_y, MapTile::Void);
            break;
        }
        // Check if there is a door.
        Item * door = currentRoom->findDoor();
        if (door != nullptr)
        {
            if (HasFlag(door->flags, ItemFlag::Closed))
            {
                map.set(curr_x, curr_y, MapTile::ClosedDoor);
                break;
            }
        }
        // Set the tile to visible.
        map.set(curr_x, curr_y, MapTile::Walkable);
        // Updated current coordinates.
        ox += incr_x;
        oy += incr_y;
        oz += incr_z;
    }
}

bool Area::fastInSight(
    int origin_x,
    int origin_y,
    int origin_z,
    int target_x,
    int target_y,
    int target_z,
    unsigned int radius)
{
    int dx = target_x - origin_x;
    int dy = target_y - origin_y;
    int dz = target_z - origin_z;
    double nx = dx;
    double ny = dy;
    double nz = dz;
    int sign_x = dx > 0 ? 1 : -1;
    int sign_y = dy > 0 ? 1 : -1;
    int sign_z = dz > 0 ? 1 : -1;
    double ix = 0;
    double iy = 0;
    double iz = 0;
    int current_x = 0;
    int current_y = 0;
    int current_z = 0;
    unsigned int step = 0;
    while (((ix < nx) || (iy < ny) || (iz < nz)) && (step < radius))
    {
        double change_x = (0.5 + ix) / nx;
        double change_y = (0.5 + iy) / ny;
        double change_z = (0.5 + iz) / nz;

        if (change_x < change_y)
        {
            if (change_x < change_z)
            {
                // Next step is on X axis.
                if ((current_x + sign_x) < 0)
                {
                    return false;
                }
                else
                {
                    current_x += sign_x;
                    ix++;
                }
            }
            else
            {
                // Next step is on Z axis.
                if ((current_z + sign_z) < 0)
                {
                    return false;
                }
                else
                {
                    current_z += sign_z;
                    iz++;
                }
            }
        }
        else
        {
            if (change_y < change_z)
            {
                // Next step is on Y axis.
                if ((current_y + sign_y) < 0)
                {
                    return false;
                }
                else
                {
                    current_y += sign_y;
                    iy++;
                }
            }
            else
            {
                // Next step is on Z axis.
                if ((current_z + sign_z) < 0)
                {
                    return false;
                }
                else
                {
                    current_z += sign_z;
                    iz++;
                }
            }
        }
        if (this->getRoom(current_x, current_y, current_z) == nullptr)
        {
            return false;
        }
        step++;
    }
    return true;
}

bool Area::fastInSight(Coordinates<int> origin, Coordinates<int> target, unsigned int radius)
{
    return this->fastInSight(origin.x, origin.y, origin.z, target.x, target.y, target.z, radius);
}

void Area::luaRegister(lua_State * L)
{
    luabridge::getGlobalNamespace(L) //
    .beginClass<Area>("Area") //
    .addData("vnum", &Area::vnum, false) //
    .addData("name", &Area::name, false) //
    .addData("builder", &Area::builder, false) //
    .addData("continent", &Area::continent, false) //
    .addData("width", &Area::width, false) //
    .addData("height", &Area::height, false) //
    .addData("elevation", &Area::elevation, false) //
    .endClass();
}
