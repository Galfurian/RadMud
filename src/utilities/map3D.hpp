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
    std::map<std::tuple<int, int, int>, T> data;

public:
    /// Iterator for the 3D structure.
    using iterator = typename std::map<std::tuple<int, int, int>, T>::iterator;
    /// Const iterator for the 3D structure.
    using const_iterator = typename std::map<std::tuple<int, int, int>, T>::const_iterator;

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
        for (int z = 0; z < width; z++)
        {
            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < height; x++)
                {
                    data[std::make_tuple(z, y, x)] = value;
                }
            }
        }
    }

    /// @brief Destructor.
    ~Map3D()
    {
        // Nothing to do.
    }

    /// @brief Set the object at the given Coordinates3D.
    /// @param x     Coordinate on width.
    /// @param y     Coordinate on heigth.
    /// @param z     Coordinate on altitude.
    /// @param value The value that has to be set.
    bool set(int x, int y, int z, T value)
    {
#if 0
        const key_t key = std::make_tuple(x, y, z);
        auto ret = data.insert(std::make_pair(key, value));
        if (!ret.second)
        {
            std::cout << "Not unique at " << ToString(x) << " " << ToString(y) << " " << ToString(z) << "\n";
        }
        else
        {
            std::cout << "Set at " << ToString(x) << " " << ToString(y) << " " << ToString(z) << "\n";
        }
        return ret.second;
#else
        data[std::make_tuple(x, y, z)] = value;
        return true;
#endif
    }

    /// @brief Retrieve the object at the given Coordinates3D.
    /// @param x Coordinate on width.
    /// @param y Coordinate on heigth.
    /// @param z Coordinate on altitude.
    /// @return The object at the given Coordinates3D.
    T & get(int x, int y, int z)
    {
        return data[std::make_tuple(x, y, z)];
    }

    /// @brief Erase the object at the given Coordinates3D and returns an iterator to the.
    /// @param x Coordinate on width axis.
    /// @param y Coordinate on heigth axis.
    /// @param z Coordinate on altitude axis.
    /// @return The object at the given Coordinates3D.
    iterator erase(int x, int y, int z)
    {
        return FindErase(data, std::make_tuple(x, y, z));
    }

    /// @brief Provides an iterator to the begin of the list of data.
    /// @return An iterator to the begin of the 3D map.
    iterator begin()
    {
        return data.begin();
    }

    /// @brief Provides an iterator to the end of the list of data.
    /// @return An iterator to the end of the 3D map.
    iterator end()
    {
        return data.end();
    }

    /// @brief Provides the size of the data.
    /// @return The size of the 3D map.
    size_t size()
    {
        return data.size();
    }

    /// Disable copy constructor.
    Map3D(const Map3D<T> &) = delete;

    /// Disable assignment.
    Map3D & operator=(const Map3D<T> &) = delete;
};
