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

#include "structure/coordinates.hpp"
#include <map>

/// @brief Used to create and manage a bidimensional map.
template <typename T> class Map2D {
private:
	/// Width of th map.
	int width;
	/// Height of th map.
	int height;
	/// Null value.
	T nullValue;
	/// Data contained inside the map.
	std::map<std::tuple<int, int>, T> data;

public:
	/// @brief Constructor.
	Map2D() : width(), height(), nullValue(), data()
	{
		// Nothing to do.
	}

	/// @brief Constructor.
	Map2D(const int &_width, const int &_height) :
		width(_width),
		height(_height),
		nullValue(),
		data()
	{
		// Nothing to do.
	}

	/// @brief Constructor.
	Map2D(int _width, int _height, T _nullValue) :
		width(_width),
		height(_height),
		nullValue(_nullValue),
		data()
	{
		// Nothing to do.
	}

	/// @brief Constructor.
	Map2D(int _width, int _height, T _nullValue, T value) :
		width(_width),
		height(_height),
		nullValue(_nullValue),
		data()
	{
		for (int y = 0; y < width; y++) {
			for (int x = 0; x < height; x++) {
				data[std::make_tuple(y, x)] = value;
			}
		}
	}

	/// @brief Move constructor.
	Map2D(const Map2D<T> &&other) :
		width(std::move(other.width)),
		height(std::move(other.height)),
		nullValue(std::move(other.nullValue)),
		data(std::move(other.data))
	{
		// Nothing to do.
	}

	/// @brief Disable Copy Constructor.
	Map2D(const Map2D<T> &) = delete;

	/// @brief Destructor.
	~Map2D()
	{
		// Nothing to do.
	}

	/// @brief Allows to set the width of the map.
	void setWidth(const int &_width)
	{
		width = _width;
	}

	/// @brief Allows to set the height of the map.
	void setHeight(const int &_height)
	{
		height = _height;
	}

	/// @brief Provide the width of the map.
	int getWidth() const
	{
		return width;
	}

	/// @brief Provide the height of the map.
	int getHeight() const
	{
		return height;
	}

	/// @brief Give an access to an object of given Coordinates2D.
	/// @param x Coordinate on width axis.
	/// @param y Coordinate on height axis.
	/// @return The object at the given Coordinates2D.
	T &operator()(int x, int y)
	{
		return data[std::make_tuple(x, y)];
	}

	/// @brief Set the object at the given Coordinates2D.
	/// @param x     Coordinate on width axis.
	/// @param y     Coordinate on height axis.
	/// @param value The value that has to be set.
	void set(int x, int y, T value)
	{
		data[std::make_tuple(x, y)] = value;
	}

	/// @brief Set the object at the given Coordinates2D.
	/// @param coordinates The coordinate.
	/// @param value The value that has to be set.
	void set(const Coordinates &coordinates, T value)
	{
		data[std::make_tuple(coordinates.x, coordinates.y)] = value;
	}

	/// @brief Retrieve the object at the given Coordinates2D.
	/// @param x Coordinate on width axis.
	/// @param y Coordinate on height axis.
	/// @return The object at the given Coordinates2D.
	T &get(int x, int y)
	{
		return data[std::make_tuple(x, y)];
	}

	/// @brief Retrieve the object at the given Coordinates2D.
	/// @param x Coordinate on width axis.
	/// @param y Coordinate on height axis.
	/// @return The object at the given Coordinates2D.
	T get(int x, int y) const
	{
		return data.at(std::make_tuple(x, y));
	}

	/// @brief Erase the object at the given Coordinates2D.
	/// @param x Coordinate on width axis.
	/// @param y Coordinate on height axis.
	void erase(int x, int y)
	{
		set(x, y, nullValue);
	}

	/// Disable assign operator.
	Map2D &operator=(const Map2D<T> &) = delete;

	/// @brief Move operator.
	Map2D &operator=(const Map2D<T> &&right)
	{
		data = std::move(right.data);
		return (*this);
	}
};
