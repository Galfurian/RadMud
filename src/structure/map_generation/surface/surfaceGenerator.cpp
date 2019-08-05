/// @file   surfaceGenerator.cpp
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

#include "surfaceGenerator.hpp"
#include "structureUtils.hpp"
#include "noiseGenerator.hpp"

#include <iostream>
#include <list>

namespace MapGen
{
bool generate_mountains(Map2D<MapTile> &map, MapConfiguration const &config)
{
	std::cout << "Generating the mountains.\n";
	for (int i = 0; i < config.numMountains; ++i) {
		// Generate a random dimension for the mountain.
		auto radius =
			TRand<int>(config.minMountainRadius, config.maxMountainRadius);
		// Generate a random place for the mountain.
		auto xCenter = TRand<int>(-radius, config.width + radius);
		auto yCenter = TRand<int>(-radius, config.height + radius);
		// Determine the boundaries.
		auto xMin = std::max(xCenter - radius - 1, 0);
		auto xMax = std::min(xCenter + radius + 1, config.width - 1);
		auto yMin = std::max(yCenter - radius - 1, 0);
		auto yMax = std::min(yCenter + radius + 1, config.height - 1);
		// Evaluate the square of the radius.
		auto squareRadius = radius * radius;
		// Evaluate the height of each tile inside the boundaries.
		for (auto x = xMin; x <= xMax; ++x) {
			for (auto y = yMin; y <= yMax; ++y) {
				// Determine the distance between the tile and the center.
				auto distance = pow(xCenter - x, 2) + pow(yCenter - y, 2);
				// Determine the height of the tile based on the distance.
				auto height = std::ceil(squareRadius - distance);
				// Set the height value on the map.
				if (height > 0)
					map(x, y)->z =
						(map(x, y)->z + static_cast<int>(height)) / 2;
			}
		}
	}
	std::cout << dump_map(map, 0);
	return true;
}

bool generate_noise(Map2D<MapTile> &map, MapConfiguration const &config)
{
	std::cout << "Applying noise to the map.\n";
	// Noise generator.
	NoiseGenerator ng;
	for (int x = 0; x < config.width; ++x) {
		for (int y = 0; y < config.height; ++y) {
			map(x, y)->z -= std::abs(ng.getNoise(x, y, 10.0, 0.5));
		}
	}
	return true;
}

bool normalize_map(Map2D<MapTile> &map, MapConfiguration const &config)
{
	// Find the minimum and maximum map.
	int lowest, highest;
	if (!get_extremes(map, config.width, config.height, lowest, highest)) {
		std::cerr << "Min and max height are the same.\n";
		return false;
	}
	std::cout << "Normalizing the map from (" << lowest << ", " << highest
			  << ") to (0, 100).\n";
	// Normalize the map to values between 0 and 100.
	for (int x = 0; x < config.width; ++x)
		for (int y = 0; y < config.height; ++y)
			map(x, y)->z = Normalize(map(x, y)->z, lowest, highest, 0, 100);

	std::cout << dump_map(map, 0);
	return true;
}

bool generate_rivers(Map2D<MapTile> &map, MapConfiguration const &config)
{
	std::cout << "Generating rivers on the map.\n";
	// List of possible starting points for a river.
	std::list<MapTile *> startingPoints;
	// Retrieve all the starting points for rivers.
	for (auto y = 0; y < config.height; ++y) {
		for (auto x = 0; x < config.width; ++x) {
			if (map(x, y)->terrain->liquidSources.empty()) {
				continue;
			}
			if (std::find_if(startingPoints.begin(), startingPoints.end(),
							 [&](MapTile *tile) {
								 return (StructUtils::getDistance(
											 x, tile->x, y, tile->y,
											 map(x, y)->z, tile->z) <=
										 config.minRiverDistance);
							 }) != startingPoints.end()) {
				continue;
			}
			startingPoints.emplace_back(map(x, y));
			std::cout << "River starts from " << x << ";" << y << "\n";
		}
	}
	if (startingPoints.empty()) {
		return true;
	}
	// Number of dropped rivers.
	auto iterations =
		std::min(static_cast<size_t>(config.numRivers), startingPoints.size());
	for (unsigned int it = 0; it < iterations; ++it) {
		// Prepare a vector for the river.
		std::vector<MapTile *> river;
		// Set the starting tile.
		auto tile = startingPoints.front();
		startingPoints.pop_front();
		// Pick from the available liquids of the starting tile.
		auto liquid = tile->terrain->getRandomLiquidSource();
		if (liquid == nullptr) {
			continue;
		}
		tile->liquidContent = std::make_pair(liquid, tile->terrain->space);
		// Add the starting tile to the river.
		river.push_back(tile);
		while (true) {
			// Get the lowest nearby tile.
			auto nextTile = find_lowest_nearby_tile(map, config, tile);
			// Check if the lowest is the current tile itself.
			if (nextTile == nullptr) {
				auto neighbours = get_neighbours(map, config, tile);
				if (neighbours.empty())
					break;
				if (TRand(0, 100) <= 45) {
					auto index = TRand<size_t>(0, neighbours.size() - 1);
					nextTile = neighbours[index];
					if (nextTile == nullptr) {
						break;
					}
				}
				break;
			}
			// If a liquid is reached stop.
			if (nextTile->liquidContent.first != nullptr) {
				break;
			}
			nextTile->liquidContent =
				std::make_pair(liquid, nextTile->terrain->space);
			// Add the next tile to the path of the river.
			river.push_back(nextTile);
			// Set the current tile to the next tile.
			tile = nextTile;
		}
	}
	return true;
}

bool apply_terrain(Map2D<MapTile> &map, MapConfiguration const &config,
				   std::shared_ptr<HeightMap> const &heightMap)
{
	std::cout << "Applying the terrain to the map.\n";
	for (auto x = 0; x < config.width; ++x) {
		for (auto y = 0; y < config.height; ++y) {
			// Get the terrain associated with the tile height.
			auto terrain = heightMap->getTerrain(map(x, y)->z);
			if (terrain == nullptr) {
				std::cerr << "Error while applying a terrain.\n";
				return false;
			}
			map(x, y)->terrain = terrain;
			map(x, y)->liquidContent = terrain->liquidContent;
		}
	}
	// Print the map for debug purpose.
	std::cout << dump_map(map, 0);
	return true;
}

bool flat_out_mainland(Map2D<MapTile> &map, MapConfiguration const &config,
					   std::shared_ptr<HeightMap> const &heightMap)
{
	std::cout << "Levelling mainland terrains.\n";
	auto seaLevel = heightMap->getSeaLevelHeight();
	std::cout << "Sea level : " << seaLevel << "\n";
	for (auto x = 0; x < config.width; ++x) {
		for (auto y = 0; y < config.height; ++y) {
			if (!heightMap->isAboveSeaLevel(map(x, y)->terrain)) {
				continue;
			}
			auto tileOffset = heightMap->getOffset(map(x, y)->terrain);
			map(x, y)->z = seaLevel + tileOffset;
		}
	}
	// Print the map for debug purpose.
	std::cout << dump_map(map, 1);
	std::cout << dump_map(map, 0);
	return true;
}

} // namespace MapGen
