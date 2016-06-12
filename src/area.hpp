/// @file   area.hpp
/// @brief  An area contains a 3D map of rooms, this file defines all the methods needed to interact with an area.
/// @author Enrico Fraccaroli
/// @date   Mar 10 2015
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

#ifndef AREA_HPP
#define AREA_HPP

#include <string>
#include <vector>

#include "utilities/coordinates.hpp"
#include "utilities/map3D.hpp"
#include "utilities/map2D.hpp"
#include "defines.hpp"

class Room;

/// @brief This class contains all the information regarding an entire area.
class Area
{
    private:
        /// The list of integer used to identify different obstacles inside the map.
        typedef enum class MapTiles
        {
            None,
            /// It's an empty tile.
            Void,
            /// It's a walkable tile.
            Walkable,
            /// It's a closed door.
            ClosedDoor,
        } MapTile;

    public:
        /// The virtual number of the area.
        int vnum;
        /// The name of the area.
        std::string name;
        /// The name of the original builder of this area.
        std::string builder;
        /// The continent where the area is placed.
        std::string continent;
        /// The 3D grid of the map.
        Map3D<Room *> areaMap;
        /// The width of the area.
        int width;
        /// The height of the area.
        int height;
        /// The elevation of the area.
        int elevation;
        /// The tileset of the entire area.
        int tileSet;
        /// The type of area.
        AreaType type;
        /// The status of the area.
        AreaStatus status;

        /// Constructor.
        Area();

        /// Destructor.
        virtual ~Area();

        /// @brief Check the correctness of the area.
        /// @return <b>True</b> if the area has correct values,<br><b>False</b> otherwise.
        bool check();

        /// @brief Check if the given coordinates is inside the boundaries.
        /// @param x Coordinate on width axis.
        /// @param y Coordinate on height axis.
        /// @param z Coordinate on altitude axis.
        /// @return <b>True</b> if the coordinates are valid,<br>
        ///         <b>False</b> otherwise..
        bool inBoundaries(const int & x, const int & y, const int & z);

        /// @brief Check if the given coordinates is inside the boundaries.
        /// @param coord The coordinates to check.
        /// @return <b>True</b> if the coordinates are valid,<br>
        ///         <b>False</b> otherwise.
        bool inBoundaries(const Coordinates<int> & coord);

        /// @brief Add the passed room to its coordinates inside the area.
        /// @param room The room that has to be added.
        /// @return <b>True</b> if the room has been added,<br>
        ///         <b>False</b> otherwise.
        bool addRoom(Room * room);

        /// @brief Remove the passed room.
        /// @param room The room that has to be removed.
        /// @return <b>True</b> if the room has been removed,<br>
        ///         <b>False</b> otherwise.
        bool remRoom(Room * room);

        /// @brief Find a room  given its vnum.
        /// @param room_vnum The vnum of the room.
        /// @return The desired room.
        Room * getRoom(int room_vnum);

        /// @brief Find a room in a precise spot.
        /// @param x Coordinate on width axis.
        /// @param y Coordinate on height axis.
        /// @param z Coordinate on altitude axis.
        /// @return The room at the selected spot.
        Room * getRoom(int x, int y, int z);

        /// @brief Find a room in a precise spot.
        /// @param coord The coordiantes where search the room.
        /// @return The room at the selected spot.
        Room * getRoom(Coordinates<int> coord);

        /// @brief Draw the Filed of View for a character.
        /// @param centerRoom The room from where the algorithm has to compute the Field of View.
        /// @param radius     The radius of visibility of the character.
        /// @return The map containing all the Information about the Field of View of a character.
        std::vector<std::string> drawFov(Room * centerRoom, int radius);

        /// @brief Draw the Filed of View for a character (ASCII).
        /// @param centerRoom The room from where the algorithm has to compute the Field of View.
        /// @param radius     The radius of visibility of the character.
        /// @return The map containing all the Information about the Field of View of a character.
        std::string drawASCIIFov(Room * centerRoom, int radius);

        /// @brief Default version of a FOV alforithm.
        /// @param map      A 2D map, where the the Field of View will be drawn.
        /// @param origin_x The x coordinate of the central room.
        /// @param origin_y The y coordinate of the central room.
        /// @param origin_z The z coordinate of the central room.
        /// @param radius   The radius of visibility of the character.
        void fov(Map2D<MapTile> & map, int origin_x, int origin_y, int origin_z, int radius);

        /// @brief A simple line of sight algorithm.
        /// @param map      The map where the LOS algorithm has to write the line.
        /// @param origin_x The x coordinate of the central room.
        /// @param origin_y The y coordinate of the central room.
        /// @param origin_z The z coordinate of the central room.
        /// @param incr_x   The value of which the x coordiante must be incremented at each step.
        /// @param incr_y   The value of which the y coordiante must be incremented at each step.
        /// @param incr_z   The value of which the z coordiante must be incremented at each step.
        /// @param radius   The radius of visibility.
        void los(
            Map2D<MapTile> & map,
            int origin_x,
            int origin_y,
            int origin_z,
            double incr_x,
            double incr_y,
            double incr_z,
            int radius);

        /// @brief Determine if a coordinate is in sight from a starting one.
        /// @param origin_x The x coordinate of the central room.
        /// @param origin_y The y coordinate of the central room.
        /// @param origin_z The z coordinate of the central room.
        /// @param target_x The target x coordinate.
        /// @param target_y The target y coordinate.
        /// @param target_z The target z coordinate.
        /// @param radius   The radius of visibility.
        bool fastInSight(
            int origin_x,
            int origin_y,
            int origin_z,
            int target_x,
            int target_y,
            int target_z,
            unsigned int radius);

        /// @brief Determine if a coordinate is in sight from a starting one.
        /// @param origin The coordinates of the origin.
        /// @param origin The coordinates of the target room.
        /// @param radius The radius of visibility.
        bool fastInSight(Coordinates<int> origin, Coordinates<int> target, unsigned int radius);

        /// @brief Function used to register inside the lua environment the class.
        /// @param L The lua environment.
        static void luaRegister(lua_State * L);
};

/// Area list handler.
typedef std::vector<Area *> AreaList;

/// AreaList iterator.
typedef AreaList::iterator AreaListIterator;

/// Area map handler.
typedef std::map<int, Area *> AreaMap;

/// AreaMap iterator.
typedef AreaMap::iterator AreaMapIterator;

#endif
