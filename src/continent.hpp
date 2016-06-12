/// @file   continent.hpp
/// @brief  Define the continent class, used to create and manage a continent.
/// @author Enrico Fraccaroli
/// @date   Aug 1 2015
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

#ifndef CONTINENT_HPP
#define CONTINENT_HPP

#include "utilities/coordinates.hpp"
#include "utilities/map2D.hpp"
#include "utilities/map3D.hpp"

class Room;

#include <vector>
#include <string>

/// The list of tiles used in the continental map.
typedef enum class ContinentTiles
{
    /// It's an empty tile.
    Void = 0,
    /// It's a wall tile.
    Wall = '#',
    /// It's a walkable tile.
    Walk = '.',
    /// It's the player.
    Play = '@',
    /// It's a mobile.
    Mobile = 'M',
    /// It's a player.
    Player = 'P',
    /// It's an UpDown staris.
    UpDownStairs = 'x',
    /// It's an Up staris.
    UpStairs = '>',
    /// It's a Down staris.
    DownStairs = '<',
    /// It's a Pit.
    Pit = 'p',
    /// It's a piece of equipment.
    Equipment = 'e',
    /// It's a generic item.
    Item = 'i',
    /// It's a door tile.
    Door = 'D',
    /// It's a door tile.
    DoorOpen = 'd',
} ContinentTile;

/// @brief Holds details about a continent.
class Continent
{
    public:
        /// The virtual number of the continent.
        int vnum;
        /// The name of the continent.
        std::string name;
        /// The name of the original builder of this continent.
        std::string builder;
        /// The width of the continent.
        int width;
        /// The height of the continent.
        int height;
        /// The elevation of the continent.
        int elevation;
        /// The textual version of the continent map.
        std::string txtMap;
        /// The grid of the continent map.
        Map2D<char> charMap;
        /// The map of rooms.
        Map3D<Room> continentMap;
        /// The tileset of the entire continent.
        int tileSet;

        /// Constructor.
        Continent();

        /// Destructor.
        ~Continent();

        /// @brief Check the correctness of the continent.
        /// @return <b>True</b> if it has correct values,<br><b>False</b> otherwise.
        bool check();

        /// @brief Initialize the continent.
        void init();

        /// @brief Print the map of the continent.
        void print();

        /// @brief Check if the given coordinates is inside the boundaries.
        /// @param x Coordinate on width axis.
        /// @param y Coordinate on height axis.
        /// @param z Coordinate on altitude axis.
        /// @return <b>True</b> if the coordinates are valid,<br>
        ///         <b>False</b> otherwise..
        bool inBoundaries(int x, int y, int z);

        /// @brief Check if the given coordinates is inside the boundaries.
        /// @param coord The coordinates to check.
        /// @return <b>True</b> if the coordinates are valid,<br>
        ///         <b>False</b> otherwise.
        bool inBoundaries(Coordinates<int> coord);

        /// @brief Get the room at the given coordinates.
        /// @param x Coordinate on width axis.
        /// @param y Coordinate on height axis.
        /// @param z Coordinate on altitude axis.
        /// @return The room at the selected spot.
        Room * getRoom(int x, int y, int z);

        /// @brief Get the room at the given coordinates.
        /// @param coord The coordiantes where search the room.
        /// @return The room at the selected spot.
        Room * getRoom(Coordinates<int> coord);

        /// @brief Draw the Filed of View for a character.
        /// @param room   The room from where the algorithm has to compute the Field of View.
        /// @param radius The radius of visibility of the character.
        /// @return The map containing all the Information about the Field of View of a character.
        std::vector<std::string> drawFov(Room * room, int radius);

        /// @brief A faster but a little inacurate version of a FOV alforithm.
        /// @param map      A 2D map, where the the Field of View will be drawn.
        /// @param origin_x The x coordinate of the central room.
        /// @param origin_y The y coordinate of the central room.
        /// @param origin_z The z coordinate of the central room.
        /// @param radius   The radius of visibility of the character.
        void fieldOfView(Map2D<ContinentTile> & map, int origin_x, int origin_y, int origin_z, int radius);

        /// @brief A fast line of sight algorithm between two points.
        /// @param map      The map where the LOS algorithm has to write the line.
        /// @param origin_x The x coordinate of the central room.
        /// @param origin_y The y coordinate of the central room.
        /// @param origin_z The z coordinate of the central room.
        /// @param incr_x   The value of which the x coordiante must be incremented at each step.
        /// @param incr_y   The value of which the y coordiante must be incremented at each step.
        /// @param incr_z   The value of which the z coordiante must be incremented at each step.
        /// @param radius   The radius of visibility of the character.
        void lineOfSight(
            Map2D<ContinentTile> & map,
            int origin_x,
            int origin_y,
            int origin_z,
            double incr_x,
            double incr_y,
            double incr_z,
            int radius);
};

/// Continent list handler.
typedef std::vector<Continent *> ContinentList;

/// Continent map handler.
typedef std::map<int, Continent *> ContinentMap;

#endif
