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

#ifndef MAP3D_HPP
#define MAP3D_HPP

#include <map>

/// Used to create and manage a tridimensional map.
template<typename T>
class Map3D
{
    private:
        /// Width of th map.
        unsigned width;
        /// Height of th map.
        unsigned height;
        /// Elevation of th map.
        unsigned elevation;
        /// Data contained inside the map.
        std::map<std::tuple<int, int, int>, T> data;

    public:
        /// @brief Constructor.
        Map3D() :
                width(100),
                height(100),
                elevation(100),
                data()
        {
            // Nothing to do.
        }

        /// @brief Constructor.
        /// @param _width     The width of the 3D map.
        /// @param _height    The height of the 3D map.
        /// @param _elevation The elevation of the 3D map.
        Map3D(unsigned _width, unsigned _height, unsigned _elevation) :
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
        Map3D(unsigned _width, unsigned _height, unsigned _elevation, T value) :
                width(_width),
                height(_height),
                elevation(_elevation),
                data()
        {
            for (unsigned z = 0; z < width; z++)
            {
                for (unsigned y = 0; y < height; y++)
                {
                    for (unsigned x = 0; x < height; x++)
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

        /// @brief Give an access to an object of given Coordinates3D.
        /// @param x Coordinate on width.
        /// @param y Coordinate on heigth.
        /// @param z Coordinate on altitude.
        /// @return The object at the given Coordinates3D.
        T & operator()(unsigned x, unsigned y, unsigned z)
        {
            return data[std::make_tuple(x, y, z)];
        }

        /// @brief Set the object at the given Coordinates3D.
        /// @param x     Coordinate on width.
        /// @param y     Coordinate on heigth.
        /// @param z     Coordinate on altitude.
        /// @param value The value that has to be set.
        void set(unsigned x, unsigned y, unsigned z, T value)
        {
            data[std::make_tuple(x, y, z)] = value;
        }

        /// @brief Retrieve the object at the given Coordinates3D.
        /// @param x Coordinate on width.
        /// @param y Coordinate on heigth.
        /// @param z Coordinate on altitude.
        /// @return The object at the given Coordinates3D.
        T & get(unsigned x, unsigned y, unsigned z)
        {
            return data[std::make_tuple(x, y, z)];
        }

        /// @brief Erase the object at the given Coordinates3D.
        /// @param x Coordinate on width axis.
        /// @param y Coordinate on heigth axis.
        /// @param z Coordinate on altitude axis.
        void erase(unsigned x, unsigned y, unsigned z)
        {
            data[std::make_tuple(x, y, z)] = nullptr;
        }

        /// Disable copy constructor.
        Map3D(const Map3D<T> &) = delete;

        /// Disable assignment.
        Map3D & operator=(const Map3D<T> &) = delete;
};

#endif
