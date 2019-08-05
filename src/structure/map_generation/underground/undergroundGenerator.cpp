/// @file   undegroundGenerator.cpp
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

#include <iostream>
#include <list>

#include "undergroundGenerator.hpp"
#include "terrainFactory.hpp"
#include "heightMap.hpp"
#include "noiseGenerator.hpp"

namespace MapGen
{
#if 0
inline std::shared_ptr<Terrain> get_random_terrain()
{
	if (TRand(0, 100) <= 60) {
		return TerrainFactory::getCave();
	}
	return TerrainFactory::getSoil();
}

bool create_room(Map2D<MapTile> &map, std::shared_ptr<MapRoom> const &room)
{
	for (int x = room->x1; x < room->x2; ++x) {
		for (int y = room->y1; y < room->y2; ++y) {
			map(x, y)->terrain = TerrainFactory::getCave();
		}
	}
	return true;
}

bool generate_rooms(Map2D<MapTile> &map, MapConfiguration const &config,
					RoomList &roomList)
{
	auto IntersectObjects = [&map](std::shared_ptr<MapRoom> &room) {
		for (int x = room->x1; x < room->x2; ++x) {
			for (int y = room->y1; y < room->y2; ++y) {
				if (map(x, y)->terrain != nullptr) {
					return true;
				}
			}
		}
		return false;
	};
	auto IntersectOthers = [&roomList](std::shared_ptr<MapRoom> &room) {
		for (auto const &other : roomList) {
			if (room->intersects(other)) {
				return true;
			}
		}
		return false;
	};
	int z = TRand(0, config.elevation - 1);
	std::cout << "Generating underground rooms at z=" << z << ".\n";
	// Choose a level where the caves should be placed.
	for (int i = 0; i < config.maxCaves; ++i) {
		auto w = config.minCaveSize +
				 TRand<int>(0, config.maxCaveSize - config.minCaveSize + 1);
		auto h = config.minCaveSize +
				 TRand(0, config.maxCaveSize - config.minCaveSize + 1);
		auto x = TRand(0, config.width - w - 1);
		auto y = TRand(0, config.height - h - 1);
		auto newRoom = std::make_shared<MapRoom>(x, y, w, h);
		std::cout << "Generated room " << newRoom->toString() << ".\n";
		if (IntersectOthers(newRoom)) {
			continue;
		}
		if (IntersectObjects(newRoom)) {
			continue;
		}
		std::cout << "Placed room " << newRoom->toString() << ".\n";
		roomList.emplace_back(newRoom);
	}
	for (auto const &room : roomList) {
		std::cout << "Created room " << room->toString() << ".\n";
		create_room(map, room);
	}
	std::cout << "Successfully generated " << roomList.size() << " rooms.\n";
	// Print the map for debug purpose.
	std::cout << dump_map(map,  0);
	return true;
}

void init_undeground_map(MapConfiguration const &config, Map2D<MapTile> &map,
						 Map2D<MapTile> &support_map)
{
	for (auto xi = 0; xi < config.width; ++xi) {
		for (auto yi = 0; yi < config.height; ++yi) {
			if ((yi == 0) || (yi == (config.height - 1))) {
				map(xi, yi)->terrain = TerrainFactory::getSoil();
			} else if ((xi == 0) || (xi == (config.width - 1))) {
				map(xi, yi)->terrain = TerrainFactory::getSoil();
			} else {
				map(xi, yi)->terrain = get_random_terrain();
			}
			support_map(xi, yi)->terrain = TerrainFactory::getSoil();
		}
	}
}

void generate_caves(MapConfiguration const &config, Map2D<MapTile> &map)
{
	Map2D<MapTile> support;
	init_map(support, config.width, config.height);
	init_undeground_map(config, map, support);
	for (auto xi = 1; xi < config.width - 1; xi++) {
		for (auto yi = 1; yi < config.height - 1; yi++) {
			int adjcount_r1 = 0, adjcount_r2 = 0;
			for (auto ii = -1; ii <= 1; ii++) {
				for (auto jj = -1; jj <= 1; jj++) {
					if (map(xi + jj, yi + ii)->terrain ==
						TerrainFactory::getSoil()) {
						++adjcount_r1;
					}
				}
			}
			for (auto ii = yi - 2; ii <= yi + 2; ii++) {
				for (auto jj = xi - 2; jj <= xi + 2; jj++) {
					if (abs(ii - yi) == 2 && abs(jj - xi) == 2)
						continue;
					if (ii < 0 || jj < 0 || ii >= config.height ||
						jj >= config.width) {
						continue;
					}
					if (map(jj, ii)->terrain == TerrainFactory::getSoil())
						++adjcount_r2;
				}
			}
			if (adjcount_r1 >= 4 || adjcount_r2 <= 1) {
				support(xi, yi)->terrain = TerrainFactory::getSoil();
			} else {
				support(xi, yi)->terrain = TerrainFactory::getCave();
			}
		}
	}
	for (auto xi = 1; xi < config.width - 1; xi++)
		for (auto yi = 1; yi < config.height - 1; yi++)
			(*map(xi, yi)) = (*support(xi, yi));
	// Print the map for debug purpose.
	std::cout << dump_map(map,  0);
}
#endif

} // namespace MapGen