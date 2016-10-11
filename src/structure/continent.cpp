/// @file   continent.cpp
/// @brief  Implements the methods of class Continent.
/// @author Enrico Fraccaroli
/// @date   Aug 1 2015
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

#include "continent.hpp"

#include <cmath>

#include "room.hpp"
#include "../mud.hpp"

Continent::Continent() :
    vnum(),
    name(),
    builder(),
    width(),
    height(),
    elevation(),
    txtMap(),
    charMap(),
    continentMap(),
    tileSet()
{
    // Nothing to do.
}

Continent::~Continent()
{
    Logger::log(LogLevel::Debug, "Deleted continent\t[%s]\t\t(%s)", ToString(this->vnum), this->name);
}

bool Continent::check()
{
    assert(vnum > 0);
    assert(!name.empty());
    assert(!builder.empty());
    assert(width > 0);
    assert(height > 0);
    return true;
}

void Continent::init()
{
    // Nothing to do.
}

void Continent::print()
{
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            printf("%c", charMap(x, y));
        }
        printf("\n");
    }
}

bool Continent::inBoundaries(int x, int y, int z)
{
    bool result = true;
    result &= (x >= 0) && (x <= width);
    result &= (y >= 0) && (y <= height);
    result &= (z >= 0) && (y <= elevation);
    return result;
}

bool Continent::inBoundaries(Coordinates coord)
{
    return this->inBoundaries(coord.x, coord.y, coord.z);
}

Room * Continent::getRoom(int x, int y, int z)
{
    return &continentMap.get(x, y, z);
}

Room * Continent::getRoom(Coordinates coord)
{
    return &continentMap.get(coord.x, coord.y, coord.z);
}

std::vector<std::string> Continent::drawFov(Room * centerRoom, const unsigned int & radius)
{
    std::vector<std::string> layers(3);
    if (!this->inBoundaries(centerRoom->coord))
    {
        return layers;
    }
    int signedRadius = static_cast<int>(radius);
    // Retrieve the coordinates of the room.
    int origin_x = centerRoom->coord.x;
    int origin_y = centerRoom->coord.y;
    int origin_z = centerRoom->coord.z;
    // Evaluate the minimum and maximum value for x and y.
    int min_x = (origin_x < signedRadius) ? 0 : (origin_x - signedRadius);
    int max_x = ((origin_x + signedRadius) > this->width) ? this->width : (origin_x + signedRadius);
    int min_y = (origin_y < signedRadius) ? 0 : (origin_y - signedRadius);
    int max_y = ((origin_y + signedRadius) > this->height) ? this->height : (origin_y + signedRadius);
    // Create a 2D map of chararacters.
    Map2D<ContinentTile> map(signedRadius * 2, signedRadius * 2);
    // Evaluate the field of view.
    this->fieldOfView(map, origin_x, origin_y, origin_z, radius);
    // Prepare Enviroment layer.
    for (int y = max_y; y > min_y; --y)
    {
        for (int x = min_x; x < max_x; ++x)
        {
            ContinentTile tile = map.get(x, y);
            std::string tileCode = " : ";

            if ((tile == ContinentTile::Walk) || (tile == ContinentTile::DoorOpen))
            {
                Room * room = this->getRoom(x, y, origin_z);
                std::shared_ptr<Exit> up = room->findExit(Direction::Up);
                std::shared_ptr<Exit> down = room->findExit(Direction::Down);

                // By default set it to walkable tile.
                tileCode = ToString(15) + ":" + ToString(this->tileSet + 0);

                // III - Check if there are STAIRS in the tile.
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

            if (x != (origin_x + signedRadius - 1))
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
            Room * room = this->getRoom(x, y, origin_z);

            ContinentTile tile = map.get(x, y);

            std::string tileCode = " : ";

            if (tile == ContinentTile::Walk)
            {
                // II  - Check if there are ITEMS in the tile.
                if (room->items.size() > 0)
                {
                    tileCode = room->items.back()->model->getTile();
                }
            }
            else if (tile == ContinentTile::Door)
            {
                Item * door = room->findDoor();
                if (door != nullptr)
                {
                    tileCode = door->model->getTile(+1);
                }
            }
            else if (tile == ContinentTile::DoorOpen)
            {
                Item * door = room->findDoor();
                if (door != nullptr)
                {
                    tileCode = door->model->getTile(+3);
                }
            }

            layers[1] += tileCode;

            if (x != (origin_x + signedRadius - 1))
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
            ContinentTile tile = map.get(x, y);

            std::string tileCode = " : ";

            if ((origin_x == x) && (origin_y == y))
            {
                tileCode = ToString(1) + ":" + ToString(480);
            }
            else if (tile == ContinentTile::Walk)
            {
                Room * room = this->getRoom(x, y, centerRoom->coord.z);

                // Check if there are creatures in the tile.
                if (room->characters.size() > 0)
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

            if (x != (origin_x + signedRadius - 1))
            {
                layers[2] += ",";
            }
        }
        layers[2] += ";";
    }

    return layers;
}

void Continent::fieldOfView(Map2D<ContinentTile> & map, int origin_x, int origin_y, int origin_z, const unsigned int & radius)
{
    double incr_x = 0.0;
    double incr_y = 0.0;
    double incr_z = 0.0;
    for (float i = 0; i < 360; ++i)
    {
        // The x and y coordinates used by LOS Algorithm.
        incr_x = cos(static_cast<double>(i * 0.0174533f));
        incr_y = sin(static_cast<double>(i * 0.0174533f));
        this->lineOfSight(map, origin_x, origin_y, origin_z, incr_x, incr_y, incr_z, radius);
    }
}

void Continent::lineOfSight(
    Map2D<ContinentTile> & map,
    int origin_x,
    int origin_y,
    int origin_z,
    double incr_x,
    double incr_y,
    double incr_z,
    const unsigned int & radius)
{
    double ox = origin_x + static_cast<double>(0.5f);
    double oy = origin_y + static_cast<double>(0.5f);
    double oz = origin_z;

    for (unsigned int i = 0; i <= radius; ++i)
    {
        // Transform into integer the coordinates.
        int curr_x = static_cast<int>(ox);
        int curr_y = static_cast<int>(oy);
        int curr_z = static_cast<int>(oz);

        // Check if out of boundaries.
        if (!this->inBoundaries(curr_x, curr_y, curr_z))
        {
            break;
        }

        // Get the room at the current coordinates.
        Room * currentRoom = this->getRoom(curr_y, curr_x, curr_z);
        if (currentRoom == nullptr)
        {
            map.set(curr_x, curr_y, ContinentTile::Void);
            break;
        }

        // Check if there is a door.
        Item * door = currentRoom->findDoor();
        if (door != nullptr)
        {
            if (HasFlag(door->flags, ItemFlag::Closed))
            {
                map.set(curr_x, curr_y, ContinentTile::Door);
                return;
            }
            else
            {
                map.set(curr_x, curr_y, ContinentTile::DoorOpen);
            }
        }
        else
        {
            map.set(curr_x, curr_y, ContinentTile::Walk);
        }
        // Updated current coordinates.
        ox += incr_x;
        oy += incr_y;
        oz += incr_z;
    }
}
