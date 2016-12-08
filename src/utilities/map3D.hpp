/// @file    map3D.hpp
/// @brief   Define the class Map3D.
/// @details This class allows to hadle a templatic tridimensional map.
/// @author  Enrico Fraccaroli
/// @date   Sep 8 2015
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

#pragma once

#include <map>
#include <unordered_map>

/// Used to create and manage a tridimensional map.
template<typename T>
class Map3D
{
private:
    /// Width of th map.
    int width;
    /// Height of th map.
    int height;
    /// Elevation of th map.
    int elevation;
    /// Data contained inside the map.
    std::map<int, std::map<int, std::map<int, T>>> data;

public:
    /// @brief Constructor.
    Map3D() :
        width(),
        height(),
        elevation(),
        data()
    {
        // Nothing to do.
    }

    /// @brief Constructor.
    /// @param _width     The width of the 3D map.
    /// @param _height    The height of the 3D map.
    /// @param _elevation The elevation of the 3D map.
    Map3D(int _width, int _height, int _elevation) :
        width(_width),
        height(_height),
        elevation(_elevation),
        data()
    {
        // Nothing to do.
    }

    /// @brief Constructor.
    /// @param _width     The width of the 3D map.
    /// @param _height    The height of the 3D map.
    /// @param _elevation The elevation of the 3D map.
    /// @param value     The initial value of the cells.
    Map3D(int _width, int _height, int _elevation, T value) :
        width(_width),
        height(_height),
        elevation(_elevation),
        data()
    {
        for (int z = 0; z < width; ++z)
        {
            for (int y = 0; y < height; ++y)
            {
                for (int x = 0; x < height; ++x)
                {
                    data[z][y][x] = value;
                }
            }
        }
    }

    /// Disable copy constructor.
    Map3D(const Map3D<T> &) = delete;

    /// Disable assignment.
    Map3D & operator=(const Map3D<T> &) = delete;

    /// @brief Destructor.
    ~Map3D()
    {
        // Nothing to do.
    }

    /// @brief Provides the width.
    int getWidth() const
    {
        return width;
    }

    /// @brief Provides the height.
    int getHeight() const
    {
        return height;
    }

    /// @brief Provides the elevation.
    int getElevation() const
    {
        return elevation;
    }

    /// @brief Set the object at the given Coordinates3D.
    /// @param x     Coordinate on width.
    /// @param y     Coordinate on heigth.
    /// @param z     Coordinate on altitude.
    /// @param value The value that has to be set.
    /// @return <b>True</b> if the value has been set,<br>
    ///         <b>False</b> otherwise.
    bool set(int x, int y, int z, T value)
    {
        data[x][y][z] = value;
        return true;
    }

    /// @brief Retrieve the object at the given Coordinates3D.
    /// @param x Coordinate on width.
    /// @param y Coordinate on heigth.
    /// @param z Coordinate on altitude.
    /// @return The object at the given Coordinates3D.
    T & get(int x, int y, int z)
    {
        return data[x][y][z];
    }

    /// @brief Checks if there is an object at the given Coordinates3D.
    /// @param x Coordinate on width.
    /// @param y Coordinate on heigth.
    /// @param z Coordinate on altitude.
    /// @return <b>True</b> if if there is an object at the given coordinates,<br>
    ///         <b>False</b> otherwise.
    bool has(int x, int y, int z) const
    {
        typename std::map<int, std::map<int, std::map<int, T>>>::const_iterator it = data.find(x);
        if (it != data.end())
        {
            typename std::map<int, std::map<int, T>>::const_iterator it2 = it->second.find(y);
            if (it2 != it->second.end())
            {
                return it2->second.find(z) != it2->second.end();
            }
        }
        return false;
    }

    /// @brief Erase the object at the given Coordinates3D and returns an iterator to the.
    /// @param x Coordinate on width axis.
    /// @param y Coordinate on heigth axis.
    /// @param z Coordinate on altitude axis.
    /// @return <b>True</b> if if the object where removed,<br>
    ///         <b>False</b> otherwise.
    bool erase(int x, int y, int z)
    {
        if (this->has(x, y, z))
        {
            auto it = data[x][y].find(z);
            data[x][y].erase(it);
            return true;
        }
        return false;
    }

    /// @brief Provides an iterator to the begin of the list of data.
    /// @return An iterator to the begin of the 3D map.
    typename std::map<int, std::map<int, std::map<int, T>>>::iterator begin()
    {
        return data.begin();
    }

    /// @brief Provides an iterator to the end of the list of data.
    /// @return An iterator to the end of the 3D map.
    typename std::map<int, std::map<int, std::map<int, T>>>::iterator end()
    {
        return data.end();
    }

    /// @brief Provides the size of the data.
    /// @return The size of the 3D map.
    size_t size() const
    {
        return data.size();
    }
};
