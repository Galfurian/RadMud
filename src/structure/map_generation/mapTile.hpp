/// @file   tile.hpp
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
#include "liquid.hpp"
#include "utils.hpp"
#include "align.hpp"
#include "room.hpp"

namespace MapGen
{
/// @brief A single tile of the map.
class MapTile {
public:
	/// The associated room.
	Room *room;
	/// The x-axis coorinates.
	int x;
	/// The y-axis coorinates.
	int y;
	/// The z-axis coorinates.
	int z;
	/// The terrain applied to the tile.
	std::shared_ptr<Terrain> terrain;
	/// The liquid which fills the terrain by default.
	std::pair<Liquid *, unsigned int> liquidContent;

	/// @brief Constructor.
	MapTile() : room(), x(), y(), z(), terrain(), liquidContent()
	{
		// Nothing to do.
	}
};

/// A generic 2d map.
template <typename T> class Map2D {
private:
	std::vector<T> map;

public:
	int width;
	int height;

	Map2D() : map(), width(), height()
	{
	}

	Map2D(int _width, int _height) : map(), width(_width), height(_height)
	{
		map.reserve(width * height);
	}

	inline void set_size(int _width, int _height)
	{
		assert(map.empty());
		assert(_width >= 0);
		assert(_height >= 0);
		width = _width;
		height = _height;
		map.reserve(static_cast<unsigned int>(width * height));
	}

	inline T *operator()(int x, int y)
	{
		assert(x >= 0);
		assert(x < width);
		assert(y >= 0);
		assert(y < height);
		return &map[static_cast<unsigned int>(y * width + x)];
	}

	inline T const *operator()(int x, int y) const
	{
		assert(x >= 0);
		assert(x < width);
		assert(y >= 0);
		assert(y < height);
		return &map[static_cast<unsigned int>(y * width + x)];
	}
};

inline void init_map(Map2D<MapTile> &map, int width, int height)
{
	map.set_size(width, height);
	for (int x = 0; x < width; ++x) {
		for (int y = 0; y < height; ++y) {
			MapTile *tile = map(x, y);
			tile->x = x;
			tile->y = y;
			tile->z = 0;
		}
	}
}

inline std::string dump_map(Map2D<MapTile> &map, int layer)
{
	std::string s;
	s += std::string(static_cast<size_t>(map.width), '-') + "\n";
	for (int y = 0; y < map.height; ++y) {
		std::string line;
		for (int x = 0; x < map.width; ++x) {
			MapTile *tile = map(x, y);
			if (layer == 0) {
				line += Align(ToString(tile->z), align::center, 4) + ", ";
			} else if (layer == 1) {
				if (tile->terrain) {
					if ((tile->terrain->liquidContent.first == nullptr) &&
						tile->liquidContent.first) {
						line += Formatter::blue("r");
					} else {
						line += tile->terrain->symbol;
					}
				}
			}
		}
		s += line + "\n";
	}
	s += std::string(static_cast<size_t>(map.width), '-') + "\n";
	return s;
}

inline bool get_extremes(Map2D<MapTile> &map, int width, int height,
						 int &lowest, int &highest)
{
	for (int x = 0; x < width; ++x) {
		for (int y = 0; y < height; ++y) {
			MapTile *map_tile = map(x, y);
			if (map_tile->z < lowest)
				lowest = map_tile->z;
			if (map_tile->z > highest)
				highest = map_tile->z;
		}
	}
	return lowest != highest;
}

/// @brief Checks if the given tile is valid.
inline bool is_valid(MapConfiguration const &config, int x, int y, int z = 0)
{
	return (x >= 0) && (x < config.width) && (y >= 0) && (y < config.height) &&
		   (z >= 0) && (z < config.elevation);
}

inline MapTile *get_tile(Map2D<MapTile> &map, MapConfiguration const &config,
						 int x, int y, int z = 0)
{
	if (is_valid(config, x, y, z))
		return map(x, y);
	return nullptr;
}

inline std::vector<MapTile *> get_neighbours(Map2D<MapTile> &map,
											 MapConfiguration const &config,
											 MapTile *tile)
{
	std::vector<MapTile *> neighbours;
	auto north = get_tile(map, config, tile->x, tile->y + 1);
	if (north)
		neighbours.emplace_back(north);
	auto south = get_tile(map, config, tile->x, tile->y - 1);
	if (south)
		neighbours.emplace_back(south);
	auto west = get_tile(map, config, tile->x - 1, tile->y);
	if (west)
		neighbours.emplace_back(west);
	auto east = get_tile(map, config, tile->x + 1, tile->y);
	if (east)
		neighbours.emplace_back(east);
	return neighbours;
}

inline MapTile *find_lowest_nearby_tile(Map2D<MapTile> &map,
										MapConfiguration const &config,
										MapTile *tile)
{
	auto lowest = tile;
	auto neighbours = get_neighbours(map, config, tile);
	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(neighbours.begin(), neighbours.end(), g);
	for (auto const &neighbour : neighbours)
		if (neighbour->z < lowest->z)
			lowest = neighbour;
	return (lowest == tile) ? nullptr : lowest;
}

inline void decrese_nearby_tiles(Map2D<MapTile> &map,
								 MapConfiguration const &config, MapTile *tile,
								 int amplitude)
{
	auto neighbours = get_neighbours(map, config, tile);
	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(neighbours.begin(), neighbours.end(), g);
	std::uniform_int_distribution<int> dis(0, amplitude);
	for (auto const &neighbour : neighbours) {
		neighbour->z -= dis(g);
	}
}

} // namespace MapGen