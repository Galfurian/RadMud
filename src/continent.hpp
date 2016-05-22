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
    private:
        /// It's a multimap of rooms defined inside a 2D grid.
        typedef std::map<std::tuple<int, int, int>, Room> ContinentMap;

    public:
        /// The virtual number of the continent.
        int vnum;
        /// The name of the continent.
        std::string name;
        /// The name of the original builder of this continent.
        std::string builder;
        /// The width of the continent.
        unsigned int width;
        /// The height of the continent.
        unsigned int height;
        /// The textual version of the continent map.
        std::string txtMap;
        /// The grid of the continent map.
        Map2D<char> charMap;
        /// The map of rooms.
        ContinentMap continentMap;
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
        bool inBoundaries(unsigned int x, unsigned int y, unsigned int z);

        /// @brief Check if the given coordinates is inside the boundaries.
        /// @param coord The coordinates to check.
        /// @return <b>True</b> if the coordinates are valid,<br>
        ///         <b>False</b> otherwise.
        bool inBoundaries(Coordinates coord);

        /// @brief Get the room at the given coordinates.
        /// @param y Coordinate on height axis.
        /// @param x Coordinate on width axis.
        /// @param x Coordinate on elevation axis.
        /// @return The room at the selected spot.
        Room * getRoom(unsigned int y, unsigned int x, unsigned int z);

        /// @brief Get the room at the given coordinates.
        /// @param coord The coordiantes where search the room.
        /// @return The room at the selected spot.
        Room * getRoom(Coordinates coord);

        /// @brief Draw the Filed of View for a character.
        /// @param room   The room from where the algorithm has to compute the Field of View.
        /// @param radius The radius of visibility of the character.
        /// @return The map containing all the Information about the Field of View of a character.
        std::vector<std::string> drawFov(Room * room, unsigned int radius);

        /// @brief A faster but a little inacurate version of a FOV alforithm.
        /// @param map    A 2D map, where the the Field of View will be drawn.
        /// @param x      Coordinate on width axis.
        /// @param y      Coordinate on height axis.
        /// @param radius The radius of visibility of the character.
        void fieldOfView(
            Map2D<ContinentTile> & map,
            unsigned int origin_x,
            unsigned int origin_y,
            unsigned int origin_z,
            unsigned int radius);

        /// @brief A fast line of sight algorithm between two points.
        /// @param map The map where the LOS algorithm has to write the line.
        /// @param x0  The starting x coordinate.
        /// @param y0  The starting y coordinate.
        /// @param x1  The final x coordinate.
        /// @param y1  The final y coordinate.
        /// @param radius The radius of visibility of the character.
        void lineOfSight(
            Map2D<ContinentTile> & map,
            unsigned int origin_x,
            unsigned int origin_y,
            unsigned int origin_z,
            double incr_x,
            double incr_y,
            double incr_z,
            unsigned int radius);
};

/// Continent list handler.
typedef std::vector<Continent *> ContinentList;

/// Continent map handler.
typedef std::map<int, Continent *> ContinentMap;

#endif
