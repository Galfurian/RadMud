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
    this->fov(map, origin_x, origin_y, origin_z, radius);
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
    this->fov(map, origin_x, origin_y, origin_z, radius);
    // Prepare Living Creatures layer.
    for (int y = max_y; y >= min_y; --y)
    {
        for (int x = min_x; x < max_x; ++x)
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

bool Area::fastInSight(
    int origin_x,
    int origin_y,
    int origin_z,
    int target_x,
    int target_y,
    int /*target_z*/,
    const unsigned int & radius)
{
    // The steps on y and x axis.
    int ystep, xstep;
    // The error accumulated during the increment.
    int error;
    // *vision the previous value of the error variable.
    int errorprev;
    // The points of the line.
    int x = origin_x;
    int y = origin_y;
    int z = origin_z;
    int dx = target_x - origin_x;
    int dy = target_y - origin_y;
    // Compulsory variables: the double values of dy and dx.
    // Work with double values for full precision.
    int ddy = 2 * dy;
    int ddx = 2 * dx;
    // NB the last point can't be here, because of its previous point (which has to be verified)
    if (dy < 0)
    {
        ystep = -1;
        dy = -dy;
    }
    else
    {
        ystep = 1;
    }
    if (dx < 0)
    {
        xstep = -1;
        dx = -dx;
    }
    else
    {
        xstep = 1;
    }
    if (ddx >= ddy)
    {
        // first octant (0 <= slope <= 1)
        // compulsory initialization (even for errorprev, needed when dx==dy)
        errorprev = error = dx;  // start in the middle of the square
        for (int i = 0; i < dx; ++i)
        {
            // do not use the first point (already done)
            x += xstep;
            error += ddy;
            if (error > ddx)
            {
                // increment y if AFTER the middle ( > )
                y += ystep;
                error -= ddx;
                // three cases (octant == right->right-top for directions below):
                if (error + errorprev < ddx)
                {
                    // bottom square also
                    if (!this->isValid(x, y - ystep, z)) return false;
                }
                else if (error + errorprev > ddx)
                {
                    // left square also
                    if (!this->isValid(x - xstep, y, z)) return false;
                }
                // This branch covers the case in which the line passes through a corner.
                //else
                //{
                //    if (!this->isValid(x - xstep, y, z)) return false;
                //    if (!this->isValid(x, y - ystep, z)) return false;
                //}
            }
            if (!this->isValid(x, y, z)) return false;
            errorprev = error;
            // Check the distance.
            auto distance = static_cast<unsigned int>(std::sqrt(std::pow(target_x - x, 2) + std::pow(target_y - y, 2)));
            if (distance > radius)
            {
                return false;
            }
        }
    }
    else
    {
        // The same as above.
        errorprev = error = dy;
        for (int i = 0; i < dy; ++i)
        {
            y += ystep;
            error += ddx;
            if (error > ddy)
            {
                x += xstep;
                error -= ddy;
                if (error + errorprev < ddy)
                {
                    if (!this->isValid(x - xstep, y, z)) return false;
                }
                else if (error + errorprev > ddy)
                {
                    if (!this->isValid(x, y - ystep, z)) return false;
                }
                // This branch covers the case in which the line passes through a corner.
                //else
                //{
                //    if (!this->isValid(x - xstep, y, z)) return false;
                //    if (!this->isValid(x, y - ystep, z)) return false;
                //}
            }
            if (!this->isValid(x, y, z)) return false;
            errorprev = error;
            // Check the distance.
            auto distance = static_cast<unsigned int>(std::sqrt(std::pow(target_x - x, 2) + std::pow(target_y - y, 2)));
            if (distance > radius)
            {
                return false;
            }
        }
    }
    return true;
}

bool Area::fastInSight(const Coordinates & origin, const Coordinates & target, const unsigned int & radius)
{
    return this->fastInSight(origin.x, origin.y, origin.z, target.x, target.y, target.z, radius);
}

bool Area::getCharactersInSight(
    std::vector<Character *> & targets,
    std::vector<Character *> & exceptions,
    int origin_x,
    int origin_y,
    int origin_z,
    const unsigned int & radius)
{
    double increment_x = 0.0, increment_y = 0.0, increment_z = 0.0;
    for (float i = 0.0; i < 360; ++i)
    {
        // The x and y coordinates used by LOS Algorithm.
        increment_x = cos(static_cast<double>(i * 0.0174533f));
        increment_y = sin(static_cast<double>(i * 0.0174533f));
        double ox = origin_x + static_cast<double>(0.5f);
        double oy = origin_y + static_cast<double>(0.5f);
        double oz = origin_z + static_cast<double>(0.5f);
        for (unsigned int distance = 0; distance <= radius; ++distance)
        {
            // Transform into integer the coordinates.
            int curr_x = static_cast<int>(ox);
            int curr_y = static_cast<int>(oy);
            int curr_z = static_cast<int>(oz);
            // Get the room at the current coordinates.
            Room * room = this->getRoom(curr_x, curr_y, curr_z);
            if (room != nullptr)
            {
                // Check if there is a door.
                Item * door = room->findDoor();
                if (door != nullptr)
                {
                    if (HasFlag(door->flags, ItemFlag::Closed))
                    {
                        break;
                    }
                }
                // Check if there is a character inside the room.
                for (auto it : room->characters)
                {
                    bool skip = false;
                    for (auto it2 : exceptions)
                    {
                        if (it->name == it2->name)
                        {
                            skip = true;
                            break;
                        }
                    }
                    if (!skip)
                    {
                        skip = false;
                        for (auto it2 : targets)
                        {
                            if (it->name == it2->name)
                            {
                                skip = true;
                                break;
                            }
                        }
                        if (!skip)
                        {
                            targets.push_back(it);
                        }
                    }
                }
                // Updated current coordinates.
                ox += increment_x;
                oy += increment_y;
                oz += increment_z;
            }
        }
    }
    return (!targets.empty());
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

