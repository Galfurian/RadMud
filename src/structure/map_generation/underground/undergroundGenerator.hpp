/// @file   undegroundGenerator.hpp
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

#include <iostream>
#include <list>

#include "mapConfiguration.hpp"
#include "mapRoom.hpp"
#include "mapTile.hpp"

namespace MapGen
{
#if 0

inline std::shared_ptr<Terrain> get_random_terrain();

bool create_room(Map2D<MapTile> &map, std::shared_ptr<MapRoom> const &room);

bool generate_rooms(Map2D<MapTile> &map, MapConfiguration const &config,
					RoomList &roomList);

void init_undeground_map(MapConfiguration const &config, Map2D<MapTile> &map,
						 Map2D<MapTile> &support_map);

void generate_caves(MapConfiguration const &config, Map2D<MapTile> &map);

#endif
} // namespace MapGen