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
#include <iostream>

#include "mapGeneratorConfiguration.hpp"
#include "formatter.hpp"
#include "terrain.hpp"
#include "liquid.hpp"
#include "align.hpp"
#include "utils.hpp"

namespace MapGen
{

/// @brief A tile of the map.
class Tile
{
public:
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
    Tile() :
        x(),
        y(),
        z(),
        terrain(),
        liquidContent()
    {
        // Nothing to do.
    }
};

/// A 2d map of tiles.
typedef std::map<int, std::map<int, std::shared_ptr<Tile>>> Map2D;

inline void init_map(Map2D & map, int width, int height)
{
    for (auto x = 0; x < width; ++x)
    {
        for (auto y = 0; y < height; ++y)
        {
            auto tile = std::make_shared<Tile>();
            tile->x = x;
            tile->y = y;
            tile->z = 0;
            map[x][y] = tile;
        }
    }
}

inline void dump_map(Map2D & map,
                     int width,
                     int height,
                     int layer)
{
    std::cout << std::string(static_cast<size_t>(width), '-') << "\n";
    for (int y = 0; y < height; ++y)
    {
        std::string debug;
        for (int x = 0; x < width; ++x)
        {
            if (layer == 0)
            {
                std::string tile = " ";
                if (map[x][y]->terrain)
                {
                    if ((map[x][y]->terrain->liquidContent.first == nullptr) &&
                        map[x][y]->liquidContent.first)
                    {
                        tile = Formatter::blue("r");
                    }
                    else
                    {
                        tile = map[x][y]->terrain->symbol;
                    }
                }
                debug += tile;
            }
            else
            {
                debug += Align(ToString(map[x][y]->z), align::center, 2);
            }
        }
        std::cout << debug << "\n";
    }
    std::cout << std::string(static_cast<size_t>(width), '-') << "\n";
}

inline bool get_extremes(Map2D & map,
                         int width,
                         int height,
                         int & lowest,
                         int & highest)
{
    for (int x = 0; x < width; ++x)
    {
        for (int y = 0; y < height; ++y)
        {
            if (map[x][y]->z < lowest)
            {
                lowest = map[x][y]->z;
            }
            if (map[x][y]->z > highest)
            {
                highest = map[x][y]->z;
            }
        }
    }
    return lowest != highest;
}

/// @brief Checks if the given tile is valid.
inline bool is_valid(MapGeneratorConfiguration const & config,
                     int x, int y, int z = 0)
{
    return (x >= 0) && (x < config.width) &&
           (y >= 0) && (y < config.height) &&
           (z >= 0) && (z < config.elevation);
}

inline std::shared_ptr<Tile> get_tile(
    Map2D & map,
    MapGeneratorConfiguration const & config,
    int x, int y, int z = 0)
{
    if (is_valid(config, x, y, z)) return map[x][y];
    return nullptr;
}

inline std::vector<std::shared_ptr<Tile>> get_neighbours(
    Map2D & map,
    MapGeneratorConfiguration const & config,
    std::shared_ptr<Tile> const & tile)
{
    std::vector<std::shared_ptr<Tile>> neighbours;
    auto north = get_tile(map, config, tile->x, tile->y + 1);
    if (north) neighbours.emplace_back(north);
    auto south = get_tile(map, config, tile->x, tile->y - 1);
    if (south) neighbours.emplace_back(south);
    auto west = get_tile(map, config, tile->x - 1, tile->y);
    if (west) neighbours.emplace_back(west);
    auto east = get_tile(map, config, tile->x + 1, tile->y);
    if (east) neighbours.emplace_back(east);
    return neighbours;
}

inline std::shared_ptr<Tile> find_lowest_nearby_tile(
    Map2D & map,
    MapGeneratorConfiguration const & config,
    std::shared_ptr<Tile> const & tile)
{
    auto lowest = tile;
    auto neighbours = get_neighbours(map, config, tile);
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(neighbours.begin(), neighbours.end(), g);
    for (auto const & neighbour : neighbours)
    {
        if (neighbour->z < lowest->z)
        {
            lowest = neighbour;
        }
    }
    return (lowest == tile) ? nullptr : lowest;
}

inline void decrese_nearby_tiles(
    Map2D & map,
    MapGeneratorConfiguration const & config,
    std::shared_ptr<Tile> const & tile,
    int amplitude)
{
    auto neighbours = get_neighbours(map, config, tile);
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(neighbours.begin(), neighbours.end(), g);
    std::uniform_int_distribution<int> dis(0, amplitude);
    for (auto const & neighbour : neighbours)
    {
        neighbour->z -= dis(g);
    }
}

} // namespace MapGen