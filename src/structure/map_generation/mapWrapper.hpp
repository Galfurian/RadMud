/// @file   mapWrapper.hpp
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

#pragma once

#include "mapCell.hpp"

class MapWrapper
{
public:
    /// The unique virtual number.
    unsigned int vnum;
    /// Width of th map.
    int width;
    /// Height of th map.
    int height;
    /// The map.
    std::map<int, std::map<int, MapCell>> map;

    /// @brief Constructor.
    MapWrapper();

    /// @brief Destructor.
    ~MapWrapper();

    /// @brief Allows to set the width of the map.
    inline void setWidth(const int & _width)
    {
        width = _width;
    }

    /// @brief Allows to set the height of the map.
    inline void setHeight(const int & _height)
    {
        height = _height;
    }

    /// @brief Provide the width of the map.
    inline int getWidth() const
    {
        return width;
    }

    /// @brief Provide the height of the map.
    inline int getHeight() const
    {
        return height;
    }

    /// @brief Returns the cell at the given position.
    inline MapCell * getCell(int x, int y)
    {
        if ((x < 0) || (x >= width)) return nullptr;
        if ((y < 0) || (y >= height)) return nullptr;
        return &map[x][y];
    }

    /// @brief Set the object at the given Coordinates2D.
    /// @param x     Coordinate on width axis.
    /// @param y     Coordinate on height axis.
    /// @param value The value that has to be set.
    inline void set(int x,
                    int y,
                    const MapCell & mapCell)
    {
        map[x][y] = mapCell;
    }

    /// @brief Destroy the map.
    void destroy();

    /// @brief Build the map.
    bool buildMap();
};