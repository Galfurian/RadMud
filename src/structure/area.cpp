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

#include "area.hpp"

#include "room.hpp"

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
    Logger::log(LogLevel::Debug, "Deleted area\t\t[%s]\t\t(%s)", ToString(this->vnum), this->name);
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
    assert(type != AreaType::NoType);
    return true;
}

bool Area::inBoundaries(const int & x, const int & y, const int & z) const
{
    bool result = true;
    result &= (x >= 0) && (x <= width);
    result &= (y >= 0) && (y <= height);
    result &= (z >= 0) && (z <= elevation);
    return result;
}

bool Area::inBoundaries(const Coordinates & coord) const
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
            Logger::log(
                LogLevel::Error,
                "Room's insertion could not be completed %s.",
                room->coord.toString());
        }
    }
    else
    {
        Logger::log(
            LogLevel::Error,
            "Room's coordinates are not inside the boundaries %s.",
            room->coord.toString());
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
    if (this->inBoundaries(x, y, z))
    {
        return areaMap.get(x, y, z);
    }
    return nullptr;
}

Room * Area::getRoom(Coordinates coord)
{
    return this->getRoom(coord.x, coord.y, coord.z);
}

std::vector<std::string> Area::drawFov(Room * centerRoom, const unsigned int & radius)
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
    int max_y = ((origin_y + signedRadius - 1) > this->height) ? this->height : (origin_y + signedRadius - 1);
    // Create a 2D map of characters.
    Map2D<MapTile> map(signedRadius * 2, signedRadius * 2);
    // Evaluate the field of view.
    //this->fov(map, origin_x, origin_y, origin_z, radius);
    this->simpleFov(map, origin_x, origin_y, origin_z, radius);
    // Prepare Environment layer.
    for (int y = max_y; y >= min_y; --y)
    {
        for (int x = min_x; x < max_x; ++x)
        {
            std::string tileCode = " : ";
            if (map.get(x, y) == MapTile::Walkable)
            {
                Room * room = this->getRoom(x, y, origin_z);
                std::shared_ptr<Exit> up = room->findExit(Direction::Up);
                std::shared_ptr<Exit> down = room->findExit(Direction::Down);

                // By default set it to walkable tile.
                tileCode = ToString(15) + ":" + ToString(this->tileSet + 0);

                if ((up != nullptr) && (down != nullptr))
                {
                    if (HasFlag(up->flags, ExitFlag::Stairs)
                        && HasFlag(down->flags, ExitFlag::Stairs))
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
            if (x != (origin_x + signedRadius - 1))
            {
                layers[2] += ",";
            }
        }
        layers[2] += ";";
    }
    return layers;
}

std::string Area::drawASCIIFov(Room * centerRoom, const unsigned int & radius)
{
    std::string result;
    if (!this->inBoundaries(centerRoom->coord))
    {
        return result;
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
    int max_y = ((origin_y + signedRadius - 1) > this->height) ? this->height : (origin_y + signedRadius - 1);
    // Create a 2D map of characters.
    Map2D<MapTile> map(signedRadius * 2, signedRadius * 2, MapTile::Void);
    // Evaluate the field of view.
    //this->fov(map, origin_x, origin_y, origin_z, radius);
    this->simpleFov(map, origin_x, origin_y, origin_z, radius);
    // Prepare Living Creatures layer.
    for (int y = max_y; y >= min_y; --y)
    {
        for (int x = min_x; x <= max_x; ++x)
        {
            std::string tile = " ";
            Room * room = this->getRoom(x, y, origin_z);
            if (map.get(x, y) == MapTile::Void)
            {
                tile = " ";
            }
            else if (room == nullptr)
            {
                tile = " ";
            }
            else
            {
                std::shared_ptr<Exit> up = room->findExit(Direction::Up);
                std::shared_ptr<Exit> down = room->findExit(Direction::Down);
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
                    if (HasFlag(up->flags, ExitFlag::Stairs)
                        && HasFlag(down->flags, ExitFlag::Stairs))
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

void Area::fov(Map2D<MapTile> & map, int origin_x, int origin_y, int origin_z, const unsigned int & radius)
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
    const unsigned int & radius)
{
    double ox = origin_x + static_cast<double>(0.5f);
    double oy = origin_y + static_cast<double>(0.5f);
    double oz = origin_z + static_cast<double>(0.5f);
    for (unsigned int i = 0; i <= radius; ++i)
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

void Area::simpleFov(Map2D<MapTile> & map, int origin_x, int origin_y, int origin_z, const unsigned int & radius)
{
    int r = static_cast<int>(radius);
    for (int x = (origin_x - r); x <= origin_x; ++x)
    {
        for (int y = (origin_y - r); y <= origin_y; ++y)
        {
            if ((x == origin_x) && (y == origin_y))
            {
                map.set(x, y, MapTile::Walkable);
                continue;
            }
            if ((x - origin_x) * (x - origin_x) + (y - origin_y) * (y - origin_y) <= (r * r))
            {
                int symmetric_x = origin_x - (x - origin_x);
                int symmetric_y = origin_y - (y - origin_y);
                // [X, Y]
                if (this->fastInSight(origin_x, origin_y, origin_z, x, y, origin_z, radius))
                {
                    map.set(x, y, MapTile::Walkable);
                }
                // [X, sY]
                if (this->fastInSight(origin_x, origin_y, origin_z, x, symmetric_y, origin_z, radius))
                {
                    map.set(x, symmetric_y, MapTile::Walkable);
                }
                // [sX, Y]
                if (this->fastInSight(origin_x, origin_y, origin_z, symmetric_x, y, origin_z, radius))
                {
                    map.set(symmetric_x, y, MapTile::Walkable);
                }
                // [sX, sY]
                if (this->fastInSight(origin_x, origin_y, origin_z, symmetric_x, symmetric_y, origin_z, radius))
                {
                    map.set(symmetric_x, symmetric_y, MapTile::Walkable);
                }
                // (x, y), (x, ySym), (xSym , y), (xSym, ySym) are in the circle
            }
        }
    }
}

//#define USE_CUSTOM 0

bool Area::fastInSight(
    int origin_x,
    int origin_y,
    int origin_z,
    int target_x,
    int target_y,
    int /*target_z*/,
    const unsigned int & /*radius*/)
{
    int w = target_x - origin_x;
    int h = target_y - origin_y;
    int dx1 = (w < 0) ? -1 : (w > 0) ? 1 : 0;
    int dy1 = (h < 0) ? -1 : (h > 0) ? 1 : 0;
    int dx2 = (w < 0) ? -1 : (w > 0) ? 1 : 0;
    int dy2 = (h < 0) ? -1 : (h > 0) ? 1 : 0;
    int longest = std::abs(w);
    int shortest = std::abs(h);
    int cx = origin_x;
    int cy = origin_y;
    int cz = origin_z;
    if (!(longest > shortest))
    {
        longest = std::abs(h);
        shortest = std::abs(w);
        if (h < 0) dy2 = -1; else if (h > 0) dy2 = 1;
        dx2 = 0;
    }
    int numerator = longest >> 1;
    for (int i = 0; i <= longest; i++)
    {
        if (!this->isValid(cx, cy, cz))
        {
            break;
        }
        if ((cx == target_x) && (cy == target_y))
        {
            return true;
        }
        numerator += shortest;
        if (!(numerator < longest))
        {
            numerator -= longest;
            cx += dx1;
            cy += dy1;
        }
        else
        {
            cx += dx2;
            cy += dy2;
        }
    }
    return false;
}

bool Area::fastInSight(const Coordinates & origin, const Coordinates & target, const unsigned int & radius)
{
    return this->fastInSight(origin.x, origin.y, origin.z, target.x, target.y, target.z, radius);
}

CharacterContainer Area::getCharactersInSight(
    CharacterContainer & exceptions,
    int origin_x,
    int origin_y,
    int origin_z,
    const unsigned int & radius)
{
    CharacterContainer characterContainer;
    int r = static_cast<int>(radius);
    for (int x = (origin_x - r); x <= origin_x; ++x)
    {
        for (int y = (origin_y - r); y <= origin_y; ++y)
        {
            if ((x == origin_x) && (y == origin_y))
            {
                characterContainer.addUnique(this->getCharactersAt(x, y, origin_z, exceptions));
            }
            else
            {
                if ((x - origin_x) * (x - origin_x) + (y - origin_y) * (y - origin_y) <= (r * r))
                {
                    int symmetric_x = origin_x - (x - origin_x);
                    int symmetric_y = origin_y - (y - origin_y);
                    // [X, Y]
                    if (this->fastInSight(origin_x, origin_y, origin_z, x, y, origin_z, radius))
                    {
                        characterContainer.addUnique(
                            this->getCharactersAt(x,
                                                  y,
                                                  origin_z,
                                                  exceptions));
                    }
                    // [X, sY]
                    if (this->fastInSight(origin_x, origin_y, origin_z, x, symmetric_y, origin_z, radius))
                    {
                        characterContainer.addUnique(
                            this->getCharactersAt(x,
                                                  symmetric_y,
                                                  origin_z,
                                                  exceptions));
                    }
                    // [sX, Y]
                    if (this->fastInSight(origin_x, origin_y, origin_z, symmetric_x, y, origin_z, radius))
                    {
                        characterContainer.addUnique(
                            this->getCharactersAt(symmetric_x,
                                                  y,
                                                  origin_z,
                                                  exceptions));
                    }
                    // [sX, sY]
                    if (this->fastInSight(origin_x, origin_y, origin_z, symmetric_x, symmetric_y, origin_z, radius))
                    {
                        characterContainer.addUnique(
                            this->getCharactersAt(symmetric_x,
                                                  symmetric_y,
                                                  origin_z,
                                                  exceptions));
                    }
                    // (x, y), (x, ySym), (xSym , y), (xSym, ySym) are in the circle
                }
            }
        }
    }
    return characterContainer;
}

void Area::luaRegister(lua_State * L)
{
    luabridge::getGlobalNamespace(L)
        .beginClass<Area>("Area")
        .addData("vnum", &Area::vnum, false)
        .addData("name", &Area::name, false)
        .addData("builder", &Area::builder, false)
        .addData("continent", &Area::continent, false)
        .addData("width", &Area::width, false)
        .addData("height", &Area::height, false)
        .addData("elevation", &Area::elevation, false)
        .endClass();
}

bool Area::isValid(int x, int y, int z)
{
    // Check if out of boundaries.
    if (!this->inBoundaries(x, y, z))
    {
        return false;
    }
    // Get the room at the coordinates.
    Room * currentRoom = this->getRoom(x, y, z);
    if (currentRoom == nullptr)
    {
        return false;
    }
    // Check if there is a door.
    Item * door = currentRoom->findDoor();
    if (door != nullptr)
    {
        if (HasFlag(door->flags, ItemFlag::Closed))
        {
            return false;
        }
    }
    return true;
}

CharacterContainer Area::getCharactersAt(const int x, const int y, const int z, const CharacterContainer & exceptions)
{
    CharacterContainer characterContainer;
    if (this->isValid(x, y, z))
    {
        Room * room = this->getRoom(x, y, z);
        // Check if there is a character inside the room.
        for (auto it : room->characters)
        {
            if (!exceptions.containsCharacter(it))
            {
                characterContainer.emplace_back_character(it);
            }
        }
    }
    return characterContainer;
}

int Area::getDistance(const int x0, const int y0, const int z0, const int x1, const int y1, const int z1)
{
    return static_cast<int>(std::pow((x0 - x1) * (x0 - x1) + (y0 - y1) * (y0 - y1) + (z0 - z1) * (z0 - z1), 0.5));
}

int Area::getDistance(const Coordinates & source, const Coordinates & target)
{
    return Area::getDistance(source.x, source.y, source.z, target.x, target.y, target.z);
}

int Area::getDistance(Character * source, Character * target)
{
    return Area::getDistance(source->room->coord, target->room->coord);
}
