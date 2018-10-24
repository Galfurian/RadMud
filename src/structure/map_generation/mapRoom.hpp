/// @file   mapRoom.hpp
/// @author Enrico Fraccaroli
/// @date   24 03 2018
/// @copyright
/// Copyright (c) 2018 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
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

#pragma once

#include <memory>
#include <map>

#include "mapConfiguration.hpp"
#include "formatter.hpp"
#include "terrain.hpp"
#include "mapTile.hpp"
#include "liquid.hpp"
#include "utils.hpp"

namespace MapGen
{

/// @brief A group of tiles of the map, forming a room.
class MapRoom
{
public:
    int x1;
    int x2;
    int y1;
    int y2;

    int width;
    int height;

    int center_x;
    int center_y;
    /// The list of tiles which belong to this room.
    TileList tileList;

    MapRoom(int x, int y, int _width, int _height) :
        x1(x),
        x2(x + _width),
        y1(y),
        y2(y + _height),
        width(_width),
        height(_height),
        center_x(static_cast<int>(std::floor((x1 + x2) / 2))),
        center_y(static_cast<int>(std::floor((y1 + y2) / 2))),
        tileList()
    {
        // Nothing to do.
    }

    inline bool intersects(std::shared_ptr<MapRoom> const & other) const
    {
        return (x1 <= other->x2) && (x2 >= other->x1) &&
               (y1 <= other->y2) && (y2 >= other->y1);
    }

    inline std::string toString() const
    {
        std::stringstream ss;
        ss << "[" << x1 << "]" << "[" << x2 << "]"
           << "[" << y1 << "]" << "[" << y2 << "]"
           << "[W=" << width << "]" << "[H=" << height << "]";
        return ss.str();
    }
};

/// A 2d map of tiles.
typedef std::vector<std::shared_ptr<MapRoom>> RoomList;

}
