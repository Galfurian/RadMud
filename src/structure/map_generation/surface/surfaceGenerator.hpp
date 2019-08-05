/// @file   surfaceGenerator.hpp
/// @author Enrico Fraccaroli
/// @date   Mar 18 2018
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

#include "mapConfiguration.hpp"
#include "heightMap.hpp"
#include "mapTile.hpp"

namespace MapGen
{
/// @brief Creates the mountains on the map.
/// @param map      The map.
/// @param config   The configuration file.
/// @return Error state.
bool generate_mountains(Map2D<MapTile> &map, MapConfiguration const &config);

/// @brief Generates noise on the map.
/// @param map      The map.
/// @param config   The configuration file.
/// @return Error state.
bool generate_noise(Map2D<MapTile> &map, MapConfiguration const &config);

/// @brief Normalizes the map with values between a specific range
/// according to the HeightMap.
/// @param map      The map.
/// @param config   The configuration file.
/// @return Error state.
bool normalize_map(Map2D<MapTile> &map, MapConfiguration const &config);

bool generate_rivers(Map2D<MapTile> &map, MapConfiguration const &config);

/// @brief Apply the terrain on the map based on the height map.
/// @param map The map.
/// @return the error state.
bool apply_terrain(Map2D<MapTile> &map, MapConfiguration const &config,
				   std::shared_ptr<HeightMap> const &heightMap);

/// @brief Finalizes the z coordinates of the tiles inside the map around
/// the mid-value of the altitude.
/// @param map The map.
/// @return the error state.
bool flat_out_mainland(Map2D<MapTile> &map, MapConfiguration const &config,
					   std::shared_ptr<HeightMap> const &heightMap);

} // namespace MapGen
