/// @file    map2D.hpp
/// @brief   Define the class Map2D.
/// @details This class allows to hadle a templatic bidimensional map.
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

/// @brief Used to create and manage a bidimensional map.
template<typename T>
class Map2D
{
    private:
        /// Width of th map.
        int width;
        /// Height of th map.
        int height;
        /// Data contained inside the map.
        std::map<std::tuple<int, int>, T> data;

    public:

        /// @brief Constructor.
        Map2D() :
            width(),
            height(),
            data()
        {
            // Nothing to do.
        }

        /// @brief Constructor.
        /// @param _width  The width of the 2D map.
        /// @param _height The height of the 2D map.
        Map2D(int _width, int _height) :
            width(_width),
            height(_height),
            data()
        {
            // Nothing to do.
        }

        /// @brief Constructor.
        /// @param _width  The width of the 2D map.
        /// @param _height The height of the 2D map.
        /// @param value  The initial value of the cells.
        Map2D(int _width, int _height, T value) :
            width(_width),
            height(_height),
            data()
        {
            for (int y = 0; y < width; y++)
            {
                for (int x = 0; x < height; x++)
                {
                    data[std::make_tuple(y, x)] = value;
                }
            }
        }

        /// @brief Destructor.
        ~Map2D()
        {
            // Nothing to do.
        }

        /// @brief Give an access to an object of given Coordinates2D.
        /// @param x Coordinate on width axis.
        /// @param y Coordinate on heigth axis.
        /// @return The object at the given Coordinates2D.
        T & operator()(int x, int y)
        {
            return data[std::make_tuple(x, y)];
        }

        /// @brief Set the object at the given Coordinates2D.
        /// @param x     Coordinate on width axis.
        /// @param y     Coordinate on heigth axis.
        /// @param value The value that has to be set.
        void set(int x, int y, T value)
        {
            data[std::make_tuple(x, y)] = value;
        }

        /// @brief Retrieve the object at the given Coordinates2D.
        /// @param x Coordinate on width axis.
        /// @param y Coordinate on heigth axis.
        /// @return The object at the given Coordinates2D.
        T & get(int x, int y)
        {
            return data[std::make_tuple(x, y)];
        }

        /// @brief Erase the object at the given Coordinates2D.
        /// @param x Coordinate on width axis.
        /// @param y Coordinate on heigth axis.
        void erase(int x, int y)
        {
            set(x, y, nullptr);
        }

        /// Disable copy constructor.
        Map2D(const Map2D<T> &) = delete;

        /// Disable assignment.
        Map2D & operator=(const Map2D<T> &) = delete;
};
