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

// Basic Include.
#include "continent.hpp"

#include <cmath>

// Other Include.
#include "mud.hpp"
#include "room.hpp"
#include "utils.hpp"
#include "logger.hpp"
#include "constants.hpp"

Continent::Continent() :
    vnum(),
    name(),
    builder(),
    width(),
    height(),
    txtMap(),
    charMap(),
    continentMap(),
    tileSet()
{
}

Continent::~Continent()
{
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
    unsigned int y = 0, x = 0;
    for (char c : txtMap)
    {
        if (c == '\n')
        {
            x = 0;
            y++;
            continue;
        }
        charMap.set(x, y, c);

        Room * room = &continentMap[std::make_tuple(x, y, 0)];
        room->vnum = -1;
        room->area = nullptr;
        room->continent = this;
        room->coord = Coordinates<unsigned int>(x, y, 0);
        room->terrain = "outside";
        room->name = "Travelling";
        room->description = "You are traveling along the continent.";
        room->flags = 0;
        x++;
    }
    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            continentMap[std::make_tuple(x, y, 0)].connectExits();
        }
    }
}

void Continent::print()
{
    for (unsigned int y = 0; y < height; y++)
    {
        for (unsigned int x = 0; x < width; x++)
        {
            printf("%c", charMap(x, y));
        }
        printf("\n");
    }
}

bool Continent::inBoundaries(unsigned int x, unsigned int y, unsigned int z)
{
    if (x <= width)
    {
        if (y <= height)
        {
            if (z == 0)
            {
                return true;
            }
            else
            {
                Logger::log(LogLevel::Error, "[Area::AddRoom] Wrong altitude.");
            }
        }
        else
        {
            Logger::log(LogLevel::Error, "[Area::AddRoom] Wrong height.");
        }
    }
    else
    {
        Logger::log(LogLevel::Error, "[Area::AddRoom] Wrong width.");
    }
    return false;
}

bool Continent::inBoundaries(Coordinates<unsigned int> coord)
{
    bool accepted = false;
    if ((coord.x > 0) && (coord.y > 0) && (coord.z > 0))
    {
        accepted = this->inBoundaries(static_cast<unsigned int>(coord.x), static_cast<unsigned int>(coord.y),
            static_cast<unsigned int>(coord.z));
    }
    return accepted;
}

Room * Continent::getRoom(unsigned int x, unsigned int y, unsigned int z)
{
    ContinentMap::iterator it = continentMap.find(std::make_tuple(x, y, z));
    if (it != continentMap.end())
    {
        return &it->second;
    }
    return nullptr;
}

Room * Continent::getRoom(Coordinates<unsigned int> coord)
{
    Room * room = nullptr;
    if ((coord.x > 0) && (coord.y > 0) && (coord.z > 0))
    {
        room = this->getRoom(static_cast<unsigned int>(coord.x), static_cast<unsigned int>(coord.y),
            static_cast<unsigned int>(coord.z));
    }
    return room;
}

std::vector<std::string> Continent::drawFov(Room * centerRoom, unsigned int radius)
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
    else
    {
        min_x = 0;
    }
    unsigned int min_y = 0;
    if (origin_y >= radius)
    {
        min_y = origin_y - radius;
    }
    else
    {
        min_y = 0;
    }

    // Create a 2D map of chararacters.
    Map2D<ContinentTile> map(radius * 2, radius * 2, ContinentTile::Void);
    // Evaluate the field of view.
    this->fieldOfView(map, origin_x, origin_y, origin_z, radius);

    // Prepare Enviroment layer.

    // Prepare Enviroment layer.
    for (unsigned int y = (origin_y + radius); y > min_y; --y)
    {
        for (unsigned int x = min_x; x < (origin_x + radius); ++x)
        {
            ContinentTile tile = map.get(x, y);
            std::string tileCode = " : ";

            if ((tile == ContinentTile::Walk) || (tile == ContinentTile::DoorOpen))
            {
                Room * room = this->getRoom(x, y, origin_z);
                Exit * up = room->findExit(Direction::Up);
                Exit * down = room->findExit(Direction::Down);

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

            if (x != (origin_x + radius - 1))
            {
                layers[0] += ",";
            }
        }
        layers[0] += ";";
    }

    // Prepare Objects layer.
    for (unsigned int y = (origin_y + radius); y > min_y; --y)
    {
        for (unsigned int x = min_x; x < (origin_x + radius); ++x)
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

            if (x != (origin_x + radius - 1))
            {
                layers[1] += ",";
            }
        }
        layers[1] += ";";
    }

    // Prepare Living Creatures layer.
    for (unsigned int y = (origin_y + radius); y > min_y; --y)
    {
        for (unsigned int x = min_x; x < (origin_x + radius); ++x)
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

            if (x != (origin_x + radius - 1))
            {
                layers[2] += ",";
            }
        }
        layers[2] += ";";
    }

    return layers;
}

void Continent::fieldOfView(
    Map2D<ContinentTile> & map,
    unsigned int origin_x,
    unsigned int origin_y,
    unsigned int origin_z,
    unsigned int radius)
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
