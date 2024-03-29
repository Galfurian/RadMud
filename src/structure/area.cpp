/// @file   area.cpp
/// @brief  This file implements all the methods needed to interact with an area.
/// @author Enrico Fraccaroli
/// @date   Mar 10 2015
/// @copyright
/// Copyright (c) 2016 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
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

#include "structure/area.hpp"

#include "structure/structureUtils.hpp"
#include "utilities/logger.hpp"
#include "structure/room.hpp"

Area::Area() :
    vnum(),
    name(),
    builder(),
    map(),
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
//    Logger::log(LogLevel::Debug,
//                "Deleted area\t\t[%s]\t\t(%s)",
//                ToString(this->vnum),
//                this->name);
}

bool Area::check()
{
    if (vnum <= 0) return false;
    if (name.empty()) return false;
    if (builder.empty()) return false;
    if (width <= 0) return false;
    if (height <= 0) return false;
    if (elevation <= 0) return false;
    return true;
}

bool Area::inBoundaries(const Coordinates & coordinates) const
{
    if ((coordinates.x < 0) || (coordinates.x > width)) return false;
    if ((coordinates.y < 0) || (coordinates.y > height)) return false;
    return !((coordinates.z < 0) || (coordinates.z > elevation));
}

bool Area::isValid(const Coordinates & coordinates)
{
    // Check if out of boundaries.
    if (!this->inBoundaries(coordinates)) return false;
    // Get the room at the coordinates and check if there is a room.
    auto room = this->getRoom(coordinates);
    if (room == nullptr) return false;
    // Check if there is a door.
    auto door = StructUtils::findDoor(room);
    if (door != nullptr)
    {
        // Check if the door is closed.
        if (HasFlag(door->flags, ItemFlag::Closed)) return false;
    }
    return true;
}

CharacterVector Area::getCharactersAt(const CharacterVector & exceptions,
                                      const Coordinates & coordinates)
{
    CharacterVector characterContainer;
    if (this->isValid(coordinates))
    {
        for (auto it : this->getRoom(coordinates)->characters)
        {
            if (!exceptions.containsCharacter(it))
            {
                characterContainer.emplace_back_character(it);
            }
        }
    }
    return characterContainer;
}

ItemVector Area::getItemsAt(const ItemVector & exceptions,
                            const Coordinates & coordinates)
{
    ItemVector itemContainer;
    if (this->isValid(coordinates))
    {
        for (auto it : this->getRoom(coordinates)->items)
        {
            if (!exceptions.findItem(it->vnum))
            {
                itemContainer.emplace_back(it);
            }
        }
    }
    return itemContainer;
}

bool Area::addRoom(Room * room)
{
    if (this->inBoundaries(room->coord))
    {
        if (map.set(room->coord.x, room->coord.y, room->coord.z, room))
        {
            // Set the room area to be this one.
            room->area = this;
            return true;
        }
        else
        {
            Logger::log(LogLevel::Error,
                        "Room's insertion could not be completed %s.",
                        room->coord.toString());
        }
    }
    else
    {
        Logger::log(LogLevel::Error,
                    "Room's coordinates are not inside the boundaries %s.",
                    room->coord.toString());
    }
    return false;
}

bool Area::remRoom(Room * room)
{
    return map.erase(room->coord.x, room->coord.y, room->coord.z);
}

Room * Area::getRoom(int room_vnum)
{
    for (auto it : map)
    {
        for (auto it2 : it.second)
        {
            for (auto it3 : it2.second)
            {
                if (it3.second != nullptr)
                {
                    if (it3.second->vnum == room_vnum)
                    {
                        return it3.second;
                    }
                }
            }
        }
    }
    return nullptr;
}

Room * Area::getRoom(const Coordinates & coordinates)
{
    if (this->inBoundaries(coordinates))
    {
        return map.get(coordinates.x, coordinates.y, coordinates.z);
    }
    return nullptr;
}

std::vector<std::string> Area::drawFov(Room * centerRoom, const int & radius)
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
    int max_x = ((origin_x + radius) > this->width)
                ? this->width : (origin_x + radius);
    int min_y = (origin_y < radius) ? 0 : (origin_y - radius);
    int max_y = ((origin_y + radius - 1) > this->height)
                ? this->height : (origin_y + radius - 1);
    // Evaluate the field of view.
    auto coordinatesInFov = this->fov(centerRoom->coord, radius);
    // Prepare Environment layer.
    for (int y = max_y; y >= min_y; --y)
    {
        for (int x = min_x; x < max_x; ++x)
        {
            std::string tileCode = " : ";
            Coordinates coordinates(x, y, origin_z);
            bool found = false;
            for (auto it : coordinatesInFov)
            {
                if (coordinates == it)
                {
                    found = true;
                    break;
                }
            }
            if (found)
            {
                auto room = this->getRoom(Coordinates(x, y, origin_z));
                auto up = room->findExit(Direction::Up);
                auto down = room->findExit(Direction::Down);

                // By default set it to walkable tile.
                tileCode = ToString(15) + ":" + ToString(this->tileSet + 0);

                if ((up != nullptr) && (down != nullptr))
                {
                    if (HasFlag(up->flags, ExitFlag::Stairs)
                        && HasFlag(down->flags, ExitFlag::Stairs))
                    {
                        tileCode = ToString(18) + ":" +
                                   ToString(this->tileSet + 1);
                    }
                }
                else if (up != nullptr)
                {
                    if (HasFlag(up->flags, ExitFlag::Stairs))
                    {
                        tileCode = ToString(18) + ":" +
                                   ToString(this->tileSet + 1);
                    }
                }
                else if (down != nullptr)
                {
                    if (HasFlag(down->flags, ExitFlag::Stairs))
                    {
                        tileCode = ToString(18) + ":" +
                                   ToString(this->tileSet + 0);
                    }
                    else
                    {
                        tileCode = ToString(18) + ":" +
                                   ToString(this->tileSet + 4);
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
            Coordinates coordinates(x, y, origin_z);
            bool found = false;
            for (auto it : coordinatesInFov)
            {
                if (coordinates == it)
                {
                    found = true;
                    break;
                }
            }
            if (found)
            {
                Room * room = this->getRoom(coordinates);
                if (room != nullptr)
                {
                    Item * door = StructUtils::findDoor(room);
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
            Coordinates coordinates(x, y, origin_z);
            bool found = false;
            for (auto it : coordinatesInFov)
            {
                if (coordinates == it)
                {
                    found = true;
                    break;
                }
            }
            if ((origin_x == x) && (origin_y == y))
            {
                tileCode = ToString(1) + ":" + ToString(480);
            }
            else if (found)
            {
                Room * room = this->getRoom(Coordinates(x, y, origin_z));
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

std::string Area::drawASCIIFov(Room * centerRoom, const int & radius)
{
    if (!this->inBoundaries(centerRoom->coord))
    {
        return "";
    }
    std::string result;
    result.reserve(static_cast<size_t >(radius * radius));
    // Evaluate the minimum and maximum value for x and y.
    int min_x = (centerRoom->coord.x - radius);
    int max_x = (centerRoom->coord.x + radius);
    int min_y = (centerRoom->coord.y - radius);
    int max_y = (centerRoom->coord.y + radius);
    // Evaluate the field of view.
    auto view = this->fov(centerRoom->coord, radius);
    // Draw the fov.
    Coordinates point = centerRoom->coord;
    for (point.y = max_y; point.y >= min_y; --point.y)
    {
        for (point.x = min_x; point.x <= max_x; ++point.x)
        {
            if (std::find(view.begin(), view.end(), point) == view.end())
            {
                result += ' ';
                continue;
            }
            Room * room = this->getRoom(point);
            if (room == nullptr)
            {
                result += ' ';
                continue;
            }
            if (!room->isLit())
            {
                result += ' ';
                continue;
            }
            std::string tile;
            auto up = room->findExit(Direction::Up);
            auto down = room->findExit(Direction::Down);
            // VI  - WALKABLE
            if (room->liquidContent.first != nullptr)
            {
                tile = 'w';
            }
            else if (HasFlag(room->flags, RoomFlags::SpawnTree))
            {
                tile = 't';
            }
            else
            {
                tile = room->terrain->symbol;
            }
            // V   - OPEN DOOR
            auto door = StructUtils::findDoor(room);
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
                    tile = 'X';
                }
            }
            else if (up != nullptr)
            {
                if (HasFlag(up->flags, ExitFlag::Stairs))
                {
                    tile = '>';
                }
            }
            else if (down != nullptr)
            {
                if (HasFlag(down->flags, ExitFlag::Stairs))
                {
                    tile = '<';
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
                    if (!HasFlag(iterator->flags,
                                 CharacterFlag::Invisible))
                    {
                        tile = iterator->race->getTile();
                    }
                }
            }
            // I   - PLAYER
            if (centerRoom->coord == point)
            {
                tile = '@';
            }
            result += tile;
        }
        result += '\n';
    }
    return result;
}

CharacterVector Area::getCharactersInSight(CharacterVector & exceptions,
                                           Coordinates & origin,
                                           const int & radius)
{
    CharacterVector characterContainer;
    auto validCoordinates = this->fov(origin, radius);
    for (auto coordinates : validCoordinates)
    {
        characterContainer.addUnique(
            this->getCharactersAt(exceptions, coordinates));
    }
    return characterContainer;
}

ItemVector Area::getItemsInSight(ItemVector & exceptions,
                                 Coordinates & origin,
                                 const int & radius)
{
    ItemVector foundItems;
    auto validCoordinates = this->fov(origin, radius);
    for (auto coordinates : validCoordinates)
    {
        for (auto it :this->getItemsAt(exceptions, coordinates))
        {
            foundItems.emplace_back(it);
        }
    }
    return foundItems;
}

std::vector<Coordinates> Area::fov(Coordinates & origin, const int & radius)
{
    std::vector<Coordinates> cfov;
    auto CheckCoordinates = [&](const Coordinates & coordinates)
    {
        if (std::find(cfov.begin(), cfov.end(), coordinates) == cfov.end())
        {
            if (origin == coordinates)
            {
                cfov.emplace_back(coordinates);
            }
            else if (this->los(origin, coordinates, radius))
            {
                cfov.emplace_back(coordinates);
            }
        }
    };
    Coordinates point;
    while (point.x <= radius)
    {
        while ((point.y <= point.x) && (point.square() <= pow(radius, 2)))
        {
            // ---------------------------------------------------------
            CheckCoordinates(Coordinates(origin.x + point.x,
                                         origin.y + point.y,
                                         origin.z + point.z));
            // ---------------------------------------------------------
            CheckCoordinates(Coordinates(origin.x - point.x,
                                         origin.y + point.y,
                                         origin.z + point.z));
            // ---------------------------------------------------------
            CheckCoordinates(Coordinates(origin.x + point.x,
                                         origin.y - point.y,
                                         origin.z + point.z));
            // ---------------------------------------------------------
            CheckCoordinates(Coordinates(origin.x - point.x,
                                         origin.y - point.y,
                                         origin.z + point.z));
            // ---------------------------------------------------------
            CheckCoordinates(Coordinates(origin.x + point.y,
                                         origin.y + point.x,
                                         origin.z + point.z));
            // ---------------------------------------------------------
            CheckCoordinates(Coordinates(origin.x - point.y,
                                         origin.y + point.x,
                                         origin.z + point.z));
            // ---------------------------------------------------------
            CheckCoordinates(Coordinates(origin.x + point.y,
                                         origin.y - point.x,
                                         origin.z + point.z));
            // ---------------------------------------------------------
            CheckCoordinates(Coordinates(origin.x - point.y,
                                         origin.y - point.x,
                                         origin.z + point.z));
            // ---------------------------------------------------------
            ++point.y;
        }
        ++point.x;
        point.y = 0;
    }
    return cfov;
}

bool Area::los(const Coordinates & source,
               const Coordinates & target,
               const int & radius)
{
    // Deal with the easiest case.
    if (source == target) return true;
    // Check if there is a room at the given coordinates.
    if (this->getRoom(target) == nullptr) return false;
    // Ensure that the line will not extend too long.
    if (StructUtils::getDistance(source, target) > radius) return false;
    // Evaluates the difference.
    double dx = target.x - source.x;
    double dy = target.y - source.y;
    double dz = target.z - source.z;
    // Evaluate the distance between the
    double distance = std::sqrt((dx * dx) + (dy * dy) + (dz * dz));
    // Evaluate the unit increment for both X and Y.
    // Decrease the value of precision for a faster execution with
    //  a worsening in terms of accuracy (default 6).
    double precision = 5;
    double unitx = dx / (distance * precision);
    double unity = dy / (distance * precision);
    double unitz = dz / (distance * precision);
    // Evaluate the minimum value of increment.
    double min = 0.1;
    auto absX = std::abs(unitx);
    auto absY = std::abs(unity);
    auto absZ = std::abs(unitz);
    if (absX > 0.0) min = std::min(min, absX);
    if (absY > 0.0) min = std::min(min, absY);
    if (absZ > 0.0) min = std::min(min, absZ);
    // Set the initial values for X and Y.
    double x = source.x + 0.5;
    double y = source.y + 0.5;
    double z = source.z + 0.5;
    Coordinates coordinates(source.x, source.y, source.z);
    Coordinates previous = coordinates;
    for (double i = 0; i <= distance; i += min)
    {
        // Evaluate the integer version of the coordinates
        //  using the floor value.
        coordinates = Coordinates(std::floor(x), std::floor(y), std::floor(z));
        if (!this->isValid(coordinates))
        {
            return false;
        }
        if (coordinates.z > previous.z)
        {
            auto downstair = this->getRoom(previous);
            if (!downstair->findExit(Direction::Up))
            {
                return false;
            }
        }
        if (coordinates.z < previous.z)
        {
            auto upstair = this->getRoom(previous);
            if (!upstair->findExit(Direction::Down))
            {
                return false;
            }
        }
        if (coordinates == target)
        {
            return true;
        }
        // Increment the coordinates.
        x += unitx;
        y += unity;
        z += unitz;
    }
    return false;
}
