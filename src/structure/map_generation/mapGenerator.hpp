/// @file   mapGenerator.hpp
/// @author Enrico Fraccaroli
/// @date   Jan 05 2017
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

#include "undergroundGenerator.hpp"
#include "surfaceGenerator.hpp"
#include "mapConfiguration.hpp"
#include "heightMap.hpp"
#include "formatter.hpp"
#include "mapRoom.hpp"
#include "utils.hpp"
#include "noiseGenerator.hpp"
#include "mapTile.hpp"
#include "mapWrapper.hpp"

#include <memory>
#include <map>
#include <iostream>
#include <list>
#include <cassert>

namespace MapGen
{
/// @brief Automatic generator of maps.
class MapGenerator {
private:
	/// Generator configuration.
	MapConfiguration config;
	/// Height map.
	std::shared_ptr<HeightMap> heightMap;

public:
	/// @brief Constructor.
	MapGenerator(MapConfiguration const &_config,
				 std::shared_ptr<HeightMap> const &_heightMap) :
		config(_config),
		heightMap(_heightMap)
	{
		// Nothing to do.
	}

	/// @brief Generates a new map.
	std::shared_ptr<MapWrapper> generateMap()
	{
		auto wrapper = std::make_shared<MapWrapper>();
		//Map2D<MapTile> underground;
		//RoomList roomList;

		// --------------------------------------------------------------------
		// Phase 0 : Initialize the map.
		init_map(wrapper->map, config.width, config.height);
		//init_map(underground, config.width, config.height);

		// --------------------------------------------------------------------
		// Phase I : use a 2d map to generate the terrain layer.
		// Generate the mountains.
		if (!generate_mountains(wrapper->map, config))
			return nullptr;
		// Normalize the map in order to have values between 0 and 100.
		if (!normalize_map(wrapper->map, config))
			return nullptr;
		// Add noise to the map.
		if (!generate_noise(wrapper->map, config))
			return nullptr;
		// Normalize the map in order to have values between 0 and 100.
		if (!normalize_map(wrapper->map, config))
			return nullptr;
		// Apply the terrain to the map.
		if (!apply_terrain(wrapper->map, config, heightMap))
			return nullptr;
		// Generate the rivers.
		if (!generate_rivers(wrapper->map, config))
			return nullptr;
		// Flat out the mainland.
		if (!flat_out_mainland(wrapper->map, config, heightMap))
			return nullptr;

		// --------------------------------------------------------------------
		// Phase II : undeground caves.
		//generate_rooms(underground, config, roomList);
		//generate_caves(config, underground);
		return wrapper;
	}
};
} // namespace MapGen