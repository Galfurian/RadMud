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

#pragma once

#include <string>
#include <vector>

#include "coordinates.hpp"
#include "map3D.hpp"
#include "map2D.hpp"
#include "character.hpp"
#include "defines.hpp"

class Room;

/// @brief This class contains all the information regarding an entire area.
class Area
{
private:
    /// The list of integer used to identify different obstacles inside the map.
    using MapTile = enum class MapTile_t
    {
        Void,       ///< It's an empty tile.
        Walkable,   ///< It's a walkable tile.
        ClosedDoor, ///< It's a closed door.
    };

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
    /// @param coordinates The coordinates to check.
    /// @return <b>True</b> if the coordinates are valid,<br>
    ///         <b>False</b> otherwise.
    bool inBoundaries(const Coordinates & coordinates) const;

    /// @brief Checks if At the given coordinates there is a valid room through which a player Can See.
    /// @param coordinates The provided coordinates.
    /// @return <b>True</b> if there is a valid room at the given coordinates,<br>
    ///         <b>False</b> otherwise.
    bool isValid(const Coordinates & coordinates);

    /// @brief Provides the distance between the source and the target.
    /// @param source The source coordinates.
    /// @param target The target coordinates.
    /// @return The distance between the two coordinates.
    static int getDistance(const Coordinates & source, const Coordinates & target);

    /// @brief Provides the direction of the target w.r.t. the source.
    Direction getDirection(const Coordinates & source, const Coordinates & target);

    /// @brief Provides all the charactes inside the room at the given coordinates.
    /// @param exceptions  The exceptions.
    /// @param coordinates The desired coordinates.
    /// @return The list of characters at the given coordinates.
    CharacterContainer getCharactersAt(const CharacterContainer & exceptions, const Coordinates & coordinates);

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
    /// @param coordinates The coordiantes where search the room.
    /// @return The room at the selected spot.
    Room * getRoom(const Coordinates & coordinates);

    /// @brief Draw the Filed of View for a character.
    /// @param centerRoom The room from where the algorithm has to compute the Field of View.
    /// @param radius     The radius of visibility of the character.
    /// @return The map containing all the Information about the Field of View of a character.
    std::vector<std::string> drawFov(Room * centerRoom, const unsigned int & radius);

    /// @brief Draw the Filed of View for a character (ASCII).
    /// @param centerRoom The room from where the algorithm has to compute the Field of View.
    /// @param radius     The radius of visibility of the character.
    /// @return The map containing all the Information about the Field of View of a character.
    std::string drawASCIIFov(Room * centerRoom, const unsigned int & radius);

    /// @brief Provides a list of characters which are in sight.
    /// @param exceptions A list of excections.
    /// @param origin The coordinate of the central room.
    /// @param radius     The radius of visibility.
    /// @return The list containing the targets.
    CharacterContainer getCharactersInSight(CharacterContainer & exceptions,
                                            Coordinates & origin,
                                            const unsigned int & radius);

    /// @brief A Field of View algorithm which provides all the rooms which are inside the
    ///         radius of the field of view.
    /// @param origin The coordinate of the central room.
    /// @param radius The radius of visibility of the character.
    /// @return A vector containing all the coordinates of valid rooms.
    std::vector<Coordinates> fov(Coordinates & origin, const unsigned int & radius);

    /// @brief Determine if a coordinate is in sight from a starting one.
    /// @param source The coordinates of the origin.
    /// @param target The coordinates of the target room.
    /// @param radius   The radius of visibility.
    /// @return <b>True</b> if the target room is in sight,<br>
    ///         <b>False</b> otherwise.
    bool los(const Coordinates & source, const Coordinates & target, const unsigned int & radius);

    /// @brief Function used to register inside the lua environment the class.
    /// @param L The lua environment.
    static void luaRegister(lua_State * L);
};
