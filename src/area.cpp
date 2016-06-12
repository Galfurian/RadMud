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

bool Area::inBoundaries(const unsigned int & x, const unsigned int & y, const unsigned int & z)
{
    return ((x <= width) && (y <= height) && (z <= elevation));
}

bool Area::inBoundaries(const Coordinates<unsigned int> & coord)
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

Room * Area::getRoom(unsigned int x, unsigned int y, unsigned int z)
{
    return areaMap.get(x, y, z);
}

Room * Area::getRoom(Coordinates<unsigned int> coord)
{
    if (this->inBoundaries(coord))
    {
        return this->getRoom(coord.x, coord.y, coord.z);
    }
    return nullptr;
}

std::vector<std::string> Area::drawFov(Room * centerRoom, unsigned int radius)
{
    std::vector<std::string> layers(3);
    if (!this->inBoundaries(centerRoom->coord))
    {
        return layers;
    }
    unsigned int origin_x = static_cast<unsigned int>(centerRoom->coord.x);
    unsigned int origin_y = static_cast<unsigned int>(centerRoom->coord.y);
    unsigned int origin_z = static_cast<unsigned int>(centerRoom->coord.z);

    unsigned int min_x = 0;
    if (origin_x > radius)
    {
        min_x = origin_x - radius;
    }
    unsigned int max_x = (origin_x + radius);
    if (max_x > this->width)
    {
        max_x = this->width;
    }
    unsigned int min_y = 0;
    if (origin_y >= radius)
    {
        min_y = origin_y - radius;
    }
    unsigned int max_y = (origin_y + radius);
    if (max_y > this->height)
    {
        max_y = this->height;
    }

    // Create a 2D map of chararacters.
    Map2D<MapTile> map(radius * 2, radius * 2);

    // Evaluate the field of view.
    this->fov(map, origin_x, origin_y, origin_z, radius);

    // Prepare Enviroment layer.
    for (unsigned int y = max_y; y > min_y; --y)
    {
        for (unsigned int x = min_x; x < max_x; ++x)
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
    for (unsigned int y = max_y; y > min_y; --y)
    {
        for (unsigned int x = min_x; x < max_x; ++x)
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
    for (unsigned int y = max_y; y > min_y; --y)
    {
        for (unsigned int x = min_x; x < max_x; ++x)
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

std::string Area::drawASCIIFov(Room * centerRoom, unsigned int radius)
{
    std::string result;
    if (!this->inBoundaries(centerRoom->coord))
    {
        return result;
    }
    unsigned int origin_x = static_cast<unsigned int>(centerRoom->coord.x);
    unsigned int origin_y = static_cast<unsigned int>(centerRoom->coord.y);
    unsigned int origin_z = static_cast<unsigned int>(centerRoom->coord.z);

    unsigned int min_x = 0;
    if (origin_x > radius)
    {
        min_x = origin_x - radius;
    }
    unsigned int max_x = (origin_x + radius);
    if (max_x > this->width)
    {
        max_x = this->width;
    }
    unsigned int min_y = 0;
    if (origin_y >= radius)
    {
        min_y = origin_y - radius;
    }
    unsigned int max_y = (origin_y + radius);
    if (max_y > this->height)
    {
        max_y = this->height;
    }

    // Create a 2D map of chararacters.
    Map2D<MapTile> map(radius * 2, radius * 2);
    // Evaluate the field of view.
    this->fov(map, origin_x, origin_y, origin_z, radius);

    // Prepare Living Creatures layer.
    for (unsigned int y = max_y; y > min_y; --y)
    {
        for (unsigned int x = min_x; x < max_x; ++x)
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
                            break;
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

void Area::fov(
    Map2D<MapTile> & map,
    unsigned int origin_x,
    unsigned int origin_y,
    unsigned int origin_z,
    unsigned int radius)
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
    unsigned int origin_x,
    unsigned int origin_y,
    unsigned int origin_z,
    double incr_x,
    double incr_y,
    double incr_z,
    unsigned int radius)
{
    double ox = origin_x + static_cast<double>(0.5f);
    double oy = origin_y + static_cast<double>(0.5f);
    double oz = origin_z;

    for (unsigned int i = 0; i < radius; ++i)
    {
        // Transform into integer the coordinates.
        unsigned int curr_x = static_cast<unsigned int>(ox);
        unsigned int curr_y = static_cast<unsigned int>(oy);
        unsigned int curr_z = static_cast<unsigned int>(oz);
        // Check if out of boundaries.
        if (!this->inBoundaries(curr_x, curr_y, curr_z))
        {
            Logger::log(LogLevel::Error, "Not in boundaries: %s %s %s\n", ToString(curr_x), ToString(curr_y),
                ToString(curr_z));
            map.set(curr_x, curr_y, MapTile::Void);
            break;
        }
        // Get the room at the current coordinates.
        Room * currentRoom = this->getRoom(curr_x, curr_y, curr_z);
        if (currentRoom == nullptr)
        {
            Logger::log(LogLevel::Error, "No room at: %s %s %s\n", ToString(curr_x), ToString(curr_y),
                ToString(curr_z));
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
    unsigned int origin_x,
    unsigned int origin_y,
    unsigned int origin_z,
    unsigned int target_x,
    unsigned int target_y,
    unsigned int target_z,
    unsigned int radius)
{
    int dx = static_cast<int>(target_x) - static_cast<int>(origin_x);
    int dy = static_cast<int>(target_y) - static_cast<int>(origin_y);
    int dz = static_cast<int>(target_z) - static_cast<int>(origin_z);
    double nx = dx;
    double ny = dy;
    double nz = dz;
    int sign_x = dx > 0 ? 1 : -1;
    int sign_y = dy > 0 ? 1 : -1;
    int sign_z = dz > 0 ? 1 : -1;
    unsigned int x = origin_x;
    unsigned int y = origin_y;
    unsigned int z = origin_z;
    double ix = 0;
    double iy = 0;
    double iz = 0;
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
                if ((static_cast<int>(x) + sign_x) < 0)
                {
                    return false;
                }
                else
                {
                    x = static_cast<unsigned int>(static_cast<int>(x) + sign_x);
                    ix++;
                }
            }
            else
            {
                // Next step is on Z axis.
                if ((static_cast<int>(z) + sign_z) < 0)
                {
                    return false;
                }
                else
                {
                    z = static_cast<unsigned int>(static_cast<int>(z) + sign_z);
                    iz++;
                }
            }
        }
        else
        {
            if (change_y < change_z)
            {
                // Next step is on Y axis.
                if ((static_cast<int>(y) + sign_y) < 0)
                {
                    return false;
                }
                else
                {
                    y = static_cast<unsigned int>(static_cast<int>(y) + sign_y);
                    iy++;
                }
            }
            else
            {
                // Next step is on Z axis.
                if ((static_cast<int>(z) + sign_z) < 0)
                {
                    return false;
                }
                else
                {
                    z = static_cast<unsigned int>(static_cast<int>(z) + sign_z);
                    iz++;
                }
            }
        }
        if (this->getRoom(x, y, z) == nullptr)
        {
            return false;
        }
        step++;
    }
    return true;
}

bool Area::fastInSight(Coordinates<unsigned int> origin, Coordinates<unsigned int> target, unsigned int radius)
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
